// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * system.c
 *

 This module supports keeping track of the built-in objects, which we
 have to send messages to sometimes (like garbage collection
 notifications for system and things like that). System is not currently
 ever not object 0, but there's no permanent reason for that.

 To add a new built-in object:
   *** Compiler ***
   Step 1: Add class of object in .\blakcomp\blakcomp.h (incrementing MAX_BUILTIN_CLASS)
           and also .\blakcomp\function.c.
   Step 2: Add to the name/ID check in .\blakcomp\kodbase.c (function load_add_class())
           to invalidate older copies of kodbase.txt.
   *** Server ***
   Step 3: Add class to enum in .\blakserv\blakserv.h (matching compiler ones).
           Increment MAX_BUILTIN_CLASS.
   Step 4: Add object constant also in .\blakserv\blakserv.h, incrementing
           MAX_BUILTIN_OBJECTS and NUM_BUILTIN_OBJECTS.
   Step 5: Add in a global variable here to keep track of it, and add the
           appropriate data to existing functions as well as creating new
           set/get functions (headers in system.h).
   Step 6: Add the new built-in to SaveBuiltInObjects() in savegame.c.
   *** Blakod ***
   Step 8: Add the object constant to blakston.khd, and remove any object
           references to the built-in class.
   Step 9: If required, add a call to Recreate() for any new built-in object
           during System's RecreateAll.

 Note that if the class already existed, adding it as a built-in will break
 old copies of kodbase.txt.
           
 */

#include "blakserv.h"

__int64 system_obj_id;
__int64 settings_obj_id;
__int64 realtime_obj_id;
__int64 eventengine_obj_id;

// Creates all the built-in objects that aren't referenced by other objects
// in blakod, but can be called by constant (e.g. SYSTEM).
void CreateBuiltInObjects()
{
   SetSystemObjectID(CreateObject(SYSTEM_CLASS, 0, NULL));
   SetSettingsObjectID(CreateObject(SETTINGS_CLASS, 0, NULL));
   SetRealTimeObjectID(CreateObject(REALTIME_CLASS, 0, NULL));
   SetEventEngineObjectID(CreateObject(EVENTENGINE_CLASS, 0, NULL));
}

__int64 GetBuiltInObjectID(__int64 id)
{
   switch(id)
   {
      case SYSTEM_OBJECT: return system_obj_id;
      case SETTINGS_OBJECT: return settings_obj_id;
      case REALTIME_OBJECT: return realtime_obj_id;
      case EVENTENGINE_OBJECT: return eventengine_obj_id;
   }

   return INVALID_OBJECT;
}

void SetBuiltInObjectID(__int64 ref_id, __int64 obj_id)
{
   if (ref_id == SYSTEM_OBJECT)
      system_obj_id = obj_id;
   else if (ref_id == SETTINGS_OBJECT)
      settings_obj_id = obj_id;
   else if (ref_id == REALTIME_OBJECT)
      realtime_obj_id = obj_id;
   else if (ref_id == EVENTENGINE_OBJECT)
      eventengine_obj_id = obj_id;
}

void SetBuiltInObjectIDByClass(__int64 ref_id, __int64 obj_id)
{
   if (ref_id == SYSTEM_CLASS)
      system_obj_id = obj_id;
   else if (ref_id == SETTINGS_CLASS)
      settings_obj_id = obj_id;
   else if (ref_id == REALTIME_CLASS)
      realtime_obj_id = obj_id;
   else if (ref_id == EVENTENGINE_CLASS)
      eventengine_obj_id = obj_id;
}

__int64 GetSystemObjectID()
{
   return system_obj_id;
}

void SetSystemObjectID(__int64 new_id)
{
   system_obj_id = new_id;
}

__int64 GetSettingsObjectID()
{
   return settings_obj_id;
}

void SetSettingsObjectID(__int64 new_id)
{
   settings_obj_id = new_id;
}

__int64 GetRealTimeObjectID()
{
   return realtime_obj_id;
}

void SetRealTimeObjectID(__int64 new_id)
{
   realtime_obj_id = new_id;
}

__int64 GetEventEngineObjectID()
{
   return eventengine_obj_id;
}

void SetEventEngineObjectID(__int64 new_id)
{
   eventengine_obj_id = new_id;
}
