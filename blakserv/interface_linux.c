#include "blakserv.h"

int sessions_logged_on;

void InitInterface(void)
{
    // TODO: stub
    lprintf("Entering InitInterface");
}

void StartupPrintf(const char *fmt,...)
{
    // TODO: stub
}

void StartupComplete(void)
{
    // TODO: stub
}

int GetUsedSessions(void)
{
   return sessions_logged_on;
}

void InterfaceUpdate(void)
{
    // TODO: stub
}

void InterfaceLogon(session_node *s)
{
    // TODO: stub
}

void InterfaceLogoff(session_node *s)
{
    // TODO: stub
}

void InterfaceUpdateSession(session_node *s)
{
    // TODO: stub
}

void InterfaceUpdateChannel(void)
{
    // TODO: stub
}

void InterfaceSendBufferList(buffer_node *blist)
{
    // TODO: stub
}

void InterfaceSendBytes(char *buf,int len_buf)
{
    // TODO: stub
}

void StartAsyncSocketAccept(SOCKET sock,int connection_type)
{
    // TODO: stub
}

HANDLE StartAsyncNameLookup(char *peer_addr,char *buf)
{
    // TODO: stub
    return 0;
}

void StartAsyncSession(session_node *s)
{
    // TODO: stub
}

void FatalErrorShow(const char *filename,int line,const char *str)
{
    // TODO: stub
}

