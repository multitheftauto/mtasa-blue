#pragma once

#include "Stream.h"
#include "RwTypes.h"
#include "MaterialList.h"
#include "Mesh.h"
#include "ExtraVertColour.h"
#include "2dEffect.h"
#include "Native.h"
#include "Skin.h"
#include "Breakable.h"

struct gtaRwGeometry
{
	union{
		gtaRwInt32 format;  // Compression flags and number of texture coord sets
		struct{
			gtaRwUInt16 tristrip : 1;   // This geometry's meshes can be rendered as strips.
			gtaRwUInt16 positions : 1;  // This geometry has positions
			gtaRwUInt16 textured : 1;   // This geometry has only one set of texture coordinates. Texture coordinates are specified on a per vertex basis
			gtaRwUInt16 prelit : 1;     // This geometry has pre-light colors
			gtaRwUInt16 normals : 1;    // This geometry has vertex normals
			gtaRwUInt16 light : 1;      // This geometry will be lit
			gtaRwUInt16 modulateMaterialColor : 1; // Modulate material color with vertex colors (pre-lit + lit)
			gtaRwUInt16 textured2 : 1;  // This geometry has at least 2 sets of texture coordinates.
			gtaRwUInt8 numTexCoordSets;
			gtaRwUInt8 native : 1;
			gtaRwUInt8 nativeInstance : 1;
		};
	};
	gtaRwInt32 numTriangles;
	gtaRwInt32 numVertices;
	gtaRwInt32 numMorphTargets;
	gtaRwSurfaceProperties geometryLighting; // only if dff version is lower than 3.4.0.0.1
	gtaRwRGBA *preLitLum;          // The pre-lighting values
	gtaRwTexCoords *texCoords[8];  // Texture coordinates
	gtaRwTriangle *triangles;      // The triangles
	gtaRwMorphTarget *morphTarget; // The Morph Target
	gtaRwMaterialList matList;
	struct{
		gtaRwGeometryBinMesh mesh;
		gtaRwGeometryNative native;
		gtaRwGeometrySkin skin;
		gtaGeometryBreakable breakable;
		gtaGeometryExtraVertColour extraColour;
		gtaGeometry2dEffect effect2d;
	} Extension;

	gtaRwUInt8 GetTexCoordsCount();

	void Initialise(gtaRwInt32 NumTriangles, gtaRwInt32 NumVertices, gtaRwInt32 NumMorphTargets, gtaRwInt32 Format, gtaRwUInt8 NumTexCoordsCustom,
		gtaRwReal CenterX, gtaRwReal CenterY, gtaRwReal CenterZ, gtaRwReal Radius);

	void Destroy();

	gtaRwGeometry();

	gtaRwBool StreamRead(gtaRwStream *stream, gtaRwUInt32 ClumpVersion);

	gtaRwBool StreamWrite(gtaRwStream *stream);

	gtaRwUInt32 GetStreamActualSize();

	gtaRwUInt32 GetStreamSize();

	gtaRwBool ConvertFromOGlNative();

	void RecalculateFaces();

	void ConvertFromNative(gtaPlatformId Platform);
};