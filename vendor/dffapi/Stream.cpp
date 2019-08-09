#pragma once

#include "Stream.h"
#include <stdio.h>

#pragma comment(lib, "User32")

bool gtaRwErrorSet(const char *message, ...)
{
	char text[512];
	va_list ap;
	va_start(ap, message);
	vsnprintf(text, 512, message, ap);
	va_end(ap);
	MessageBoxA(NULL, text, "gtaRwApi", 0);
	return false;
}

gtaRwStream *gtaRwStreamOpen(gtaRwStreamType type, gtaRwStreamAccessType accessType, const void *pData)
{
	gtaRwStream *stream = (gtaRwStream *)malloc(sizeof(gtaRwStream));
	if(stream)
	{
		stream->type = type;
		stream->accessType = accessType;
		stream->rwOwned = 1;
		switch(type)
		{
			case rwSTREAMFILE:
				if(ftell((FILE *)pData) == -1)
				{
					free(stream);
					return NULL;
				}
				stream->Type.file.fpFile = (void *)pData;
				break;
			case rwSTREAMFILENAME:
				switch(accessType)
				{
				case rwSTREAMREAD:
					stream->Type.file.fpFile = fopen((char *)pData, "rb");
					break;
				case rwSTREAMWRITE:
					stream->Type.file.fpFile = fopen((char *)pData, "wb");
					break;
				case rwSTREAMAPPEND:
					stream->Type.file.fpFile = fopen((char *)pData, "ab");
					break;
				default:
					gtaRwErrorSet("Invalid stream access type.");
					free(stream);
					return NULL;
				}
				if(!stream->Type.file.fpFile)
				{
					gtaRwErrorSet("Error opening the file %s", pData);
					free(stream);
					return NULL;
				}
				break;
			case rwSTREAMMEMORY:
				switch(accessType)
				{
				case rwSTREAMREAD:
					stream->Type.memory.position = 0;
					stream->Type.memory.nSize = ((RwMemory *)pData)->length;
					stream->Type.memory.memBlock = ((RwMemory *)pData)->start;
					break;
				case rwSTREAMWRITE:
					stream->Type.memory.position = 0;
					stream->Type.memory.nSize = 0;
					stream->Type.memory.memBlock = 0;
					break;
				case rwSTREAMAPPEND:
					stream->Type.memory.position = ((RwMemory *)pData)->length;
					stream->Type.memory.nSize = ((RwMemory *)pData)->length;
					stream->Type.memory.memBlock = ((RwMemory *)pData)->start;
					break;
				default:
					gtaRwErrorSet("Invalid stream access type.");
					free(stream);
					return NULL;
				}
				break;
			case rwSTREAMCUSTOM:
				memcpy(&stream->Type, pData, sizeof(gtaRwStreamCustom));
				break;
			default:
				gtaRwErrorSet("Invalid stream type.");
				free(stream);
				return NULL;
		}
		return stream;
	}
	else
		return NULL;
}

bool gtaRwStreamClose(gtaRwStream * stream, void *pData)
{
	bool result;
	switch(stream->type)
	{
	case rwSTREAMFILENAME:
		result = fclose((FILE *)stream->Type.file.fpFile) == 0;
		break;
	case rwSTREAMMEMORY:
		result = true;
		if(stream->accessType != rwSTREAMREAD && pData)
		{
			((RwMemory *)pData)->start = stream->Type.memory.memBlock;
			((RwMemory *)pData)->length = stream->Type.memory.position;
		}
		break;
	case rwSTREAMCUSTOM:
		if(stream->Type.custom.sfnclose)
			stream->Type.custom.sfnclose(stream->Type.custom.data);
		result = true;
		break;
	case rwSTREAMFILE:
		result = true;
		break;
	default:
		gtaRwErrorSet("Invalid stream type.");
		free(stream);
		return NULL;
	}
	if(stream->rwOwned)
		free(stream);
	return result;
}

