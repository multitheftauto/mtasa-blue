#include "IncrementalReadInterface.h"
#include <stdio.h>

unsigned int IncrementalReadInterface::GetFilePart( char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context)
{
	FILE *fp = fopen(filename, "rb");
	if (fp==0)
		return 0;
	fseek(fp,startReadBytes,SEEK_SET);
	unsigned int numRead = (unsigned int) fread(preallocatedDestination,1,numBytesToRead, fp);
	fclose(fp);
	return numRead;
}
