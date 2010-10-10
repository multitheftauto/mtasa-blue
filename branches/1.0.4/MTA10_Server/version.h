//
// MTA10_Server/version.h
//
// Server version file
//

//
// To compile a Linux public server:
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_RELEASE
//      2. Use net.so from the latest Linux build (nightly.mtasa.com)
//
// To compile a Windows public server:
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_RELEASE
//      2. Use net.dll from the latest untested (nightly.mtasa.com)
//
// To compile a Windows custom server:
//      1. set MTASA_VERSION_TYPE to VERSION_TYPE_CUSTOM
//      2. Use net.dll from the latest unstable (nightly.mtasa.com)
//


// New version info
#define MTASA_VERSION_MAJOR         1
#define MTASA_VERSION_MINOR         0
#define MTASA_VERSION_MAINTENANCE   4
#define MTASA_VERSION_TYPE          VERSION_TYPE_RELEASE
#define MTASA_VERSION_BUILD         0

// Old version info
#define MTA_DM_VERSION              ( ( ( MTASA_VERSION_MAJOR ) << 8 ) | ( ( MTASA_VERSION_MINOR ) << 4 ) | ( ( MTASA_VERSION_MAINTENANCE ) << 0 ) )
#define MTA_DM_VERSIONSTRING        QUOTE_DEFINE ( MTASA_VERSION_MAJOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MINOR ) "." QUOTE_DEFINE ( MTASA_VERSION_MAINTENANCE )
#define MTA_DM_FULL_STRING          "MTA:SA Server"
#define MTA_DM_BITSTREAM_VERSION    0x14

// Compile types
#define VERSION_TYPE_CUSTOM         0x01
#define VERSION_TYPE_EXPERIMENTAL   0x03
#define VERSION_TYPE_UNSTABLE       0x05
#define VERSION_TYPE_UNTESTED       0x07
#define VERSION_TYPE_RELEASE        0x09


#define QUOTE_DEFINE2(c) #c
#define QUOTE_DEFINE(x) QUOTE_DEFINE2(x)

// Implement compile types
#if !defined(WIN32)

    // Only one type of build for non-windows servers
    #define MTA_DM_BUILDTYPE        ""
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" QUOTE_DEFINE ( MTASA_VERSION_BUILD )
    #define MTA_DM_CONNECT_FROM_PUBLIC

#elif MTASA_VERSION_TYPE == VERSION_TYPE_CUSTOM

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


// To avoid user confusion, make sure the ASE version matches only if communication is possible
#if defined(MTA_DM_CONNECT_FROM_PUBLIC)
    #define MTA_DM_ASE_VERSION      "1.0"
    #define MTA_DM_NETCODE_VERSION  0x164
    #define MTA_DM_NET_MODULE_VERSION   0x001A
#else
    #define MTA_DM_ASE_VERSION      "1.0n"
    #define MTA_DM_NETCODE_VERSION  0x165
    #define MTA_DM_NET_MODULE_VERSION   0x041A
#endif
