//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwHAnimNode gtaRwHAnimNode;
typedef struct gtaRwFrameHAnim gtaRwFrameHAnim;

struct gtaRwHAnimNode {
    gtaRwInt32 nodeID;    // User defined ID for this node
    gtaRwInt32 nodeIndex; // Array index of node
    union {
        gtaRwInt32 flags;     // Matrix push/pop flags
        struct {
            gtaRwInt32 popParentMatrix : 1;
            gtaRwInt32 pushParentMatrix : 1;
        };
    };
};

struct gtaRwFrameHAnim {
    gtaRwBool enabled;
    gtaRwUInt32 hAnimVersion;
    gtaRwInt32 id;
    gtaRwUInt32 numNodes; // Number of nodes in the hierarchy
    union {
        gtaRwUInt32 flags; // Flags for the hierarchy
        struct {
            gtaRwUInt32 subHierarchy : 1;
            gtaRwUInt32 noMatrices : 1;
            gtaRwUInt32 unused : 10;
            gtaRwUInt32 updateModellingMatrices : 1;
            gtaRwUInt32 updateLTMs : 1;
            gtaRwUInt32 localSpaceMatrices : 1;
        };
    };
    gtaRwUInt32 keyFrameSize;
    gtaRwHAnimNode *nodes; // Array of node information (push/pop flags etc)
};

void gtaRwHAnimNodeInit(gtaRwHAnimNode *nodeObj, gtaRwInt32 nodeID, gtaRwInt32 nodeIndex, gtaRwInt32 flags);
void gtaRwHAnimNodeDestroy(gtaRwHAnimNode *nodeObj);

void gtaRwFrameHAnimInitEmpty(gtaRwFrameHAnim *hAnimObj, gtaRwInt32 id);
void gtaRwFrameHAnimInit(gtaRwFrameHAnim *hAnimObj, gtaRwInt32 id, gtaRwUInt32 NumNodes, gtaRwUInt32 Flags, gtaRwUInt32 KeyFrameSize);
void gtaRwFrameHAnimDestroy(gtaRwFrameHAnim *hAnimObj);
gtaRwBool gtaRwFrameHAnimWrite(gtaRwFrameHAnim *hAnimObj, gtaRwStream *stream);
gtaRwBool gtaRwFrameHAnimRead(gtaRwFrameHAnim *hAnimObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwFrameHAnimSize(gtaRwFrameHAnim *hAnimObj);
