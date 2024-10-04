/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CModelInfoSA.h
 *  PURPOSE:     Header file for entity model information handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/Common.h>
#include <game/CModelInfo.h>
#include "CRenderWareSA.h"

class CPedModelInfoSA;
class CPedModelInfoSAInterface;
struct CColModelSAInterface;
struct RpMaterial;

#define     RpGetFrame(__c)                 ((RwFrame*)(((RwObject *)(__c))->parent))

// #define     ARRAY_ModelLoaded               0x8E4CD0 // ##SA##
static void* CStreaming__ms_aInfoForModel = *(void**)(0x5B8B08 + 6);
static void* ARRAY_ModelLoaded = (char*)CStreaming__ms_aInfoForModel + 0x10;

#define     FUNC_CStreaming__HasModelLoaded 0x4044C0

// CModelInfo/ARRAY_ModelInfo __thiscall to load/replace vehicle models
#define     FUNC_LoadVehicleModel           0x4C95C0
#define     FUNC_LoadWeaponModel            0x4C9910
#define     FUNC_LoadPedModel               0x4C7340

#define     DWORD_AtomicsReplacerModelID    0xB71840
#define     FUNC_AtomicsReplacer            0x537150

// #define     ARRAY_ModelInfo                 0xA9B0C8
static void* ARRAY_ModelInfo = *(void**)(0x403DA4 + 3);
#define     CLASS_CText                     0xC1B340
#define     FUNC_CText_Get                  0x6A0050
#define     FUNC_GetBoundingBox             0x4082F0

#define     FUNC_RemoveRef                  0x4C4BB0
#define     FUNC_IsBoatModel                0x4c5a70
#define     FUNC_IsCarModel                 0x4c5aa0
#define     FUNC_IsTrainModel               0x4c5ad0
#define     FUNC_IsHeliModel                0x4c5b00
#define     FUNC_IsPlaneModel               0x4c5b30
#define     FUNC_IsBikeModel                0x4c5b60
#define     FUNC_IsFakePlaneModel           0x4c5b90
#define     FUNC_IsMonsterTruckModel        0x4c5bc0
#define     FUNC_IsQuadBikeModel            0x4c5bf0
#define     FUNC_IsBmxModel                 0x4c5c20
#define     FUNC_IsTrailerModel             0x4c5c50
#define     FUNC_IsVehicleModelType         0x4c5c80
#define     FUNC_RemoveModel                0x4089a0
#define     FUNC_FlushRequestList           0x40E4E0
#define     FUNC_RequestVehicleUpgrade      0x408C70

#define     FUNC_CVehicleModelInfo__GetNumRemaps        0x4C86B0

#define     VAR_CTempColModels_ModelPed1    0x968DF0

class CBaseModelInfoSAInterface;
class CModelInfoSAInterface
{
public:
    // Use GetModelInfo(int index) to get model info by id
    static CBaseModelInfoSAInterface** ms_modelInfoPtrs;
    static CBaseModelInfoSAInterface*  GetModelInfo(int index) { return ms_modelInfoPtrs[index]; }
};

/**
 * \todo Fill this class with info from R*
 */
class CBaseModelInfo_SA_VTBL
{
public:
    DWORD Destructor;
    DWORD AsAtomicModelInfoPtr;                  // ()
    DWORD AsDamageAtomicModelInfoPtr;            // ()
    DWORD AsLodAtomicModelInfoPtr;               // ()
    DWORD GetModelType;                          // ()           - Not defined in the base
    DWORD GetTimeInfo;                           // ()
    DWORD Init;                                  // ()
    DWORD Shutdown;                              // ()
    DWORD DeleteRwObject;                        // ()           - Not defined in the base
    DWORD GetRwModelType;                        // ()           - Not defined in the base
    DWORD CreateInstance_;                       // (RwMatrix*)   - Not defined in the base
    DWORD CreateInstance;                        // ()           - Not defined in the base
    DWORD SetAnimFile;                           // (char const*)
    DWORD ConvertAnimFileIndex;                  // ()
    DWORD GetAnimFileIndex;                      // ()
};

