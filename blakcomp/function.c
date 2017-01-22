// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/* 
 * function.c
 * "Function prototypes" for built in C-language functions 
 */
#include "blakcomp.h"
#include "bkod.h"

/*
 * The function_type table below defines signatures for all the built-in
 * C functions.  Each argument to one of these functions can be an
 * expression, a class name, a message name, a setting of the form a=b,
 * or a variable number of expressions or settings.  
 * Using this table lets us handle all function calls identically in the 
 * grammar.
 */

/* Use "ANONE" at end of line to indicate end of function header.
 * ASETTINGS and AEXPRESSIONS should only appear at the end of a list; they should
 * only be followed by ANONE.
 */
function_type Functions[] = {
{"Send",                SENDMESSAGE,     STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  ASETTINGS, ANONE},
{"Create",              CREATEOBJECT,    STORE_OPTIONAL, AEXPRESSION,   ASETTINGS,    ANONE},
{"Cons",                CONS,            STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"First",               FIRST,           STORE_REQUIRED, AEXPRESSION,   ANONE},
{"Rest",                REST,            STORE_REQUIRED, AEXPRESSION,   ANONE},
{"Length",              LENGTH,          STORE_REQUIRED, AEXPRESSION,   ANONE},
{"List",                MLIST,           STORE_REQUIRED, AEXPRESSIONS,  ANONE},
{"Nth",                 NTH,             STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"IsListMatch",         ISLISTMATCH,     STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"SetFirst",            SETFIRST,        STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  ANONE},
{"SetNth",              SETNTH,          STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"SwapListElem",        SWAPLISTELEM,    STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"InsertListElem",      INSERTLISTELEM,  STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"DelListElem",         DELLISTELEM,     STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"DelLastListElem",     DELLASTLISTELEM, STORE_REQUIRED, AEXPRESSION,   ANONE },
{"FindListElem",        FINDLISTELEM,    STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"AppendListElem",      APPENDLISTELEM,  STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"GetListElemByClass",  GETLISTELEMBYCLASS, STORE_REQUIRED, AEXPRESSION, AEXPRESSION,  ANONE},
{"GetListNode",         GETLISTNODE,     STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"GetAllListNodesByClass",GETALLLISTNODESBYCLASS, STORE_REQUIRED, AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE},
{"ListCopy",            LISTCOPY,        STORE_REQUIRED, AEXPRESSION,   ANONE},
{"Last",                LAST,            STORE_REQUIRED, AEXPRESSION,   ANONE},
{"SendList",            SENDLISTMSG,     STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION,
                                         ASETTINGS, ANONE},
{"SendListBreak",       SENDLISTMSGBREAK, STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  AEXPRESSION,
                                         ASETTINGS, ANONE},
{"SendListByClass",     SENDLISTMSGBYCLASS, STORE_OPTIONAL, AEXPRESSION, AEXPRESSION, AEXPRESSION,
                                            AEXPRESSION, ASETTINGS, ANONE},
{"SendListByClassBreak",SENDLISTMSGBYCLASSBREAK, STORE_REQUIRED, AEXPRESSION, AEXPRESSION, AEXPRESSION,
                                            AEXPRESSION, ASETTINGS, ANONE},
{"Random",              RANDOM,          STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"SaveGame",            SAVEGAME,        STORE_REQUIRED, ANONE},
{"LoadGame",            LOADGAME,        STORE_OPTIONAL, AEXPRESSION,   ANONE},
{"AddPacket",           ADDPACKET,       STORE_OPTIONAL, AEXPRESSIONS,  ANONE},
{"SendPacket",          SENDPACKET,      STORE_OPTIONAL, AEXPRESSION,   ANONE},
{"SendCopyPacket",      SENDCOPYPACKET,  STORE_OPTIONAL, AEXPRESSION,   ANONE},
{"ClearPacket",         CLEARPACKET,     STORE_OPTIONAL, ANONE},
{"GodLog",              GODLOG,          STORE_OPTIONAL, AEXPRESSIONS,  ANONE},
{"Debug",               DEBUG,           STORE_OPTIONAL, AEXPRESSIONS,  ANONE},
{"GetInactiveTime",     GETINACTIVETIME, STORE_REQUIRED, AEXPRESSION,   ANONE},
{"DumpStack",           DUMPSTACK,       STORE_OPTIONAL, ANONE},
{"StringEqual",         STRINGEQUAL,     STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"StringContain",       STRINGCONTAIN,   STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"StringSubstitute",    STRINGSUBSTITUTE, STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"StringLength",        STRINGLENGTH,    STORE_REQUIRED, AEXPRESSION,   ANONE},
{"StringConsistsOf",    STRINGCONSISTSOF, STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  ANONE},
{"CreateTimer",         CREATETIMER,     STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"DeleteTimer",         DELETETIMER,     STORE_OPTIONAL, AEXPRESSION,   ANONE},
{"IsTimer",             ISTIMER,         STORE_REQUIRED, AEXPRESSION,   ANONE},
{"IsList",              ISLIST,          STORE_REQUIRED, AEXPRESSION,   ANONE},
{"RoomData",            ROOMDATA,        STORE_REQUIRED, AEXPRESSION,   ANONE},
{"LoadRoom",            CREATEROOMDATA,  STORE_REQUIRED, AEXPRESSION,   ANONE},
{"FreeRoom",            FREEROOM,        STORE_OPTIONAL, AEXPRESSION,   ANONE},
{"GetClass",            GETCLASS,        STORE_REQUIRED, AEXPRESSION,   ANONE},
{"GetTime",             GETTIME,         STORE_REQUIRED, ANONE},
{"GetTickCount",        GETTICKCOUNT,    STORE_REQUIRED, ANONE},
{"GetDateAndTime",      GETDATEANDTIME,  STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,
         AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE},
{"SetClassVar",         SETCLASSVAR,     STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"GetLocationInfoBSP",  GETLOCATIONINFOBSP, STORE_OPTIONAL, AEXPRESSION, AEXPRESSION,
         AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE },
{"CanMoveInRoomBSP",    CANMOVEINROOMBSP, STORE_REQUIRED, AEXPRESSION, AEXPRESSION, AEXPRESSION,
         AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION,
         AEXPRESSION, AEXPRESSION, ANONE },
{"LineOfSightBSP",      LINEOFSIGHTBSP, STORE_REQUIRED, AEXPRESSION, AEXPRESSION, AEXPRESSION,
         AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE },
{"LineOfSightView",     LINEOFSIGHTVIEW, STORE_REQUIRED, AEXPRESSION, AEXPRESSION, AEXPRESSION,
         AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE },
{"ChangeTextureBSP",    CHANGETEXTUREBSP, STORE_OPTIONAL, AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE },
{"MoveSectorBSP",       MOVESECTORBSP,   STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE },
{"BlockerAddBSP",       BLOCKERADDBSP,   STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, AEXPRESSION,
                                         AEXPRESSION,   AEXPRESSION,  ANONE },
{"BlockerMoveBSP",      BLOCKERMOVEBSP,  STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, AEXPRESSION,
                                         AEXPRESSION,   AEXPRESSION,  ANONE },
{"BlockerRemoveBSP",    BLOCKERREMOVEBSP, STORE_OPTIONAL, AEXPRESSION,  AEXPRESSION,  ANONE },
{"BlockerClearBSP",     BLOCKERCLEARBSP, STORE_OPTIONAL, AEXPRESSION,   ANONE },
{"GetRandomPointBSP",   GETRANDOMPOINTBSP, STORE_OPTIONAL, AEXPRESSION, AEXPRESSION,  AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE },
{"GetStepTowardsBSP",   GETSTEPTOWARDSBSP, STORE_OPTIONAL, AEXPRESSION, AEXPRESSION,  AEXPRESSION, AEXPRESSION, AEXPRESSION, AEXPRESSION,
                                           AEXPRESSION, AEXPRESSION,  AEXPRESSION, AEXPRESSION, AEXPRESSION, ANONE },
{"SetResource",         SETRESOURCE,     STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  ANONE},
{"Post",                POSTMESSAGE,     STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  ASETTINGS, ANONE},
{"Abs",                 ABS,             STORE_REQUIRED, AEXPRESSION,   ANONE},
{"Sqrt",                SQRT,            STORE_REQUIRED, AEXPRESSION,   ANONE},
{"ParseString",         PARSESTRING,     STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"CreateTable",         CREATETABLE,     STORE_REQUIRED, AEXPRESSIONS,  ANONE},
{"AddTableEntry",       ADDTABLEENTRY,   STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"GetTableEntry",       GETTABLEENTRY,   STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  ANONE},
{"DeleteTableEntry",    DELETETABLEENTRY,STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  ANONE},
{"DeleteTable",         DELETETABLE,     STORE_OPTIONAL, AEXPRESSION,   ANONE},
{"IsTable",             ISTABLE,         STORE_REQUIRED, AEXPRESSION,   ANONE},
{"Bound",               BOUND,           STORE_REQUIRED, AEXPRESSION,   AEXPRESSION,  AEXPRESSION, ANONE},
{"GetTimeRemaining",    GETTIMEREMAINING, STORE_REQUIRED, AEXPRESSION,   ANONE},
{"SetString",           SETSTRING,       STORE_OPTIONAL, AEXPRESSION,   AEXPRESSION,  ANONE},
{"AppendTempString",    APPENDTEMPSTRING, STORE_OPTIONAL, AEXPRESSION,   ANONE},
{"ClearTempString",     CLEARTEMPSTRING, STORE_OPTIONAL, ANONE},
{"GetTempString",       GETTEMPSTRING,   STORE_REQUIRED, ANONE},
{"CreateString",        CREATESTRING,    STORE_REQUIRED, ANONE},
{"IsString",            ISSTRING,        STORE_REQUIRED, AEXPRESSION,   ANONE},
{"IsObject",            ISOBJECT,        STORE_REQUIRED, AEXPRESSION,   ANONE},
{"RecycleUser",         RECYCLEUSER,     STORE_OPTIONAL, AEXPRESSION,   ANONE},
{"StringToNumber",      STRINGTONUMBER,  STORE_REQUIRED, AEXPRESSION,   ANONE},
{"RecordStat",          RECORDSTAT,      STORE_OPTIONAL, AEXPRESSIONS,  ANONE},
{"GetSessionIP",        GETSESSIONIP,    STORE_REQUIRED, AEXPRESSION,   ANONE},
};

