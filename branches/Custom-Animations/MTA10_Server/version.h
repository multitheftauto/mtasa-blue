#define MTA_DM_VERSION              0x0104
#define MTA_DM_VERSIONSTRING        "1.0.4"
#define MTA_DM_FULL_STRING          "MTA:SA Server"
#define MTA_DM_BITSTREAM_VERSION    0x07

// These defines are dependent on the build configuration:
// MTA_DM_ASE_VERSION
// MTA_DM_BUILDTYPE
// MTA_DM_BUILDTAG_SHORT
// MTA_DM_BUILDTAG_LONG
// MTA_DM_NETCODE_VERSION

#define QUOTE_DEFINE2(c) #c
#define QUOTE_DEFINE(x) QUOTE_DEFINE2(x)

#if !defined(WIN32)
    // Only one type of build for non-windows servers
    #define MTA_DM_BUILDTYPE        ""
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" "rc-0"    // Note: This has to be changed manually for each Linux update
    #define MTA_DM_CONNECT_TO_PUBLIC

#elif defined(BUILD_CONFIG_)
    #pragma message("NOTE: It's safe to ignore 'environment variables were not found' warning.")

    #define MTA_DM_BUILDTYPE        "Custom"
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE
    #define MTA_DM_BUILDTAG_LONG    MTA_DM_VERSIONSTRING "-" MTA_DM_BUILDTYPE

#elif defined(BUILD_CONFIG_EXPERIMENTAL)

    #define MTA_DM_BUILDTYPE        "Experimental"
    #define MTA_DM_BUILDTAG_SHORT   QUOTE_DEFINE ( BUILD_NUMBER )
    #define MTA_DM_BUILDTAG_LONG    QUOTE_DEFINE ( BUILD_NUMBER )

#elif defined(BUILD_CONFIG_UNSTABLE)

    #define MTA_DM_BUILDTYPE        "Unstable"
    #define MTA_DM_BUILDTAG_SHORT   QUOTE_DEFINE ( BUILD_NUMBER )
    #define MTA_DM_BUILDTAG_LONG    QUOTE_DEFINE ( BUILD_NUMBER )

#elif defined(BUILD_CONFIG_UNTESTED)

    #define MTA_DM_BUILDTYPE        "Untested"
    #define MTA_DM_BUILDTAG_SHORT   QUOTE_DEFINE ( BUILD_NUMBER )
    #define MTA_DM_BUILDTAG_LONG    QUOTE_DEFINE ( BUILD_NUMBER )
    #define MTA_DM_CONNECT_TO_PUBLIC
    #define MTA_DM_EXPIRE_DAYS      30

#elif defined(BUILD_CONFIG_RELEASE_CANDIDATE)

    #define MTA_DM_BUILDTYPE        "Release"
    #define MTA_DM_BUILDTAG_SHORT   MTA_DM_VERSIONSTRING
    #define MTA_DM_BUILDTAG_LONG    QUOTE_DEFINE ( BUILD_NUMBER )
    #define MTA_DM_CONNECT_TO_PUBLIC

#else
    #error "'mergancy"
#endif

// To avoid user confusion, make sure the ASE version matches only if communication is possible
#if defined(MTA_DM_CONNECT_TO_PUBLIC)
    #define MTA_DM_ASE_VERSION      "1.0"
    #define MTA_DM_NETCODE_VERSION  0x164
    #define MTA_DM_NET_MODULE_VERSION   0x0010
#else
    #define MTA_DM_ASE_VERSION      "1.0n"
    #define MTA_DM_NETCODE_VERSION  0x165
    #define MTA_DM_NET_MODULE_VERSION   0x0410
#endif

