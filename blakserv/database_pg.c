// Meridian 59 - PostgreSQL database implementation.
// Replaces MySQL database.c with libpq-based PostgreSQL support.

#include "blakserv.h"
#ifndef BLAK_PLATFORM_WINDOWS
#include <pthread.h>
#include <unistd.h>
#endif
#include <libpq-fe.h>
#define DATABASE_SCHEMA_POSTGRESQL
#include "database_schema.h"

// Worker thread state
enum { DB_STOPPED=0, DB_STOPPING, DB_STARTING, DB_CONNECTED, DB_READY };
static int db_state = DB_STOPPED;

// Connection
static PGconn *pg_conn = NULL;
static char *db_host = NULL;
static int db_port = 0;
static char *db_user = NULL;
static char *db_pass = NULL;
static char *db_name = NULL;

// Queue
static sql_queue queue;
static UINT64 record_count = 0;

// Worker thread
#ifdef BLAK_PLATFORM_WINDOWS
static HANDLE db_thread = NULL;
#else
static pthread_t db_thread;
#endif

#define MAX_RECORD_QUEUE 15000
#define MAX_SQL_PARAMS 45
#define DB_WORKER_SLEEP_MS 10
#define DB_RECONNECT_SLEEP_MS 5000
#define MAX_ITEMS_PER_LOOP 10

#ifdef BLAK_PLATFORM_WINDOWS
static bool PgQueueInit(void)
{
   queue.mutex = CreateMutex(NULL, FALSE, NULL);
   return queue.mutex != NULL;
}

static void PgQueueShutdown(void)
{
   if (queue.mutex != NULL)
   {
      CloseHandle(queue.mutex);
      queue.mutex = NULL;
   }
}

static void PgQueueLock(void)
{
   WaitForSingleObject(queue.mutex, INFINITE);
}

static void PgQueueUnlock(void)
{
   ReleaseMutex(queue.mutex);
}

static void PgSleepMs(unsigned int delay_ms)
{
   Sleep(delay_ms);
}

static char *PgStrdup(const char *value)
{
   return value != NULL ? _strdup(value) : NULL;
}

static bool PgThreadStart(void);
static void PgThreadJoin(void);
static unsigned __stdcall PgWorker(void *arg);
#else
static bool PgQueueInit(void)
{
   return pthread_mutex_init(&queue.mutex, NULL) == 0;
}

static void PgQueueShutdown(void)
{
   pthread_mutex_destroy(&queue.mutex);
}

static void PgQueueLock(void)
{
   pthread_mutex_lock(&queue.mutex);
}

static void PgQueueUnlock(void)
{
   pthread_mutex_unlock(&queue.mutex);
}

static void PgSleepMs(unsigned int delay_ms)
{
   usleep(delay_ms * 1000);
}

static char *PgStrdup(const char *value)
{
   return value != NULL ? strdup(value) : NULL;
}

static bool PgThreadStart(void);
static void PgThreadJoin(void);
static void *PgWorker(void *arg);
#endif

static bool PgThreadStart(void)
{
#ifdef BLAK_PLATFORM_WINDOWS
   uintptr_t thread_handle = _beginthreadex(NULL, 0, PgWorker, NULL, 0, NULL);
   if (thread_handle == 0)
      return false;

   db_thread = (HANDLE)thread_handle;
   return true;
#else
   return pthread_create(&db_thread, NULL, PgWorker, NULL) == 0;
#endif
}

static void PgThreadJoin(void)
{
#ifdef BLAK_PLATFORM_WINDOWS
   if (db_thread != NULL)
   {
      WaitForSingleObject(db_thread, INFINITE);
      CloseHandle(db_thread);
      db_thread = NULL;
   }
#else
   pthread_join(db_thread, NULL);
#endif
}

/******************************************************************************/
// Internal functions
/******************************************************************************/

