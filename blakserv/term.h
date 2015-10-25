// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * term.h
 *
 */

#ifndef _TERM_H
#define _TERM_H

void cprintf(__int64 session_id,const char *fmt,...);
void TermConvertBuffer(char *s,int len_s);

const char * GetTagName(val_type val);
const char * GetDataName(val_type val);

__int64 GetTagNum(const char *tag_str);
__int64 GetDataNum(__int64 tag_val, const char *data_str);

#endif
