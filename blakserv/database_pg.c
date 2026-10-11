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

// Statistics table - maps STAT_TYPE to table name and expected field count
// timestamp_pos: 0 = no auto-timestamp, 1 = NOW() as first value, -1 = NOW() as last value
static sql_statistic_type Statistics_Table[] = {
   {STAT_NONE,             0, false, NULL,                        0},
   {STAT_TOTALMONEY,       1, true,  "money_total",               1},
   {STAT_MONEYCREATED,     1, true,  "money_created",             1},
   {STAT_PLAYERLOGIN,      3, true,  "player_logins",            -1},
   {STAT_ASSESS_DAM,       7, true,  "player_damaged",           -1, true},
   {STAT_PLAYERDEATH,      5, true,  "player_death",             -1},
   {STAT_PLAYER,          13, true,  "player",                    0},
   {STAT_PLAYERSUICIDE,    2, true,  "player",                    0},
   {STAT_GUILD,            4, true,  "guild",                     0},
   {STAT_GUILDDISBAND,     1, true,  "guild",                     0},
   {STAT_SPELLS,          14, true,  "wiki_spells",               0},
   {STAT_SPELL_REAGENT,    3, true,  "wiki_spell_reagent",        0},
   {STAT_TREASURE_GEN,     3, true,  "wiki_treasure_gen",         0},
   {STAT_MONSTER,         13, true,  "wiki_monster",              0},
   {STAT_MONSTER_ZONE,     3, true,  "wiki_monster_zone",         0},
   {STAT_NPCS,             7, true,  "wiki_npcs",                 0},
   {STAT_WEAPONS,         14, true,  "wiki_weapons",              0},
   {STAT_ROOMS,            6, true,  "wiki_rooms",                0},
   {STAT_NPC_ZONE,         4, true,  "wiki_npc_zone",             0},
   {STAT_NPC_SELLITEM,     3, true,  "wiki_npc_sellitem",         0},
   {STAT_NPC_SELLSKILL,    2, true,  "wiki_npc_sellskill",        0},
   {STAT_NPC_SELLSPELL,    2, true,  "wiki_npc_sellspell",        0},
   {STAT_REAGENTS,        11, true,  "wiki_reagents",             0},
   {STAT_FOOD,            11, true,  "wiki_food",                 0},
   {STAT_AMMO,            11, true,  "wiki_ammo",                 0},
   {STAT_ARMOR,           11, true,  "wiki_armor",                0},
   {STAT_MISCITEMS,       11, true,  "wiki_miscitems",            0},
   {STAT_RINGS,           11, true,  "wiki_rings",                0},
   {STAT_RODS,            11, true,  "wiki_rods",                 0},
   {STAT_POTIONS,         11, true,  "wiki_potions",              0},
   {STAT_SCROLLS,         11, true,  "wiki_scrolls",              0},
   {STAT_WANDS,           11, true,  "wiki_wands",                0},
   {STAT_QUESTITEMS,      11, true,  "wiki_questitems",           0},
   {STAT_SKILLS,          12, true,  "wiki_skills",               0},
   {STAT_NECKLACE,        11, true,  "wiki_necklace",             0},
   {STAT_INSTRUMENTS,     11, true,  "wiki_instruments",          0},
   {STAT_GEMS,            11, true,  "wiki_gems",                 0},
   {STAT_OFFERINGS,       11, true,  "wiki_offerings",            0},
   {STAT_QUESTS,          11, true,  "wiki_quests",               0},
   {STAT_TREASURE_EXTRA,   4, true,  "wiki_treasure_extra",       0},
   {STAT_TREASURE_MAGIC,   3, true,  "wiki_treasure_magic",       0},
   {STAT_NPC_SELLCOND,     4, true,  "wiki_npc_sellcond",         0},
   {STAT_LOGPEN,           7, true,  "player_logpen",             0},
   {STAT_LOGPEN_ITEM,      3, true,  "player_logpen_items",       0},
   {STAT_QUESTGIVER,       2, true,  "wiki_quest_giver",          0},
   {STAT_ACCOUNTS,         8, true,  "server_accounts",           0},
   {STAT_ACCOUNT_CHARS,    2, true,  "server_account_chars",      0},
   {STAT_COMPL_QUESTS,     5, true,  "player_completed_quests",   0},
   {STAT_COMPL_QUEST_ITEMS,4, true,  "player_quest_reward_items", 0}
};

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

