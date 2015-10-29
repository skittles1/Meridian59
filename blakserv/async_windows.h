

#ifndef _ASYNC_WINDOWS_H
#define _ASYNC_WINDOWS_H
void StartAsyncSocketAccept(SOCKET sock,int connection_type);
HANDLE StartAsyncNameLookup(char *peer_addr,char *buf);
void StartAsyncSession(session_node *s);
void AsyncSocketAccept(SOCKET sock,int event,int error,int connection_type);
void AsyncSocketSelect(SOCKET sock,int event,int error);

#endif