static bool PgConnect(void)
{
   char conninfo[512];
   snprintf(conninfo, sizeof(conninfo),
      "host=%s port=%d user=%s password=%s dbname=%s",
      db_host, db_port, db_user, db_pass, db_name);

   pg_conn = PQconnectdb(conninfo);
   if (PQstatus(pg_conn) != CONNECTION_OK)
   {
      eprintf("PostgreSQL connection failed: %s\n", PQerrorMessage(pg_conn));
      PQfinish(pg_conn);
      pg_conn = NULL;
      return false;
   }

   // Set client encoding to LATIN1 for German umlauts and legacy data
   PQsetClientEncoding(pg_conn, "LATIN1");

   lprintf("PostgreSQL connected to %s:%d/%s\n", db_host, db_port, db_name);
   return true;
}

static bool PgVerifySchema(void)
{
   for (size_t i = 0; i < DATABASE_SCHEMA_STATEMENT_COUNT; ++i)
   {
      PGresult *res = PQexec(pg_conn, DatabaseSchemaStatements[i]);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
         eprintf("PostgreSQL schema creation failed: %s\n", PQerrorMessage(pg_conn));
         PQclear(res);
         return false;
      }
      PQclear(res);
   }
   dprintf("PostgreSQL schema verified\n");
   return true;
}

static bool PgIsConnected(void)
{
   if (!pg_conn)
      return false;
   if (PQstatus(pg_conn) != CONNECTION_OK)
   {
      // Try to reset the connection
      PQreset(pg_conn);
      return PQstatus(pg_conn) == CONNECTION_OK;
   }
   return true;
}

static bool PgAppendSql(char *sql, size_t capacity, size_t *length, const char *text)
{
   size_t text_length = strlen(text);
   if (text_length >= capacity - *length)
      return false;

   memcpy(sql + *length, text, text_length);
   *length += text_length;
   sql[*length] = 0;
   return true;
}

static bool PgAppendSqlItem(char *sql, size_t capacity, size_t *length,
   bool *first_item, const char *text)
{
   if (!*first_item && !PgAppendSql(sql, capacity, length, ", "))
      return false;

   *first_item = false;
   return PgAppendSql(sql, capacity, length, text);
}

static bool PgGetColumn(const char *columns, int column_index,
   char *column, size_t column_capacity)
{
   const char *start = columns;
   for (int i = 0; i < column_index; ++i)
   {
      start = strchr(start, ',');
      if (!start)
         return false;
      ++start;
   }

   const char *end = strchr(start, ',');
   size_t column_length = end ? (size_t)(end - start) : strlen(start);
   if (column_length == 0 || column_length >= column_capacity)
      return false;

   memcpy(column, start, column_length);
   column[column_length] = 0;
   return true;
}

static bool PgAppendInsertColumns(char *sql, size_t capacity, size_t *length,
   const sql_statistic_type *operation, bool values)
{
   bool first_item = true;
   for (int i = 0; i <= operation->num_fields; ++i)
   {
      if (operation->timestamp_column && operation->timestamp_position == i)
      {
         if (!PgAppendSqlItem(sql, capacity, length, &first_item,
            values ? "NOW()" : operation->timestamp_column))
            return false;
      }

      if (i == operation->num_fields)
         break;

      char item[64];
      if (values)
         snprintf(item, sizeof(item), "$%d", i + 1);
      else if (!PgGetColumn(operation->columns, i, item, sizeof(item)))
         return false;

      if (!PgAppendSqlItem(sql, capacity, length, &first_item, item))
         return false;
   }

   return true;
}

