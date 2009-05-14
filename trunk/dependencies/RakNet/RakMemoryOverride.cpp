#include "RakMemoryOverride.h"
#include "RakAssert.h"
#include <stdlib.h>

#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
#include "PS3Includes.h"
#endif

using namespace RakNet;

#if defined(_USE_RAK_MEMORY_OVERRIDE)
	#if defined(malloc)
	#pragma push_macro("malloc")
	#undef malloc
	#define RMO_MALLOC_UNDEF
	#endif

	#if defined(realloc)
	#pragma push_macro("realloc")
	#undef realloc
	#define RMO_REALLOC_UNDEF
	#endif

	#if defined(free)
	#pragma push_macro("free")
	#undef free
	#define RMO_FREE_UNDEF
	#endif
#endif

void DefaultOutOfMemoryHandler(const char *file, const long line)
{
	(void) file;
	(void) line;
	RakAssert(0);
}

void * (*rakMalloc) (size_t size) = RakNet::_RakMalloc;
void* (*rakRealloc) (void *p, size_t size) = RakNet::_RakRealloc;
void (*rakFree) (void *p) = RakNet::_RakFree;
void* (*rakMalloc_Ex) (size_t size, const char *file, unsigned int line) = RakNet::_RakMalloc_Ex;
void* (*rakRealloc_Ex) (void *p, size_t size, const char *file, unsigned int line) = RakNet::_RakRealloc_Ex;
void (*rakFree_Ex) (void *p, const char *file, unsigned int line) = RakNet::_RakFree_Ex;
void (*notifyOutOfMemory) (const char *file, const long line)=DefaultOutOfMemoryHandler;

void SetMalloc( void* (*userFunction)(size_t size) )
{
	rakMalloc=userFunction;
}
void SetRealloc( void* (*userFunction)(void *p, size_t size) )
{
	rakRealloc=userFunction;
}
void SetFree( void (*userFunction)(void *p) )
{
	rakFree=userFunction;
}
void SetMalloc_Ex( void* (*userFunction)(size_t size, const char *file, unsigned int line) )
{
	rakMalloc_Ex=userFunction;
}
void SetRealloc_Ex( void* (*userFunction)(void *p, size_t size, const char *file, unsigned int line) )
{
	rakRealloc_Ex=userFunction;
}
void SetFree_Ex( void (*userFunction)(void *p, const char *file, unsigned int line) )
{
	rakFree_Ex=userFunction;
}
void SetNotifyOutOfMemory( void (*userFunction)(const char *file, const long line) )
{
	notifyOutOfMemory=userFunction;
}
void * (*GetMalloc()) (size_t size)
{
	return rakMalloc;
}
void * (*GetRealloc()) (void *p, size_t size)
{
	return rakRealloc;
}
void (*GetFree()) (void *p)
{
	return rakFree;
}
void * (*GetMalloc_Ex()) (size_t size, const char *file, unsigned int line)
{
	return rakMalloc_Ex;
}
void * (*GetRealloc_Ex()) (void *p, size_t size, const char *file, unsigned int line)
{
	return rakRealloc_Ex;
}
void (*GetFree_Ex()) (void *p, const char *file, unsigned int line)
{
	return rakFree_Ex;
}

void* RakNet::_RakMalloc (size_t size)
{
	return malloc(size);
}

void* RakNet::_RakRealloc (void *p, size_t size)
{
	return realloc(p,size);
}

void RakNet::_RakFree (void *p)
{
	free(p);
}

void* RakNet::_RakMalloc_Ex (size_t size, const char *file, unsigned int line)
{
	(void) file;
	(void) line;

	return malloc(size);
}

void* RakNet::_RakRealloc_Ex (void *p, size_t size, const char *file, unsigned int line)
{
	(void) file;
	(void) line;

	return realloc(p,size);
}

void RakNet::_RakFree_Ex (void *p, const char *file, unsigned int line)
{
	(void) file;
	(void) line;

	if (p)
		free(p);
}

#if defined(_USE_RAK_MEMORY_OVERRIDE)
	#if defined(RMO_MALLOC_UNDEF)
	#pragma pop_macro("malloc")
	#undef RMO_MALLOC_UNDEF
	#endif

	#if defined(RMO_REALLOC_UNDEF)
	#pragma pop_macro("realloc")
	#undef RMO_REALLOC_UNDEF
	#endif

	#if defined(RMO_FREE_UNDEF)
	#pragma pop_macro("free")
	#undef RMO_FREE_UNDEF
	#endif
#endif
