/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.mem.cpp
*  PURPOSE:     RenderWare memory management implementation
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include <StdInc.h>

// This basically is a complete rewrite of the internal RenderWare memory management.
// Feel free to make changes (if you are daring).

static bool _allowPreallocation = false;

#define DIV_CEIL( num, sector ) (((num) + ((sector) - 1)) / (sector))
#define ALIGN_SIZE( num, sector ) ( ALIGN( (num), (sector), (sector) ) )

static RwFreeList *freeListMemory = NULL;       // Manager of all free list allocations.
static bool isMemoryInitiated = false;          // True whether system successfully initiated.
static RwList <RwFreeList> freeListRoot;        // List of all free lists (except freeListMemory var)

/*=========================================================
    _DeallocFreeList

    Arguments:
        list - RenderWare memory management struct
    Purpose:
        Deallocates all memory which is associated with the
        given RenderWare free list.
    Note:
        This function has been inlined into RwFreeListCreateEx
        and RwFreeListDestroy.
=========================================================*/
__forceinline void _DeallocFreeList( RwFreeList *list )
{
    // free the list again.
    LIST_FOREACH_BEGIN( RwFreeListMemBlock, list->m_memBlocks.root, list )
        LIST_REMOVE( item->list );

        RenderWare::GetInterface()->m_memory.m_free( item );
    LIST_FOREACH_END

    if ( list->m_flags == RWALLOC_SELFCONSTRUCT )
        return;

    // If we have free list information of free lists, free using it.
    RwFreeList *freeListInfo = freeListMemory;

    if ( list != freeListInfo && freeListInfo )
    {
        RenderWare::GetInterface()->m_freeStruct( freeListInfo, list );
    }
    else
    {
        // Free using global allocator.
        RenderWare::GetInterface()->m_memory.m_free( list );
    }
}

/*=========================================================
    RwFreeListSetPreallocationEnabled

    Arguments:
        enabled - boolean to turn preallocation on or off
    Purpose:
        Determines whether free list preallocation should be
        enabled or not. If enabled then the allocCount parameter
        to RwFreeListCreateEx is not set to 0.
    Binary offsets:
        (1.0 US): 0x008019B0
        (1.0 EU): 0x00801970
    Note:
        By default, RenderWare enabled pre-allocation during
        start-up.
=========================================================*/
void __cdecl RwFreeListSetPreallocationEnabled( unsigned int enabled )
{
    // Determines whether free lists may preallocate memory during construction.
    _allowPreallocation = enabled != 0;
}

/*=========================================================
    _RwFreeListAllocateBlock

    Arguments:
        list - RenderWare memory management struct
        blockCount - number of possible allocations per data chunk
        blockSize - total block size (including alignment)
        metaDataSize - size of meta data of chunk
        alignment - number of bytes used for block alignment
        allocFlags - passed to malloc function
    Purpose:
        Allocates another data block for the RenderWare free list
        and returns it. It is inserted into the free list if successful.
        If the memory fails to allocate, NULL is returned.
    Note:
        This function has been inlined into RwFreeListCreateEx
        and RwFreeListAllocate.
=========================================================*/
__forceinline RwFreeListMemBlock* _RwFreeListAllocateBlock( RwFreeList *list, unsigned int blockCount, size_t blockSize, size_t metaDataSize, size_t alignment, unsigned int allocFlags )
{
    const size_t allocSize = blockCount * blockSize + metaDataSize + (alignment + 7);

    // Blocks contain a header, metaData and the actual data blocks.
    // The sections are placed in said order.
    RwFreeListMemBlock *block = (RwFreeListMemBlock*)RenderWare::GetInterface()->m_memory.m_malloc( allocSize, allocFlags );

    if ( !block )
        return NULL;

    // Insert the new memory chunk into the free list.
    LIST_INSERT( list->m_memBlocks.root, block->list );

    void *memBuff = block + 1;

    // Initialize the meta data
    memset( memBuff, 0, metaDataSize );
    return block;
}