static bool PgBuildWriteQuery(char *sql, size_t capacity,
   const sql_statistic_type *operation)
{
   size_t length = 0;
   bool first_item = true;
   sql[0] = 0;

   if (operation->write_mode == DB_WRITE_UPDATE)
   {
      if (!PgAppendSql(sql, capacity, &length, "UPDATE ") ||
         !PgAppendSql(sql, capacity, &length, operation->table_name) ||
         !PgAppendSql(sql, capacity, &length, " SET ") ||
         !PgAppendSql(sql, capacity, &length, operation->update_assignments) ||
         !PgAppendSql(sql, capacity, &length, " WHERE "))
         return false;

      for (int i = 0; i < operation->num_fields; ++i)
      {
         char column[64];
         char predicate[96];
         if (!PgGetColumn(operation->columns, i, column, sizeof(column)))
            return false;
         snprintf(predicate, sizeof(predicate), "%s = $%d", column, i + 1);
         if ((i > 0 && !PgAppendSql(sql, capacity, &length, " AND ")) ||
            !PgAppendSql(sql, capacity, &length, predicate))
            return false;
      }
      return true;
   }

   if (!PgAppendSql(sql, capacity, &length, "INSERT INTO ") ||
      !PgAppendSql(sql, capacity, &length, operation->table_name) ||
      !PgAppendSql(sql, capacity, &length, " ("))
      return false;

   if (!PgAppendInsertColumns(sql, capacity, &length, operation, false) ||
      !PgAppendSql(sql, capacity, &length, ") VALUES ("))
      return false;

   first_item = true;
   if (!PgAppendInsertColumns(sql, capacity, &length, operation, true) ||
      !PgAppendSql(sql, capacity, &length, ")"))
      return false;

   if (operation->write_mode == DB_WRITE_UPSERT)
   {
      if (!PgAppendSql(sql, capacity, &length, " ON CONFLICT (") ||
         !PgAppendSql(sql, capacity, &length, operation->conflict_columns) ||
         !PgAppendSql(sql, capacity, &length, ") DO UPDATE SET "))
         return false;

      first_item = true;
      for (int i = 0; ; ++i)
      {
         char column[64];
         char assignment[160];
         if (!PgGetColumn(operation->update_columns, i, column, sizeof(column)))
            break;
         snprintf(assignment, sizeof(assignment), "%s = EXCLUDED.%s", column, column);
         if (!PgAppendSqlItem(sql, capacity, &length, &first_item, assignment))
            return false;
      }
   }

   return true;
}

static void PgWriteNode(sql_queue_node *node)
{
   sql_data_node *data = (sql_data_node *)node->data;
   int type = node->type;
   int nf = node->num_fields;

   if (type <= STAT_NONE || type > STAT_MAXSTAT)
      return;

   if (!Statistics_Table[type].enabled)
      return;

   const sql_statistic_type *operation = &Statistics_Table[type];
   if (!operation->table_name)
      return;

   if (nf != operation->num_fields || nf > MAX_SQL_PARAMS)
   {
      eprintf("Invalid PostgreSQL field count for statistic %d: found %d, expected %d\n",
         type, nf, operation->num_fields);
      return;
   }

   // Build parameterized INSERT
   // Convert all fields to string params for PQexecParams
   const char *paramValues[MAX_SQL_PARAMS];
   char numbufs[MAX_SQL_PARAMS][20];
   int nParams = 0;

   for (int i = 0; i < nf && i < MAX_SQL_PARAMS; i++)
   {
      switch (data[i].type)
      {
      case TAG_NIL:
         paramValues[nParams] = NULL;
         break;
      case TAG_INT:
         snprintf(numbufs[nParams], sizeof(numbufs[0]), "%d", data[i].value.num);
         paramValues[nParams] = numbufs[nParams];
         break;
      case TAG_STRING:
      case TAG_RESOURCE:
         paramValues[nParams] = data[i].value.str ? data[i].value.str : "";
         break;
      case TAG_SESSION:
         snprintf(numbufs[nParams], sizeof(numbufs[0]), "%d", data[i].value.num);
         paramValues[nParams] = numbufs[nParams];
         break;
      default:
         paramValues[nParams] = NULL;
         break;
      }
      nParams++;
   }

   char sql[4096];
   if (!PgBuildWriteQuery(sql, sizeof(sql), operation))
   {
      eprintf("Could not build PostgreSQL write query for statistic %d\n", type);
      return;
   }

   PGresult *res = PQexecParams(pg_conn, sql, nParams, NULL,
      paramValues, NULL, NULL, 0);

   if (PQresultStatus(res) != PGRES_COMMAND_OK)
   {
      // Don't spam errors - just log once
      static int error_count = 0;
      if (error_count++ < 10)
         eprintf("PostgreSQL write to %s failed: %s\n", operation->table_name, PQerrorMessage(pg_conn));
   }
   else
   {
      record_count++;
   }
   PQclear(res);
}

