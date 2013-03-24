/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/ijsify.h
*  PURPOSE:     Misc defines and ijsify logic
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// User-defined warnings
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : warning C0000 *MTA Developers*: "

// Concept code

/** Buffer overrun trace - attach debugger and watch out for EXCEPTION_GUARD_PAGE (0x80000001) **/
#ifdef IJSIFY
    #pragma message(__LOC__ "YOU HAVE ENABLED THE BOUNDS CHECKER. This may cause performance and/or stability issues!")

    #include <windows.h>
    #include <math.h>

    inline void* __cdecl operator new ( size_t size )
    {
        DWORD dwOld;
        DWORD dwPageSpan = ( DWORD ) ceil ( ( float ) size / 4096.0f ) * 4096;

        DWORD dwPage = 0;
        while ( dwPage == NULL ) {
            dwPage = (DWORD)VirtualAlloc ( NULL, dwPageSpan + 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
        }
        VirtualProtect ( (LPVOID)(dwPage + dwPageSpan), 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &dwOld );
        dwPage += ( dwPageSpan - size );

        return (LPVOID)dwPage;
    };

    inline void* __cdecl operator new [] ( size_t size )
    {
        DWORD dwOld;
        DWORD dwPageSpan = ( DWORD ) ceil ( ( float ) size / 4096.0f ) * 4096;

        DWORD dwPage = 0;
        while ( dwPage == NULL ) {
            dwPage = (DWORD)VirtualAlloc ( NULL, dwPageSpan + 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
        }
        VirtualProtect ( (LPVOID)(dwPage + dwPageSpan), 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &dwOld );
        dwPage += ( dwPageSpan - size );

        return (LPVOID)dwPage;
    };

    inline void __cdecl operator delete ( LPVOID pPointer )
    {
        VirtualFree ( pPointer, NULL, MEM_RELEASE );
    };

    inline void __cdecl operator delete [] ( LPVOID pPointer )
    {
        VirtualFree ( pPointer, NULL, MEM_RELEASE );
    };
#endif

// ** Detection of buffer overflows **
#define DETECT_OVERFLOWS
#ifdef DETECT_OVERFLOWS
    #include <stdlib.h>
    #include <windows.h>

    // DOESNT WORK FOR 64-BIT APPS !

    #pragma warning ( disable:4311 )
    #pragma warning ( disable:4312 )

    extern bool g_bBoundsChecker;
    extern DWORD* g_Table;
    extern DWORD* g_TableSize;
    extern DWORD g_dwTable;

    /*
    inline void* __cdecl operator new ( unsigned int uiSize )
    {
        DWORD dwOld, dwData;

        if ( !g_bBoundsChecker ) {
            dwData = (DWORD)HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, uiSize);
            VirtualProtect ( (void*)dwData, uiSize, PAGE_EXECUTE_READWRITE, &dwOld );
            return (void*)dwData;
        }
        // define the guard bytes
        DWORD dwGuardBegin = 0xBEEFFDFD;
        WORD wGuardEnd = 0xDEAC;

        // allocate ( size + 4 (begin) + 2 (end) )
        dwData = (DWORD) HeapAlloc ( GetProcessHeap (), HEAP_ZERO_MEMORY, uiSize + 6 );

        // [0   -> 4]       insert begin guard bytes
        memcpy ( (void*)(dwData), &dwGuardBegin, 4 );
        uiSize += 4;
        
        // [x   -> x+2]     insert end guard bytes
        memcpy ( (void*)(dwData + uiSize), &wGuardEnd, 2 );
        uiSize += 2;

        // set guard protection state
        VirtualProtect ( (void*)dwData, uiSize, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &dwOld );

        g_Table[g_dwTable]          = dwData;
        g_TableSize[g_dwTable++]    = (DWORD)uiSize;

        // and return the correct data pointer
        return (void*)( dwData + 4 );

    };

    inline void __cdecl operator delete ( void* pData )
    {
        HeapFree ( GetProcessHeap(), 0, pData );
    };
    */
#endif