/*=========================================================
    RwFreeListCreateEx

    Arguments:
        dataSize - size of the data structure to allocate from the free list
        blockCount - number of blocks per allocated data chunk
        alignment - number of bytes used for block alignment
        allocCount - number of chunks to preallocate
        list - if specified, the RenderWare memory system is skipped
               and the given memory region used for the free list
        allocFlags - flags passed to malloc function
    Purpose:
        Creates a new memory management structure and returns it.
        If memory failed to allocate for the free list itself or
        any pre-allocated memory blocks, NULL is returned. The
        free list is added to the global list of all free lists.
    Binary offsets:
        (1.0 US): 0x008019B0
        (1.0 EU): 0x008019F0
    Note:
        All plugin data types are stored in free lists. You can
        count the total memory RenderWare memory usage by investigating
        them.
=========================================================*/
RwFreeList* __cdecl RwFreeListCreateEx( unsigned int dataSize, unsigned int blockCount, size_t alignment, unsigned int allocCount, RwFreeList *list, unsigned int allocFlags )
{
    if ( !_allowPreallocation )
        allocCount = 0;

    if ( !alignment )
        alignment = 4;

    // Did the runtime provide a memory space for our free list?
    if ( !list )
    {
        // Allocate a slot for us
        if ( RwFreeList *freeListInfo = freeListMemory )
            list = (RwFreeList*)RenderWare::GetInterface()->m_allocStruct( freeListInfo, allocFlags & 0xFF0000 );
        else
            list = (RwFreeList*)RenderWare::GetInterface()->m_memory.m_malloc( sizeof(RwFreeList), allocFlags & 0xFF0000 );

        if ( !list )
            return NULL;

        // Mark that we support garbage collection!
        list->m_flags = RWALLOC_RUNTIME;
    }
    else
        list->m_flags = RWALLOC_RUNTIME | RWALLOC_SELFCONSTRUCT;

    size_t blockSize = ALIGN_SIZE( dataSize, alignment );
    size_t metaDataSize = DIV_CEIL( blockCount, 8 );

    // Set up free list properties.
    list->m_blockSize = blockSize;
    list->m_blockCount = blockCount;
    list->m_metaDataSize = metaDataSize;
    list->m_alignment = alignment;
    LIST_CLEAR( list->m_memBlocks.root );

    // Preallocate the requested memory chunks.
    while ( allocCount )
    {
        if ( !_RwFreeListAllocateBlock( list, blockCount, blockSize, metaDataSize, alignment, allocFlags ) )
        {
            // If any memory chunk failed to pre-allocate, we destroy the free list (prematurely).
            _DeallocFreeList( list );
            return NULL;
        }

        allocCount--;
    }

    LIST_INSERT( freeListRoot.root, list->m_globalLists );
    return list;
}

/*=========================================================
    RwFreeListCreate

    Arguments:
        dataSize - size of the data structure to allocate from the free list
        blockCount - number of blocks per allocated data chunk
        alignment - number of bytes used for block alignment
        allocFlags - flags passed to malloc function
    Purpose:
        Creates a RenderWare free list (simplified version of
        RwFreeListCreateEx). By default, one data chunk is pre-allocated.
        If successful, the free list is returned.
    Binary offsets:
        (1.0 US): 0x00801980
        (1.0 EU): 0x008019C0
=========================================================*/
RwFreeList* __cdecl RwFreeListCreate( size_t dataSize, unsigned int blockCount, size_t alignment, unsigned int allocFlags )
{
    return RwFreeListCreateEx( dataSize, blockCount, alignment, 1, NULL, allocFlags );
}

