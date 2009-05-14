#include <stdlib.h>

void *RakNet_malloc( size_t size  )
{
	return malloc(size);
}
void *RakNet_realloc( void *memblock, size_t size )
{
	return realloc(memblock, size);
}
void RakNet_free( void *memblock  )
{
	free(memblock);
}
