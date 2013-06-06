/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        debugsdk/dbgheap.cpp
*  PURPOSE:     Heap management tools for error isolation & debugging
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

/*
    DebugHeap memory debugging environment

    You can use this tool to find memory corruption and leaks in your C++ projects.
    It supports per-module heaps, so that errors can be isolated to game_sa, multiplayer_sa,
    deathmatch, core, GUI, etc. Use global defines in StdInc.h to set debugging properties...

    USE_HEAP_DEBUGGING
        Enables the heap debugger. The global new and delete operators are overloaded, so
        that the memory allocations are monitored. When the module terminates, all its
        memory is free'd. Requirement for DebugHeap to function.
    USE_FULL_PAGE_HEAP
        Enables full-page heap debugging. This option enables you to catch very crusty
        memory corruption issues (heavy out-of-bounds read/writes, buffer overflows, ...).
        For that the Windows Heap management is skipped. VirtualAlloc is used for every
        memory allocation, so that objects reside on their own pages.

        If full-page heap is disabled, the allocation defaults to the Windows Heap. It
        uses its own heap validation routines.

        Options can be used in combination...

        PAGE_HEAP_INTEGRITY_CHECK
            The memory is guarded by checksums on the object intro and outro regions and the
            remainder of the page is filled with a pattern. Once the memory is free'd or a
            validation is requested, the checksums and the pattern are checked using MEM_INTERRUPT.

            You have to enable this option if page heap memory should be free'd on termination.
        PAGE_HEAP_MEMORY_STATS
            Once the module terminates, all leaked memory is counted and free'd. Statistics
            are printed using OutputDebugString. This option only works with PAGE_HEAP_INTEGRITY_CHECK.

    You can define the macro MEM_INTERRUPT( bool_expr ) yourself. The most basic content is a redirect
    to assert( bool_expr ). If bool_expr is false, a memory error occured. MEM_INTERRUPT can be invoked
    during runtime and termination of your module.

    FEATURE SET:
        finds memory leaks,
        finds invalid (page heap) object free requests,
        detects memory corruption
*/

/*
=====================================================================================================================
    TRIVIA!

    You can use this space to report your own experiences with heap debugging. Sharing is caring!
    (The_GTA):
    -   If a memory object is not freed properly, DebugHeap (up to a previous version) did not report
        corruption to me. The corruption was caused by GTA:SA writing on a too small memory structure
        which was allocated in MTA code.
        -> make sure your structures are properly sized! otherwise you'd be debugging all day on clumsy errors.
           C_ASSERT is a very nice tool for that; kudos to the guy who created it.

=====================================================================================================================
*/

#ifdef USE_HEAP_DEBUGGING
#include <windows.h>
#ifndef LIST_VALIDATE
#include <rwlist.hpp>
#endif //LIST_VALIDATE

#ifndef MEM_INTERRUPT
#define MEM_INTERRUPT( expr )   assert( expr )
#endif //MEM_INTERRUPT

SYSTEM_INFO g_systemInfo;

#ifdef USE_FULL_PAGE_HEAP

// Since we know that memory is allocated along pages, we can check for
// invalid pointers given to the manager.
#define PAGE_MEM_ADJUST( ptr )  (void*)( (char*)ptr - ( (unsigned int)ptr % g_systemInfo.dwPageSize ) )

