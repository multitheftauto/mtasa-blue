/*
ASE Query & Reporting SDK           Copyright (C) 2003 UDP Soft Ltd
http://www.udpsoft.com/eye/         All Rights Reserved

Enables reporting to the ASE master server and responding to server
browser queries.

Before reading further, see example.c for a quick display of how simple this
really is.

A quick run-down of things to do:

1. Initialization

extern int ASEQuery_initialize(int hostport, int internet, char *address);

hostport = the join port for the server
internet = flag indicating whether we should report to the ASE master server
           (LAN servers should set this to 0)
address = IP address to bind to (pass as NULL if your game does not have
          support for multihomed hosts)

This function should be called when the server is started. Does a DNS lookup,
so call only when net already running. NOTE! Winsock must be initialized
before calling the init function (see example.c).

Returns 0 on error, char *ASEQuery_error contains the error message.


2. The worker function

extern int ASEQuery_check(void);

Ideally this should be called every millisecond. The simplest implementation
would be to call this every "frame". Longer delays cause bigger ping
fluctuations ie. not so accurate pings in the server browser.

Returns 0 on error, char *ASEQuery_error contains the error message.


3. Functions that the _game_ has to implement

void ASEQuery_wantstatus(void);
void ASEQuery_wantrules(void);
void ASEQuery_wantplayers(void);

These are called when the SDK needs information about the game state.
In each of these functions, the game must call back to the SDK providing
the required information.


4. Functions that the game calls to provide information back to the SDK

extern void ASEQuery_status(const char *hostname, const char *gametype, const char *mapname, const char *gamever, int password, int numplayers, int maxplayers);
extern void ASEQuery_addrule(const char *key, const char *value);
extern void ASEQuery_addplayer(const char *name, const char *team, const char *skin, const char *score, const char *ping, const char *time);

These can only be called in the corresponding functions described earlier.
ASEQuery_status must only be called once. ASEQuery_addrule and
ASEQuery_addplayer should be called for each rule/player that the game
wishes to be visible in the server browser. Player info fields that don't
apply can be passed as NULL.


5. Shutting down

extern void ASEQuery_shutdown(void);


6. How to link

In C:
#include "ASEQuerySDK.h" in every module that uses the SDK.

In C++:
extern "C" {
#include "ASEQuerySDK.h"
}

Link with ASEQuerySDK.LIB

The SDK uses the __cdecl calling convention.

7. Used ports

The SDK uses gameport+123 (UDP) as the default port for queries. If that port
is not available, the next available port is used (+124, +125...). This port
must have unrestricted access to/from the internet.

8. Other stuff

For ASE support, your game also has to implement commandline parsing for
server address, player name, password etc.

The standard implementation would be:
game.exe +connect ip:port +name "my name" +config "myconfig.cfg" +password "server password"

Let us know the commandline options for your game so we can update ASE
accordingly. In addition to this we'll need the executable name and the
registry key for the install folder.

For the demo version of the SDK, there's built-in ASE support already. Just
go to View -> Options -> Games -> Not Installed -> SDK test and check
"Visible in filter list". Any server running the demo SDK will now appear
under the "SDK test" filter.


actual .h stuff follows
*/
extern "C" {
    extern int ASEQuery_initialize(int hostport, int internet, char *address);

    extern void ASEQuery_shutdown(void);

    extern int ASEQuery_check(void);

    void ASEQuery_wantstatus(void);
    void ASEQuery_wantrules(void);
    void ASEQuery_wantplayers(void);

    extern void ASEQuery_status(const char *hostname, const char *gametype, const char *mapname, const char *gamever, int password, int numplayers, int maxplayers);
    extern void ASEQuery_addrule(const char *key, const char *value);
    extern void ASEQuery_addplayer(const char *name, const char *team, const char *skin, const char *score, const char *ping, const char *time);

    extern char *ASEQuery_error;
}
