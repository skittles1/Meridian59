// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
* database.h
*

 BASIC USAGE:
 --------------------------
  * You must not call any method marked with '_' prefix from outside database.c
  * You should call method MySQLRecorGeneric to record data to DB

 STEPS TO ADD A RECORD:
 --------------------------
   1) Add the table definition to database_schema.h.
   2) Add the record mapping and write behavior to database_procedures.h.
       MySQL procedure DDL and PostgreSQL statements are generated from this mapping.
   3) Add the sql_recordtype constant; it must also be added to blakston.khd.
   4) Build both database backends and verify the write behavior.

  NOTES: Only use the kod types TAG_NIL, TAG_INT, TAG_STRING, TAG_RESOURCE
  and TAG_SESSION (which converts to account ID). If recording stuff from the
  server code directly (i.e. not through RecordStat/kod) you can use int/string/null
  but no other types are supported (but can be added as required).
*/

#ifndef _DATABASE_H
#define _DATABASE_H

#include <mysql.h>

#pragma region Structs/Enums
typedef struct sql_queue_node sql_queue_node;
typedef struct sql_queue sql_queue;
typedef struct sql_data_node sql_data_node;
typedef enum sql_recordtype sql_recordtype;
typedef enum sql_worker_state sql_worker_state;

struct sql_data_node
{
   int type; // kod type
   union {
      int num; // also stores 0 for nil, num for session etc
      char *str; // string, resource, debugstr
   } value;
};

struct sql_queue_node
{
	sql_recordtype type;
   int num_fields;
	void* data;
	sql_queue_node* next;
};

struct sql_queue
{
	HANDLE mutex;
	int count;
	sql_queue_node* first;
	sql_queue_node* last;
};

enum sql_recordtype
{
   STAT_NONE = 0,
   STAT_TOTALMONEY = 1,
   STAT_MONEYCREATED = 2,
   STAT_PLAYERLOGIN = 3,
   STAT_ASSESS_DAM = 4,
   STAT_PLAYERDEATH = 5,
   STAT_PLAYER = 6,
   STAT_PLAYERSUICIDE = 7,
   STAT_GUILD = 8,
   STAT_GUILDDISBAND = 9,
   STAT_SPELLS = 10,
   STAT_SPELL_REAGENT = 11,
   STAT_TREASURE_GEN = 12,
   STAT_MONSTER = 13,
   STAT_MONSTER_ZONE = 14,
   STAT_NPCS = 15,
   STAT_WEAPONS = 16,
   STAT_ROOMS = 17,
   STAT_NPC_ZONE = 18,
   STAT_NPC_SELLITEM = 19,
   STAT_NPC_SELLSKILL = 20,
   STAT_NPC_SELLSPELL = 21,
   STAT_REAGENTS = 22,
   STAT_FOOD = 23,
   STAT_AMMO = 24,
   STAT_ARMOR = 25,
   STAT_MISCITEMS = 26,
   STAT_RINGS = 27,
   STAT_RODS = 28,
   STAT_POTIONS = 29,
   STAT_SCROLLS = 30,
   STAT_WANDS = 31,
   STAT_QUESTITEMS = 32,
   STAT_SKILLS = 33,
   STAT_NECKLACE = 34,
   STAT_INSTRUMENTS = 35,
   STAT_GEMS = 36,
   STAT_OFFERINGS = 37,
   STAT_QUESTS = 38,
   STAT_TREASURE_EXTRA = 39,
   STAT_TREASURE_MAGIC = 40,
   STAT_NPC_SELLCOND = 41,
   STAT_LOGPEN = 42,
   STAT_LOGPEN_ITEM = 43,
   STAT_QUESTGIVER = 44,
   STAT_ACCOUNTS = 45,
   STAT_ACCOUNT_CHARS = 46,
   STAT_COMPL_QUESTS = 47,
   STAT_COMPL_QUEST_ITEMS = 48,
   STAT_MAXSTAT = 48 // Equal to highest defined stat.
};

#include "database_procedures.h"

enum sql_worker_state
{
	STOPPED			= 0,
	STOPPING		= 1,
	STARTING		= 2,
	INITIALIZED		= 3,
	CONNECTED		= 4,
	SCHEMAVERIFIED	= 5
};
#pragma endregion

UINT64 MySQLGetRecordCount();
char *MySQLDuplicateString(char *str);
bool MySQLIsTypeEnabled(int type);
bool MySQLSetTypeEnabled(int type, bool enabled);
void FreeDataNodeMemory(int total_fields, int fields_entered, sql_data_node data[]);
void MySQLInit(char* Host, int Port, char* User, char* Password, char* DB);
void MySQLEnd();
BOOL MySQLRecordGeneric(int type, int num_fields, sql_data_node data[]);
BOOL MySQLEmptyTable(int type);
void __cdecl _MySQLWorker(void* Parameters);
void _MySQLVerifySchema();
BOOL _MySQLEnqueue(sql_queue_node* Node);
BOOL _MySQLDequeue(BOOL processNode);
void _MySQLCallProc(char* Name, MYSQL_BIND Parameters[], BOOL nullParams);
void _MySQLWriteNode(sql_queue_node* Node, BOOL ProcessNode);
void _MySQLTruncate(sql_queue_node* Node, BOOL ProcessNode);
#endif
