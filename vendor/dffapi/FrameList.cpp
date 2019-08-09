#pragma once

#include "FrameList.h"
#include "StdInc.h"

gtaRwFrame::gtaRwFrame()
{
	memset(this, 0, sizeof(gtaRwFrame));
}

void gtaRwFrame::Initialise(gtaRwV3d *Rotation, gtaRwV3d *Position, gtaRwInt32 Parent, gtaRwUInt32 MatrixFlags)
{
	memset(this, 0, sizeof(gtaRwFrame));
	if(Rotation)
		memcpy(&right, Rotation, 36);
	if(Position)
		memcpy(&pos, Position, 12);
	parent = Parent;
	matrixFlags = MatrixFlags;
}

void gtaRwFrame::Initialise(gtaRwInt32 Parent, gtaRwUInt32 MatrixFlags)
{
	memset(this, 0, sizeof(gtaRwFrame));
	right.x = 1.0f;
	up.y = 1.0f;
	at.z = 1.0f;
	parent = Parent;
	matrixFlags = MatrixFlags;
}

void gtaRwFrame::Initialise(gtaRwV3d *Right, gtaRwV3d *Up, gtaRwV3d *At, gtaRwV3d *Position, gtaRwInt32 Parent, gtaRwUInt32 MatrixFlags)
{
	memset(this, 0, sizeof(gtaRwFrame));
	if(Right)
		memcpy(&right, Right, 12);
	if(Up)
		memcpy(&up, Up, 12);
	if(At)
		memcpy(&at, At, 12);
	if(Position)
		memcpy(&pos, Position, 12);
	parent = Parent;
	matrixFlags = MatrixFlags;
}

void gtaRwFrame::Destroy()
{
	Extension.hAnim.Destroy();
	Extension.nodeName.Destroy();
	memset(this, 0, sizeof(gtaRwFrame));
}

void gtaRwFrame::SetName(gtaRwChar *Name)
{
	Extension.nodeName.Initialise(Name);
}

gtaRwFrameList::gtaRwFrameList()
{
	memset(this, 0, sizeof(gtaRwFrameList));
}

void gtaRwFrameList::Initialise(gtaRwInt32 FrameCount)
{
	memset(this, 0, sizeof(gtaRwFrameList));
	frameCount = FrameCount;
	frames = (gtaRwFrame *)malloc(sizeof(gtaRwFrame) * FrameCount);
	memset(frames, 0, sizeof(gtaRwFrame) * FrameCount);
}

void gtaRwFrameList::Destroy()
{
	if(frames)
	{
		for(gtaRwInt32 i = 0; i < frameCount; i++)
			frames[i].Destroy();
		free(frames);
	}
	memset(this, 0, sizeof(gtaRwFrameList));
}

gtaRwBool gtaRwFrameList::StreamRead(gtaRwStream *stream)
{
	gtaRwUInt32 type, length, entryLength;
	memset(this, 0, sizeof(gtaRwFrameList));
	if(!gtaRwStreamFindChunk(stream, rwID_FRAMELIST, NULL, NULL))
		return false;
	if(!gtaRwStreamFindChunk(stream, rwID_STRUCT, NULL, NULL))
		return false;
	if(gtaRwStreamRead(stream, &frameCount, 4) != 4)
		return false;
	if(frameCount > 0)
	{
		frames = (gtaRwFrame *)malloc(sizeof(gtaRwFrame) * frameCount);
		memset(frames, 0, sizeof(gtaRwFrame) * frameCount);
		for(gtaRwInt32 i = 0; i < frameCount; i++)
		{
			if(gtaRwStreamRead(stream, &frames[i], 56) != 56)
			{
				Destroy();
				return false;
			}
		}
		for(gtaRwInt32 i = 0; i < frameCount; i++)
		{
			if(!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, NULL))
			{
				Destroy();
				return false;
			}
			while(length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, NULL, NULL))
			{
				switch(type)
				{
				case rwID_HANIM:
					if(!frames[i].Extension.hAnim.StreamRead(stream))
					{
						Destroy();
						return false;
					}
					break;
				case gtaID_NODENAME:
					if(!frames[i].Extension.nodeName.StreamRead(stream, entryLength))
					{
						Destroy();
						return false;
					}
					break;
				default:
					if(!gtaRwStreamSkip(stream, entryLength))
					{
						Destroy();
						return false;
					}
				}
				length += -12 - entryLength;
			}
		}
	}
	return true;
}

gtaRwBool gtaRwFrameList::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_FRAMELIST, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4 + frameCount * 56, gtaRwVersion, gtaRwBuild))
		return false;
	if(!gtaRwStreamWrite(stream, &frameCount, 4))
		return false;
	for(gtaRwInt32 i = 0; i < frameCount; i++)
	{
		if(!gtaRwStreamWrite(stream, &frames[i], 56))
			return false;
	}
	for(gtaRwInt32 i = 0; i < frameCount; i++)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, frames[i].Extension.nodeName.GetStreamSize() +
			frames[i].Extension.hAnim.GetStreamSize(), gtaRwVersion, gtaRwBuild))
			return false;
		if(!frames[i].Extension.hAnim.StreamWrite(stream))
			return false;
		if(!frames[i].Extension.nodeName.StreamWrite(stream))
			return false;
	}
	return true;
}

gtaRwUInt32 gtaRwFrameList::GetStreamSize()
{
	gtaRwUInt32 size = 28;
	for(gtaRwInt32 i = 0; i < frameCount; i++)
		size += 68 + frames[i].Extension.hAnim.GetStreamSize() + frames[i].Extension.nodeName.GetStreamSize();
	return size;
}