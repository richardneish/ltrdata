#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wio.h>
#include <wsync.h>

#include <stdio.h>

#pragma comment(lib, "winstrcp")

class ClientManager
{
  static WCriticalSection cs;  // Synchronization
  static ClientManager *first;  // First in chain
  ClientManager *next;
  HANDLE h;
  char *name;
  void SendMsg(const char *msg)
  {
    StrSend(h, msg);
  }

  void BroadcastMsg(const char *msg);
  bool ApplyName(const char *namebuf);

  /// Returns false if connection should be closed.
  bool ParseCommand(const char *cmd);

public:
  ClientManager(HANDLE hConn);
  ~ClientManager();
  void Server();
};

ClientManager *ClientManager::first = NULL;
WCriticalSection ClientManager::cs;

ClientManager::ClientManager(HANDLE hConn) : h(hConn), name(NULL)
{
  cs.Enter();
  next = first;
  first = this;
  cs.Leave();
}

ClientManager::~ClientManager()
{
  cs.Enter();

  if (first == this)
    first = next;
  else
    for (ClientManager *cm = first; cm->next; cm = cm->next)
      if (cm->next == this)
	{
	  cm->next = next;
	  break;
	}

  cs.Leave();

  if (name)
    delete name;
}

// Check if name is already used
void ClientManager::BroadcastMsg(const char *msg)
{
  printf("ChatServer: %s", msg);

  cs.Enter();
  for (ClientManager *cm = first; cm; cm = cm->next)
    {
      cm->SendMsg(msg);
      Sleep(0);
    }
  cs.Leave();
}

// Check if name is already used
bool ClientManager::ApplyName(const char *namebuf)
{
  cs.Enter();
  for (ClientManager *cm = first; cm; cm = cm->next)
    {
      if (cm->name)
	if (strcmpi(namebuf, cm->name) == 0)
	  {
            cs.Leave();
            SendMsg("Sorry, your nickname is already in use.\n");
            return false;
	  }

      Sleep(0);
    }

  name = new char[strlen(namebuf)+1];
  if (name == NULL)
    fputs("CHATSRV[ClientManager::ApplyName()]: Out of memory.\r\n", stderr);
  else
    strcpy(name, namebuf);

  cs.Leave();
  return name != NULL;
}

// Returns false if connection should be closed.
bool ClientManager::ParseCommand(const char *cmd)
{
  // Send list of connected users.
  if (strcmpi(cmd, "/list") == 0)
    {
      SendMsg("List of connected users:\n");
      cs.Enter();
      int iUserCounter = 0;
      for (ClientManager *cm = first; cm; cm = cm->next)
	{
	  char cBuf[1024];
	  _snprintf(cBuf, sizeof cBuf,
		    "User %i: %s\n",
		    ++iUserCounter, cm->name);
	  cBuf[sizeof(cBuf)-1] = 0;
	  SendMsg(cBuf);
	}
      cs.Leave();
      char cBuf[1024];
      _snprintf(cBuf, sizeof cBuf,
		"Total %i users connected.\n",
		iUserCounter);
      cBuf[sizeof(cBuf)-1] = 0;
      SendMsg(cBuf);

      return true;
    }

  // Cancel connection.
  if (strcmpi(cmd, "/quit") == 0)
    return false;

  SendMsg("Unknown command.\n");
  return true;
}

void ClientManager::Server()
{
  char buf[MAX_PATH];

  if (LineRecv(h, buf, sizeof buf) == 0)
    return;

  if (!ApplyName(buf))
    return;

  _snprintf(buf, sizeof buf,
	    "%s joined the chat.\n", name);
  buf[sizeof(buf)-1] = 0;
  BroadcastMsg(buf);

  char *bufptr = buf + strlen(name) + 2;
  for (;;)
    {
      _snprintf(buf, sizeof buf,
		"%s> ", name);

      if (LineRecv(h, bufptr, sizeof(buf)-(bufptr-buf)-1) == 0)
	break;

      if (bufptr[0] == '/')
	if (ParseCommand(bufptr))
	  continue;
	else
	  break;

      strcat(buf, "\n");

      BroadcastMsg(buf);
    }

  _snprintf(buf, sizeof buf,
	    "%s left the chat.\n", name);
  buf[sizeof(buf)-1] = 0;
  BroadcastMsg(buf);
}

EXTERN_C __declspec(dllexport) DWORD WINAPI
ChatServer(HANDLE hConn, LPCSTR)
{
  ClientManager cm(hConn);
  cm.Server();
  return 0;
}