/*=========================================================
    RwFreeListDestroy

    Arguments:
        list - RenderWare memory management struct
    Purpose:
        Destroys an active RenderWare free list. All memory which
        is assigned to it is free'd. Then the free list is
        removed from the global list of free lists.
    Binary offsets:
        (1.0 US): 0x00801B80
        (1.0 EU): 0x00801BC0
=========================================================*/
unsigned int __cdecl RwFreeListDestroy( RwFreeList *list )
{
    // Since the free list is assumed to be properly constructed,
    // we remove it from the free list registry.
    LIST_REMOVE( list->m_globalLists );

    _DeallocFreeList( list );
    return 1;
}

/*=========================================================
    RwFreeListSetFlags

    Arguments:
        list - RenderWare memory management struct
        flags - new flags which will replace the current ones
    Purpose:
        Sets flags to a RenderWare free list. They are mainly
        used to determine allocation techniques.
    Binary offsets:
        (1.0 US): 0x00801C10
        (1.0 EU): 0x00801C50
=========================================================*/
void __cdecl RwFreeListSetFlags( RwFreeList *list, unsigned int flags )
{
    list->m_flags = flags & ~1;
}

/*=========================================================
    RwFreeListGetFlags

    Arguments:
        list - RenderWare memory management struct
    Purpose:
        Returns the allocation flags of the RenderWare free list.
    Binary offsets:
        (1.0 US): 0x00801C20
        (1.0 EU): 0x00801C60
=========================================================*/
unsigned int __cdecl RwFreeListGetFlags( RwFreeList *list )
{
    return list->m_flags;
}

/*=========================================================
    RwFreeListBlockAllocateChunk

    Arguments:
        block - memory chunk inside of a free list
        blockCount - number of blocks in this chunk
        blockSize - size of the block (including alignment)
        alignment - number of bytes used for alignment
        metaDataSize - size of the meta data in this chunk
    Purpose:
        Attempts to allocate a memory block in the given chunk
        and returns it if successful.
    Note:
        This function has been inlined into RwFreeListAllocate.
=========================================================*/
#define REVERSE_BIT_FIELD( cnt ) ( 0x80 >> (cnt) )
#define IS_BIT_SET_REVERSE( num, cnt ) ( IS_ANY_FLAG( (num), REVERSE_BIT_FIELD( cnt ) ) )

__forceinline void* RwFreeListBlockAllocateChunk( RwFreeListMemBlock *block, unsigned int blockCount, size_t blockSize, size_t alignment, size_t metaDataSize )
{
    unsigned char *metaData = block->GetMetaData();
    
    for ( unsigned int i = 0; i < metaDataSize; i++ )
    {
        unsigned char byte = metaData[i];

        // If all slots are taken, we can skip right away.
        if ( byte == 0xFF )
        {
            blockCount -= 8;
            continue;
        }

        for ( unsigned int j = 0; j < 8; j++, blockCount-- )
        {
            if ( blockCount == 0 )
                break;  // was an error introduced here because of not wanting to use goto?

            unsigned char checkAgainst = REVERSE_BIT_FIELD( j );

            if ( !IS_ANY_FLAG( byte, checkAgainst ) )
            {
                // Mark as used.
                byte |= checkAgainst;
                metaData[i] = byte;

                // Return the chunk
                if ( void *data = block->GetBlockPointer( i * 8 + j, blockSize, alignment, metaDataSize ) )
                    return data;
            }
        }
    }

    return NULL;
}

