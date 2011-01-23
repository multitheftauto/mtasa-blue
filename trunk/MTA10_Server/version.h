//
// MTA10_Server/version.h
//
// Server version file
//

//
// To compile a public server:
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_RELEASE
//      2. Use net.dll/net.so from the latest 1.1 untested/rc/release (nightly.mtasa.com)
//
// To compile a custom server:
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_CUSTOM
//      2. Use net.dll/net.so from the latest 1.1 unstable (nightly.mtasa.com)


// New version info
#define MTASA_VERSION_MAJOR         1
#define MTASA_VERSION_MINOR         1
#define MTASA_VERSION_MAINTENANCE   0
#define MTASA_VERSION_TYPE          VERSION_TYPE_CUSTOM
#define MTASA_VERSION_BUILD         0

// Old version info
#define MTA_DM_VERSION              ( ( ( MTASA_VERSION_MAJOR ) << 8 ) | ( ( MTASA_VERSION_MINOR ) << 4 ) | ( ( MTASA_VERSION_MAINTENANCE ) << 0 ) )
#if MTASA_VERSION_MAINTENANCE == 0
    #define MTA_DM_VERSIONSTRING        QUOTE_DEFINE ( MTASA_VERSION_MAJOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MINOR )
#else
    #define MTA_DM_VERSIONSTRING        QUOTE_DEFINE ( MTASA_VERSION_MAJOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MINOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MAINTENANCE )
#endif
#define MTA_DM_FULL_STRING          "MTA:SA Server"
#define MTA_DM_BITSTREAM_VERSION    0x16

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
    #define MTA_DM_CONNECT_FROM_PUBLIC

#elif MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE

    #define MTA_DM_BUILDTYPE        "release"
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE "-" QUOTE_DEFINE ( MTASA_VERSION_BUILD )
    #define MTA_DM_CONNECT_FROM_PUBLIC

#else
    #error "Incorrect MTASA_VERSION_TYPE"
#endif


#define _ASE_VERSION QUOTE_DEFINE(MTASA_VERSION_MAJOR) "." QUOTE_DEFINE(MTASA_VERSION_MINOR)
#define _NETCODE_VERSION                0x01A0      // Increment when net messages change
#define _SERVER_NET_MODULE_VERSION      0x0025      // Lvl9 wizards only

// To avoid user confusion, make sure the ASE version matches only if communication is possible
#if defined(MTA_DM_CONNECT_FROM_PUBLIC)
    #define MTA_DM_ASE_VERSION                  _ASE_VERSION
    #define MTA_DM_NETCODE_VERSION              _NETCODE_VERSION
    #define MTA_DM_SERVER_NET_MODULE_VERSION    _SERVER_NET_MODULE_VERSION
#else
    #define MTA_DM_ASE_VERSION                  _ASE_VERSION "n"
    #define MTA_DM_NETCODE_VERSION              ( _NETCODE_VERSION + 0x4000 )
    #define MTA_DM_SERVER_NET_MODULE_VERSION    ( _SERVER_NET_MODULE_VERSION + 0x4000 )
#endif

// Handy self compile message
#ifndef MTA_DM_CONNECT_FROM_PUBLIC
    #ifdef SHOW_SELF_COMPILE_WARNING
        #ifdef WIN32
            #pragma message("-------------------------------------------------------------------------")
            #pragma message("MTASA_VERSION_TYPE is not set to VERSION_TYPE_RELEASE")
            #pragma message("Server will not work with release clients")
            #pragma message("-------------------------------------------------------------------------")
            #pragma message("If you want the server to work with release clients,")
            #pragma message("set MTASA_VERSION_TYPE to VERSION_TYPE_RELEASE in MTA10_Server/version.h")
            #pragma message("-------------------------------------------------------------------------")
        #else
            #warning "-------------------------------------------------------------------------"
            #warning "MTASA_VERSION_TYPE is not set to VERSION_TYPE_RELEASE"
            #warning "Server will not work with release clients"
            #warning "-------------------------------------------------------------------------"
            #warning "If you want the server to work with release clients,"
            #warning "set MTASA_VERSION_TYPE to VERSION_TYPE_RELEASE in MTA10_Server/version.h"
            #warning "-------------------------------------------------------------------------"
        #endif
    #endif
#endif
