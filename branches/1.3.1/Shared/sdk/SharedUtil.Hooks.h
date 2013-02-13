/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Hooks.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    VOID HookInstallMethod( DWORD dwInstallAddress, DWORD dwHookFunction );
    VOID HookInstallCall( DWORD dwInstallAddress, DWORD dwHookFunction );
    BOOL HookInstall( DWORD dwInstallAddress, DWORD dwHookHandler, int iJmpCodeSize );
    BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray );
    eGameVersion GetGameVersion ( void );

    // Auto detect requirement of US/EU hook installation
    #define EZHookInstall(type) \
            __if_not_exists( RETURN_##type##_US ) \
            { \
                HookInstall( HOOKPOS_##type, (DWORD)HOOK_##type, HOOKSIZE_##type ) \
            } \
            __if_exists( RETURN_##type##_US ) \
            { \
                if ( GetGameVersion () == VERSION_US_10 ) \
                { \
                    EZHookInstall_HERE( type, US ) \
                } \
                else \
                { \
                    EZHookInstall_HERE( type, EU ) \
                } \
            }


    // US/EU hook installation
    // Includes additional return pointer copies if required
    #define EZHookInstall_HERE(type,CO) \
            HookInstall( HOOKPOS_##type##_##CO##, (DWORD)HOOK_##type, HOOKSIZE_##type##_##CO## ); \
            RETURN_##type##_BOTH = RETURN_##type##_##CO##; \
            __if_exists( RETURN_##type##B_##CO## ) \
            { \
                RETURN_##type##B_BOTH = RETURN_##type##B_##CO##; \
            } \
            __if_exists( RETURN_##type##C_##CO## ) \
            { \
                RETURN_##type##C_BOTH = RETURN_##type##C_##CO##; \
            }


    // Structure for holding hook info
    struct SHookInfo
    {
        template < class T > SHookInfo ( DWORD dwAddress, T dwHook, uint uiSize ): dwAddress ( dwAddress ) , dwHook ( (DWORD)dwHook ) , uiSize ( uiSize ) {}
        DWORD dwAddress;
        DWORD dwHook;
        uint uiSize;
    };

    #define MAKE_HOOK_INFO(type)  SHookInfo ( HOOKPOS_##type, HOOK_##type, HOOKSIZE_##type )

    // Structure for holding poke info
    struct SPokeInfo
    {
        DWORD dwAddress;
        BYTE ucValue;
    };


    //
    // Use MemSet/Cpy/Put/Add for the following memory regions:
    //
    // 0x401000 - 0x4C02FF
    // 0x4C0400 - 0x5022FF
    // 0x502400 - 0x50AAFF
    // 0x50AC00 - 0x50BEFF
    // 0x50C000 - 0x5331FF
    // 0x533300 - 0x60D7FF
    // 0x60D900 - 0x609BFF
    // 0x609D00 - 0x60F1FF
    // 0x60F300 - 0x686FFF
    // 0x687100 - 0x6AE9FF
    // 0x6AEB00 - 0x729AFF
    // 0x729C00 - 0x742AFF
    // 0x742C00 - 0x8A4000
    //

    void MemSet ( void* dwDest, int cValue, uint uiAmount );
    void MemCpy ( void* dwDest, const void* dwSrc, uint uiAmount );

    template < class T, class U >
    void MemPut ( U ptr, const T value )
    {
        if ( *(T*)ptr != value )
            MemCpy ( (void*)ptr, &value, sizeof ( T ) );
    }

    template < class T, class U >
    void MemAdd ( U ptr, const T value )
    {
        T current;
        memcpy ( &current, (void*)ptr, sizeof ( T ) );
        current += value;
        MemCpy ( (void*)ptr, &current, sizeof ( T ) );
    }


    //
    // Use Mem*Fast for everything else
    //
    inline void MemCpyFast ( void* dwDest, const void* dwSrc, uint uiAmount )
    {
        memcpy ( dwDest, dwSrc, uiAmount );
    }

    inline void MemSetFast ( void* dwDest, int cValue, uint uiAmount )
    {
        memset ( dwDest, cValue, uiAmount );
    }

    template < class T, class U >
    void MemPutFast ( U ptr, const T value )
    {
        *(T*)ptr = value;
    }

    template < class T, class U >
    void MemAddFast ( U ptr, const T value )
    {
        *(T*)ptr += value;
    }

    template < class T, class U >
    void MemSubFast ( U ptr, const T value )
    {
        *(T*)ptr -= value;
    }

    template < class T, class U >
    void MemAndFast ( U ptr, const T value )
    {
        *(T*)ptr &= value;
    }

    template < class T, class U >
    void MemOrFast ( U ptr, const T value )
    {
        *(T*)ptr |= value;
    }

};

using namespace SharedUtil;

