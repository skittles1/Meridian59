// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * intrlock.c
 *
 
 This module maintains the locks for thread synchronization between
 the interface thread and the main thread.  There is a lock to do
 anything with the server and one to do quitting.

 */

#include "blakserv.h"


Mutex muxServer;

CRITICAL_SECTION csQuit;
Bool quit;

void InitInterfaceLocks()
{
   muxServer = MutexCreate();

   quit = False;
   InitializeCriticalSection(&csQuit);
   
}

void EnterServerLock()
{
   MutexAcquire(muxServer);
}

Bool TryEnterServerLock()
{
   return MutexAcquireWithTimeout(muxServer, 50);
}

void LeaveServerLock()
{
   MutexRelease(muxServer);
}

void SetQuit()
{
   EnterCriticalSection(&csQuit);   
   quit = True;
   MessagePost(main_thread_id,WM_QUIT,0,0);
   LeaveCriticalSection(&csQuit);
}

Bool GetQuit()
{
   Bool copy_quit;

   EnterCriticalSection(&csQuit);   
   copy_quit = quit;
   LeaveCriticalSection(&csQuit);

   return copy_quit;
}

void SignalSession(__int64 session_id)
{
   MessagePost(main_thread_id,WM_BLAK_MAIN_READ,0,(int)session_id);
}
