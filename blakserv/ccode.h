// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * ccode.h
 *
 */

#ifndef _CCODE_H
#define _CCODE_H

__int64 C_Invalid(__int64 object_id,local_var_type *local_vars,
	      int num_normal_parms,parm_node normal_parm_array[],
	      int num_name_parms,parm_node name_parm_array[]);

__int64 C_SaveGame(__int64 object_id,local_var_type *local_vars,
      int num_normal_parms,parm_node normal_parm_array[],
      int num_name_parms,parm_node name_parm_array[]);

__int64 C_LoadGame(__int64 object_id, local_var_type *local_vars,
      int num_normal_parms, parm_node normal_parm_array[],
      int num_name_parms, parm_node name_parm_array[]);

__int64 C_AddPacket(__int64 object_id,local_var_type *local_vars,
		int num_normal_parms,parm_node normal_parm_array[],
		int num_name_parms,parm_node name_parm_array[]);

__int64 C_SendPacket(__int64 object_id,local_var_type *local_vars,
		 int num_normal_parms,parm_node normal_parm_array[],
		 int num_name_parms,parm_node name_parm_array[]);

__int64 C_SendCopyPacket(__int64 object_id,local_var_type *local_vars,
		     int num_normal_parms,parm_node normal_parm_array[],
		     int num_name_parms,parm_node name_parm_array[]);

