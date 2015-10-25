// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * nameid.h
 *
 */

#ifndef _NAMEID_H
#define _NAMEID_H

#define INIT_NAMEID_NODES 32768

typedef struct nameid_struct
{
   char *name;
   __int64 id;
   struct nameid_struct *next;
} nameid_node;

void InitNameID(void);
void ResetNameID(void);
void CreateNameID(char *parm_name, __int64 parm_id);
nameid_node * GetNameIDNode(__int64 id);
__int64 GetIDByName(const char *name);
char* GetNameByID(__int64 id);
int GetMaxNameID(void);
void ForEachNameID(void(*callback_func)(nameid_node *n));

#endif
