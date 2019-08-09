#pragma once

#include "RwTypes.h"
#include "Geometry.h"
#include "Rights.h"
#include "Pipeline.h"
#include "MatFX.h"

enum gtaRwAtomicFlag
{
    rpATOMICCOLLISIONTEST = 0x01, // A generic collision flag to indicate that the atomic should be considered in collision tests.
    rpATOMICRENDER = 0x04         //The atomic is rendered if it is in the view frustum.
};

struct gtaRwAtomic
{
	gtaRwInt32 frameIndex;
	gtaRwInt32 geometryIndex;
	union{
		gtaRwUInt32 flags;
		struct{
			gtaRwUInt32 collisionTest : 1;
			gtaRwUInt32 unused : 1;
			gtaRwUInt32 render : 1;
		};
	};
	gtaRwInt32 unused2; // sets to 0 by default

	// ONLY if clump's geometry list is empty.
	gtaRwGeometry *internalGeometry;

	struct {
		gtaRwAtomicMatFx matFx;
		gtaRwRights rights[2]; // skin & normal map rights
		gtaRwAtomicPipeline pipeline;
	} Extension;

	gtaRwAtomic();

	gtaRwBool StreamRead(gtaRwStream *Stream, gtaRwBool GeometryListIsEmpty, gtaRwUInt32 ClumpVersion);

	gtaRwBool StreamWrite(gtaRwStream *Stream);

	gtaRwUInt32 GetStreamSize();

	void Initialise(gtaRwInt32 FrameIndex, gtaRwInt32 GeometryIndex, gtaRwUInt32 Flags, gtaRwBool UsesInternalGeometry);

	void Destroy();
};