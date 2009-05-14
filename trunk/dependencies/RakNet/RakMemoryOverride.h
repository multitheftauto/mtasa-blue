/// \file
/// \brief If _USE_RAK_MEMORY_OVERRIDE is defined, memory allocations go through rakMalloc, rakRealloc, and rakFree
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __RAK_MEMORY_H
#define __RAK_MEMORY_H

#include "Export.h"
#include "RakNetDefines.h"
#include <new>

#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
// Causes linker errors
// #include <stdlib.h>
typedef unsigned int size_t;
#endif

#include "RakAlloca.h"

#if defined(_USE_RAK_MEMORY_OVERRIDE)
	#if defined(new)
		#pragma push_macro("new")
		#undef new
		#define RMO_NEW_UNDEF
	#endif
#endif


// These pointers are statically and globally defined in RakMemoryOverride.cpp
// Change them to point to your own allocators if you want.
// Use the functions for a DLL, or just reassign the variable if using source
extern RAK_DLL_EXPORT void * (*rakMalloc) (size_t size);
extern RAK_DLL_EXPORT void * (*rakRealloc) (void *p, size_t size);
extern RAK_DLL_EXPORT void (*rakFree) (void *p);
extern RAK_DLL_EXPORT void * (*rakMalloc_Ex) (size_t size, const char *file, unsigned int line);
extern RAK_DLL_EXPORT void * (*rakRealloc_Ex) (void *p, size_t size, const char *file, unsigned int line);
extern RAK_DLL_EXPORT void (*rakFree_Ex) (void *p, const char *file, unsigned int line);
extern RAK_DLL_EXPORT void (*notifyOutOfMemory) (const char *file, const long line);

// Change to a user defined allocation function
void RAK_DLL_EXPORT SetMalloc( void* (*userFunction)(size_t size) );
void RAK_DLL_EXPORT SetRealloc( void* (*userFunction)(void *p, size_t size) );
void RAK_DLL_EXPORT SetFree( void (*userFunction)(void *p) );
void RAK_DLL_EXPORT SetMalloc_Ex( void* (*userFunction)(size_t size, const char *file, unsigned int line) );
void RAK_DLL_EXPORT SetRealloc_Ex( void* (*userFunction)(void *p, size_t size, const char *file, unsigned int line) );
void RAK_DLL_EXPORT SetFree_Ex( void (*userFunction)(void *p, const char *file, unsigned int line) );
// Change to a user defined out of memory function
void RAK_DLL_EXPORT SetNotifyOutOfMemory( void (*userFunction)(const char *file, const long line) );


extern RAK_DLL_EXPORT void * (*GetMalloc()) (size_t size);
extern RAK_DLL_EXPORT void * (*GetRealloc()) (void *p, size_t size);
extern RAK_DLL_EXPORT void (*GetFree()) (void *p);
extern RAK_DLL_EXPORT void * (*GetMalloc_Ex()) (size_t size, const char *file, unsigned int line);
extern RAK_DLL_EXPORT void * (*GetRealloc_Ex()) (void *p, size_t size, const char *file, unsigned int line);
extern RAK_DLL_EXPORT void (*GetFree_Ex()) (void *p, const char *file, unsigned int line);


namespace RakNet
{

	template <class Type>
	RAK_DLL_EXPORT Type* OP_NEW(const char *file, unsigned int line)
	{
#if defined(_USE_RAK_MEMORY_OVERRIDE)
		char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
		Type *t = new (buffer) Type;
		return t;
#else
		(void) file;
		(void) line;
		return new Type;
#endif
	}

	template <class Type>
	RAK_DLL_EXPORT Type* OP_NEW_ARRAY(const int count, const char *file, unsigned int line)
	{
		if (count==0)
			return 0;

#if defined(_USE_RAK_MEMORY_OVERRIDE)
		Type *t;
		char *buffer = (char *) (GetMalloc_Ex())(sizeof(int)+sizeof(Type)*count, file, line);
		((int*)buffer)[0]=count;
		for (int i=0; i<count; i++)
		{
			t = new(buffer+sizeof(int)+i*sizeof(Type)) Type;
		}
		return (Type *) (buffer+sizeof(int));
#else
		(void) file;
		(void) line;
		return new Type[count];
#endif

	}

	template <class Type>
	RAK_DLL_EXPORT void OP_DELETE(Type *buff, const char *file, unsigned int line)
	{
#if defined(_USE_RAK_MEMORY_OVERRIDE)
		if (buff==0) return;
		buff->~Type();
		(GetFree_Ex())((char*)buff, file, line );
#else
		(void) file;
		(void) line;
		delete buff;
#endif

	}

	template <class Type>
	RAK_DLL_EXPORT void OP_DELETE_ARRAY(Type *buff, const char *file, unsigned int line)
	{
#if defined(_USE_RAK_MEMORY_OVERRIDE)
		if (buff==0)
			return;

		int count = ((int*)((char*)buff-sizeof(int)))[0];
		Type *t;
		for (int i=0; i<count; i++)
		{
			t = buff+i;
			t->~Type();
		}
		(GetFree_Ex())((char*)buff-sizeof(int), file, line );
#else
		(void) file;
		(void) line;
		delete [] buff;
#endif

	}

void RAK_DLL_EXPORT * _RakMalloc (size_t size);
void RAK_DLL_EXPORT * _RakRealloc (void *p, size_t size);
void RAK_DLL_EXPORT _RakFree (void *p);
void RAK_DLL_EXPORT * _RakMalloc_Ex (size_t size, const char *file, unsigned int line);
void RAK_DLL_EXPORT * _RakRealloc_Ex (void *p, size_t size, const char *file, unsigned int line);
void RAK_DLL_EXPORT _RakFree_Ex (void *p, const char *file, unsigned int line);

}

#if defined(_USE_RAK_MEMORY_OVERRIDE)
	#if defined(RMO_NEW_UNDEF)
	#pragma pop_macro("new")
	#undef RMO_NEW_UNDEF
	#endif
#endif

#endif
