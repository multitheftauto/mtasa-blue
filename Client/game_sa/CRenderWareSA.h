/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.h
 *  PURPOSE:     Header file for RenderWare game engine class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CRenderWare.h>
#include "CModelInfoSA.h"
#include "CRenderWareSA.ShaderSupport.h"
#include <unordered_set>

class CMatchChannelManager;
struct CModelTexturesInfo;
struct RpAtomic;
struct SShaderReplacementStats;
struct STexInfo;
struct STexTag;

class CRenderWareSA : public CRenderWare
{
public:
    ZERO_ON_NEW
    CRenderWareSA();
    ~CRenderWareSA();
    void Initialize();
    bool ModelInfoTXDLoadTextures(SReplacementTextures* pReplacementTextures, const SString& strFilename, const SString& buffer, bool bFilteringEnabled,
                                  SString* pOutError = nullptr) override;
    bool ModelInfoTXDAddTextures(SReplacementTextures* pReplacementTextures, unsigned short usModelId);
    void ModelInfoTXDRemoveTextures(SReplacementTextures* pReplacementTextures);
    void ModelInfoTXDDeferCleanup(SReplacementTextures* pReplacementTextures) override;
    void CleanupIsolatedTxdForModel(unsigned short usModelId, bool bSkipStreamingLoads = false) override;
    void CleanupReplacementsInTxdSlot(unsigned short usTxdSlotId) override;
    void StaticResetModelTextureReplacing();
    void StaticResetShaderSupport();
    void ClothesAddReplacement(char* pFileData, size_t fileSize, unsigned short usFileId);
    void ClothesRemoveReplacement(char* pFileData);
    bool HasClothesReplacementChanged();
    bool ClothesAddFile(const char* fileData, std::size_t fileSize, const char* fileName) override;
    bool ClothesRemoveFile(char* fileData) override;
    bool HasClothesFile(const char* fileName) const override;

    // Reads and parses a TXD file specified by a path (szTXD)
    RwTexDictionary* ReadTXD(const SString& strFilename, const SString& buffer);

    // Reads and parses a DFF file specified by a path (szDFF) into a CModelInfo identified by the object id (usModelID)
    RpClump* ReadDFF(const SString& strFilename, const SString& buffer, unsigned short usModelID, bool bLoadEmbeddedCollisions);

    // Destroys a DFF instance
    void DestroyDFF(RpClump* pClump);

    // Destroys a TXD instance
    void DestroyTXD(RwTexDictionary* pTXD);

    // Destroys a texture
    void DestroyTexture(RwTexture* pTex);

    // Parses TXD buffer data and injects it directly into an allocated pool slot
    bool LoadTxdSlotFromBuffer(std::uint32_t uiSlotId, const std::string& buffer) override;

    // Reads and parses a COL file (versions 1-4: COLL, COL2, COL3, COL4)
    CColModel* ReadCOL(const SString& buffer);

    // Replaces a CColModel for a specific object identified by the object id (usModelID)
    void ReplaceCollisions(CColModel* pColModel, unsigned short usModelID);

    // Loads all atomics from a clump into a container struct and returns the number of atomics it loaded
    unsigned int LoadAtomics(RpClump* pClump, RpAtomicContainer* pAtomics);

    // Replaces all atomics for a specific model
    bool ReplaceAllAtomicsInModel(RpClump* pSrc, unsigned short usModelID) override;

    // Replaces all atomics in a clump
    void ReplaceAllAtomicsInClump(RpClump* pDst, RpAtomicContainer* pAtomics, unsigned int uiAtomics);

