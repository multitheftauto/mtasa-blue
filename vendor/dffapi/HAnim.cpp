#pragma once

#include "HAnim.h"
#include "StdInc.h"

gtaRwHAnimNode::gtaRwHAnimNode()
{
	memset(this, 0, sizeof(gtaRwHAnimNode));
}

void gtaRwHAnimNode::Initialise(gtaRwInt32 NodeID, gtaRwInt32 NodeIndex, gtaRwInt32 Flags)
{
	nodeID = NodeID;
	nodeIndex = NodeIndex;
	flags = Flags;
}

void gtaRwHAnimNode::Destroy()
{
	memset(this, 0, sizeof(gtaRwHAnimNode));
}

gtaRwFrameHAnim::gtaRwFrameHAnim()
{
	memset(this, 0, sizeof(gtaRwFrameHAnim));
}

void gtaRwFrameHAnim::Initialise(gtaRwInt32 Id)
{
	memset(this, 0, sizeof(gtaRwFrameHAnim));
	enabled = true;
	hAnimVersion = rpHANIMSTREAMCURRENTVERSION;
	id = Id;
}

void gtaRwFrameHAnim::Initialise(gtaRwInt32 Id, gtaRwUInt32 NumNodes, gtaRwUInt32 Flags, gtaRwUInt32 KeyFrameSize)
{
	memset(this, 0, sizeof(gtaRwFrameHAnim));
	enabled = true;
	id = Id;
	numNodes = NumNodes;
	flags = Flags;
	keyFrameSize = KeyFrameSize;
	hAnimVersion = rpHANIMSTREAMCURRENTVERSION;
	if(NumNodes > 0)
	{
		nodes = (gtaRwHAnimNode *)malloc(sizeof(gtaRwHAnimNode) * NumNodes);
		memset(nodes, 0, sizeof(gtaRwHAnimNode) * NumNodes);
	}
}

void gtaRwFrameHAnim::Destroy()
{
	if(nodes)
	{
		for(gtaRwInt32 i = 0; i < numNodes; i++)
			nodes[i].Destroy();
		free(nodes);
	}
	memset(this, 0, sizeof(gtaRwFrameHAnim));
}

bool gtaRwFrameHAnim::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_HANIM, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &hAnimVersion, 12))
			return false;
		if(numNodes > 0)
		{
			if(!gtaRwStreamWrite(stream, &flags, 8))
				return false;
			for(gtaRwInt32 i = 0; i < numNodes; i++)
			{
				if(!gtaRwStreamWrite(stream, &nodes[i].nodeID, 12))
					return false;
			}
		}
	}
	return true;
}

bool gtaRwFrameHAnim::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwFrameHAnim));
	enabled = true;
	if(gtaRwStreamRead(stream, &hAnimVersion, 12) != 12)
		return false;
	if(numNodes > 0)
	{
		if(gtaRwStreamRead(stream, &flags, 8) != 8)
			return false;
		nodes = (gtaRwHAnimNode *)malloc(sizeof(gtaRwHAnimNode) * numNodes);
		memset(nodes, 0, sizeof(gtaRwHAnimNode) * numNodes);
		for(gtaRwInt32 i = 0; i < numNodes; i++)
		{
			if(gtaRwStreamRead(stream, &nodes[i].nodeID, 12) != 12)
			{
				Destroy();
				return false;
			}
		}
	}
	return true;
}

unsigned int gtaRwFrameHAnim::GetStreamSize()
{
	if(enabled)
	{
		gtaRwUInt32 size = 24;
		if(numNodes > 0)
			size += 12 * numNodes + 8;
		return size;
	}
	return 0;
}