//
// Shared/version.h
//
#pragma once

//
// To compile a client for mass consumption by players (releasing your own "MTA fork"):
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_UNSTABLE
//      2. Use netc.dll from https://mirror-cdn.multitheftauto.com/bdata/netc.dll (preferably run utils/buildactions/install_data.lua)
// Per the above, take note of AC constraints of building a fork (see https://wiki.multitheftauto.com/wiki/Forks)
// If you wish to get past "15% AC" constraints and use "official MTA full anticheat", read this new article: https://wiki.multitheftauto.com/wiki/Forks_Full_AC
//
// To compile a client for development and debugging purposes (e.g to avoid AC kicks for attaching a debugger like WinDbg):
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_CUSTOM
//      2. Use netc.dll from https://mirror-cdn.multitheftauto.com/bdata/netc.dll (preferably run utils/buildactions/install_data.lua)
// Never use 'custom' build type for building a fork: you would change the final "15% AC protection" described at https://wiki.multitheftauto.com/wiki/Forks to only 1%, as netc switches to disable everything in favour of MTA contributors' ability to do things like attach debuggers
//
// To compile a public server (players that use official MTA client can connect, as long you don't introduce incompatible patches):
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_RELEASE
//      2. Use net.dll/net.so from the latest untested/rc/release (nightly.mtasa.com)
//
// To compile a custom server (only custom build players can connect):
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_CUSTOM
//      2. Use net.dll/net.so from the latest unstable (nightly.mtasa.com)

// New version info
#define MTASA_VERSION_MAJOR         1
#define MTASA_VERSION_MINOR         5
#define MTASA_VERSION_MAINTENANCE   9
#define MTASA_VERSION_TYPE          VERSION_TYPE_UNSTABLE
#define MTASA_VERSION_BUILD         0

#include "../build_overrides.h"

// Old version info
#define MTA_DM_VERSION              ( ( ( MTASA_VERSION_MAJOR ) << 8 ) | ( ( MTASA_VERSION_MINOR ) << 4 ) | ( ( MTASA_VERSION_MAINTENANCE ) << 0 ) )
#if MTASA_VERSION_MAINTENANCE == 0
    #define MTA_DM_VERSIONSTRING        QUOTE_DEFINE ( MTASA_VERSION_MAJOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MINOR )
#else
    #define MTA_DM_VERSIONSTRING        QUOTE_DEFINE ( MTASA_VERSION_MAJOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MINOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MAINTENANCE )
#endif

#ifdef MTA_CLIENT
#define MTA_DM_FULL_STRING "MTA:SA Client"
#else
#define MTA_DM_FULL_STRING "MTA:SA Server"
#endif

// Compile types
#define VERSION_TYPE_CUSTOM         0x01
#define VERSION_TYPE_EXPERIMENTAL   0x03
#define VERSION_TYPE_UNSTABLE       0x05
#define VERSION_TYPE_UNTESTED       0x07
#define VERSION_TYPE_RELEASE        0x09

#define QUOTE_DEFINE2(c) #c
#define QUOTE_DEFINE(x) QUOTE_DEFINE2(x)

// Implement compile types
#if MTASA_VERSION_TYPE == VERSION_TYPE_CUSTOM

    #define MTA_DM_BUILDTYPE        "custom"
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE

#elif MTASA_VERSION_TYPE == VERSION_TYPE_UNSTABLE

    #define MTA_DM_BUILDTYPE        "unstable"
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE "-" QUOTE_DEFINE ( MTASA_VERSION_BUILD )
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE "-" QUOTE_DEFINE ( MTASA_VERSION_BUILD )

#elif MTASA_VERSION_TYPE == VERSION_TYPE_UNTESTED

    #define MTA_DM_BUILDTYPE        "untested"
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE "-" QUOTE_DEFINE ( MTASA_VERSION_BUILD )
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE "-" QUOTE_DEFINE ( MTASA_VERSION_BUILD )
    #define MTA_DM_PUBLIC_CONNECTIONS
    #define MTA_DM_EXPIRE_DAYS      60

#elif MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE

    #define MTA_DM_BUILDTYPE        "release"
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE "-" QUOTE_DEFINE ( MTASA_VERSION_BUILD )
    #define MTA_DM_PUBLIC_CONNECTIONS

#else
    #error "Incorrect MTASA_VERSION_TYPE"
#endif

#define _ASE_VERSION QUOTE_DEFINE(MTASA_VERSION_MAJOR) "." QUOTE_DEFINE(MTASA_VERSION_MINOR)
#define _NETCODE_VERSION_BRANCH_ID      0x4         // Use 0x1 - 0xF to indicate an incompatible branch is being used (0x0 is reserved, 0x4 is trunk)
#define _CLIENT_NET_MODULE_VERSION      0x0AD       // (0x000 - 0xfff) Lvl9 wizards only
#define _SERVER_NET_MODULE_VERSION      0x0AB       // (0x000 - 0xfff) Lvl9 wizards only
#define _NETCODE_VERSION                0x1DA       // (0x000 - 0xfff) Increment when net messages change (pre-release)

// (0x000 - 0xfff) Update bitstream.h when net messages change (post-release). (Changing will also require additional backward compatibility code).
#define MTA_DM_BITSTREAM_VERSION eBitStreamVersion::Latest

// To avoid user confusion, make sure the ASE version matches only if communication is possible
#if defined(MTA_DM_PUBLIC_CONNECTIONS)
    #define MTA_DM_ASE_VERSION                  _ASE_VERSION
    #define MTA_DM_NETCODE_VERSION              _NETCODE_VERSION
    #define MTA_DM_CLIENT_NET_MODULE_VERSION    _CLIENT_NET_MODULE_VERSION
    #define MTA_DM_SERVER_NET_MODULE_VERSION    _SERVER_NET_MODULE_VERSION
#else
    #if _NETCODE_VERSION_BRANCH_ID < 1 || _NETCODE_VERSION_BRANCH_ID > 15
        #error "_NETCODE_VERSION_BRANCH_ID wrong"
    #endif
    #define MTA_DM_ASE_VERSION                  _ASE_VERSION "n"
    #define MTA_DM_NETCODE_VERSION              ( _NETCODE_VERSION + ( _NETCODE_VERSION_BRANCH_ID << 12 ) )
    #define MTA_DM_CLIENT_NET_MODULE_VERSION    ( _CLIENT_NET_MODULE_VERSION + ( 4 << 12 ) )
    #define MTA_DM_SERVER_NET_MODULE_VERSION    _SERVER_NET_MODULE_VERSION
#endif

// Handy self compile message
#ifndef MTA_DM_PUBLIC_CONNECTIONS
    #if defined(SHOW_SELF_COMPILE_WARNING) && !defined(CI_BUILD) && !defined(MTA_CLIENT)
        #pragma message("\n\
----------------------------------------------------------------------\n\
MTASA_VERSION_TYPE is not set to VERSION_TYPE_RELEASE\n\
Server will not work with release clients\n\
-------------------------------------------------------------------------\n\
If you want the server to work with release clients\n\
set MTASA_VERSION_TYPE to VERSION_TYPE_RELEASE in Server/version.h\n\
\n\
Client can't enter the game? Use Server net.dll from an official build.\n\
(Official dll uses VERSION_TYPE_RELEASE, win-install-data gives UNSTABLE.)\n\
-------------------------------------------------------------------------")
    #endif
#endif
