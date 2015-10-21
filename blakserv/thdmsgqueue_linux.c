// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
 
#include "blakserv.h"

MSGQUEUE Msgs;

BOOL InitMsgQueue()
{
    Msgs.head = NULL;
    Msgs.tail = NULL;
    Msgs.count = 0;
    pthread_mutex_init(&Msgs.mux, NULL);
    pthread_cond_init(&Msgs.msgEvent, NULL);
}
               
bool WaitForAnyMessageWithTimeout(INT64 ms)
{
   // TODO: incomplete, should at least handle multiple thread ids

   timespec ts;
   int retVal = 1;

   clock_gettime(CLOCK_REALTIME, &ts);

   ts.tv_sec += (ms / 1000L);
   ts.tv_nsec += ( ms - (( ms  / 1000L ) * 1000L )) * 1000000L;

   // check for tv_nsec overflow (no more than 1,000,000,000)
   if (ts.tv_nsec > 1000000000)
   {
      ts.tv_nsec -= 1000000000;
      ts.tv_sec += 1;
   }

   pthread_mutex_lock(&Msgs.mux);

   // wait for "message waiting" is signaled
   retVal = pthread_cond_timedwait(&Msgs.msgEvent, &Msgs.mux, &ts);

   // cond_wait will unlock the mutex while waiting then lock it when it returns, we must unlock it now
   pthread_mutex_unlock(&Msgs.mux);

   if (retVal == 0)
      return true;

   return false;
}

bool MessagePost(DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    // TODO: Review
    MsgNode* temp;

    if (idThread != main_thread_id)
    {
        eprintf("PostThreadMessage: only main thread messages currently supported on Linux!");
        return FALSE;
    }

    pthread_mutex_lock(&Msgs.mux);


    if (Msgs.head == NULL)
    {
        Msgs.head = (MsgNode*)malloc(sizeof(MsgNode));
        Msgs.head->next = NULL;
        Msgs.head->prev = NULL;
        Msgs.tail = Msgs.head;
    }
    else
    {
        Msgs.tail->next = (MsgNode*)malloc(sizeof(MsgNode));
        Msgs.tail->next->prev = Msgs.tail;
        Msgs.tail->next->next = NULL;
        Msgs.tail = Msgs.tail->next;
    }

    Msgs.tail->msg.hwnd = (HWND)idThread;
    Msgs.tail->msg.message = Msg;
    Msgs.tail->msg.lParam = lParam;
    Msgs.tail->msg.wParam = wParam;

    Msgs.count++;
    pthread_cond_signal(&Msgs.msgEvent);
    pthread_mutex_unlock(&Msgs.mux);

    return TRUE;
}

bool MessagePeek(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT  wMsgFilterMax, UINT wRemoveMsg)
{
   // TODO: Finish
   // TODO: handle wRemoveMsg

   lprintf("MARK MessagePeek\n"); FlushDefaultChannels(); // TODO: Remove Me

   MsgNode* current;

   pthread_mutex_lock(&Msgs.mux);

   if (Msgs.head == NULL)
   {
     return FALSE;
   }

   current = Msgs.head;

   while(current != NULL &&
            !((current->msg.hwnd == hWnd) &&
               (current->msg.message >= wMsgFilterMin &&
                   current->msg.message <= wMsgFilterMax)))
    {
        current = current->next;
    }

    if (current == NULL)
    {
       return FALSE;
    }

    lpMsg = (MSG*)malloc(sizeof(MSG));
    lpMsg->hwnd = current->msg.hwnd;
    lpMsg->message = current->msg.message;
    lpMsg->lParam = current->msg.lParam;
    lpMsg->wParam = current->msg.wParam;
    pthread_mutex_unlock(&Msgs.mux);

    return TRUE;
}

