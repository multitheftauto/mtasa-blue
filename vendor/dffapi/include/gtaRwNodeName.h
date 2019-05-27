//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwFrameNodeName gtaRwFrameNodeName;

struct gtaRwFrameNodeName {
    gtaRwBool enabled;
    gtaRwChar *name;
};

gtaRwBool gtaRwFrameNodeNameWrite(gtaRwFrameNodeName *nodeNameObj, gtaRwStream *stream);
gtaRwBool gtaRwFrameNodeNameRead(gtaRwFrameNodeName *nodeNameObj, gtaRwStream *stream, gtaRwUInt32 length);
gtaRwUInt32 gtaRwFrameNodeNameSize(gtaRwFrameNodeName *nodeNameObj);
void gtaRwFrameNodeNameInit(gtaRwFrameNodeName *nodeNameObj, gtaRwChar const *name);
void gtaRwFrameNodeNameDestroy(gtaRwFrameNodeName *nodeNameObj);