/*=========================================================
    RwFreeListAllocate

    Arguments:
        list - RenderWare memory management struct
        allocFlags - flags passed to malloc function
    Purpose:
        Attempts to allocate a memory block from the free list.
        Returns a pointer to it if successful. If no block was
        found available in the already allocated memory chunks,
        a new memory chunk is allocated.
    Binary offsets:
        (1.0 US): 0x00801C30
        (1.0 EU): 0x00801C70
=========================================================*/
void* __cdecl RwFreeListAllocate( RwFreeList *list, unsigned int allocFlags )
{
    RwList <RwFreeListMemBlock>& root = list->m_memBlocks;
    size_t metaDataSize = list->m_metaDataSize;
    size_t blockSize = list->m_blockSize;
    size_t alignment = list->m_alignment;

    // Try to allocate in already allocated chunks.
    LIST_FOREACH_BEGIN( RwFreeListMemBlock, root.root, list )
        // The_GTA: I have opimized this loop by using the data variable directly.
        // Maybe Criterion did not use inlining, because this function looked messy.
        if ( void *data = RwFreeListBlockAllocateChunk( item, list->m_blockCount, blockSize, alignment, metaDataSize ) )
            return data;
    LIST_FOREACH_END

    // We need a new chunk. Allocate it.
    RwFreeListMemBlock *block = _RwFreeListAllocateBlock( list, list->m_blockCount, blockSize, metaDataSize, alignment, allocFlags );

    // Allocate first chunk
    block->SetBlockUsed( 0 );
    return block->GetBlockPointer( 0, blockSize, alignment, metaDataSize );
}

/*=========================================================
    _RwFreeListGetValidBlock

    Arguments:
        list - RenderWare memory management struct
        ptr - pointer to a theoretically valid data block
        metaDataSize - size of meta data for the free list
        blockSize - size of every block in chunks
        alignment - number of bytes used for alignment
        firstBlock - if successful, the pointer to the first
                     block in the returned chunk is written
    Purpose:
        Scans the given free list for a memory chunk which
        can resolve the given pointer and returns the correct
        memory chunk. Only the block regions of the memory
        chunk are valid, not meta data and not header. Returns
        NULL if no valid block was found.
    Note:
        This function has been inlined into RwFreeListFree.
=========================================================*/
__forceinline RwFreeListMemBlock* _RwFreeListGetValidBlock( RwFreeList *list, void *ptr, size_t metaDataSize, size_t blockSize, size_t alignment, void*& firstBlock )
{
    RwList <RwFreeListMemBlock>& root = list->m_memBlocks;
    
    // Check whether block pointer is valid.
    LIST_FOREACH_BEGIN( RwFreeListMemBlock, root.root, list )
        void *fBlock = item->GetBlockPointer( 0, blockSize, alignment, metaDataSize );

        if ( ptr >= fBlock &&
             ptr <= (unsigned char*)fBlock + list->m_blockCount * blockSize )
        {
            firstBlock = fBlock;
            return item;
        }
    LIST_FOREACH_END

    // The given pointer is not resolved in the given free list.
    return NULL;
}

/*=========================================================
    RwFreeListBlockFree

    Arguments:
        block - memory chunk inside of a free list
        metaDataSize - size of the chunk's meta data
        metaData - pointer to the chunk's meta data
    Purpose:
        Attempts to free the given memory chunk. It succeeds
        if there is no block allocated in the chunk anymore.
        Otherwise it fails and returns false.
    Note:
        This function has been inlined into RwFreeListFree and
        RwFreeListPurge.
=========================================================*/
__forceinline bool RwFreeListBlockFree( RwFreeListMemBlock *block, size_t metaDataSize, unsigned char *metaData )
{
    // Free the block if possible (that is, it ain't used anymore)
    for ( unsigned int n = 0; n < metaDataSize; n++ )
    {
        // The_GTA: I optimized this function by checking if any meta data slot is != 0.
        // Previously it added all blocks together and checked for != 0 after the loop,
        // which is not required (same result, less CPU usage).
        if ( metaData[n] != 0 )
            return false;
    }

    // Unregister the block (kind of like garbage collection)
    LIST_REMOVE( block->list );

    RenderWare::GetInterface()->m_memory.m_free( block );
    return true;
}

