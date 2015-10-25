// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * timer.h
 *
 */

#ifndef _TIMER_H
#define _TIMER_H

#define INIT_TIMER_NODES (2000)

typedef struct timer_struct
{
   UINT64 time;
   __int64 timer_id;
   __int64 object_id;
   __int64 message_id;
   __int64 garbage_ref;
   int heap_index;
} timer_node;

bool TimerHeapCheck(int i, int level);
void InitTimer(void);
void ResetTimer(void);
void ClearTimer(void);
void PauseTimers(void);
void UnpauseTimers(void);
__int64 CreateTimer(__int64 object_id, __int64 message_id, int milliseconds);
Bool LoadTimer(__int64 timer_id, __int64 object_id, char *message_name, int milliseconds);
Bool DeleteTimer(__int64 timer_id);
void ServiceTimers(void);
void QuitTimerLoop(void);
timer_node * GetTimerByID(__int64 timer_id);
void ForEachTimer(void (*callback_func)(timer_node *t));
void SetNumTimers(__int64 new_next_timer_num);
Bool InMainLoop(void);
__int64  GetNumActiveTimers(void);

#endif
