/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.MemAccess.hpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{

    // Returns true if matching memset would have no affect
    bool ismemset( const void* pInAddr, int cValue, uint uiAmount )
    {
        const uchar* pAddr = (const uchar*)pInAddr;
        uchar ucValue = cValue;
        for ( uint i = 0 ; i < uiAmount ; i++ )
            if ( pAddr[i] != ucValue )
                return false;
        return true;
    }

    // Unprotect fast mem areas
    void SetInitialVirtualProtect( void )
    {
        DWORD oldProt;
        VirtualProtect((LPVOID)0x4C0000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x502000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x50A000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x50B000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x533000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x60D000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x609000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x60F000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x687000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x6A0000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x6AE000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x729000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
        VirtualProtect((LPVOID)0x742000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProt);
    }

    // Returns true if pAddr is in slow mem area
    bool IsSlowMem( const void* pAddr, uint uiAmount )
    {
        DWORD uiAddr = ((DWORD)pAddr) >> 8;
        DWORD uiAddrLast = (((DWORD)pAddr) + uiAmount - 1) >> 8;

        for ( ; uiAddr <= uiAddrLast ; uiAddr++ )
        {
            if ( uiAddr < 0x4010
                || uiAddr > 0x8A40
                || uiAddr == 0x4C03
                || uiAddr == 0x5023
                || uiAddr == 0x50AB
                || uiAddr == 0x50BF
                || uiAddr == 0x5332
                || uiAddr == 0x60D8
                || uiAddr == 0x609C
                || uiAddr == 0x60F2
                || uiAddr == 0x6870
                || uiAddr == 0x6A07
                || uiAddr == 0x6AEA
                || uiAddr == 0x729B
                || uiAddr == 0x742B )
                return false;
        }
        return true;
    }

    // Returns true if pAddr is in protected slow mem area
    bool IsProtectedSlowMem( const void* pAddr )
    {
        uint uiAddr = (uint)pAddr >> 12;
        if ( uiAddr >= 0x401
            && uiAddr < 0x8A4
            && uiAddr != 0x4C0
            && uiAddr != 0x502
            && uiAddr != 0x50A
            && uiAddr != 0x50B
            && uiAddr != 0x533
            && uiAddr != 0x60D
            && uiAddr != 0x609
            && uiAddr != 0x60F
            && uiAddr != 0x687
            && uiAddr != 0x6A0
            && uiAddr != 0x6AE
            && uiAddr != 0x729
            && uiAddr != 0x742 )
            return true;
        return false;
    }

    // Temporarily unprotect slow mem area
    SMemWrite OpenMemWrite( const void* pAddr, uint uiAmount )
    {
        SMemWrite hMem;

        // Check for incorrect use of function
        if ( !IsSlowMem( pAddr, uiAmount ) )
        {
            dassert( 0 && "Should use Mem*Fast function" );
            hMem.dwFirstPage = 0;
            return hMem;
        }

        // Checks
        assert( IsMainThread() );

        // Calc info for VirtualProtect
        hMem.dwFirstPage = ((DWORD)pAddr) & ~0xFFF;
        DWORD dwLastPage = (((DWORD)pAddr) + uiAmount - 1) & ~0xFFF;
        hMem.dwSize = dwLastPage - hMem.dwFirstPage + 0x1000;
        VirtualProtect((LPVOID)hMem.dwFirstPage, 0x1000, PAGE_EXECUTE_READWRITE, &hMem.oldProt);

        // Make sure not using this slow function too much
        OutputDebugLine( SString( "[Mem] OpenMemWrite at %08x for %d bytes  (oldProt:%04x)", pAddr, uiAmount, hMem.oldProt ) );

#ifdef MTA_DEBUG
        // Checks
        if ( IsProtectedSlowMem( (const void*)hMem.dwFirstPage ) )
            assert( hMem.oldProt == PAGE_EXECUTE_READ || hMem.oldProt == PAGE_READONLY );
        else
            assert( hMem.oldProt == PAGE_EXECUTE_READWRITE || hMem.oldProt == PAGE_EXECUTE_WRITECOPY );
#endif

        // Extra if more than one page
        for ( uint i = 0x1000 ; i < hMem.dwSize ; i += 0x1000 )
        {
            DWORD oldProtCheck;
            VirtualProtect((LPVOID)(hMem.dwFirstPage + i), 0x1000, PAGE_EXECUTE_READWRITE, &oldProtCheck);
            dassert( hMem.oldProt == oldProtCheck );
        }

        return hMem;
    }

    // Restore slow mem area protection
    void CloseMemWrite( SMemWrite& hMem )
    {
        if ( hMem.dwFirstPage == 0 )
            return;
        DWORD oldProt;
        VirtualProtect((LPVOID)hMem.dwFirstPage, hMem.dwSize, hMem.oldProt, &oldProt);
        dassert( oldProt == PAGE_EXECUTE_READWRITE );
    }

}
