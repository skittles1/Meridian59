// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * system.h
 *
 */

#ifndef _SYSTEM_H
#define _SYSTEM_H

void CreateBuiltInObjects();
__int64 GetBuiltInObjectID(__int64 id);
void SetBuiltInObjectID(__int64 ref_id, __int64 obj_id);
void SetBuiltInObjectIDByClass(__int64 ref_id, __int64 obj_id);
__int64 GetSystemObjectID(void);
void SetSystemObjectID(__int64 new_id);
__int64 GetSettingsObjectID();
void SetSettingsObjectID(__int64 new_id);
__int64 GetRealTimeObjectID();
void SetRealTimeObjectID(__int64 new_id);
__int64 GetEventEngineObjectID();
void SetEventEngineObjectID(__int64 new_id);

#endif
