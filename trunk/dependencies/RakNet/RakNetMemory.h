// This is just here so you can override it if you want, for custom memory allocation or tracking.
// Just don't include the cpp file and include your own instead.
// If you want to change new and delete how to do so is described here http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=40&rl=1
void *RakNet_malloc( size_t size  );
void *RakNet_realloc( void *memblock, size_t size );
void RakNet_free( void *memblock  );