int numfuncs = (sizeof(Functions)/sizeof(function_type));

/* 
 * BuiltinIds is a bunch of identifiers that are automatically put into the 
 * compiler's symbol tables before any input is read.  This assures that the
 * identifiers will have the id #s given in the table, and so the server will
 * be able to use these numbers to access the identifiers from C.
 * The id #s of builtin ids should be below IDBASE defined in blakcomp.h.
 */
id_struct BuiltinIds[] = {
{"self",          I_PROPERTY,   0,  0,  0,   COMPILE},
{"user",          I_MISSING,    1,  0,   0,   I_CLASS},
{"userlogon",     I_MISSING,    2,  0,   0,   I_MESSAGE},
{"session_id",    I_MISSING,    3,  0,   0,   I_PARAMETER},
{"system",        I_MISSING,    4,  0,   0,   I_CLASS},
{"system_id",     I_MISSING,    5,  0,   0,   I_PARAMETER},
{"receiveclient", I_MISSING,    6,  0,   0,   I_MESSAGE},
{"client_msg",    I_MISSING,    7,  0,   0,   I_PARAMETER},
{"garbagecollecting",I_MISSING, 8,  0,   0,   I_MESSAGE},
{"loadedfromdisk",I_MISSING,    9,  0,   0,   I_MESSAGE},
{"constructor",   I_MISSING,   10,  0,   0,   I_MESSAGE},
{"destructor",    I_MISSING,   11,  0,   0,   I_MESSAGE},
{"number_stuff",  I_MISSING,   12,  0,   0,   I_PARAMETER},
{"garbagecollectingdone",I_MISSING, 13,  0,   0,   I_MESSAGE},
{"string",        I_MISSING,   14,  0,   0,   I_PARAMETER},
{"adminsystemmessage",I_MISSING, 15,  0,  0,  I_MESSAGE},
{"getusername",   I_MISSING,   16,  0,   0,   I_MESSAGE},
{"getusericon",   I_MISSING,   17,  0,   0,   I_MESSAGE},
{"sendcharinfo",  I_MISSING,   18,  0,   0,   I_MESSAGE},
{"newgamehour",   I_MISSING,   19,  0,   0,   I_MESSAGE},
{"guest",         I_MISSING,   20,  0,   0,   I_CLASS},
{"name",          I_MISSING,   21,  0,   0,   I_PARAMETER},
{"icon",          I_MISSING,   22,  0,   0,   I_PARAMETER},
{"admin",         I_MISSING,   23,  0,   0,   I_CLASS},
{"systemlogon",   I_MISSING,   24,  0,   0,   I_MESSAGE},
{"finduserbyinternetmailname",I_MISSING,25,0,0,I_MESSAGE},
{"receiveinternetmail",I_MISSING,26,0,   0,   I_MESSAGE},
{"perm_string",   I_MISSING,   27,  0,   0,   I_PARAMETER},
{"isfirsttime",   I_MISSING,   28,  0,   0,   I_MESSAGE},
{"delete",        I_MISSING,   29,  0,   0,   I_MESSAGE},
{"timer",         I_MISSING,   30,  0,   0,   I_PARAMETER},
{"type",          I_MISSING,   31,  0,   0,   I_PARAMETER},
{"DM",            I_MISSING,   32,  0,   0,   I_CLASS},
{"finduserbystring",I_MISSING, 33,  0,   0,   I_MESSAGE},
{"creator",       I_MISSING,   34,  0,   0,   I_CLASS},
{"settings",      I_MISSING,   35,  0,   0,   I_CLASS},
{"realtime",      I_MISSING,   36,  0,   0,   I_CLASS},
{"gameeventengine",I_MISSING,  37,  0,   0,   I_CLASS},
{"escapedconvict",I_MISSING,  38,  0,   0,   I_CLASS},
};

int numbuiltins = (sizeof(BuiltinIds)/sizeof(id_struct));

const char * get_function_name_by_opcode(int opcode)
{
   for (int i = 0; i < numfuncs; ++i)
   {
      if (Functions[i].opcode == opcode)
         return Functions[i].name;
   }

   return "Unknown";
}
