/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CRenderWare.h
 *  PURPOSE:     RenderWare engine interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>

class CClientEntityBase;
class CD3DDUMMY;
class CMatrix;
class CPixels;
class CShaderItem;
class SString;
class CColModel;
struct RpAtomicContainer;
struct RwFrame;
struct RwMatrix;
struct RwTexDictionary;
struct RwTexture;
struct RpClump;

typedef CShaderItem CSHADERDUMMY;

// A list of custom textures to add to a model's txd
struct SReplacementTextures
{
    struct SPerTxd
    {
        std::vector<RwTexture*> usingTextures;
        ushort                  usTxdId;
        bool                    bTexturesAreCopies;
    };

    std::vector<RwTexture*> textures;              // List of textures we want to inject into TXD's
    std::vector<SPerTxd>    perTxdList;            // TXD's which have been modified
    std::vector<ushort>     usedInTxdIds;
    std::vector<ushort>     usedInModelIds;
};

// Shader layers to render
struct SShaderItemLayers
{
    SShaderItemLayers() : pBase(NULL), bUsesVertexShader(false) {}
    CShaderItem*              pBase;
    std::vector<CShaderItem*> layerList;
    bool                      bUsesVertexShader;
};

enum EEntityTypeMask
{
    TYPE_MASK_NONE = 0,
    TYPE_MASK_WORLD = 1,
    TYPE_MASK_PED = 2,
    TYPE_MASK_VEHICLE = 4,
    TYPE_MASK_OBJECT = 8,
    TYPE_MASK_OTHER = 16,
    TYPE_MASK_ALL = 127,
};

typedef void (*PFN_WATCH_CALLBACK)(CSHADERDUMMY* pContext, CD3DDUMMY* pD3DDataNew, CD3DDUMMY* pD3DDataOld);

#define MAX_ATOMICS_PER_CLUMP   128

class CRenderWare
{
public:
    virtual bool             ModelInfoTXDLoadTextures(SReplacementTextures* pReplacementTextures, const SString& strFilename, const SString& buffer,
                                                      bool bFilteringEnabled) = 0;
    virtual bool             ModelInfoTXDAddTextures(SReplacementTextures* pReplacementTextures, ushort usModelId) = 0;
    virtual void             ModelInfoTXDRemoveTextures(SReplacementTextures* pReplacementTextures) = 0;
    virtual void             ClothesAddReplacement(char* pFileData, size_t fileSize, ushort usFileId) = 0;
    virtual void             ClothesRemoveReplacement(char* pFileData) = 0;
    virtual bool             HasClothesReplacementChanged() = 0;
    virtual bool             ClothesAddFile(const char* fileData, std::size_t fileSize, const char* fileName) = 0;
    virtual bool             ClothesRemoveFile(char* fileData) = 0;
    virtual bool             HasClothesFile(const char* fileName) const noexcept = 0;
    virtual RwTexDictionary* ReadTXD(const SString& strFilename, const SString& buffer) = 0;
    virtual RpClump*         ReadDFF(const SString& strFilename, const SString& buffer, unsigned short usModelID, bool bLoadEmbeddedCollisions) = 0;
    virtual CColModel*       ReadCOL(const SString& buffer) = 0;
    virtual void             DestroyDFF(RpClump* pClump) = 0;
    virtual void             DestroyTXD(RwTexDictionary* pTXD) = 0;
    virtual void             DestroyTexture(RwTexture* pTex) = 0;
    virtual void             ReplaceCollisions(CColModel* pColModel, unsigned short usModelID) = 0;
    virtual unsigned int     LoadAtomics(RpClump* pClump, RpAtomicContainer* pAtomics) = 0;
    virtual bool             ReplaceAllAtomicsInModel(RpClump* pSrc, unsigned short usModelID) = 0;
    virtual void             ReplaceAllAtomicsInClump(RpClump* pDst, RpAtomicContainer* pAtomics, unsigned int uiAtomics) = 0;
    virtual void             ReplaceWheels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szWheel) = 0;
    virtual void             RepositionAtomic(RpClump* pDst, RpClump* pSrc, const char* szName) = 0;
    virtual void             AddAllAtomics(RpClump* pDst, RpClump* pSrc) = 0;
    virtual bool             ReplaceVehicleModel(RpClump* pNew, unsigned short usModelID) = 0;
    virtual bool             ReplaceWeaponModel(RpClump* pNew, unsigned short usModelID) = 0;
    virtual bool             ReplacePedModel(RpClump* pNew, unsigned short usModelID) = 0;
    virtual bool             ReplacePartModels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szName) = 0;
    virtual void             PulseWorldTextureWatch() = 0;
    virtual void             GetModelTextureNames(std::vector<SString>& outNameList, ushort usModelID) = 0;
    virtual bool GetModelTextures(std::vector<std::tuple<std::string, CPixels>>& outTextureList, ushort usModelID, std::vector<SString> vTextureNames) = 0;
    virtual const char* GetTextureName(CD3DDUMMY* pD3DData) = 0;
    virtual ushort      GetTXDIDForModelID(ushort usModelID) = 0;

    virtual void               SetRenderingClientEntity(CClientEntityBase* pClientEntity, ushort usModelId, int iTypeMask) = 0;
    virtual SShaderItemLayers* GetAppliedShaderForD3DData(CD3DDUMMY* pD3DData) = 0;
    virtual void     AppendAdditiveMatch(CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* strTextureNameMatch, float fShaderPriority,
                                         bool bShaderLayered, int iTypeMask, uint uiShaderCreateTime, bool bShaderUsesVertexShader, bool bAppendLayers) = 0;
    virtual void     AppendSubtractiveMatch(CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* strTextureNameMatch) = 0;
    virtual void     RemoveClientEntityRefs(CClientEntityBase* pClientEntity) = 0;
    virtual void     RemoveShaderRefs(CSHADERDUMMY* pShaderItem) = 0;
    virtual RwFrame* GetFrameFromName(RpClump* pRoot, SString strName) = 0;
    virtual bool     RightSizeTxd(const SString& strInTxdFilename, const SString& strOutTxdFilename, uint uiSizeLimit) = 0;
    virtual void     TxdForceUnload(ushort usTxdId, bool bDestroyTextures) = 0;

    virtual void CMatrixToRwMatrix(const CMatrix& mat, RwMatrix& rwOutMatrix) = 0;
    virtual void RwMatrixToCMatrix(const RwMatrix& rwMatrix, CMatrix& matOut) = 0;
    virtual void RwMatrixGetRotation(const RwMatrix& rwMatrix, CVector& vecOutRotation) = 0;
    virtual void RwMatrixSetRotation(RwMatrix& rwInOutMatrix, const CVector& vecRotation) = 0;
    virtual void RwMatrixGetPosition(const RwMatrix& rwMatrix, CVector& vecOutPosition) = 0;
    virtual void RwMatrixSetPosition(RwMatrix& rwInOutMatrix, const CVector& vecPosition) = 0;
    virtual void RwMatrixGetScale(const RwMatrix& rwMatrix, CVector& vecOutScale) = 0;
    virtual void RwMatrixSetScale(RwMatrix& rwInOutMatrix, const CVector& vecScale) = 0;
};
