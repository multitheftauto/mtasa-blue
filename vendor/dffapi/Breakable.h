#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"
#include "StdInc.h"

struct gtaBreakableFace
{
	gtaRwUInt16 a, b, c;
};

struct gtaBreakableMaterialColor
{
	gtaRwReal red, green, blue;
};

struct gtaGeometryBreakable : public gtaRwExtension
{
	gtaGeometryBreakable *thisData;
	gtaRwBool posnRelativeToCollision;
	gtaRwUInt32 vertexCount;
	gtaRwV3d *vertices;
	gtaRwTexCoords *texCoors;
	gtaRwRGBA *colors;
	gtaRwUInt32 faceCount;
	gtaBreakableFace *faces;
	gtaRwUInt16 *faceMaterials;
	gtaRwUInt32 meshCount;
	gtaRwChar (*texNames)[32];
	gtaRwChar (*texMaskNames)[32];
	gtaBreakableMaterialColor *matColors;

	gtaGeometryBreakable();

	void Initialise(gtaRwUInt32 VertexCount, gtaRwUInt32 FaceCount, gtaRwUInt32 MeshCount, gtaRwBool PosnRelativeToCollision);

	void Destroy();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};