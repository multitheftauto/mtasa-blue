//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwFrame gtaRwFrame;
typedef struct gtaRwFrameList gtaRwFrameList;

struct gtaRwFrame {
    gtaRwV3d right;
    gtaRwV3d up;
    gtaRwV3d at;
    gtaRwV3d pos;
    gtaRwInt32 parent;
    gtaRwUInt32 matrixFlags;

    // extensions

    gtaRwFrameHAnim hAnim;
    gtaRwFrameNodeName nodeName;

    gtaRwUnknownExtension *unknownExtensions;
    gtaRwUInt32 numUnknownExtensions;
};

struct gtaRwFrameList {
    gtaRwUInt32 frameCount;
    gtaRwFrame *frames;
};

void gtaRwFrameInitIdentity(gtaRwFrame *frameObj, gtaRwInt32 parent, gtaRwUInt32 matrixFlags);
void gtaRwFrameInitWithMatrix(gtaRwFrame *frameObj, gtaRwMatrix *matrix, gtaRwInt32 parent, gtaRwUInt32 matrixFlags);
void gtaRwFrameInit(gtaRwFrame *frameObj, gtaRwV3d *right, gtaRwV3d *up, gtaRwV3d *at, gtaRwV3d *pos, gtaRwInt32 parent, gtaRwUInt32 matrixFlags);
void gtaRwFrameDestroy(gtaRwFrame *frameObj);
void gtaRwFrameSetName(gtaRwFrame *frameObj, gtaRwChar const *name);

void gtaRwFrameListInit(gtaRwFrameList *frameListObj, gtaRwInt32 FrameCount);
void gtaRwFrameListDestroy(gtaRwFrameList *frameListObj);
gtaRwBool gtaRwFrameListRead(gtaRwFrameList *frameListObj, gtaRwStream *stream);
gtaRwBool gtaRwFrameListWrite(gtaRwFrameList *frameListObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwFrameListSize(gtaRwFrameList *frameListObj);