class CAtomicModelInfo_SA_VTBL : public CBaseModelInfo_SA_VTBL
{
    DWORD SetAtomic;            // (RpAtomic*)
};

class CDamageAtomicModelInfo_SA_VTBL : public CAtomicModelInfo_SA_VTBL
{
};

class CLodAtomicModelInfo_SA_VTBL : public CAtomicModelInfo_SA_VTBL
{
};

class CTimeModelInfo_SA_VTBL : public CAtomicModelInfo_SA_VTBL
{
};

class CLodTimeModelInfo_SA_VTBL : public CLodAtomicModelInfo_SA_VTBL
{
};

class CClumpModelInfo_SA_VTBL : public CBaseModelInfo_SA_VTBL
{
    DWORD SetClump;            // (RpClump*)
};

class CWeaponModelInfo_SA_VTBL : public CClumpModelInfo_SA_VTBL
{
};

class CPedModelInfo_SA_VTBL : public CClumpModelInfo_SA_VTBL
{
};

class CVehicleModelInfo_SA_VTBL : public CClumpModelInfo_SA_VTBL
{
};

enum class eModelSpecialType : unsigned char
{
    NONE = 0,
    TREE = 1,
    PALM = 2,
    GLASS_1 = 4,
    GLASS_2 = 5,
    TAG = 6,
    GARAGE_DOOR = 7,
    CRANE = 9,                // Not present in IDE but used in gta
    UNKNOW_1 = 10,            // Read only
    BREAKABLE_STATUE = 11,
};

class CBaseModelInfoSAInterface
{
public:
    CBaseModelInfo_SA_VTBL* VFTBL;            // +0

    unsigned long  ulHashKey;            // +4   Generated by CKeyGen::GetUppercaseKey(char const *) called by CBaseModelInfo::SetModelName(char const *)
    unsigned short usNumberOfRefs : 16;                 // +8
    unsigned short usTextureDictionary : 16;            // +10
    unsigned char  ucAlpha : 8;                         // +12

    unsigned char  ucNumOf2DEffects : 8;            // +13
    unsigned short usUnknown : 16;                  // +14     Something with 2d effects

    unsigned short usDynamicIndex : 16;            // +16

    // Flags used by CBaseModelInfo
    union
    {
        struct
        {
            unsigned char     bHasBeenPreRendered : 1;            // we use this because we need to apply changes only once
            unsigned char     bAlphaTransparency : 1;             // bDrawLast
            unsigned char     bAdditiveRender : 1;
            unsigned char     bDontWriteZBuffer : 1;
            unsigned char     bDontCastShadowsOn : 1;
            unsigned char     bDoWeOwnTheColModel : 1;
            unsigned char     bIsBackfaceCulled : 1;
            unsigned char     bIsColLoaded : 1;
            unsigned char     bIsRoad : 1;
            unsigned char     bHasComplexHierarchy : 1;
            unsigned char     bDontCollideWithFlyer : 1;
            eModelSpecialType eSpecialModelType : 4;
            unsigned char     bWetRoadReflection : 1;            // Used for tags
        };

        unsigned short usFlags;
    };

    CColModelSAInterface* pColModel;            // +20      CColModel: public CBoundingBox

    float     fLodDistanceUnscaled;            // +24      Scaled is this value multiplied with flt_B6F118
    RwObject* pRwObject;                       // +28

    // CWeaponModelInfo:
    // +36 = Weapon info as int

    // CPedModelInfo:
    // +36 = Motion anim group (AssocGroupID, long)
    // +40 = Default ped type (long)
    // +44 = Default ped stats (ePedStats)
    // +48 = Can drive cars (byte)
    // +50 = Ped flags (short)
    // +52 = Hit col model (CColModel*)
    // +56 = First radio station
    // +57 = Second radio station
    // +58 = Race (byte)
    // +60 = Audio ped type (short)
    // +62 = First voice
    // +64 = Last voice
    // +66 = Next voice (short)

