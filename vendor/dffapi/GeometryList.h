#pragma once

#include "Geometry.h"

struct gtaRwGeometryList
{
	gtaRwUInt32 geometryCount;
	gtaRwGeometry *geometries;

	void Initialise(gtaRwInt32 GeometryCount);

	void Destroy();

	gtaRwGeometryList();

	gtaRwBool StreamRead(gtaRwStream *stream, gtaRwUInt32 ClumpVersion);

	gtaRwBool StreamWrite(gtaRwStream *stream);

	gtaRwUInt32 GetStreamSize();
};