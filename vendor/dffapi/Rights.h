#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

struct gtaRwRights : public gtaRwExtension
{
	gtaRwUInt32 pluginId;
	gtaRwUInt32 pluginData;

	gtaRwRights();

	void Initialise(gtaRwUInt32 PluginId, gtaRwUInt32 PluginData);

	void Destroy();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);

	unsigned int GetStreamSize();
};