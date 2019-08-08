//
// dffapi
// https://github.com/DK22Pac/dffapi
//

struct gtaRwAtomic
{
    gtaRwInt32 frameIndex;
    gtaRwInt32 geometryIndex;
    union {
        gtaRwUInt32 flags;
        struct {
            gtaRwUInt32 collisionTest : 1;
            gtaRwUInt32 unused : 1;
            gtaRwUInt32 render : 1;
        };
    };
    gtaRwInt32 unused2; // sets to 0 by default

    // ONLY if clump's geometry list is empty.
    gtaRwGeometry *internalGeometry;

    gtaRwAtomicMatFX matFx;
    gtaRwRights *rights; // skin & normal map rights
    gtaRwUInt32 numRights;
    gtaRwAtomicPipeline pipeline;
};

gtaRwBool gtaRwAtomicRead(gtaRwAtomic *atomicObj, gtaRwStream *stream, gtaRwBool geometryListIsEmpty, gtaRwUInt32 clumpVersion);
gtaRwBool gtaRwAtomicWrite(gtaRwAtomic *atomicObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwAtomicSize(gtaRwAtomic *atomicObj);
void gtaRwAtomicInit(gtaRwAtomic *atomicObj, gtaRwInt32 frameIndex, gtaRwInt32 geometryIndex, gtaRwUInt32 flags, gtaRwBool usesInternalGeometry);
void gtaRwAtomicDestroy(gtaRwAtomic *atomicObj);
