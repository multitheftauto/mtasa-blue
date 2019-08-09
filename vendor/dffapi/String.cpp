#pragma once

#include "String.h"
#include <Windows.h>
#include "StdInc.h"
#include "Memory.h"

gtaRwString::gtaRwString()
{
	memset(this, 0, sizeof(gtaRwString));
}

void gtaRwString::Initialise(gtaRwChar *String, gtaRwBool IsUnicode)
{
	memset(this, 0, sizeof(gtaRwString));
	if(String)
	{
		string = (gtaRwChar *)gtaMemAlloc(strlen(String) + 1);
		strcpy(string, String);
	}
	else
		string = NULL;
	isUnicode = IsUnicode;
}

void gtaRwString::Destroy()
{
	if(string)
		gtaMemFree(string);
	memset(this, 0, sizeof(gtaRwString));
}

bool gtaRwString::StreamRead(gtaRwStream *stream)
{
	gtaRwUInt32 type, length;
	gtaRwChar *data;
	memset(this, 0, sizeof(gtaRwString));
	if(gtaRwStreamReadChunkHeader(stream, &type, &length, NULL, NULL))
	{
		while(type != rwID_STRING && type != rwID_UNICODESTRING)
		{
			if(!gtaRwStreamSkip(stream, length))
				return false;
			if(!gtaRwStreamReadChunkHeader(stream, &type, &length, NULL, NULL))
				return false;
		}
		data = (gtaRwChar *)gtaMemAlloc(length + 1);
		if(gtaRwStreamRead(stream, data, length) != length)
		{
			gtaMemFree(data);
			return false;
		}
		if(type == rwID_UNICODESTRING)
		{
			int i;
			for(i = 0; ((gtaRwUInt16 *)data)[i]; i++)
				data[i] = ((gtaRwUInt16 *)data)[i];
			data[i] = '\0';
			isUnicode = true;
		}
		length = strlen(data);
		string = (gtaRwChar *)gtaMemAlloc(length + 1);
		strcpy(string, data);
		gtaMemFree(data);
		return true;
	}
	return false;
}

bool gtaRwString::StreamWrite(gtaRwStream *stream)
{
	gtaRwUInt32 length;
	gtaRwChar *data;
	if(isUnicode)
	{
		if(!string)
			length = 4;
		else
			length = ((strlen(string) * 2) + 4) & ~3;
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_UNICODESTRING, length, gtaRwVersion, gtaRwBuild))
			return false;
		data = (gtaRwChar *)malloc(length);
		memset(data, 0, length);
		if(string)
		{
			for(gtaRwInt32 i = 0; string[i]; i++)
				((gtaRwUInt16 *)data)[i] = string[i];
		}
	}
	else
	{
		if(!string)
			length = 4;
		else
			length = (strlen(string) + 4) & ~3;
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRING, length, gtaRwVersion, gtaRwBuild))
			return false;
		data = (gtaRwChar *)malloc(length);
		memset(data, 0, length);
		if(string)
			strcpy(data, string); 
	}
	if(!gtaRwStreamWrite(stream, data, length))
	{
		free(data);
		return false;
	}
	free(data);
	return true;
}

unsigned int gtaRwString::GetStreamSize()
{
	if(!string)
		return 16;
	else
	{
		if(isUnicode)
			return (((strlen(string) * 2) + 4) & ~3) + 12;
		else
			return ((strlen(string) + 4) & ~3) + 12;
	}
}