/*=========================================================
    RwFreeListFree

    Arguments:
        list - RenderWare memory management struct
        ptr - pointer to data block (or inside of it)
    Purpose:
        Attempts to deallocate a block inside of the given
        free list. If successful, the return value is != NULL.
    Binary offsets:
        (1.0 US): 0x00801D50
        (1.0 EU): 0x00801D90
=========================================================*/
RwFreeList* __cdecl RwFreeListFree( RwFreeList *list, void *ptr )
{
    // Cache important values
    size_t metaDataSize = list->m_metaDataSize;
    size_t blockSize = list->m_blockSize;
    size_t alignment = list->m_alignment;

    void *firstBlock;
    RwFreeListMemBlock *memBlock = _RwFreeListGetValidBlock( list, ptr, metaDataSize, blockSize, alignment, firstBlock );

    if ( !memBlock )
        return NULL;    // failed to find valid block

    // Calculate basic values
    unsigned int blockOffset = (char*)ptr - (char*)firstBlock;
    unsigned int blockIndex = blockOffset / list->m_blockSize;

    unsigned char *metaData = memBlock->GetMetaData();

    // Deallocate the block
    RwFreeListMemBlock::SetBlockUsed( metaData, blockIndex, false );

    if ( list->m_flags & RWALLOC_RUNTIME )
        RwFreeListBlockFree( memBlock, metaDataSize, metaData );

    return list;
}

/*=========================================================
    RwFreeListForAllUsedBlocks

    Arguments:
        list - RenderWare memory management struct
        callback - function to call for every active block
        ud - user data to pass to the callback
    Purpose:
        Loops through all memory chunks and their respective
        blocks. If any block is allocated, the callback is called
        and the block passed to it.
    Binary offsets:
        (1.0 US): 0x00801E90
        (1.0 EU): 0x00801ED0
=========================================================*/
void __cdecl RwFreeListForAllUsedBlocks( RwFreeList *list, void (__cdecl*callback)( void *block, void *ud ), void *ud )
{
    // Cache important values
    size_t metaDataSize = list->m_metaDataSize;
    size_t blockSize = list->m_blockSize;
    size_t alignment = list->m_alignment;

    RwList <RwFreeListMemBlock>& root = list->m_memBlocks;

    LIST_FOREACH_BEGIN( RwFreeListMemBlock, root.root, list )
        unsigned char *metaCopy = (unsigned char*)RenderWare::GetInterface()->m_memory.m_malloc( metaDataSize, 0x10000 );

        if ( !metaCopy )
            return;

        // We need to create the copy so that deallocated blocks will still be called upon.
        // Do not point at me! Criterion wanted this!
        memcpy( metaCopy, item->GetMetaData(), metaDataSize );

        for ( unsigned int n = 0; n < metaDataSize; n++ )
        {
            unsigned char byte = metaCopy[n];

            if ( byte == 0 )
                continue;

            for ( unsigned int i = 0; i < 8; i++ )
            {
                unsigned char checkAgainst = REVERSE_BIT_FIELD( i );

                if ( IS_ANY_FLAG( byte, checkAgainst ) )
                    callback( item->GetBlockPointer( i, blockSize, alignment, metaDataSize ), ud );
            }
        }

        RenderWare::GetInterface()->m_memory.m_free( metaCopy );
    LIST_FOREACH_END
}

/*=========================================================
    RwFreeListPurge

    Arguments:
        list - RenderWare memory management struct
    Purpose:
        Scans through the free list and deallocates all memory
        chunks which have no more allocated blocks in them.
        Returns the amount of memory free'd.
    Binary offsets:
        (1.0 US): 0x00801E00
        (1.0 EU): 0x00801E40
=========================================================*/
size_t __cdecl RwFreeListPurge( RwFreeList *list )
{
    RwList <RwFreeListMemBlock>& root = list->m_memBlocks;
    size_t metaDataSize = list->m_metaDataSize;
    unsigned int purgedItems = 0;

    LIST_FOREACH_BEGIN( RwFreeListMemBlock, root.root, list )
        LIST_REMOVE( item->list );

        if ( RwFreeListBlockFree( item, metaDataSize, item->GetMetaData() ) )
            purgedItems++;
        else
        {
            LIST_INSERT( root.root, item->list );
        }
    LIST_FOREACH_END

    return purgedItems * list->m_blockSize;
}