__int64 C_ClearPacket(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_GodLog(__int64 object_id, local_var_type *local_vars,
			int num_normal_parms,parm_node normal_parm_array[],
			int num_name_parms,parm_node name_parm_array[]);

__int64 C_Debug(__int64 object_id,local_var_type *local_vars,
	    int num_normal_parms,parm_node normal_parm_array[],
	    int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetInactiveTime(__int64 object_id,local_var_type *local_vars,
		      int num_normal_parms,parm_node normal_parm_array[],
		      int num_name_parms,parm_node name_parm_array[]);

__int64 C_DumpStack(__int64 object_id,local_var_type *local_vars,
		      int num_normal_parms,parm_node normal_parm_array[],
		      int num_name_parms,parm_node name_parm_array[]);

__int64 C_SendMessage(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_PostMessage(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_SendListMessage(__int64 object_id,local_var_type *local_vars,
            int num_normal_parms,parm_node normal_parm_array[],
            int num_name_parms,parm_node name_parm_array[]);

__int64 C_SendListMessageBreak(__int64 object_id, local_var_type *local_vars,
            int num_normal_parms, parm_node normal_parm_array[],
            int num_name_parms, parm_node name_parm_array[]);

__int64 C_SendListMessageByClass(__int64 object_id, local_var_type *local_vars,
            int num_normal_parms, parm_node normal_parm_array[],
            int num_name_parms, parm_node name_parm_array[]);

__int64 C_SendListMessageByClassBreak(__int64 object_id, local_var_type *local_vars,
            int num_normal_parms, parm_node normal_parm_array[],
            int num_name_parms, parm_node name_parm_array[]);

__int64 C_CreateObject(__int64 object_id,local_var_type *local_vars,
		   int num_normal_parms,parm_node normal_parm_array[],
		   int num_name_parms,parm_node name_parm_array[]);

__int64 C_IsClass(__int64 object_id,local_var_type *local_vars,
	      int num_normal_parms,parm_node normal_parm_array[],
	      int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetClass(__int64 object_id,local_var_type *local_vars,
	       int num_normal_parms,parm_node normal_parm_array[],
	       int num_name_parms,parm_node name_parm_array[]);

__int64 C_StringEqual(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_StringSubstitute(__int64 object_id,local_var_type *local_vars,
		    int num_normal_parms,parm_node normal_parm_array[],
		    int num_name_parms,parm_node name_parm_array[]);

__int64 C_StringContain(__int64 object_id,local_var_type *local_vars,
		    int num_normal_parms,parm_node normal_parm_array[],
		    int num_name_parms,parm_node name_parm_array[]);

__int64 C_SetResource(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_ParseString(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_SetString(__int64 object_id,local_var_type *local_vars,
		int num_normal_parms,parm_node normal_parm_array[],
		int num_name_parms,parm_node name_parm_array[]);

__int64 C_AppendTempString(__int64 object_id,local_var_type *local_vars,
		int num_normal_parms,parm_node normal_parm_array[],
		int num_name_parms,parm_node name_parm_array[]);

__int64 C_ClearTempString(__int64 object_id,local_var_type *local_vars,
		int num_normal_parms,parm_node normal_parm_array[],
		int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetTempString(__int64 object_id,local_var_type *local_vars,
		int num_normal_parms,parm_node normal_parm_array[],
		int num_name_parms,parm_node name_parm_array[]);

__int64 C_CreateString(__int64 object_id,local_var_type *local_vars,
		   int num_normal_parms,parm_node normal_parm_array[],
		   int num_name_parms,parm_node name_parm_array[]);

__int64 C_IsString(__int64 object_id,local_var_type *local_vars,
         int num_normal_parms,parm_node normal_parm_array[],
         int num_name_parms,parm_node name_parm_array[]);

__int64 C_StringLength(__int64 object_id,local_var_type *local_vars,
		   int num_normal_parms,parm_node normal_parm_array[],
		   int num_name_parms,parm_node name_parm_array[]);

__int64 C_StringConsistsOf(__int64 object_id,local_var_type *local_vars,
                       int num_normal_parms,parm_node normal_parm_array[],
                       int num_name_parms,parm_node name_parm_array[]);

__int64 C_CreateTimer(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_DeleteTimer(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetTimeRemaining(__int64 object_id,local_var_type *local_vars,
		       int num_normal_parms,parm_node normal_parm_array[],
		       int num_name_parms,parm_node name_parm_array[]);

__int64 C_IsTimer(__int64 object_id,local_var_type *local_vars,
		 int num_normal_parms,parm_node normal_parm_array[],
		 int num_name_parms,parm_node name_parm_array[]);

__int64 C_LoadRoom(__int64 object_id,local_var_type *local_vars,
	       int num_normal_parms,parm_node normal_parm_array[],
	       int num_name_parms,parm_node name_parm_array[]);

__int64 C_FreeRoom(__int64 object_id,local_var_type *local_vars,
            int num_normal_parms,parm_node normal_parm_array[],
            int num_name_parms,parm_node name_parm_array[]);

__int64 C_RoomData(__int64 object_id,local_var_type *local_vars,
	       int num_normal_parms,parm_node normal_parm_array[],
	       int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetLocationInfoBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_CanMoveInRoomBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_LineOfSightBSP(__int64 object_id, local_var_type *local_vars,
		int num_normal_parms, parm_node normal_parm_array[],
		int num_name_parms, parm_node name_parm_array[]);

__int64 C_LineOfSightView(__int64 object_id, local_var_type *local_vars,
      int num_normal_parms, parm_node normal_parm_array[],
      int num_name_parms, parm_node name_parm_array[]);

__int64 C_ChangeTextureBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_MoveSectorBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_BlockerAddBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_BlockerMoveBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_BlockerRemoveBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_BlockerClearBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_GetRandomPointBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_GetStepTowardsBSP(__int64 object_id, local_var_type *local_vars,
	int num_normal_parms, parm_node normal_parm_array[],
	int num_name_parms, parm_node name_parm_array[]);

__int64 C_AppendListElem(__int64 object_id,local_var_type *local_vars,
         int num_normal_parms,parm_node normal_parm_array[],
         int num_name_parms,parm_node name_parm_array[]);

__int64 C_Cons(__int64 object_id,local_var_type *local_vars,
	   int num_normal_parms,parm_node normal_parm_array[],
	   int num_name_parms,parm_node name_parm_array[]);

__int64 C_First(__int64 object_id,local_var_type *local_vars,
	    int num_normal_parms,parm_node normal_parm_array[],
	    int num_name_parms,parm_node name_parm_array[]);

__int64 C_Rest(__int64 object_id,local_var_type *local_vars,
	   int num_normal_parms,parm_node normal_parm_array[],
	   int num_name_parms,parm_node name_parm_array[]);

__int64 C_Length(__int64 object_id,local_var_type *local_vars,
	     int num_normal_parms,parm_node normal_parm_array[],
	     int num_name_parms,parm_node name_parm_array[]);

__int64 C_Last(__int64 object_id,local_var_type *local_vars,
         int num_normal_parms,parm_node normal_parm_array[],
         int num_name_parms,parm_node name_parm_array[]);

__int64 C_Nth(__int64 object_id,local_var_type *local_vars,
	  int num_normal_parms,parm_node normal_parm_array[],
	  int num_name_parms,parm_node name_parm_array[]);

__int64 C_IsListMatch(__int64 object_id,local_var_type *local_vars,
		int num_normal_parms,parm_node normal_parm_array[],
		int num_name_parms,parm_node name_parm_array[]);

__int64 C_List(__int64 object_id,local_var_type *local_vars,
	   int num_normal_parms,parm_node normal_parm_array[],
	   int num_name_parms,parm_node name_parm_array[]);

__int64 C_IsList(__int64 object_id,local_var_type *local_vars,
	     int num_normal_parms,parm_node normal_parm_array[],
	     int num_name_parms,parm_node name_parm_array[]);

__int64 C_SetFirst(__int64 object_id,local_var_type *local_vars,
	       int num_normal_parms,parm_node normal_parm_array[],
	       int num_name_parms,parm_node name_parm_array[]);

__int64 C_SwapListElem(__int64 object_id,local_var_type *local_vars,
         int num_normal_parms,parm_node normal_parm_array[],
         int num_name_parms,parm_node name_parm_array[]);

__int64 C_SetNth(__int64 object_id,local_var_type *local_vars,
	     int num_normal_parms,parm_node normal_parm_array[],
	     int num_name_parms,parm_node name_parm_array[]);

__int64 C_FindListElem(__int64 object_id,local_var_type *local_vars,
		   int num_normal_parms,parm_node normal_parm_array[],
		   int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetAllListNodesByClass(__int64 object_id,local_var_type *local_vars,
         int num_normal_parms,parm_node normal_parm_array[],
         int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetListNode(__int64 object_id,local_var_type *local_vars,
         int num_normal_parms,parm_node normal_parm_array[],
         int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetListElemByClass(__int64 object_id,local_var_type *local_vars,
         int num_normal_parms,parm_node normal_parm_array[],
         int num_name_parms,parm_node name_parm_array[]);

__int64 C_ListCopy(__int64 object_id, local_var_type *local_vars,
         int num_normal_parms, parm_node normal_parm_array[],
         int num_name_parms, parm_node name_parm_array[]);

__int64 C_InsertListElem(__int64 object_id,local_var_type *local_vars,
         int num_normal_parms,parm_node normal_parm_array[],
         int num_name_parms,parm_node name_parm_array[]);

__int64 C_DelListElem(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetTimeZoneOffset(__int64 object_id, local_var_type *local_vars,
         int num_normal_parms, parm_node normal_parm_array[],
         int num_name_parms, parm_node name_parm_array[]);

__int64 C_GetTime(__int64 object_id,local_var_type *local_vars,
	      int num_normal_parms,parm_node normal_parm_array[],
	      int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetTickCount(__int64 object_id,local_var_type *local_vars,
	      int num_normal_parms,parm_node normal_parm_array[],
	      int num_name_parms,parm_node name_parm_array[]);

__int64 C_Random(__int64 object_id,local_var_type *local_vars,
	     int num_normal_parms,parm_node normal_parm_array[],
	     int num_name_parms,parm_node name_parm_array[]);

__int64 C_Abs(__int64 object_id,local_var_type *local_vars,
	  int num_normal_parms,parm_node normal_parm_array[],
	  int num_name_parms,parm_node name_parm_array[]);

__int64 C_Sqrt(__int64 object_id,local_var_type *local_vars,
	   int num_normal_parms,parm_node normal_parm_array[],
	   int num_name_parms,parm_node name_parm_array[]);

__int64 C_Bound(__int64 object_id,local_var_type *local_vars,
	    int num_normal_parms,parm_node normal_parm_array[],
	    int num_name_parms,parm_node name_parm_array[]);

__int64 C_CreateTable(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_AddTableEntry(__int64 object_id,local_var_type *local_vars,
		       int num_normal_parms,parm_node normal_parm_array[],
		       int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetTableEntry(__int64 object_id,local_var_type *local_vars,
		    int num_normal_parms,parm_node normal_parm_array[],
		    int num_name_parms,parm_node name_parm_array[]);

__int64 C_DeleteTableEntry(__int64 object_id,local_var_type *local_vars,
		       int num_normal_parms,parm_node normal_parm_array[],
		       int num_name_parms,parm_node name_parm_array[]);

__int64 C_DeleteTable(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_IsTable(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_RecycleUser(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_IsObject(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_MinigameNumberToString(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_MinigameStringToNumber(__int64 object_id,local_var_type *local_vars,
		  int num_normal_parms,parm_node normal_parm_array[],
		  int num_name_parms,parm_node name_parm_array[]);

__int64 C_RecordStat(__int64 object_id,local_var_type *local_vars,
				int num_normal_parms,parm_node normal_parm_array[],
				int num_name_parms,parm_node name_parm_array[]);

__int64 C_GetSessionIP(__int64 object_id,local_var_type *local_vars,
				int num_normal_parms,parm_node normal_parm_array[],
				int num_name_parms,parm_node name_parm_array[]);

__int64 C_SetClassVar(__int64 object_id,local_var_type *local_vars,
            int num_normal_parms,parm_node normal_parm_array[],
            int num_name_parms,parm_node name_parm_array[]);

void FuzzyCollapseString(char* pTarget, const char* pSource, int len);
bool FuzzyBufferEqual(const char *s1,int len1,const char *s2,int len2);
bool FuzzyBufferContain(const char *s1,int len1,const char *s2,int len2);

#endif