static void PgWriteNode(sql_queue_node *node)
{
   sql_data_node *data = (sql_data_node *)node->data;
   int type = node->type;
   int nf = node->num_fields;

   if (type <= STAT_NONE || type > STAT_MAXSTAT)
      return;

   if (!Statistics_Table[type].enabled)
      return;

   const char *table = Statistics_Table[type].table_name;
   if (!table)
      return;

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
   bool customSql = true;

   // These records omit generated/default columns or mirror UPDATE/UPSERT
   // procedures in database.c, so they cannot use a positional table insert.
   switch (type)
   {
   case STAT_PLAYER:
      snprintf(sql, sizeof(sql),
         "INSERT INTO player (player_account_id, player_name, player_home, player_bind, player_guild, "
         "player_max_health, player_max_mana, player_might, player_int, player_myst, player_stam, player_agil, player_aim) "
         "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13) "
         "ON CONFLICT (player_name) DO UPDATE SET "
         "player_account_id = EXCLUDED.player_account_id, player_home = EXCLUDED.player_home, "
         "player_bind = EXCLUDED.player_bind, player_guild = EXCLUDED.player_guild, "
         "player_max_health = EXCLUDED.player_max_health, player_max_mana = EXCLUDED.player_max_mana, "
         "player_might = EXCLUDED.player_might, player_int = EXCLUDED.player_int, "
         "player_myst = EXCLUDED.player_myst, player_stam = EXCLUDED.player_stam, "
         "player_agil = EXCLUDED.player_agil, player_aim = EXCLUDED.player_aim");
      break;

   case STAT_PLAYERSUICIDE:
      snprintf(sql, sizeof(sql),
         "UPDATE player SET player_suicide = 1, player_suicide_time = NOW() "
         "WHERE player_account_id = $1 AND player_name = $2");
      break;

   case STAT_GUILD:
      snprintf(sql, sizeof(sql),
         "INSERT INTO guild (guild_name, guild_leader, guild_hall, guild_rent_paid) "
         "VALUES ($1, $2, $3, $4) "
         "ON CONFLICT (guild_name) DO UPDATE SET "
         "guild_leader = EXCLUDED.guild_leader, guild_hall = EXCLUDED.guild_hall, "
         "guild_rent_paid = EXCLUDED.guild_rent_paid");
      break;

   case STAT_GUILDDISBAND:
      snprintf(sql, sizeof(sql),
         "UPDATE guild SET guild_hall = '', guild_disbanded = 1, guild_disbanded_time = NOW() "
         "WHERE guild_name = $1");
      break;

   case STAT_LOGPEN:
      snprintf(sql, sizeof(sql),
         "INSERT INTO player_logpen (player_name, logpen_time, room_id, logpen_xp, logpen_hp, "
         "logpen_spellpct, logpen_skillpct, logpen_numitems) "
         "VALUES ($1, NOW(), $2, $3, $4, $5, $6, $7)");
      break;

   case STAT_LOGPEN_ITEM:
      snprintf(sql, sizeof(sql),
         "INSERT INTO player_logpen_items (player_name, logpen_time, item_name, item_number) "
         "VALUES ($1, NOW(), $2, $3)");
      break;

   case STAT_ACCOUNT_CHARS:
      snprintf(sql, sizeof(sql),
         "INSERT INTO server_account_chars (acct_id, char_name) VALUES ($1, $2)");
      break;

   case STAT_COMPL_QUESTS:
      snprintf(sql, sizeof(sql),
         "INSERT INTO player_completed_quests "
         "(quest_id, player_name, completion_time, num_seconds_taken, xp_rewarded, tp_rewarded) "
         "VALUES ($1, $2, NOW(), $3, $4, $5)");
      break;

   case STAT_COMPL_QUEST_ITEMS:
      snprintf(sql, sizeof(sql),
         "INSERT INTO player_quest_reward_items "
         "(quest_id, player_name, completion_time, item_name, item_number) "
         "VALUES ($1, $2, NOW(), $3, $4)");
      break;

   default:
      customSql = false;
      break;
   }

   if (!customSql)
   {
      // Generic tables whose parameter order matches their physical columns.
      // Auto-columns are supported only at the beginning or end of the row.
      int ts_pos = Statistics_Table[type].timestamp_pos;
      bool has_serial = Statistics_Table[type].has_serial;
      char placeholders[1024];
      placeholders[0] = 0;

      if (has_serial)
         strcat(placeholders, "DEFAULT,");

      if (ts_pos == 1)
         strcat(placeholders, "NOW(),");

      for (int i = 0; i < nParams; i++)
      {
         char ph[8];
         snprintf(ph, sizeof(ph), "%s$%d", i > 0 ? "," : "", i + 1);
         strcat(placeholders, ph);
      }

      if (ts_pos == -1)
         strcat(placeholders, ",NOW()");

      snprintf(sql, sizeof(sql), "INSERT INTO %s VALUES (%s) ON CONFLICT DO NOTHING", table, placeholders);
   }

   PGresult *res = PQexecParams(pg_conn, sql, nParams, NULL,
      paramValues, NULL, NULL, 0);

   if (PQresultStatus(res) != PGRES_COMMAND_OK)
   {
      // Don't spam errors - just log once
      static int error_count = 0;
      if (error_count++ < 10)
         eprintf("PostgreSQL INSERT into %s failed: %s\n", table, PQerrorMessage(pg_conn));
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
