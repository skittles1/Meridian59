// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * list.h
 *
 */

#ifndef _LIST_H
#define _LIST_H

#define INIT_LIST_NODES (4000000)

typedef struct
{
   val_type first;
   val_type rest;
   __int64 garbage_ref;
} list_node;

void InitList(void);
void ResetList(void);
void ClearList(void);
int GetListNodesUsed(void);
Bool LoadList(__int64 list_id, val_type first, val_type rest);
list_node * GetListNodeByID(__int64 list_id);
Bool IsListNodeByID(__int64 list_id);
__int64 First(__int64 list_id);
__int64 Rest(__int64 list_id);
__int64 AppendListElem(val_type source, val_type list_val);
__int64 Cons(val_type source, val_type dest);
__int64 Length(__int64 list_id);
__int64 Nth(__int64 n, __int64 list_id);
__int64 IsListMatch(__int64 list_one_id, __int64 list_two_id);
__int64 Last(__int64 list_id);
__int64 SetFirst(__int64 list_id, val_type new_val);
__int64 SetNth(__int64 n, __int64 list_id, val_type new_val);
__int64 SwapListElem(__int64 list_id, __int64 elem_one, __int64 elem_two);
__int64 FindListElem(val_type list_id, val_type list_elem);
__int64 GetAllListNodesByClass(__int64 list_id, __int64 position, __int64 class_id);
__int64 GetListNode(val_type list_id, __int64 position, val_type list_elem);
__int64 GetListElemByClass(val_type list_id, __int64 class_id);
__int64 ListCopy(__int64 list_id);
__int64 InsertListElem(__int64 n, __int64 list_id, val_type new_val);
__int64 DelListElem(val_type list_id, val_type list_elem);

// List sending messages.
__int64 SendListMessage(__int64 list_id, bool ret_false, __int64 message_id,
   int num_parms, parm_node parms[]);
__int64 SendFirstListMessage(__int64 list_id, bool ret_false, __int64 message_id,
   int num_parms, parm_node parms[]);
__int64 SendNthListMessage(__int64 list_id, __int64 position, bool ret_false,
   __int64 message_id, int num_parms, parm_node parms[]);
__int64 SendListMessageByClass(__int64 list_id, __int64 class_id, bool ret_false,
   __int64 message_id, int num_parms, parm_node parms[]);
__int64 SendFirstListMessageByClass(__int64 list_id, __int64 class_id, bool ret_false,
   __int64 message_id, int num_parms, parm_node parms[]);
__int64 SendNthListMessageByClass(__int64 list_id, __int64 position, __int64 class_id,
   bool ret_false, __int64 message_id, int num_parms, parm_node parms[]);

void ForEachListNode(void (*callback_func)(list_node *l,__int64 list_id));
void MoveListNode(__int64 dest_id, __int64 source_id);
void SetNumListNodes(int new_num_nodes);



#endif
