#ifndef __FILE_OPERATIONS_H
#define __FILE_OPERATIONS_H

#include "Export.h"

bool RAK_DLL_EXPORT WriteFileWithDirectories( const char *path, char *data, unsigned dataLength );
bool RAK_DLL_EXPORT IsSlash(unsigned char c);
void RAK_DLL_EXPORT AddSlash( char *input );
void RAK_DLL_EXPORT QuoteIfSpaces(char *str);
bool RAK_DLL_EXPORT DirectoryExists(const char *directory);
unsigned int RAK_DLL_EXPORT GetFileLength(const char *path);

#endif