unsigned int gtaRwStreamRead(gtaRwStream *stream, void *buffer, unsigned int length)
{
	unsigned int numBytesRead;

	switch(stream->type)
	{
	case rwSTREAMFILE:
	case rwSTREAMFILENAME:
		numBytesRead = fread(buffer, 1, length, (FILE *)stream->Type.file.fpFile);
		if(numBytesRead != length)
		{
			if(feof((FILE *)stream->Type.file.fpFile))
				gtaRwErrorSet("At the end of the stream.");
			else
				gtaRwErrorSet("Read error on stream");
		}
		return numBytesRead;
	case rwSTREAMMEMORY:
		if(length > stream->Type.memory.nSize - stream->Type.memory.position )
		{
			gtaRwErrorSet("At the end of the stream.");
			return stream->Type.memory.nSize - stream->Type.memory.position;
		}
		else
		{
			memcpy(buffer, (const void *)(stream->Type.memory.position + stream->Type.memory.memBlock), length);
			stream->Type.memory.position += length;
			return length;
		}
	case rwSTREAMCUSTOM:
		return stream->Type.custom.sfnread(stream->Type.custom.data, buffer, length);
	}
	gtaRwErrorSet("Invalid stream type.");
	return 0;
}

gtaRwStream *gtaRwStreamSkip(gtaRwStream *stream, unsigned int offset)
{
	gtaRwStream *result; unsigned int newPosn;

	if(offset)
	{
		switch(stream->type)
		{
		case rwSTREAMFILE:
		case rwSTREAMFILENAME:
			if(fseek((FILE *)stream->Type.file.fpFile, offset, 1))
			{
				if(feof((FILE *)stream->Type.file.fpFile))
					gtaRwErrorSet("At the end of the stream.");
				result = NULL;
			}
			else
				result = stream;
			break;
		case rwSTREAMMEMORY:
			newPosn = stream->Type.memory.position + offset;
			if(newPosn <= stream->Type.memory.nSize)
			{
				stream->Type.memory.position = newPosn;
				result = stream;
			}
			else
			{
				gtaRwErrorSet("At the end of the stream.");
				result = NULL;
			}
			break;
		case rwSTREAMCUSTOM:
			result = stream->Type.custom.sfnskip(stream->Type.custom.data, offset) != 0 ? stream : NULL;
			break;
		default:
			gtaRwErrorSet("Invalid stream type.");
			result = NULL;
		}
	}
	else
		result = stream;
	return result;
}

bool gtaChunkIsComplex(gtaRwChunkHeaderInfo *chunkHeaderInfo)
{
	switch(chunkHeaderInfo->type)
	{
	case rwID_CAMERA:
    case rwID_TEXTURE:
    case rwID_MATERIAL:
    case rwID_MATLIST:
    case rwID_ATOMICSECT:
    case rwID_PLANESECT:
    case rwID_WORLD:
    case rwID_FRAMELIST:
    case rwID_GEOMETRY:
    case rwID_CLUMP:
    case rwID_LIGHT:
    case rwID_ATOMIC:
    case rwID_GEOMETRYLIST:
		return true;
    default:
		return false;
	}
}

bool gtaRwStreamReadChunkHeader(gtaRwStream *stream, unsigned int *pType, unsigned int *pLength, unsigned int *pVersion, unsigned int *pBuild)
{
	unsigned int buffer[3];
	gtaRwChunkHeaderInfo chunkHeaderInfo;
	if(gtaRwStreamRead(stream, &buffer, 12) == 12)
	{
		chunkHeaderInfo.type = buffer[0];
		chunkHeaderInfo.length = buffer[1];
		if(buffer[2] & 0xFFFF0000)
		{
			chunkHeaderInfo.version = (buffer[2] >> 16) & 0x3F | (((buffer[2] >> 14) & 0x3FF00) + 0x30000);
			chunkHeaderInfo.buildNum = (unsigned __int16)buffer[2];
		}
		else
		{
			chunkHeaderInfo.version = buffer[2] << 8;
			chunkHeaderInfo.buildNum = 0;
		}
		gtaChunkIsComplex(&chunkHeaderInfo);
		if(pType)
			*pType = chunkHeaderInfo.type;
		if(pLength)
			*pLength = chunkHeaderInfo.length;
		if(pBuild)
			*pBuild = chunkHeaderInfo.buildNum;
		if(pVersion)
			*pVersion = chunkHeaderInfo.version;
		return true;
	}
	else
	{
		gtaRwErrorSet("Read error on stream");
		return false;
	}
}