inline static void* _win32_allocMemPage( size_t memSize )
{
    return VirtualAlloc( NULL, memSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
}

inline static void _win32_freeMemPage( void *ptr )
{
    MEMORY_BASIC_INFORMATION info;

    // Do some obvious checks.
    MEM_INTERRUPT( VirtualQuery( ptr, &info, sizeof(info) ) != 0 );     // valid memory page
    MEM_INTERRUPT( info.State != MEM_FREE );                            // memory has not been free'd before
    MEM_INTERRUPT( VirtualFree( ptr, 0, MEM_RELEASE ) == TRUE );        // page release succeeded

    // WARNING: we do not check that this page memory indeed belongs to
    // the DebugHeap manager. Freeing memory from different heaps will
    // result in heap corruption. To avoid this problem, make sure you
    // deallocate memory that you know you allocated.
}

#ifdef PAGE_HEAP_INTEGRITY_CHECK

#ifndef PAGE_MEM_DEBUG_PATTERN
#define PAGE_MEM_DEBUG_PATTERN  0x6A
#endif //PAGE_MEM_DEBUG_PATTERN

#define MEM_PAGE_MOD( bytes )   ( ( (bytes) + g_systemInfo.dwPageSize - 1 ) / g_systemInfo.dwPageSize )

struct _memIntro
{
    unsigned int checksum;
    size_t  objSize;
    RwListEntry <_memIntro> memList;
};

struct _memOutro
{
    unsigned int checksum;
};

RwList <_memIntro>  g_privateMemory;

inline static void _win32_initHeap( void )
{
    LIST_CLEAR( g_privateMemory.root );
}

inline static void* _win32_allocMem( size_t memSize )
{
    _memIntro *mem = (_memIntro*)_win32_allocMemPage( memSize + sizeof(_memIntro) + sizeof(_memOutro) );
    _memOutro *outro = (_memOutro*)( (unsigned char*)( mem + 1 ) + memSize );

#ifdef PAGE_HEAP_ERROR_ON_LOWMEM
    MEM_INTERRUPT( mem == NULL );
#else
    if ( mem == NULL )
        return NULL;
#endif //PAGE_HEAP_ERROR_ON_LOWMEM

    // Fill memory with debug pattern
    memset( mem, PAGE_MEM_DEBUG_PATTERN, MEM_PAGE_MOD( memSize ) * g_systemInfo.dwPageSize );

    mem->checksum = 0xCAFEBABE;
    mem->objSize = memSize;
    LIST_APPEND( g_privateMemory.root, mem->memList );

    outro->checksum = 0xBABECAFE;

    return mem + 1;
}

inline static void _win32_checkBlockIntegrity( void *ptr )
{
    _memIntro *intro = (_memIntro*)ptr - 1;
    _memOutro *outro = (_memOutro*)( (unsigned char*)ptr + intro->objSize );

    MEM_INTERRUPT( intro->checksum == 0xCAFEBABE && outro->checksum == 0xBABECAFE );

    size_t allocSize = MEM_PAGE_MOD( intro->objSize ) * g_systemInfo.dwPageSize;
    unsigned char *endptr = (unsigned char*)intro + allocSize;
    unsigned char *seek = (unsigned char*)outro + sizeof(*outro);

    // Check memory integrity
    while ( seek < endptr )
    {
        // If this check fails, memory corruption has happened.
        MEM_INTERRUPT( *seek == PAGE_MEM_DEBUG_PATTERN );

        seek++;
    }

    LIST_VALIDATE( intro->memList );
}

inline static void _win32_freeMem( void *ptr )
{
    if ( !ptr )
        return;

    void *valid_ptr = (void*)( (_memIntro*)PAGE_MEM_ADJUST( ptr ) + 1 );
    MEM_INTERRUPT( valid_ptr == ptr );

    _win32_checkBlockIntegrity( valid_ptr );

    _memIntro *intro = (_memIntro*)valid_ptr - 1;
    LIST_REMOVE( intro->memList );

    _win32_freeMemPage( intro );
}

inline static void _win32_validateMemory( void )
{
    // Make sure the DebugHeap manager is not damaged.
    LIST_VALIDATE( g_privateMemory.root );

    // Check all blocks in order
    LIST_FOREACH_BEGIN( _memIntro, g_privateMemory.root, memList )
        _win32_checkBlockIntegrity( item + 1 );
    LIST_FOREACH_END
}

#ifdef PAGE_HEAP_MEMORY_STATS
inline static void OutputDebugStringFormat( const char *fmt, ... )
{
    char buf[0x10000];
    va_list argv;

    va_start( argv, fmt );
    _vsnprintf( buf, sizeof( buf ), fmt, argv );
    va_end( argv );

    OutputDebugString( buf );
}

inline static void _win32_shutdownHeap( void )
{
    // Make sure the DebugHeap manager is not damaged.
    LIST_VALIDATE( g_privateMemory.root );
    
    unsigned int blockCount = 0;
    unsigned int pageCount = 0;
    size_t memLeaked = 0;

    // Check all blocks in order and free them
    while ( !LIST_EMPTY( g_privateMemory.root ) )
    {
        _memIntro *item = LIST_GETITEM( _memIntro, g_privateMemory.root.next, memList );
    
        // Keep track of stats
        blockCount++;
        pageCount += MEM_PAGE_MOD( item->objSize + sizeof(_memIntro) + sizeof(_memOutro) );
        memLeaked += item->objSize;

        _win32_freeMem( item + 1 );
    }

    if ( blockCount )
    {
        OutputDebugString( "Heap Memory Leak Protocol:\n" );
        OutputDebugStringFormat(
            "* leaked memory: %u\n" \
            "* blocks/pages allocated: %u/%u [%u]\n",
            memLeaked,
            blockCount, pageCount, blockCount * g_systemInfo.dwPageSize
        );
    }
    else
        OutputDebugString( "No memory leaks detected." );
}
#else
inline static void _win32_shutdownHeap( void )
{
    LIST_VALIDATE( g_privateMemory.root );

    // Check all blocks in order and free them
    while ( !LIST_EMPTY( g_privateMemory.root ) )
        _win32_freeMem( LIST_GETITEM( _memIntro, g_privateMemory.root.next, memList ) );
}
#endif //PAGE_HEAP_MEMORY_STATS

#else
inline static void _win32_initHeap( void )
{
    return;
}

inline static void* _win32_allocMem( size_t memSize )
{
    return _win32_allocMemPage( memSize );
}

inline static void _win32_freeMem( void *ptr )
{
    if ( !ptr )
        return;

    void *valid_ptr = PAGE_MEM_ADJUST( ptr );
    MEM_INTERRUPT( valid_ptr == ptr );

    _win32_freeMemPage( valid_ptr );
}

inline static void _win32_validateMemory( void )
{
    return;
}

inline static void _win32_shutdownHeap( void )
{
    return;
}
#endif  //PAGE_HEAP_INTEGRITY_CHECK

#else
HANDLE g_privateHeap = NULL;

inline static void _win32_initHeap( void )
{
    g_privateHeap = HeapCreate( 0, 0, 0 );

    unsigned int info = 0;
    HeapSetInformation( g_privateHeap, HeapCompatibilityInformation, &info, sizeof(info) );
}

inline static void* _win32_allocMem( size_t memSize )
{
    return HeapAlloc( g_privateHeap, 0, memSize );
}

inline static void _win32_freeMem( void *ptr )
{
    if ( ptr )
    {
        MEM_INTERRUPT( HeapValidate( g_privateHeap, 0, ptr ) );
        HeapFree( g_privateHeap, 0, ptr );
    }
}

inline static void _win32_validateMemory( void )
{
    MEM_INTERRUPT( HeapValidate( g_privateHeap, 0, NULL ) );
}

inline static void _win32_shutdownHeap( void )
{
    MEM_INTERRUPT( HeapValidate( g_privateHeap, 0, NULL ) );
    HeapDestroy( g_privateHeap );
}
#endif

void* operator new( size_t memSize ) throw(std::bad_alloc)
{
    void *mem = _win32_allocMem( memSize );

    if ( !mem )
        throw std::bad_alloc( "failed to allocate memory" );

    return mem;
}

void* operator new( size_t memSize, const std::nothrow_t ) throw()
{
    return _win32_allocMem( memSize );
}

void* operator new[]( size_t memSize ) throw(std::bad_alloc)
{
    void *mem = _win32_allocMem( memSize );

    if ( !mem )
        throw std::bad_alloc( "failed to allocate memory" );

    return mem;
}

void* operator new[]( size_t memSize, const std::nothrow_t ) throw()
{
    return _win32_allocMem( memSize );
}

void operator delete( void *ptr ) throw()
{
    _win32_freeMem( ptr );
}

void operator delete[]( void *ptr ) throw()
{
    _win32_freeMem( ptr );
}

#endif

// DebugHeap initializator routine.
// Call it before CRT initialization.
void DbgHeap_Init( void )
{
#ifdef USE_HEAP_DEBUGGING
    GetSystemInfo( &g_systemInfo );

    _win32_initHeap();
#endif
}

// DebugHeap memory validation routine.
// Call it if you want to check for memory corruption globally.
void DbgHeap_Validate( void )
{
#ifdef USE_HEAP_DEBUGGING
    _win32_validateMemory();
#endif
}

// DebugHeap termination routine.
// Call it after the CRT has terminated itself.
void DbgHeap_Shutdown( void )
{
#ifdef USE_HEAP_DEBUGGING
    _win32_shutdownHeap();
#endif
}