    // CVehicleModelInfo:
    // +36 = Custom plate material (RpMaterial*)
    // +49 = Custom plate design (byte)
    // +50 = Pointer to game name (const char*)
    // +60 = Vehicle type (enum, int)
    // +64 = Wheel scale (float). Front/rear?
    // +68 = Wheel scale (float). Front/rear?
    // +72 = Wheel model id
    // +74 = Vehicle handling ID (word)
    // +76 = Number of doors (byte)
    // +77 = Vehicle list (byte)
    // +78 = Vehicle flags (byte)
    // +79 = Wheel upgrade class (byte)
    // +80 = Number of times used (byte)
    // +82 = Vehicle freq (short)
    // +84 = Component rules mask (long)
    // +88 = Steer angle
    // +92 = Pointer to some class containing back seat position @ +60 probably dummy storage.
    // +180 = Vehicle upgrade position descriptors array (32 bytes each)
    // +720 = Number of possible colors
    // +726 = Word array as referenced in CVehicleModelInfo::GetVehicleUpgrade(int)
    // +762 = Array of WORD containing something relative to paintjobs
    // +772 = Anim file index
};
static_assert(sizeof(CBaseModelInfoSAInterface) == 0x20, "Invalid size for CBaseModelInfoSAInterface");

struct CTimeInfoSAInterface
{
    CTimeInfoSAInterface() : m_nTimeOn(20), m_nTimeOff(6), m_wOtherTimeModel(0){};
    CTimeInfoSAInterface(char timeOn, char timeOff, short OtherTimeModel) : m_nTimeOn(timeOn), m_nTimeOff(timeOff), m_wOtherTimeModel(OtherTimeModel){};
    char  m_nTimeOn;
    char  m_nTimeOff;
    short m_wOtherTimeModel;
};

class CClumpModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
    union
    {
        char*    m_animFileName;
        uint32_t m_nAnimFileIndex;
    };
};

class CTimeModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
    CTimeInfoSAInterface timeInfo;
};

class CVehicleModelVisualInfoSAInterface            // Not sure about this name. If somebody knows more, please change
{
public:
    CVector vecDummies[15];
    char    m_sUpgrade[18];
};

class CVehicleModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
    uint32                              pad1;                      // +32
    RpMaterial*                         pPlateMaterial;            // +36
    char                                plateText[8];
    char                                pad[2];
    char                                gameName[8];
    char                                pad2[2];
    unsigned int                        uiVehicleType;
    float                               fWheelSizeFront;
    float                               fWheelSizeRear;
    short                               sWheelModel;
    short                               sHandlingID;
    byte                                ucNumDoors;
    byte                                ucVehicleList;
    byte                                ucVehicleFlags;
    byte                                ucWheelUpgradeClass;
    byte                                ucTimesUsed;
    short                               sVehFrequency;
    unsigned int                        uiComponentRules;
    float                               fSteeringAngle;
    CVehicleModelVisualInfoSAInterface* pVisualInfo;            // +92
    char                                pad3[464];
    char                                pDirtMaterial[64];            // *RwMaterial
    char                                pad4[64];
    char                                primColors[8];
    char                                secondColors[8];
    char                                treeColors[8];
    char                                fourColors[8];
    unsigned char                       ucNumOfColorVariations;
    unsigned char                       ucLastColorVariation;
    unsigned char                       ucPrimColor;
    unsigned char                       ucSecColor;
    unsigned char                       ucTertColor;
    unsigned char                       ucQuatColor;
    char                                upgrades[36];
    char                                anRemapTXDs[8];
    char                                pad5[2];
    char                                pAnimBlock[4];
};

