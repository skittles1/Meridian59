#ifndef DATABASE_COMMON_H
#define DATABASE_COMMON_H

#include <cstring>
#include <string>

#ifndef BLAK_PLATFORM_WINDOWS
#include <pthread.h>
#endif

#define DATABASE_MAX_RECORD_QUEUE 15000

typedef struct sql_queue_node sql_queue_node;
typedef struct sql_queue sql_queue;
typedef struct sql_data_node sql_data_node;

struct sql_data_node
{
   int type;
   union {
      int num;
      char *str;
   } value;
};

struct sql_queue_node
{
   sql_recordtype type;
   int num_fields;
   void *data;
   sql_queue_node *next;
};

struct sql_queue
{
#ifdef BLAK_PLATFORM_WINDOWS
   HANDLE mutex;
#else
   pthread_mutex_t mutex;
#endif
   int count;
   sql_queue_node *first;
   sql_queue_node *last;
};

static inline bool DatabaseQueueInit(sql_queue *queue)
{
   if (!queue)
      return false;

#ifdef BLAK_PLATFORM_WINDOWS
   queue->mutex = CreateMutex(NULL, FALSE, NULL);
   if (!queue->mutex)
      return false;
#else
   if (pthread_mutex_init(&queue->mutex, NULL) != 0)
      return false;
#endif

   queue->count = 0;
   queue->first = NULL;
   queue->last = NULL;
   return true;
}

static inline void DatabaseQueueShutdown(sql_queue *queue)
{
   if (!queue)
      return;

#ifdef BLAK_PLATFORM_WINDOWS
   if (queue->mutex)
   {
      CloseHandle(queue->mutex);
      queue->mutex = NULL;
   }
#else
   pthread_mutex_destroy(&queue->mutex);
#endif
}

static inline bool DatabaseQueueLock(sql_queue *queue)
{
#ifdef BLAK_PLATFORM_WINDOWS
   DWORD result = WaitForSingleObject(queue->mutex, INFINITE);
   return result == WAIT_OBJECT_0 || result == WAIT_ABANDONED_0;
#else
   return pthread_mutex_lock(&queue->mutex) == 0;
#endif
}

static inline void DatabaseQueueUnlock(sql_queue *queue)
{
#ifdef BLAK_PLATFORM_WINDOWS
   ReleaseMutex(queue->mutex);
#else
   pthread_mutex_unlock(&queue->mutex);
#endif
}

static inline bool DatabaseQueueEnqueue(sql_queue *queue,
   sql_queue_node *node, int max_items)
{
   if (!queue || !node || !DatabaseQueueLock(queue))
      return false;

   if (queue->count >= max_items)
   {
      DatabaseQueueUnlock(queue);
      return false;
   }

   node->next = NULL;
   if (queue->last)
      queue->last->next = node;
   else
      queue->first = node;
   queue->last = node;
   queue->count++;

   DatabaseQueueUnlock(queue);
   return true;
}

static inline sql_queue_node *DatabaseQueueDequeue(sql_queue *queue)
{
   if (!queue || !DatabaseQueueLock(queue))
      return NULL;

   sql_queue_node *node = queue->first;
   if (node)
   {
      queue->first = node->next;
      if (!queue->first)
         queue->last = NULL;
      queue->count--;
      node->next = NULL;
   }

   DatabaseQueueUnlock(queue);
   return node;
}

static inline bool DatabaseGetColumn(const char *columns, int column_index,
   std::string &column)
{
   if (!columns || column_index < 0)
      return false;

   const char *start = columns;
   for (int i = 0; i < column_index; ++i)
   {
      start = std::strchr(start, ',');
      if (!start)
         return false;
      ++start;
   }

   const char *end = std::strchr(start, ',');
   size_t column_length = end ? (size_t)(end - start) : std::strlen(start);
   if (column_length == 0)
      return false;

   column.assign(start, column_length);
   return true;
}

static inline void DatabaseAppendSqlItem(std::string &sql,
   const std::string &item, bool &first_item, const char *separator = ", ")
{
   if (!first_item)
      sql.append(separator);
   sql.append(item);
   first_item = false;
}

#endif