/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        debugsdk/dbgheap.h
*  PURPOSE:     Heap management tools for error isolation & debugging
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef HEAP_DEBUG
#define HEAP_DEBUG

#ifdef USE_HEAP_DEBUGGING

#include <exception>
#include <new>

#pragma warning(push)
#pragma warning(disable: 4290)

// Global allocator
void* operator new( size_t memSize ) throw(std::bad_alloc);
void* operator new( size_t memSize, const std::nothrow_t nothrow ) throw();
void* operator new[]( size_t memSize ) throw(std::bad_alloc);
void* operator new[]( size_t memSize, const std::nothrow_t nothrow ) throw();
void operator delete( void *ptr ) throw();
void operator delete[]( void *ptr ) throw();

#pragma warning(pop)

#endif //USE_HEAP_DEBUGGING

void DbgHeap_Init( void );
void DbgHeap_Validate( void );
void DbgHeap_Shutdown( void );

#endif //HEAP_DEBUG