class CModelInfoSA : public CModelInfo
{
protected:
    CBaseModelInfoSAInterface*                                                   m_pInterface;
    DWORD                                                                        m_dwModelID;
    DWORD                                                                        m_dwParentID;
    DWORD                                                                        m_dwReferences;
    DWORD                                                                        m_dwPendingInterfaceRef;
    CColModel*                                                                   m_pCustomColModel;
    CColModelSAInterface*                                                        m_pOriginalColModelInterface;
    std::uint16_t                                                                m_originalFlags = 0;
    RpClump*                                                                     m_pCustomClump;
    static std::map<unsigned short, int>                                         ms_RestreamTxdIDMap;
    static std::map<DWORD, float>                                                ms_ModelDefaultLodDistanceMap;
    static std::map<DWORD, unsigned short>                                       ms_ModelDefaultFlagsMap;
    static std::map<DWORD, BYTE>                                                 ms_ModelDefaultAlphaTransparencyMap;
    static std::unordered_map<std::uint32_t, std::map<eVehicleDummies, CVector>> ms_ModelDefaultDummiesPosition;
    static std::map<CTimeInfoSAInterface*, CTimeInfoSAInterface*>                ms_ModelDefaultModelTimeInfo;
    static std::unordered_map<DWORD, unsigned short>                             ms_OriginalObjectPropertiesGroups;
    static std::unordered_map<DWORD, std::pair<float, float>>                    ms_VehicleModelDefaultWheelSizes;
    static std::map<unsigned short, int>                                         ms_DefaultTxdIDMap;
    SVehicleSupportedUpgrades                                                    m_ModelSupportedUpgrades;

public:
    CModelInfoSA();

    CBaseModelInfoSAInterface* GetInterface();
    CPedModelInfoSAInterface*  GetPedModelInfoInterface() { return reinterpret_cast<CPedModelInfoSAInterface*>(GetInterface()); }

    DWORD          GetModel() { return m_dwModelID; }
    eModelInfoType GetModelType();
    uint           GetAnimFileIndex();

    bool IsPlayerModel();

    bool IsBoat();
    bool IsCar();
    bool IsTrain();
    bool IsHeli();
    bool IsPlane();
    bool IsBike();
    bool IsFakePlane();
    bool IsMonsterTruck();
    bool IsQuadBike();
    bool IsBmx();
    bool IsTrailer();
    bool IsVehicle() const override;
    bool IsUpgrade();

    char* GetNameIfVehicle();

    BYTE           GetVehicleType();
    void           Request(EModelRequestType requestType, const char* szTag);
    void           Remove();
    bool           UnloadUnused();
    bool           IsLoaded();
    bool           DoIsLoaded();
    unsigned short GetFlags();
    unsigned short GetOriginalFlags();
    void           SetIdeFlags(unsigned int uiFlags);
    void           SetIdeFlag(eModelIdeFlag eIdeFlag, bool bState);
    bool           GetIdeFlag(eModelIdeFlag eIdeFlag);
    void           SetFlags(unsigned short usFlags);
    static void    StaticResetFlags();
    CBoundingBox*  GetBoundingBox();
    bool           IsValid();
    bool           IsAllocatedInArchive();
    float          GetDistanceFromCentreOfMassToBaseOfModel();
    unsigned short GetTextureDictionaryID();
    void           SetTextureDictionaryID(unsigned short usID);
    void           ResetTextureDictionaryID();
    static void    StaticResetTextureDictionaries();
    float          GetLODDistance();
    float          GetOriginalLODDistance();
    void           SetLODDistance(float fDistance, bool bOverrideMaxDistance = false);
    static void    StaticResetLodDistances();
    void           RestreamIPL();
    static void    StaticFlushPendingRestreamIPL();
    static void    StaticSetHooks();
    bool           GetTime(char& cHourOn, char& cHourOff);
    bool           SetTime(char cHourOn, char cHourOff);
    static void    StaticResetModelTimes();

    void        SetAlphaTransparencyEnabled(bool bEnabled);
    bool        IsAlphaTransparencyEnabled();
    void        ResetAlphaTransparency();
    static void StaticResetAlphaTransparencies();

