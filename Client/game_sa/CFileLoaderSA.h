#pragma once

#include "CVector.h"
#include "CVector4D.h"

// Forward declarations
class CEntitySAInterface;
struct RpAtomic;
struct RpClump;
struct RwStream;
struct RwChunkHeaderInfo;
struct RtDict;

struct SRelatedModelInfo
{
    RpClump* pClump;
    bool     bDeleteOldRwObject;
};

struct SFileObjectInstance
{
    CVector   position;
    CVector4D rotation;
    int       modelID;
    int       interiorID;
    int       lod;
};

class CFileLoaderSA
{
public:
    CFileLoaderSA();
    ~CFileLoaderSA();

    // Two overloads for different use cases
    static CEntitySAInterface* LoadObjectInstance(SFileObjectInstance* obj);
    static CEntitySAInterface* LoadObjectInstance(const char* szLine);

    static void StaticSetHooks();
};

// Global functions that are hooked/called by the game
bool                CFileLoader_LoadAtomicFile(RwStream* stream, unsigned int modelId);
RpAtomic*           CFileLoader_SetRelatedModelInfoCB(RpAtomic* atomic, SRelatedModelInfo* pRelatedModelInfo);
CEntitySAInterface* CFileLoader_LoadObjectInstance(const char* szLine);

// Reads the UV anim dictionary preceding a clump, if present, and sets it current for
// RpClumpStreamRead. outLeadingChunk receives the peeked header either way.
RtDict* CFileLoader_ReadLeadingUVAnimDict(RwStream* stream, RwChunkHeaderInfo& outLeadingChunk);
