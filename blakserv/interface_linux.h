#ifndef _INTERFAC_LINUX_H
#define _INTERFAC_LINUX_H

#define FatalError(a) FatalErrorShow(__FILE__,__LINE__,a)

void InitInterface(void);

void StartupPrintf(const char *fmt,...);
void StartupComplete(void);

int GetUsedSessions(void);

void InterfaceUpdate(void);
void InterfaceLogon(session_node *s);
void InterfaceLogoff(session_node *s);
void InterfaceUpdateSession(session_node *s);
void InterfaceUpdateChannel(void);

void InterfaceSendBufferList(buffer_node *blist);
void InterfaceSendBytes(char *buf,int len_buf);

void StartAsyncSocketAccept(SOCKET sock,int connection_type);
HANDLE StartAsyncNameLookup(char *peer_addr,char *buf);
void StartAsyncSession(session_node *s);

void FatalErrorShow(const char *filename,int line,const char *str);

#endif