    void ModelAddRef(EModelRequestType requestType, const char* szTag);
    int  GetRefCount();
    void RemoveRef(bool bRemoveExtraGTARef = false);
    bool ForceUnload();

    // CVehicleModelInfo specific
    short        GetAvailableVehicleMod(unsigned short usSlot);
    bool         IsUpgradeAvailable(eVehicleUpgradePosn posn);
    void         SetCustomCarPlateText(const char* szText);
    unsigned int GetNumRemaps();
    void*        GetVehicleSuspensionData();
    void*        SetVehicleSuspensionData(void* pSuspensionLines);
    CVector      GetVehicleExhaustFumesPosition() override;
    void         SetVehicleExhaustFumesPosition(const CVector& vecPosition) override;
    CVector      GetVehicleDummyDefaultPosition(eVehicleDummies eDummy) override;
    CVector      GetVehicleDummyPosition(eVehicleDummies eDummy) override;
    bool         GetVehicleDummyPositions(std::array<CVector, VEHICLE_DUMMY_COUNT>& positions) const override;
    void         SetVehicleDummyPosition(eVehicleDummies eDummy, const CVector& vecPosition) override;
    void         ResetVehicleDummies(bool bRemoveFromDummiesMap);
    static void  ResetAllVehicleDummies();
    float        GetVehicleWheelSize(eResizableVehicleWheelGroup eWheelGroup) override;
    void         SetVehicleWheelSize(eResizableVehicleWheelGroup eWheelGroup, float fWheelSize) override;
    void         ResetVehicleWheelSizes(std::pair<float, float>* defaultSizes = nullptr) override;
    static void  ResetAllVehiclesWheelSizes();

    // ONLY use for peds
    void GetVoice(short* psVoiceType, short* psVoice);
    void GetVoice(const char** pszVoiceType, const char** szVoice);
    void SetVoice(short sVoiceType, short sVoice);
    void SetVoice(const char* szVoiceType, const char* szVoice);

    // Custom collision related functions
    bool SetCustomModel(RpClump* pClump) override;
    void RestoreOriginalModel() override;
    void SetColModel(CColModel* pColModel) override;
    void RestoreColModel() override;
    void MakeCustomModel() override;

    // Increases the collision slot reference counter for the original collision model
    void AddColRef() override;

    // Decreases the collision slot reference counter for the original collision model
    void RemoveColRef() override;

    void SetModelID(DWORD dwModelID) { m_dwModelID = dwModelID; }

    RwObject* GetRwObject() { return m_pInterface ? m_pInterface->pRwObject : NULL; }

    // CModelInfoSA methods
    void         MakePedModel(char* szTexture);
    void         MakeObjectModel(ushort usBaseModelID);
    void         MakeVehicleAutomobile(ushort usBaseModelID);
    void         MakeTimedObjectModel(ushort usBaseModelID);
    void         MakeClumpModel(ushort usBaseModelID);
    void         DeallocateModel(void);
    unsigned int GetParentID() { return m_dwParentID; };

    SVehicleSupportedUpgrades GetVehicleSupportedUpgrades() { return m_ModelSupportedUpgrades; }

    void InitialiseSupportedUpgrades(RpClump* pClump);
    void ResetSupportedUpgrades();

    void           SetObjectPropertiesGroup(unsigned short usObjectGroup);
    unsigned short GetObjectPropertiesGroup();
    void           RestoreObjectPropertiesGroup();
    static void    RestoreAllObjectsPropertiesGroups();

    // Vehicle towing functions
    bool IsTowableBy(CModelInfo* towingModel) override;

    bool IsDynamic() { return m_pInterface ? m_pInterface->usDynamicIndex != 0xffff : false; };

private:
    void CopyStreamingInfoFromModel(ushort usCopyFromModelID);
    void RwSetSupportedUpgrades(RwFrame* parent, DWORD dwModel);
    void SetModelSpecialType(eModelSpecialType eType, bool bState);
};
