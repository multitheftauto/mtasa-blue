#pragma once

#include "NodeName.h"
#include "StdInc.h"

gtaFrameNodeName::gtaFrameNodeName()
{
	memset(this, 0, sizeof(gtaFrameNodeName));
}

bool gtaFrameNodeName::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_NODENAME, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(name)
		{
			if(!gtaRwStreamWrite(stream, name, strlen(name)))
				return false;
		}
	}
	return true;
}

bool gtaFrameNodeName::StreamRead(gtaRwStream *stream, gtaRwUInt32 length)
{
	memset(this, 0, sizeof(gtaFrameNodeName));
	enabled = true;
	if(length > 0)
	{
		name = (gtaRwChar *)malloc(length + 1);
		memset(name, 0, length + 1);
		if(gtaRwStreamRead(stream, name, length) != length)
		{
			Destroy();
			return false;
		}
		if(name[0] == '\0')
		{
			free(name);
			name = NULL;
		}
	}
	return true;
}

unsigned int gtaFrameNodeName::GetStreamSize()
{
	if(enabled)
	{
		gtaRwUInt32 size = 12;
		if(name)
			size += strlen(name);
		return size;
	}
	return 0;
}

void gtaFrameNodeName::Initialise(gtaRwChar *Name)
{
	memset(this, 0, sizeof(gtaFrameNodeName));
	enabled = true;
	if(Name && Name[0] != '\0')
	{
		name = (gtaRwChar *)malloc(strlen(Name) + 1);
		memset(name, 0, strlen(Name) + 1);
		strcpy(name, Name);
	}
}

void gtaFrameNodeName::Destroy()
{
	if(name)
		free(name);
	memset(this, 0, sizeof(gtaFrameNodeName));
}