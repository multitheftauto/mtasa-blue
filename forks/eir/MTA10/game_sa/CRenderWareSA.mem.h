/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.mem.h
*  PURPOSE:     RenderWare memory management implementation
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_MEMORY_
#define _RENDERWARE_MEMORY_

// Global API
void __cdecl            RwFreeListSetPreallocationEnabled   ( unsigned int enabled );
RwFreeList* __cdecl     RwFreeListCreateEx                  ( unsigned int dataSize, unsigned int blockCount, size_t alignment,
                                                              unsigned int allocCount, RwFreeList *list, unsigned int allocFlags );
RwFreeList* __cdecl     RwFreeListCreate                    ( size_t dataSize, unsigned int blockCount, size_t alignment, unsigned int allocFlags );
unsigned int __cdecl    RwFreeListDestroy                   ( RwFreeList *list );
void __cdecl            RwFreeListSetFlags                  ( RwFreeList *list, unsigned int flags );
unsigned int __cdecl    RwFreeListGetFlags                  ( RwFreeList *list );
void* __cdecl           RwFreeListAllocate                  ( RwFreeList *list, unsigned int allocFlags );
RwFreeList* __cdecl     RwFreeListFree                      ( RwFreeList *list, void *ptr );
void __cdecl            RwFreeListForAllUsedBlocks          ( RwFreeList *list, void (__cdecl*callback)( void *block, void *ud ), void *ud );
size_t __cdecl          RwFreeListPurge                     ( RwFreeList *list );
size_t __cdecl          RwFreeListPurgeAll                  ( void );

// RenderWare initializators
unsigned int __cdecl    RwMemoryOpen( const RwMemoryDescriptor *memDesc );
void __cdecl            RwMemoryClose( void );

// Initializators
void RenderWareMem_Init( void );
void RenderWareMem_Shutdown( void );

#endif //_RENDERWARE_MEMORY_