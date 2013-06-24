/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.MemAccess.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    struct SMemWrite
    {
        DWORD dwFirstPage;
        DWORD dwSize;
        DWORD oldProt;
    };

    void        SetInitialVirtualProtect    ( void );
    bool        IsSlowMem                   ( const void* pAddr, uint uiAmount );
    SMemWrite   OpenMemWrite                ( const void* pAddr, uint uiAmount );
    void        CloseMemWrite               ( SMemWrite& hMem );
    bool        ismemset                    ( const void* pAddr, int cValue, uint uiAmount );

    #define DEBUG_CHECK_IS_FAST_MEM(addr,size) { dassert( !IsSlowMem( (const void*)(addr), size ) ); }
    #define DEBUG_CHECK_IS_SLOW_MEM(addr,size) { dassert( IsSlowMem( (const void*)(addr), size ) ); }
}
