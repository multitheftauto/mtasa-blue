#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

struct gtaGeometryExtraVertColour : public gtaRwExtension
{
	gtaRwRGBA *nightColors;

	gtaGeometryExtraVertColour();

	void Initialise(gtaRwUInt32 VertexCount);

	void Destroy();

	bool StreamWrite(gtaRwStream *stream, gtaRwUInt32 vertexCount);

	bool StreamRead(gtaRwStream *stream, gtaRwUInt32 vertexCount);
	
	unsigned int GetStreamSize(gtaRwUInt32 vertexCount);
};