// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * message.c
 *
 */

#ifndef _MESSAGE_H
#define _MESSAGE_H

// This causes 5347 message nodes to be allocated for every class (max num
// of messages in an actual class is 464). This is a little excessive but
// allows no collisions between hashes.

#define MESSAGE_TABLE_SIZE 5347
#define GetMessageHashNum(a) (((a + 6000)) % MESSAGE_TABLE_SIZE)

typedef struct message_struct
{
   __int64 message_id;
   __int64 dstr_id;
   __int64 trace_session_id;
   int untimed_call_count;
   int timed_call_count;
   char *handler;
   struct message_struct *propagate_message;
   struct class_struct *propagate_class;
   double total_call_time;
   struct message_struct *next; /* for open hash table linked list */
} message_node;

void InitMessage(void);
void ResetMessage(void);
void SetClassNumMessages(__int64 class_id,int num_messages);
void AddMessage(__int64 class_id, int count, __int64 message_id, char *offset, __int64 dstr_id);
void SetMessagesPropagate(void);
int GetHighestMessageCount(void);
int GetNumMessageHashCollisions(void);

/* two more header functions in class.h */

extern void PageAlert(char *subject,char *message);
#endif