/*=========================================================
    RwFreeListPurgeAll

    Purpose:
        Loops through all available free lists and deallocates
        memory chunks which have no more block allocations inside
        of them. Returns the total amount of memory free'd.
    Binary offsets:
        (1.0 US): 0x00801F90
        (1.0 EU): 0x00801FD0
=========================================================*/
size_t __cdecl RwFreeListPurgeAll( void )
{
    RwList <RwFreeList>& root = freeListRoot;
    size_t totalPurged = 0;

    LIST_FOREACH_BEGIN( RwFreeList, root.root, m_globalLists )
        size_t purged = RwFreeListPurge( item );

        if ( purged != 0 )
            totalPurged += purged;
    LIST_FOREACH_END

    return totalPurged;
}

/*=========================================================
    RwMemoryAllocate

    Arguments:
        size - size of data to allocate
        flags - reserved (a.k.a. unused)
    Purpose:
        Allocates a memory block of size.
    Binary offsets:
        (1.0 US): 0x008020A0
        (1.0 EU): 0x008020E0
=========================================================*/
static void* __cdecl RwMemoryAllocate( size_t size, unsigned int flags )
{
    return malloc( size );
}

/*=========================================================
    RwMemoryFree

    Arguments:
        ptr - pointer to previously allocated memory
    Purpose:
        Deallocated a memory block which was previously
        allocated through this system.
    Note:
        RenderWare linked to CRT free function directly.
=========================================================*/
static void __cdecl RwMemoryFree( void *ptr )
{
    free( ptr );
}

/*=========================================================
    RwMemoryRealloc

    Arguments:
        ptr - pointer to previously allocated memory
        size - new size of block at ptr
    Purpose:
        Attempts to reuse ptr and allocate a new block of
        size at it's position. If it failed, the address of
        the data is changed. Returns the actual pointer to
        the memory.
    Binary offsets:
        (1.0 US): 0x008020B0
        (1.0 EU): 0x008020F0
=========================================================*/
static void* __cdecl RwMemoryRealloc( void *ptr, size_t size, unsigned int flags )
{
    return realloc( ptr, size );
}

/*=========================================================
    RwMemoryCellAlloc

    Arguments:
        count - number of cells to row
        cellSize - size of individual cells
    Purpose:
        Allocates a chunk of count * cellSize memory and
        returns the pointer to it.
    Binary offsets:
        (1.0 US): 0x008020D0
        (1.0 EU): 0x00802110
=========================================================*/
static void* __cdecl RwMemoryCellAlloc( size_t count, size_t cellSize )
{
    return calloc( count, cellSize );
}

/*=========================================================
    RwMemoryOpen

    Arguments:
        memDesc - memory callback interface structure
    Purpose:
        Initiates the RenderWare memory management system.
        It returns whether the initialization succeeded.
        If successful, the callbacks from memDesc are set
        as the memory API (if memDesc != NULL). Otherwise,
        the standard memory API is used.
    Binary offsets:
        (1.0 US): 0x00801FD0
        (1.0 EU): 0x00802010
=========================================================*/
unsigned int __cdecl RwMemoryOpen( const RwMemoryDescriptor *memDesc )
{
    // Initiate the global list of free lists
    LIST_CLEAR( freeListRoot.root );

    RwFreeList *freeListInfo = RwFreeListCreateEx( sizeof(RwFreeList), 16, 4, 0, (RwFreeList*)0x00C9A60C, 0x40000 );

    // Set up the main free list allocation interface
    freeListMemory = freeListInfo;
    isMemoryInitiated = true;

    if ( !freeListInfo )
    {
        // We failed initializing.
        isMemoryInitiated = false;
        return false;
    }

    // Remove the main free list information from the memory manager list
    LIST_REMOVE( freeListInfo->m_globalLists );

    // Set the memory allocation interface
    RwInterface *intf = RenderWare::GetInterface();

    if ( memDesc )
    {
        intf->m_memory = *memDesc;
        return true;
    }

    // No custom interface specified; default to CRT
    intf->m_memory.m_malloc = RwMemoryAllocate;
    intf->m_memory.m_free = RwMemoryFree;
    intf->m_memory.m_realloc = RwMemoryRealloc;
    intf->m_memory.m_calloc = RwMemoryCellAlloc;
    return true;
}

