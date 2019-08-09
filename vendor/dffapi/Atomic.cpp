#pragma once

#include "Atomic.h"
#include "StdInc.h"

gtaRwAtomic::gtaRwAtomic()
{
	memset(this, 0, sizeof(gtaRwAtomic));
}

gtaRwBool gtaRwAtomic::StreamRead(gtaRwStream *Stream, gtaRwBool GeometryListIsEmpty, gtaRwUInt32 ClumpVersion)
{
	gtaRwUInt32 length, entryLength, type;
	memset(this, 0, sizeof(gtaRwAtomic));
	if(!gtaRwStreamFindChunk(Stream, rwID_ATOMIC, NULL, NULL))
		return false;
	if(!gtaRwStreamFindChunk(Stream, rwID_STRUCT, &length, NULL))
		return false;
	if(gtaRwStreamRead(Stream, &frameIndex, length) != length)
		return false;
	if(GeometryListIsEmpty)
	{
		internalGeometry = (gtaRwGeometry *)malloc(sizeof(gtaRwGeometry));
		memset(internalGeometry, 0, sizeof(gtaRwGeometry));
		if(!internalGeometry->StreamRead(Stream, ClumpVersion))
		{
			Destroy();
			return false;
		}
	}
	if(!gtaRwStreamFindChunk(Stream, rwID_EXTENSION, &length, NULL))
	{
		Destroy();
		return false;
	}
	while(length && gtaRwStreamReadChunkHeader(Stream, &type, &entryLength, NULL, NULL))
	{
		switch(type)
		{
		case rwID_RIGHTTORENDER:
			if(Extension.rights[0].enabled)
			{
				if(!Extension.rights[1].enabled)
				{
					if(!Extension.rights[1].StreamRead(Stream))
					{
						Destroy();
						return false;
					}
				}
				else
				{
					if(!gtaRwStreamSkip(Stream, entryLength))
					{
						Destroy();
						return false;
					}
				}
			}
			else
			{
				if(!Extension.rights[0].StreamRead(Stream))
				{
					Destroy();
					return false;
				}
			}
			break;
		case rwID_MATFX:
			if(!Extension.matFx.StreamRead(Stream))
			{
				Destroy();
				return false;
			}
			break;
		case gtaID_PIPELINE:
			if(!Extension.pipeline.StreamRead(Stream))
			{
				Destroy();
				return false;
			}
			break;
		default:
			if(!gtaRwStreamSkip(Stream, entryLength))
			{
				Destroy();
				return false;
			}
		}
		length += -12 - entryLength;
	}
	return true;
}

gtaRwBool gtaRwAtomic::StreamWrite(gtaRwStream *Stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_ATOMIC, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_STRUCT, 16, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(Stream, &frameIndex, 16))
		return false;
	if(internalGeometry)
	{
		if(!internalGeometry->StreamWrite(Stream))
			return false;
	}
	if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_EXTENSION, Extension.rights[0].GetStreamSize() + 
		Extension.rights[1].GetStreamSize() + Extension.pipeline.GetStreamSize() + Extension.matFx.GetStreamSize(), 
		gtaRwVersion, gtaRwBuild))
	{
		return false;
	}
	if(!Extension.rights[0].StreamWrite(Stream))
		return false;
	if(!Extension.rights[1].StreamWrite(Stream))
		return false;
	if(!Extension.matFx.StreamWrite(Stream))
		return false;
	if(!Extension.pipeline.StreamWrite(Stream))
		return false;
	return true;
}

gtaRwUInt32 gtaRwAtomic::GetStreamSize()
{
	gtaRwUInt32 size = 52 + Extension.rights[0].GetStreamSize() + Extension.rights[1].GetStreamSize() + Extension.pipeline.GetStreamSize() +
		Extension.matFx.GetStreamSize();
	if(internalGeometry)
		size += internalGeometry->GetStreamSize();
	return size;
}

void gtaRwAtomic::Initialise(gtaRwInt32 FrameIndex, gtaRwInt32 GeometryIndex, gtaRwUInt32 Flags, gtaRwBool UsesInternalGeometry)
{
	memset(this, 0, sizeof(gtaRwAtomic));
	frameIndex = FrameIndex;
	geometryIndex = GeometryIndex;
	flags = Flags;
	if(UsesInternalGeometry)
	{
		internalGeometry = (gtaRwGeometry *)malloc(sizeof(gtaRwGeometry));
		memset(internalGeometry, 0, sizeof(gtaRwGeometry));
	}
}

void gtaRwAtomic::Destroy()
{
	if(internalGeometry)
	{
		internalGeometry->Destroy();
		free(internalGeometry);
	}
	Extension.matFx.Destroy();
	Extension.rights[0].Destroy();
	Extension.rights[1].Destroy();
	Extension.pipeline.Destroy();
	memset(this, 0, sizeof(gtaRwAtomic));
}