bool gtaRwStreamFindChunk(gtaRwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut)
{
	unsigned int outType, outLength, outVersion;
	if(!gtaRwStreamReadChunkHeader(stream, &outType, &outLength, &outVersion, 0))
		return 0;
	while(outType != type)
	{
		if(!gtaRwStreamSkip(stream, outLength))
			return false;
		if(!gtaRwStreamReadChunkHeader(stream, &outType, &outLength, &outVersion, 0))
			return false;
	}
	if(lengthOut)
		*lengthOut = outLength;
	if(versionOut)
		*versionOut = outVersion;
	return true;
}

bool gtaRwStreamFindChunk(gtaRwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut, unsigned int *buildOut)
{
	unsigned int outType, outLength, outVersion, outBuild;
	if(!gtaRwStreamReadChunkHeader(stream, &outType, &outLength, &outVersion, &outBuild))
		return 0;
	while(outType != type)
	{
		if(!gtaRwStreamSkip(stream, outLength))
			return false;
		if(!gtaRwStreamReadChunkHeader(stream, &outType, &outLength, &outVersion, 0))
			return false;
	}
	if(lengthOut)
		*lengthOut = outLength;
	if(versionOut)
		*versionOut = outVersion;
	if(buildOut)
		*buildOut = outBuild;
	return true;
}

gtaRwStream *gtaRwStreamReadReal(gtaRwStream *stream, float *reals, unsigned int numBytes)
{
	if(gtaRwStreamRead(stream, reals, numBytes))
		return stream;
	else
	{
		gtaRwErrorSet("Read error on stream");
		return NULL;
	}
}

gtaRwStream *gtaRwStreamWrite(gtaRwStream *stream, const void *buffer, unsigned int length)
{
	switch(stream->type)
	{
	case rwSTREAMFILE:
	case rwSTREAMFILENAME:
		if(fwrite(buffer, 1, length, (FILE *)stream->Type.file.fpFile) != length)
		{
			gtaRwErrorSet("Write error on stream");
			return NULL;
		}
		break;
	case rwSTREAMMEMORY:
		if(!stream->Type.memory.memBlock)
		{
			stream->Type.memory.memBlock = (unsigned char *)malloc(512);
			if(!stream->Type.memory.memBlock)
			{
				gtaRwErrorSet("Unable to allocate %d bytes of memory", 512);
				return NULL;
			}
			stream->Type.memory.nSize = 512;
		}
		if(stream->Type.memory.nSize - stream->Type.memory.position < length)
		{
			stream->Type.memory.memBlock = (unsigned char *)realloc(stream->Type.memory.memBlock, length >= 512 ? 
				stream->Type.memory.nSize + length : stream->Type.memory.nSize + 512);
			if(!stream->Type.memory.memBlock)
			{
				gtaRwErrorSet("Unable to allocate %d bytes of memory", length >= 512 ? 
					stream->Type.memory.nSize + length : stream->Type.memory.nSize + 512);
				return NULL;
			}
			stream->Type.memory.nSize = length >= 512 ? stream->Type.memory.nSize + length : stream->Type.memory.nSize + 512;
		}
		memcpy((void *)(stream->Type.memory.position + stream->Type.memory.memBlock), buffer, length);
		stream->Type.memory.position += length;
		break;
	case 4:
		if(!stream->Type.custom.sfnwrite(stream->Type.custom.data, buffer, length))
			return NULL;
		break;
	default:
		gtaRwErrorSet("Invalid stream type.");
		return NULL;
  }
  return stream;
}

gtaRwStream *gtaRwStreamWriteVersionedChunkHeader(gtaRwStream *stream, unsigned int type, unsigned int size, 
													   unsigned int version, unsigned short buildNum)
{
	unsigned int data[3];
	data[0] = type;
	data[1] = size;
	data[2] = buildNum | ((version & 0x3F) << 16) | ((version + 0x10000) << 14) & 0xFFC00000;
	return gtaRwStreamWrite(stream, data, 12);
}

gtaRwStream *gtaRwStreamWriteInt32(gtaRwStream *stream, unsigned int *integers, unsigned int length)
{
	gtaRwStreamWrite(stream, integers, length);
	return stream;
}