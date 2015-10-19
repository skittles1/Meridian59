// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.

#include "blakserv.h"

Mutex MutexCreate() {
   Mutex mutex = new MutexWrapper();
   pthread_mutex_init(&mutex->mutex, NULL);

   // TODO: function should return something?
}

bool MutexAcquire(Mutex mutex) {
   return pthread_mutex_lock(&mutex->mutex) == 0;
}

bool MutexAcquireWithTimeout(Mutex mutex, int ms) {

   timespec ts;

   clock_gettime(CLOCK_REALTIME, &ts);
   ts.tv_sec += (ms / 1000L);
   ts.tv_nsec += ( ms - (( ms  / 1000L ) * 1000L )) * 1000000L;

   return pthread_mutex_timedlock(&mutex->mutex, &ts);
}

bool MutexRelease(Mutex mutex) {
   return pthread_mutex_unlock(&mutex->mutex) == 0;
}

bool MutexClose(Mutex mutex) {
   // TODO: delete() &mutex->mutex?
   return pthread_mutex_destroy(&mutex->mutex) == 0;
}
