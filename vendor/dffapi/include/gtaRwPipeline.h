//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwAtomicPipeline gtaRwAtomicPipeline;

struct gtaRwAtomicPipeline {
    gtaRwBool enabled;
    gtaRwUInt32 pipelineId;
};

gtaRwBool gtaRwAtomicPipelineWrite(gtaRwAtomicPipeline *pipelineObj, gtaRwStream *stream);
gtaRwBool gtaRwAtomicPipelineRead(gtaRwAtomicPipeline *pipelineObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwAtomicPipelineSize(gtaRwAtomicPipeline *pipelineObj);
void gtaRwAtomicPipelineInit(gtaRwAtomicPipeline *pipelineObj, gtaRwUInt32 pipelineId);
void gtaRwAtomicPipelineDestroy(gtaRwAtomicPipeline *pipelineObj);