static void PgTruncateTable(int type)
{
   if (type <= STAT_NONE || type > STAT_MAXSTAT)
      return;

   const char *table = Statistics_Table[type].table_name;
   if (!table)
      return;

   char sql[256];
   snprintf(sql, sizeof(sql), "TRUNCATE TABLE %s", table);
   PGresult *res = PQexec(pg_conn, sql);
   if (PQresultStatus(res) != PGRES_COMMAND_OK)
      eprintf("PostgreSQL TRUNCATE %s failed: %s\n", table, PQerrorMessage(pg_conn));
   PQclear(res);
}

static bool PgDequeue(void)
{
   PgQueueLock();
   if (queue.count == 0)
   {
      PgQueueUnlock();
      return false;
   }

   sql_queue_node *node = queue.first;
   queue.first = node->next;
   if (queue.first == NULL)
      queue.last = NULL;
   queue.count--;
   PgQueueUnlock();

   if (node->num_fields < 0)
      PgTruncateTable(node->type);
   else
      PgWriteNode(node);

   // Free data
   if (node->data)
      FreeDataNodeMemory(abs(node->num_fields),
         abs(node->num_fields), (sql_data_node *)node->data);
   FreeMemory(MALLOC_ID_SQL, node, sizeof(sql_queue_node));

   return true;
}

#ifdef BLAK_PLATFORM_WINDOWS
static unsigned __stdcall PgWorker(void *arg)
#else
static void *PgWorker(void *arg)
#endif
{
   if (!PgQueueInit())
   {
   db_state = DB_STOPPED;
#ifdef BLAK_PLATFORM_WINDOWS
   return 0;
#else
   return NULL;
#endif
   }

   db_state = DB_STARTING;

   // Connect
   while (db_state != DB_STOPPING)
   {
      if (PgConnect())
      {
         if (PgVerifySchema())
         {
            db_state = DB_READY;
            break;
         }
         PQfinish(pg_conn);
         pg_conn = NULL;
      }
      // Retry after delay
      PgSleepMs(DB_RECONNECT_SLEEP_MS);
   }

   // Main work loop
   while (db_state != DB_STOPPING)
   {
      if (db_state == DB_READY && PgIsConnected())
      {
         int processed = 0;
         for (int i = 0; i < MAX_ITEMS_PER_LOOP; i++)
         {
            if (PgDequeue())
               processed++;
            else
               break;
         }
         if (processed == 0)
            PgSleepMs(DB_WORKER_SLEEP_MS);
      }
      else
      {
         // Try reconnect
         if (!PgIsConnected())
         {
            eprintf("PostgreSQL connection lost, reconnecting...\n");
            if (!PgConnect())
               PgSleepMs(DB_RECONNECT_SLEEP_MS);
            else
               db_state = DB_READY;
         }
      }
   }

   // Drain remaining queue
   while (PgDequeue()) {}

   if (pg_conn)
   {
      PQfinish(pg_conn);
      pg_conn = NULL;
   }
   PgQueueShutdown();
   db_state = DB_STOPPED;
#ifdef BLAK_PLATFORM_WINDOWS
   return 0;
#else
   return NULL;
#endif
}

/******************************************************************************/
// Public API
/******************************************************************************/

void MySQLInit(char *Host, int Port, char *User, char *Password, char *DB)
{
   if (db_state != DB_STOPPED)
      return;

   if (!Host || !User || !Password || !DB)
   {
      eprintf("PostgreSQL init failed: missing connection parameters\n");
      return;
   }

   db_host = PgStrdup(Host);
   db_port = Port;
   db_user = PgStrdup(User);
   db_pass = PgStrdup(Password);
   db_name = PgStrdup(DB);

   if (!PgThreadStart())
   {
      free(db_host); db_host = NULL;
      free(db_user); db_user = NULL;
      free(db_pass); db_pass = NULL;
      free(db_name); db_name = NULL;
      db_state = DB_STOPPED;
      eprintf("PostgreSQL init failed: could not start writer thread\n");
      return;
   }

   PgSleepMs(100); // 100ms for thread startup
}

