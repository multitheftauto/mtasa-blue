//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwAtomicPipelineWrite(gtaRwAtomicPipeline *pipelineObj, gtaRwStream *stream) {
    if (pipelineObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_PIPELINE, 4, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &pipelineObj->pipelineId, 4))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwAtomicPipelineRead(gtaRwAtomicPipeline *pipelineObj, gtaRwStream *stream) {
    gtaRwAtomicPipelineDestroy(pipelineObj);
    if (gtaRwStreamRead(stream, &pipelineObj->pipelineId, 4) != 4) {
        gtaRwAtomicPipelineDestroy(pipelineObj);
        return rwFALSE;
    }
    pipelineObj->enabled = rwTRUE;
    return rwTRUE;
}

unsigned int gtaRwAtomicPipelineSize(gtaRwAtomicPipeline *pipelineObj) {
    if (pipelineObj->enabled)
        return 16;
    return 0;
}

void gtaRwAtomicPipelineInit(gtaRwAtomicPipeline *pipelineObj, gtaRwUInt32 pipelineId) {
    gtaRwAtomicPipelineDestroy(pipelineObj);
    pipelineObj->enabled = rwTRUE;
    pipelineObj->pipelineId = pipelineId;
}

void gtaRwAtomicPipelineDestroy(gtaRwAtomicPipeline *pipelineObj) {
    if (pipelineObj)
        gtaRwMemZero(pipelineObj, sizeof(gtaRwAtomicPipeline));
}
