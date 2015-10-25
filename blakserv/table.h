// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * table.h
 *
 */

#ifndef _TABLE_H
#define _TABLE_H

#define INIT_TABLE_NODES (18000)
#define MIN_TABLE_SIZE (23)
#define MAX_TABLE_SIZE (19463)
#define DEFAULT_TABLE_SIZE (73)

typedef struct hash_struct
{
   val_type key_val;
   val_type data_val;
   struct hash_struct *next;
} hash_node;

typedef struct table_struct
{
   int size;
   int num_entries;
   hash_node **table;
   __int64 garbage_ref;
} table_node;

void InitTables(void);
int GetTablesUsed(void);
void ResetTables(void);
__int64 CreateTable(int size);
table_node * GetTableByID(__int64 table_id);
void DeleteTable(__int64 table_id);
void InsertTable(__int64 table_id, val_type key_val, val_type data_val);
__int64 GetTableEntry(__int64 table_id, val_type key_val);
void DeleteTableEntry(__int64 table_id, val_type key_val);

unsigned int GetBufferHash(const char *buf,unsigned int len_buf);
void ForEachTable(void(*callback_func)(table_node *t, __int64 table_id));
void MoveTable(__int64 dest_id, __int64 source_id);
void SetNumTables(int new_num_tables);

#endif