/*=========================================================
    RwMemoryClose

    Purpose:
        Terminates the RenderWare memory management system
        by destroying all active free lists and deallocating
        the free list storage.
    Binary offsets:
        (1.0 US): 0x008020F0
        (1.0 EU): 0x00802130
=========================================================*/
void __cdecl RwMemoryClose( void )
{
    RwList <RwFreeList>& root = freeListRoot;

    // Destroy all active free lists.
    LIST_FOREACH_BEGIN( RwFreeList, root.root, m_globalLists )
        RwFreeListDestroy( item );
    LIST_FOREACH_END

    // Deallocate the special free list storage
    _DeallocFreeList( freeListMemory );

    freeListMemory = NULL;
    isMemoryInitiated = false;
}

void RenderWareMem_Init( void )
{
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x008019B0, (DWORD)RwFreeListSetPreallocationEnabled, 5 );
        HookInstall( 0x008019C0, (DWORD)RwFreeListCreate, 5 );
        HookInstall( 0x008019F0, (DWORD)RwFreeListCreateEx, 5 );
        HookInstall( 0x00801C50, (DWORD)RwFreeListSetFlags, 5 );
        HookInstall( 0x00801C60, (DWORD)RwFreeListGetFlags, 5 );
        HookInstall( 0x00801C70, (DWORD)RwFreeListAllocate, 5 );
        HookInstall( 0x00801d90, (DWORD)RwFreeListFree, 5 );
        HookInstall( 0x00801ED0, (DWORD)RwFreeListForAllUsedBlocks, 5 );
        HookInstall( 0x00801BC0, (DWORD)RwFreeListDestroy, 5 );
        HookInstall( 0x00801E40, (DWORD)RwFreeListPurge, 5 );
        HookInstall( 0x00801FD0, (DWORD)RwFreeListPurgeAll, 5 );
        HookInstall( 0x00802010, (DWORD)RwMemoryOpen, 5 );
        HookInstall( 0x00802130, (DWORD)RwMemoryClose, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x00801970, (DWORD)RwFreeListSetPreallocationEnabled, 5 );
        HookInstall( 0x00801980, (DWORD)RwFreeListCreate, 5 );
        HookInstall( 0x008019B0, (DWORD)RwFreeListCreateEx, 5 );
        HookInstall( 0x00801C10, (DWORD)RwFreeListSetFlags, 5 );
        HookInstall( 0x00801C20, (DWORD)RwFreeListGetFlags, 5 );
        HookInstall( 0x00801C30, (DWORD)RwFreeListAllocate, 5 );
        HookInstall( 0x00801D50, (DWORD)RwFreeListFree, 5 );
        HookInstall( 0x00801E90, (DWORD)RwFreeListForAllUsedBlocks, 5 );
        HookInstall( 0x00801B80, (DWORD)RwFreeListDestroy, 5 );
        HookInstall( 0x00801E00, (DWORD)RwFreeListPurge, 5 );
        HookInstall( 0x00801F90, (DWORD)RwFreeListPurgeAll, 5 );
        HookInstall( 0x00801FD0, (DWORD)RwMemoryOpen, 5 );
        HookInstall( 0x008020F0, (DWORD)RwMemoryClose, 5 );
        break;
    }
}

void RenderWareMem_Shutdown( void )
{
}
