#pragma once

#include "Rights.h"
#include "StdInc.h"

gtaRwRights::gtaRwRights()
{
	memset(this, 0, sizeof(gtaRwRights));
}

void gtaRwRights::Initialise(gtaRwUInt32 PluginId, gtaRwUInt32 PluginData)
{
	memset(this, 0, sizeof(gtaRwRights));
	enabled = true;
	pluginId = PluginId;
	pluginData = PluginData;
}

void gtaRwRights::Destroy()
{
	memset(this, 0, sizeof(gtaRwRights));
}

bool gtaRwRights::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_RIGHTTORENDER, 8, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &pluginId, 8))
			return false;
	}
	return true;
}

bool gtaRwRights::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwRights));
	enabled = true;
	if(gtaRwStreamRead(stream, &pluginId, 4) != 4)
		return false;
	if(gtaRwStreamRead(stream, &pluginData, 4) != 4)
		return false;
	return true;
}

unsigned int gtaRwRights::GetStreamSize()
{
	if(enabled)
		return 20;
	return 0;
}