    // Replaces the wheels in a vehicle
    void ReplaceWheels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szWheel = "wheel");

    // Repositions an atomic
    void RepositionAtomic(RpClump* pDst, RpClump* pSrc, const char* szName);

    // Adds the atomics from a source clump (pSrc) to a destination clump (pDst)
    void AddAllAtomics(RpClump* pDst, RpClump* pSrc);

    // Replaces a CClumpModelInfo (or CVehicleModelInfo, since its just for vehicles) clump with a new clump
    bool ReplaceVehicleModel(RpClump* pNew, unsigned short usModelID) override;

    // Replaces a generic CClumpModelInfo clump with a new clump
    bool ReplaceClumpModel(RpClump* pNew, unsigned short usModelID) override;

    // Replaces a CWeaponModelInfo clump with a new clump
    bool ReplaceWeaponModel(RpClump* pNew, unsigned short usModelID) override;

    bool ReplacePedModel(RpClump* pNew, unsigned short usModelID) override;

    bool ReplaceModel(RpClump* pNew, unsigned short usModelID, DWORD dwSetClumpFunction);

    // Replaces dynamic parts of the vehicle (models that have two different versions: 'ok' and 'dam'), such as doors
    // szName should be without the part suffix (e.g. 'door_lf' or 'door_rf', and not 'door_lf_dummy')
    bool ReplacePartModels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szName);

    unsigned short GetTXDIDForModelID(unsigned short usModelID);
    void           PulseWorldTextureWatch();
    // Compatibility wrapper; uses per-model pending processing.
    void        ProcessPendingIsolatedTxdParents();
    void        ProcessPendingIsolatedModels();
    void        GetModelTextureNames(std::vector<SString>& outNameList, unsigned short usModelID);
    bool        GetModelTextures(std::vector<std::tuple<std::string, CPixels>>& outTextureList, unsigned short usModelID, std::vector<SString> vTextureNames);
    void        GetTxdTextures(std::vector<RwTexture*>& outTextureList, unsigned short usTxdId);
    static void GetTxdTextures(std::vector<RwTexture*>& outTextureList, RwTexDictionary* pTXD);
    static void GetTxdTextures(std::unordered_set<RwTexture*>& outTextureSet, RwTexDictionary* pTXD);
    const char* GetTextureName(CD3DDUMMY* pD3DData);
    void        SetRenderingClientEntity(CClientEntityBase* pClientEntity, unsigned short usModelId, int iTypeMask);
    SShaderItemLayers* GetAppliedShaderForD3DData(CD3DDUMMY* pD3DData);
    void               AppendAdditiveMatch(CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* strTextureNameMatch, float fShaderPriority,
                                           bool bShaderLayered, int iTypeMask, unsigned int uiShaderCreateTime, bool bShaderUsesVertexShader, bool bAppendLayers);
    void               AppendSubtractiveMatch(CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const char* strTextureNameMatch);
    void               RemoveClientEntityRefs(CClientEntityBase* pClientEntity);
    void               RemoveShaderRefs(CSHADERDUMMY* pShaderItem);
    bool               RightSizeTxd(const SString& strInTxdFilename, const SString& strOutTxdFilename, unsigned int uiSizeLimit);
    void               TxdForceUnload(unsigned short usTxdId, bool bDestroyTextures);

    void CMatrixToRwMatrix(const CMatrix& mat, RwMatrix& rwOutMatrix);
    void RwMatrixToCMatrix(const RwMatrix& rwMatrix, CMatrix& matOut);
    void RwMatrixGetRotation(const RwMatrix& rwMatrix, CVector& vecOutRotation);
    void RwMatrixSetRotation(RwMatrix& rwInOutMatrix, const CVector& vecRotation);
    void RwMatrixGetPosition(const RwMatrix& rwMatrix, CVector& vecOutPosition);
    void RwMatrixSetPosition(RwMatrix& rwInOutMatrix, const CVector& vecPosition);
    void RwMatrixGetScale(const RwMatrix& rwMatrix, CVector& vecOutScale);
    void RwMatrixSetScale(RwMatrix& rwInOutMatrix, const CVector& vecScale);

    // CRenderWareSA methods
    RwTexture*          RightSizeTexture(RwTexture* pTexture, unsigned int uiSizeLimit, SString& strError);
    void                ResetStats();
    void                GetShaderReplacementStats(SShaderReplacementStats& outStats);
    CModelTexturesInfo* GetModelTexturesInfo(unsigned short usModelId, const char* callsiteTag = "unknown");

    RwFrame* GetFrameFromName(RpClump* pRoot, SString strName);

    static void  StaticSetHooks();
    static void  StaticSetClothesReplacingHooks();
    static bool  RwTexDictionaryRemoveTexture(RwTexDictionary* pTXD, RwTexture* pTex);
    static bool  RwTexDictionaryContainsTexture(RwTexDictionary* pTXD, RwTexture* pTex);
    static short CTxdStore_GetTxdRefcount(unsigned short usTxdID);
    static void  DebugTxdAddRef(unsigned short usTxdId, const char* tag = nullptr, bool enableSafetyPin = true);
    static void  DebugTxdRemoveRef(unsigned short usTxdId, const char* tag = nullptr);

    void      InitTextureWatchHooks();
    void      StreamingAddedTexture(unsigned short usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData);
    void      StreamingRemovedTxd(unsigned short usTxdId);
    void      RemoveStreamingTexture(unsigned short usTxdId, CD3DDUMMY* pD3DData);
    bool      IsTexInfoRegistered(CD3DDUMMY* pD3DData) const;
    void      ScriptAddedTxd(RwTexDictionary* pTxd);
    void      ScriptRemovedTexture(RwTexture* pTex);
    void      SpecialAddedTexture(RwTexture* texture, const char* szTextureName = NULL);
    void      SpecialRemovedTexture(RwTexture* texture);
    STexInfo* CreateTexInfo(const STexTag& texTag, const SString& strTextureName, CD3DDUMMY* pD3DData);
    void      DestroyTexInfo(STexInfo* pTexInfo);

    static void GetClumpAtomicList(RpClump* pClump, std::vector<RpAtomic*>& outAtomicList);
    static bool DoContainTheSameGeometry(RpClump* pClumpA, RpClump* pClumpB, RpAtomic* pAtomicB);

    // Rebind clump material textures to current TXD textures (fixes stale texture pointers after TXD reload)
    void RebindClumpTexturesToTxd(RpClump* pClump, unsigned short usTxdId) override;

    // Rebind single atomic's material textures to current TXD textures
    void RebindAtomicTexturesToTxd(RpAtomic* pAtomic, unsigned short usTxdId) override;

    static const char* GetInternalTextureName(const char* szExternalName);
    static const char* GetExternalTextureName(const char* szInternalName);

    void OnTextureStreamIn(STexInfo* pTexInfo);
    void OnTextureStreamOut(STexInfo* pTexInfo);
    void DisableGTAVertexShadersForAWhile();
    void UpdateDisableGTAVertexShadersTimer();
    void SetGTAVertexShadersEnabled(bool bEnable);

    // Watched world textures
    std::multimap<unsigned short, STexInfo*> m_TexInfoMap;
    CFastHashMap<CD3DDUMMY*, STexInfo*>      m_D3DDataTexInfoMap;
    // Reverse lookup for script/special textures (keyed by RwTexture* tag)
    std::unordered_map<const RwTexture*, STexInfo*> m_ScriptTexInfoMap;
    CClientEntityBase*                              m_pRenderingClientEntity;
    unsigned short                                  m_usRenderingEntityModelId;
    int                                             m_iRenderingEntityType;
    CMatchChannelManager*                           m_pMatchChannelManager;
    int                                             m_uiReplacementRequestCounter;
    int                                             m_uiReplacementMatchCounter;
    int                                             m_uiNumReplacementRequests;
    int                                             m_uiNumReplacementMatches;
    CElapsedTime                                    m_GTAVertexShadersDisabledTimer;
    bool                                            m_bGTAVertexShadersEnabled;
    std::set<RwTexture*>                            m_SpecialTextures;
    static int                                      ms_iRenderingType;
};