void MySQLEnd()
{
   if (db_state == DB_STOPPED)
      return;

   db_state = DB_STOPPING;
   PgThreadJoin();

   free(db_host); db_host = NULL;
   free(db_user); db_user = NULL;
   free(db_pass); db_pass = NULL;
   free(db_name); db_name = NULL;
}

BOOL MySQLRecordGeneric(int type, int num_fields, sql_data_node data[])
{
   if (type <= STAT_NONE || type > STAT_MAXSTAT)
      return FALSE;

   if (!Statistics_Table[type].enabled)
   {
      FreeDataNodeMemory(num_fields, num_fields, data);
      return FALSE;
   }

   if (db_state < DB_READY)
   {
      FreeDataNodeMemory(num_fields, num_fields, data);
      return FALSE;
   }

   sql_queue_node *node = (sql_queue_node *)AllocateMemory(MALLOC_ID_SQL,
      sizeof(sql_queue_node));
   node->type = (sql_recordtype)type;
   node->num_fields = num_fields;
   node->data = data;
   node->next = NULL;

   PgQueueLock();
   if (queue.count >= MAX_RECORD_QUEUE)
   {
      PgQueueUnlock();
      FreeDataNodeMemory(num_fields, num_fields, data);
      FreeMemory(MALLOC_ID_SQL, node, sizeof(sql_queue_node));
      eprintf("PostgreSQL queue full, dropping record\n");
      return FALSE;
   }

   if (queue.last)
      queue.last->next = node;
   else
      queue.first = node;
   queue.last = node;
   queue.count++;
   PgQueueUnlock();

   return TRUE;
}

BOOL MySQLEmptyTable(int type)
{
   sql_queue_node *node = (sql_queue_node *)AllocateMemory(MALLOC_ID_SQL,
      sizeof(sql_queue_node));
   node->type = (sql_recordtype)type;
   node->num_fields = -1;
   node->data = NULL;
   node->next = NULL;

   PgQueueLock();
   if (queue.last)
      queue.last->next = node;
   else
      queue.first = node;
   queue.last = node;
   queue.count++;
   PgQueueUnlock();

   return TRUE;
}

UINT64 MySQLGetRecordCount()
{
   return record_count;
}

char *MySQLDuplicateString(char *str)
{
   if (!str)
   {
      bprintf("MySQLDuplicateString could not duplicate null string!");
      return NULL;
   }
   int sLen = (int)strlen(str);
   char *ret_str = (char *)AllocateMemory(MALLOC_ID_SQL, sLen + 1);
   memcpy(ret_str, str, sLen);
   ret_str[sLen] = 0;
   return ret_str;
}

bool MySQLIsTypeEnabled(int type)
{
   if (type >= STAT_NONE && type <= STAT_MAXSTAT)
      return Statistics_Table[type].enabled;
   return false;
}

bool MySQLSetTypeEnabled(int type, bool enabled)
{
   if (type >= STAT_NONE && type <= STAT_MAXSTAT)
   {
      Statistics_Table[type].enabled = enabled;
      return true;
   }
   return false;
}

void FreeDataNodeMemory(int total_fields, int fields_entered, sql_data_node data[])
{
   for (int i = 0; i < fields_entered; i++)
   {
      if ((data[i].type == TAG_STRING || data[i].type == TAG_RESOURCE)
         && data[i].value.str)
      {
         FreeMemory(MALLOC_ID_SQL, data[i].value.str, (int)strlen(data[i].value.str) + 1);
      }
   }
   FreeMemory(MALLOC_ID_SQL, data, sizeof(sql_data_node) * total_fields);
}
