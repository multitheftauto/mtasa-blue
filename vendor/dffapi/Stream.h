#pragma once

#include "RwTypes.h"
#include <Windows.h>

enum gtaRwStreamType
{
    rwNASTREAM = 0,
    rwSTREAMFILE,
    rwSTREAMFILENAME,
    rwSTREAMMEMORY,
    rwSTREAMCUSTOM,
};

enum gtaRwStreamAccessType
{
    rwNASTREAMACCESS = 0,
    rwSTREAMREAD,
    rwSTREAMWRITE,
    rwSTREAMAPPEND,
};

typedef int(*gtaRwCustomStreamFnClose) (void *data);
typedef unsigned int(*gtaRwCustomStreamFnRead) (void *data, void *buffer, unsigned int length);
typedef int(*gtaRwCustomStreamFnWrite) (void *data, const void *buffer, unsigned int length);
typedef int(*gtaRwCustomStreamFnSkip) (void *data, unsigned int offset);

struct gtaRwStreamMemory
{
    unsigned int   position; 
    unsigned int   nSize;
    unsigned char *memBlock;
};

union gtaRwStreamFile
{
    void    *fpFile;
    const void    *constfpFile;
};

struct gtaRwStreamCustom
{
    gtaRwCustomStreamFnClose sfnclose;
    gtaRwCustomStreamFnRead sfnread;
    gtaRwCustomStreamFnWrite sfnwrite;
    gtaRwCustomStreamFnSkip sfnskip;
    void               *data;
};

union gtaRwStreamUnion
{
    gtaRwStreamMemory      memory;
    gtaRwStreamFile        file;
    gtaRwStreamCustom      custom;
};

struct gtaRwStream
{
    gtaRwStreamType        type;
    gtaRwStreamAccessType  accessType;
    int                 position;
    gtaRwStreamUnion       Type;
    int                 rwOwned;
};
// RwMemory
struct RwMemory
{
    unsigned char *start;
    unsigned int   length;
};

struct gtaRwChunkHeaderInfo
{
    unsigned int type;
    unsigned int length;
    unsigned int version; 
    unsigned int buildNum;
    int isComplex;
};

bool gtaRwErrorSet(const char *message, ...);
gtaRwStream *gtaRwStreamOpen(gtaRwStreamType type, gtaRwStreamAccessType accessType, const void *pData);
bool gtaRwStreamClose(gtaRwStream * stream, void *pData = NULL);
unsigned int gtaRwStreamRead(gtaRwStream *stream, void *buffer, unsigned int length);
gtaRwStream *gtaRwStreamSkip(gtaRwStream *stream, unsigned int offset);
bool gtaChunkIsComplex(gtaRwChunkHeaderInfo *chunkHeaderInfo);
bool gtaRwStreamReadChunkHeader(gtaRwStream *stream, unsigned int *pType, unsigned int *pLength, unsigned int *pVersion, unsigned int *pBuild);
bool gtaRwStreamFindChunk(gtaRwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut);
bool gtaRwStreamFindChunk(gtaRwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut, unsigned int *buildOut);
gtaRwStream *gtaRwStreamReadReal(gtaRwStream *stream, float *reals, unsigned int numBytes);
gtaRwStream *gtaRwStreamWrite(gtaRwStream *stream, const void *buffer, unsigned int length);
gtaRwStream *gtaRwStreamWriteVersionedChunkHeader(gtaRwStream *stream, unsigned int type, unsigned int size, 
													   unsigned int version, unsigned short buildNum);
gtaRwStream *gtaRwStreamWriteInt32(gtaRwStream *stream, unsigned int *integers, unsigned int length);