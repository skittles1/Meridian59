// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * object.h
 *
 */

#ifndef _OBJECT_H
#define _OBJECT_H

#define INIT_OBJECTS 1000000

typedef struct
{
   __int64 id;
   val_type val;
} prop_type;

typedef struct
{
   __int64 object_id;
   __int64 class_id;
   Bool deleted;
   __int64 garbage_ref;
   int num_props; /* used by garbage collect */
   prop_type *p;
} object_node;

void InitObject(void);
void ResetObject(void);
void ClearObject(void);
int GetObjectsUsed(void);
__int64 CreateObject(__int64 class_id, int num_parms, parm_node parms[]);
Bool LoadObject(__int64 object_id, char *class_name);
void DeleteBlakodObject(__int64 object_id);
object_node * GetObjectByID(__int64 object_id);
object_node * GetObjectByIDQuietly(__int64 object_id);
Bool IsObjectByID(__int64 object_id);
object_node * GetObjectByIDEvenDeleted(__int64 object_id);
Bool SetObjectPropertyByName(__int64 object_id, char *prop_name, val_type val);

void ForEachObject(void (*callback_func)(object_node *o));
void MoveObject(__int64 dest_id, __int64 source_id);
void SetNumObjects(int new_num_objects);

#endif
