//
// MTA10/version.h
//
// Client version file
//

//
// To compile a client: 
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_CUSTOM
//      2. Use netc.dll from the latest unstable build (nightly.mtasa.com)
//

// New version info
#define MTASA_VERSION_MAJOR         1
#define MTASA_VERSION_MINOR         4
#define MTASA_VERSION_MAINTENANCE   0
#define MTASA_VERSION_TYPE          VERSION_TYPE_CUSTOM
#define MTASA_VERSION_BUILD         0

#include "build_overrides_c.h"

// Old version info
#define MTA_DM_VERSION              ( ( ( MTASA_VERSION_MAJOR ) << 8 ) | ( ( MTASA_VERSION_MINOR ) << 4 ) | ( ( MTASA_VERSION_MAINTENANCE ) << 0 ) )
#if MTASA_VERSION_MAINTENANCE == 0
    #define MTA_DM_VERSIONSTRING        QUOTE_DEFINE ( MTASA_VERSION_MAJOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MINOR )
#else
    #define MTA_DM_VERSIONSTRING        QUOTE_DEFINE ( MTASA_VERSION_MAJOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MINOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MAINTENANCE )
#endif
#define MTA_DM_FULL_STRING          "MTA:SA Client"


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
    #define MTA_DM_CONNECT_TO_PUBLIC
    #define MTA_DM_EXPIRE_DAYS      60

#elif MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE

    #define MTA_DM_BUILDTYPE        "release"
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE "-" QUOTE_DEFINE ( MTASA_VERSION_BUILD )
    #define MTA_DM_CONNECT_TO_PUBLIC

#else
    #error "Incorrect MTASA_VERSION_TYPE"
#endif


#define _ASE_VERSION QUOTE_DEFINE(MTASA_VERSION_MAJOR) "." QUOTE_DEFINE(MTASA_VERSION_MINOR)
#define _NETCODE_VERSION_BRANCH_ID      0x4         // Use 0x1 - 0xF to indicate an incompatible branch is being used (0x0 is reserved, 0x4 is trunk)
#define _CLIENT_NET_MODULE_VERSION      0x076       // (0x000 - 0xfff) Lvl9 wizards only
#define _NETCODE_VERSION                0x1D4       // (0x000 - 0xfff) Increment when net messages change (pre-release)
#define MTA_DM_BITSTREAM_VERSION        0x02E       // (0x000 - 0xfff) Increment when net messages change (post-release). (Changing will also require additional backward compatibility code).

// To avoid user confusion, make sure the ASE version matches only if communication is possible
#if defined(MTA_DM_CONNECT_TO_PUBLIC)
    #define MTA_DM_ASE_VERSION                  _ASE_VERSION
    #define MTA_DM_NETCODE_VERSION              _NETCODE_VERSION
    #define MTA_DM_CLIENT_NET_MODULE_VERSION   _CLIENT_NET_MODULE_VERSION
#else
    #if _NETCODE_VERSION_BRANCH_ID < 1 || _NETCODE_VERSION_BRANCH_ID > 15
        #error "_NETCODE_VERSION_BRANCH_ID wrong"
    #endif
    #define MTA_DM_ASE_VERSION                  _ASE_VERSION "n"
    #define MTA_DM_NETCODE_VERSION              ( _NETCODE_VERSION + ( _NETCODE_VERSION_BRANCH_ID << 12 ) )
    #define MTA_DM_CLIENT_NET_MODULE_VERSION    ( _CLIENT_NET_MODULE_VERSION + ( 4 << 12 ) )
#endif
