
struct gtaRwClump
{
    gtaRwInt32        numAtomics;
    gtaRwInt32        numLights;
    gtaRwInt32        numCameras;
    gtaRwFrameList    frameList;
    gtaRwGeometryList geometryList;
    gtaRwAtomic*      atomics;
    // gtaRwLight *lights;
    // gtaRwCamera *cameras;
    gtaRwUInt32   version;
    gtaPlatformId platform;
    gtaClumpCollisionPlugin collisionPlugin;
};

	
__declspec(dllexport) gtaRwBool gtaRwClumpStreamRead(gtaRwClump* clumpObj, gtaRwStream* Stream);
__declspec(dllexport) gtaRwBool gtaRwClumpStreamWrite(gtaRwClump* clumpObj, gtaRwStream* Stream);
__declspec(dllexport) gtaRwUInt32 gtaRwClumpGetStreamSize(gtaRwClump* clumpObj);
__declspec(dllexport) void gtaRwClumpInitialise(gtaRwClump* clumpObj, gtaRwUInt32 NumAtomics, gtaRwUInt32 NumFrames, gtaRwUInt32 NumGeometries);
__declspec(dllexport) void gtaRwClumpInitialiseIdentity(gtaRwClump* clumpObj, gtaRwUInt32 NumAtomics);
__declspec(dllexport) void gtaRwClumpDestroy(gtaRwClump* clumpObj);