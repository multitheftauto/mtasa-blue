//
// dffapi
// https://github.com/DK22Pac/dffapi
//

enum gtaRwStreamType {
    rwNASTREAM = 0,
    rwSTREAMFILE,
    rwSTREAMFILENAME,
    rwSTREAMMEMORY,
    rwSTREAMCUSTOM,
};

enum gtaRwStreamAccessType {
    rwNASTREAMACCESS = 0,
    rwSTREAMREAD,
    rwSTREAMWRITE,
    rwSTREAMAPPEND,
};

struct gtaRwStreamMemory {
    gtaRwUInt32 position;
    gtaRwUInt32 nSize;
    gtaRwUInt8 *memBlock;
};

union gtaRwStreamFile {
    void *fpFile;
    const void *constfpFile;
};

typedef gtaRwInt32(*gtaRwCustomStreamFnClose) (void *data);
typedef gtaRwUInt32(*gtaRwCustomStreamFnRead) (void *data, void *buffer, gtaRwUInt32 length);
typedef gtaRwInt32(*gtaRwCustomStreamFnWrite) (void *data, const void *buffer, gtaRwUInt32 length);
typedef gtaRwInt32(*gtaRwCustomStreamFnSkip) (void *data, gtaRwUInt32 offset);

struct gtaRwStreamCustom {
    gtaRwCustomStreamFnClose sfnclose;
    gtaRwCustomStreamFnRead sfnread;
    gtaRwCustomStreamFnWrite sfnwrite;
    gtaRwCustomStreamFnSkip sfnskip;
    void *data;
};

union gtaRwStreamUnion {
    gtaRwStreamMemory memory;
    gtaRwStreamFile file;
    gtaRwStreamCustom custom;
};

struct gtaRwStream {
    gtaRwStreamType type;
    gtaRwStreamAccessType accessType;
    gtaRwInt32 position;
    gtaRwStreamUnion Type;
    gtaRwInt32 rwOwned;
};

struct gtaRwMemory {
    gtaRwUInt8 *start;
    gtaRwUInt32 length;
};

struct gtaRwChunkHeaderInfo {
    gtaRwUInt32 type;
    gtaRwUInt32 length;
    gtaRwUInt32 version;
    gtaRwUInt32 buildNum;
    gtaRwInt32 isComplex;
};

gtaRwStream* gtaRwStreamOpen(gtaRwStreamType type, gtaRwStreamAccessType accessType, const void* pData);
gtaRwBool    gtaRwStreamClose(gtaRwStream* stream, void* pData);
gtaRwUInt32 gtaRwStreamRead(gtaRwStream *stream, void *buffer, gtaRwUInt32 length);
gtaRwStream *gtaRwStreamSkip(gtaRwStream *stream, gtaRwUInt32 offset);
gtaRwBool gtaRwStreamReadChunkHeader(gtaRwStream *stream, gtaRwUInt32 *pType, gtaRwUInt32 *pLength, gtaRwUInt32 *pVersion, gtaRwUInt32 *pBuild);
gtaRwBool gtaRwStreamFindChunk(gtaRwStream *stream, gtaRwUInt32 type, gtaRwUInt32 *lengthOut, gtaRwUInt32 *versionOut, gtaRwUInt32 *buildOut);
gtaRwStream *gtaRwStreamReadReal(gtaRwStream *stream, gtaRwReal *reals, gtaRwUInt32 numBytes);
gtaRwStream *gtaRwStreamWrite(gtaRwStream *stream, const void *buffer, gtaRwUInt32 length);
gtaRwStream *gtaRwStreamWriteVersionedChunkHeader(gtaRwStream *stream, gtaRwUInt32 type, gtaRwUInt32 size, gtaRwUInt32 version, gtaRwUInt16 buildNum);
gtaRwStream *gtaRwStreamWriteInt32(gtaRwStream *stream, gtaRwUInt32 *gtaRwInt32egers, gtaRwUInt32 length);
