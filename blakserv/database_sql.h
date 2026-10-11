#ifndef DATABASE_SQL_H
#define DATABASE_SQL_H

#include <cstring>
#include <string>

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
