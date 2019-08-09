#pragma once

#include "RwTypes.h"
#include "GeometryList.h"
#include "FrameList.h"
#include "Atomic.h"
#include "CollisionPlugin.h"

struct gtaRwClump
{
	gtaRwInt32 numAtomics;
	gtaRwInt32 numLights;
	gtaRwInt32 numCameras;
	gtaRwFrameList frameList;
	gtaRwGeometryList geometryList;
	gtaRwAtomic *atomics;
	// gtaRwLight *lights;
	// gtaRwCamera *cameras;
	gtaRwUInt32 version;
	gtaPlatformId platform;

	struct {
		gtaClumpCollisionPlugin collisionPlugin;
	} Extension;

	gtaRwClump();

	gtaRwBool StreamRead(gtaRwStream *Stream);

	gtaRwBool StreamWrite(gtaRwStream *Stream);

	gtaRwUInt32 GetStreamSize();

	void Initialise(gtaRwUInt32 NumAtomics, gtaRwUInt32 NumFrames, gtaRwUInt32 NumGeometries);

	void Initialise(gtaRwUInt32 NumAtomics);

	void Destroy();
};