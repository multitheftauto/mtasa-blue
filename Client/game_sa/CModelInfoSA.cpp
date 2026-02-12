/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CModelInfoSA.cpp
 *  PURPOSE:     Entity model information handler
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <chrono>
#include <cstdint>
#include <vector>
#include <core/CCoreInterface.h>
#include "CColModelSA.h"
#include "CColStoreSA.h"
#include "CGameSA.h"
#include "CModelInfoSA.h"
#include "CPedModelInfoSA.h"
#include "CPedSA.h"
#include "CWorldSA.h"
#include "gamesa_renderware.h"

extern CCoreInterface* g_pCore;
extern CGameSA*        pGame;

CBaseModelInfoSAInterface** CModelInfoSAInterface::ms_modelInfoPtrs = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;
CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;

std::map<unsigned short, int>                                        CModelInfoSA::ms_RestreamTxdIDMap;
std::map<DWORD, float>                                               CModelInfoSA::ms_ModelDefaultLodDistanceMap;
std::map<DWORD, unsigned short>                                      CModelInfoSA::ms_ModelDefaultFlagsMap;
std::map<DWORD, BYTE>                                                CModelInfoSA::ms_ModelDefaultAlphaTransparencyMap;
std::unordered_map<std::uint32_t, std::map<VehicleDummies, CVector>> CModelInfoSA::ms_ModelDefaultDummiesPosition;
std::map<DWORD, CTimeInfoSAInterface>                                CModelInfoSA::ms_ModelDefaultModelTimeInfo;
std::unordered_map<DWORD, unsigned short>                            CModelInfoSA::ms_OriginalObjectPropertiesGroups;
std::unordered_map<DWORD, std::pair<float, float>>                   CModelInfoSA::ms_VehicleModelDefaultWheelSizes;
std::map<unsigned short, int>                                        CModelInfoSA::ms_DefaultTxdIDMap;

void CModelInfoSA::ClearModelDefaults(DWORD modelId)
{
    ms_DefaultTxdIDMap.erase(static_cast<unsigned short>(modelId));
    ms_ModelDefaultFlagsMap.erase(modelId);
    ms_ModelDefaultLodDistanceMap.erase(modelId);
    ms_ModelDefaultAlphaTransparencyMap.erase(modelId);
    ms_OriginalObjectPropertiesGroups.erase(modelId);
    ms_ModelDefaultDummiesPosition.erase(modelId);
    ms_VehicleModelDefaultWheelSizes.erase(modelId);
    ms_ModelDefaultModelTimeInfo.erase(modelId);
}

union tIdeFlags
{
    struct
    {
        char bIsRoad : 1;
        char bFlag2 : 1;
        char bDrawLast : 1;
        char bAdditive : 1;
        char bFlag5 : 1;
        char bFlag6 : 1;
        char bNoZBufferWrite : 1;
        char bDontReceiveShadows : 1;

        char bFlag9 : 1;
        char bIsGlassType1 : 1;
        char bIsGlassType2 : 1;
        char bIsGarageDoor : 1;
        char bIsDamagable : 1;
        char bIsTree : 1;
        char bIsPalm : 1;
        char bDontCollideWithFlyer : 1;

        char bFlag17 : 1;
        char bFlag18 : 1;
        char bFlag19 : 1;
        char bFlag20 : 1;
        char bIsTag : 1;
        char bDisableBackfaceCulling : 1;
        char bIsBreakableStatue : 1;
        char bFlag24 : 1;

        char cPad : 8;
    };
    unsigned int uiFlags;
};

static constexpr uintptr_t vftable_CVehicleModelInfo = 0x85C5C8u;
static constexpr size_t    RESOURCE_ID_COL = 25000;

static void CBaseModelInfo_SetColModel(CBaseModelInfoSAInterface* self, CColModelSAInterface* colModel, bool applyToPairedModel)
{
    using Signature = void(__thiscall*)(CBaseModelInfoSAInterface*, CColModelSAInterface*, bool);
    auto function = reinterpret_cast<Signature>(0x4C4BC0);
    function(self, colModel, applyToPairedModel);
}

static void CColAccel_addCacheCol(int idx, const CColModelSAInterface* colModel)
{
    using Signature = void(__cdecl*)(int, const CColModelSAInterface*);
    auto function = reinterpret_cast<Signature>(0x5B2C20);
    function(idx, colModel);
}

// Validates model info pointer by checking VFTBL is in valid GTA:SA code range.
// Uses SEH for crash protection when reading the VFTBL field, but avoids
// the expensive volatile read of VFTBL->Destructor by using address validation.
static bool IsValidModelInfoPtr(const void* ptr) noexcept
{
    if (!ptr)
        return false;

    __try
    {
        const auto* p = static_cast<const CBaseModelInfoSAInterface*>(ptr);
        const DWORD vftbl = reinterpret_cast<DWORD>(p->VFTBL);
        // VFTBL must be in valid GTA:SA code range - this implicitly validates
        // the pointer since garbage/freed memory won't have valid VFTBL addresses
        return SharedUtil::IsValidGtaSaPtr(vftbl);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

static bool SafeReadColSlot(CColModelSAInterface* pColModel, unsigned short* pOut) noexcept
{
    __try
    {
        *pOut = pColModel->m_sphere.m_collisionSlot;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

static bool SafeReadColData(CColModelSAInterface* pColModel, CColDataSA** pOut) noexcept
{
    __try
    {
        *pOut = pColModel->m_data;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

static bool SafeReadSuspLines(CColDataSA* pColData, void** pOut) noexcept
{
    __try
    {
        *pOut = pColData->m_suspensionLines;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

static bool SafeSwapSuspLines(CColDataSA* pColData, void* pNew, void** pOld) noexcept
{
    __try
    {
        *pOld = pColData->m_suspensionLines;
        pColData->m_suspensionLines = reinterpret_cast<CColLineSA*>(pNew);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

CModelInfoSA::CModelInfoSA()
{
    m_pInterface = NULL;
    m_dwModelID = 0xFFFFFFFF;
    m_dwParentID = 0;
    m_dwReferences = 0;
    m_dwPendingInterfaceRef = 0;
    m_pOriginalColModelInterface = NULL;
    m_colRefCount = 0;
    m_usColSlot = 0xFFFF;
    m_pCustomClump = NULL;
    m_pCustomColModel = NULL;
}

CBaseModelInfoSAInterface* CModelInfoSA::GetInterface()
{
    m_pInterface = ppModelInfo[m_dwModelID];
    if (!m_pInterface)
        return nullptr;

    if (!IsValidModelInfoPtr(m_pInterface))
    {
        m_pInterface = nullptr;
        return nullptr;
    }
    if (m_usColSlot == 0xFFFF && m_pInterface->pColModel)
    {
        unsigned short slot;
        if (!SafeReadColSlot(m_pInterface->pColModel, &slot))
        {
            AddReportLog(5553, SString("GetInterface: pColModel access failed for model %u", m_dwModelID), 10);
            m_pInterface = nullptr;
            return nullptr;
        }
        m_usColSlot = slot;
    }
    return m_pInterface;
}

bool CModelInfoSA::IsBoat()
{
    DWORD dwFunction = FUNC_IsBoatModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsCar()
{
    DWORD dwFunction = FUNC_IsCarModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsTrain()
{
    DWORD dwFunction = FUNC_IsTrainModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsHeli()
{
    DWORD dwFunction = FUNC_IsHeliModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsPlane()
{
    DWORD dwFunction = FUNC_IsPlaneModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsBike()
{
    DWORD dwFunction = FUNC_IsBikeModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsFakePlane()
{
    DWORD dwFunction = FUNC_IsFakePlaneModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsMonsterTruck()
{
    DWORD dwFunction = FUNC_IsMonsterTruckModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsQuadBike()
{
    DWORD dwFunction = FUNC_IsQuadBikeModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsBmx()
{
    DWORD dwFunction = FUNC_IsBmxModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

bool CModelInfoSA::IsTrailer()
{
    DWORD dwFunction = FUNC_IsTrailerModel;
    DWORD ModelID = m_dwModelID;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    // clang-format on
    return bReturn;
}

BYTE CModelInfoSA::GetVehicleType() const noexcept
{
    // This function will return a vehicle type for vehicles or 0xFF on failure
    if (!IsVehicle())
        return -1;

    auto GetVehicleModelType = reinterpret_cast<BYTE(__cdecl*)(DWORD)>(FUNC_IsVehicleModelType);
    return GetVehicleModelType(m_dwModelID);
}

bool CModelInfoSA::IsVehicle() const
{
    // NOTE(botder): This is from CModelInfo::IsVehicleModelType
    if (m_dwModelID >= 20000)
        return false;

    if (!IsAllocatedInArchive())
        return false;

    CBaseModelInfoSAInterface* model = ppModelInfo[m_dwModelID];
    if (!IsValidModelInfoPtr(model))
        return false;

    return reinterpret_cast<intptr_t>(model->VFTBL) == vftable_CVehicleModelInfo;
}

bool CModelInfoSA::IsVehicleModel(std::uint32_t model) noexcept
{
    const auto* const modelInfo = pGame->GetModelInfo(model);
    return modelInfo && modelInfo->IsVehicle();
}

bool CModelInfoSA::IsPlayerModel()
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    return pInterface && pInterface->pColModel && pInterface->pColModel == (CColModelSAInterface*)VAR_CTempColModels_ModelPed1;
}

bool CModelInfoSA::IsUpgrade()
{
    return m_dwModelID >= 1000 && m_dwModelID <= 1193;
}

char* CModelInfoSA::GetNameIfVehicle()
{
    DWORD dwModelInfo = (DWORD)ARRAY_ModelInfo;
    DWORD dwFunc = FUNC_CText_Get;
    DWORD ModelID = m_dwModelID;
    DWORD dwReturn = 0;

    // clang-format off
        __asm
        {
            push    eax
            push    ebx
            push    ecx

            mov     ebx, ModelID
            lea     ebx, [ebx*4]
            add     ebx, dword ptr[ARRAY_ModelInfo]
            mov     eax, [ebx]
            add     eax, 50

            push    eax
            mov     ecx, CLASS_CText
            call    dwFunc

            mov     dwReturn, eax

            pop     ecx
            pop     ebx
            pop     eax
        }
    // clang-format on
    return (char*)dwReturn;
}

uint CModelInfoSA::GetAnimFileIndex()
{
    if (!m_pInterface || !m_pInterface->VFTBL)
        return 0xFFFFFFFF;

    DWORD dwFunc = m_pInterface->VFTBL->GetAnimFileIndex;
    if (!SharedUtil::IsValidGtaSaPtr(dwFunc))
        return 0xFFFFFFFF;

    DWORD dwThis = (DWORD)m_pInterface;
    uint  uiReturn = 0;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     uiReturn, eax
    }
    // clang-format on
    return uiReturn;
}

void CModelInfoSA::Request(EModelRequestType requestType, const char* szTag)
{
    // don't bother loading it if it already is
    if (IsLoaded())
        return;

    // Don't request deallocated models (ppModelInfo[id] is NULL)
    if (!IsValid())
        return;

    // Bikes can sometimes get stuck when loading unless the anim file is handled like what is does here
    // Don't change the code below unless you can test it (by recreating the problem it solves)
    if (IsVehicle())
    {
        uint uiAnimFileIndex = GetAnimFileIndex();
        if (uiAnimFileIndex != 0xffffffff)
        {
            uint          uiAnimId = uiAnimFileIndex + pGame->GetBaseIDforIFP();
            CModelInfoSA* pAnim = static_cast<CModelInfoSA*>(pGame->GetModelInfo(uiAnimId));
            if (!pAnim)
            {
                if (uiAnimId != pGame->GetBaseIDforIFP() + 139)
                    LogEvent(505, "Model no anim", "", SString("%d (%d)", m_dwModelID, uiAnimId));
            }
            else if (!pAnim->IsLoaded())
            {
                OutputDebugLine(SString("[Models] Requesting anim file %d for model %d", uiAnimId, m_dwModelID));
                pAnim->Request(requestType, szTag);
            }
        }
    }
    else if (m_dwModelID <= 288 && m_dwModelID != 7 && !pGame->GetModelInfo(7)->IsLoaded())
    {
        // Skin 7 must be loaded in order for other skins to work. No, really. (#4010)
        pGame->GetModelInfo(7)->Request(requestType, "Model 7");
    }

    if (requestType == BLOCKING)
    {
        pGame->GetStreaming()->RequestModel(m_dwModelID, 0x16);
        uint32_t blockingStartTick = GetTickCount32();
        pGame->GetStreaming()->LoadAllRequestedModels(true, szTag);
        if (!IsLoaded())
        {
            // Try 3 more times, final time without high priority flag
            int iCount = 0;
            while (iCount++ < 10 && !IsLoaded())
            {
                // Cap total blocking time (including the first load attempt above)
                // to prevent extended freezes from persistent streaming I/O issues
                if ((GetTickCount32() - blockingStartTick) > 10000)
                    break;

                bool bOnlyPriorityModels = (iCount < 3 || iCount & 1);
                pGame->GetStreaming()->LoadAllRequestedModels(bOnlyPriorityModels, szTag);
            }
            if (!IsLoaded())
            {
                AddReportLog(6641, SString("Blocking load fail: %d (%s)", m_dwModelID, szTag));
                LogEvent(641, "Blocking load fail", "", SString("%d (%s)", m_dwModelID, szTag));
                dassert(0);
            }
            else
            {
                AddReportLog(6642, SString("Blocking load: %d (%s) (Took %d attempts)", m_dwModelID, szTag, iCount));
                LogEvent(642, "Blocking load", "", SString("%d (%s) (Took %d attempts)", m_dwModelID, szTag, iCount));
            }
        }
    }
    else
    {
        pGame->GetStreaming()->RequestModel(m_dwModelID, 0x06);
    }
}

void CModelInfoSA::Remove()
{
    // Don't remove if GTA refers to it somehow.
    // Or we'll screw up SA's map for example.

    m_pInterface = GetInterface();
    if (!m_pInterface)
        return;

    // Remove our reference
    if (m_pInterface->usNumberOfRefs > 0)
    {
        if (CTxdStore_GetTxd(m_pInterface->usTextureDictionary) != nullptr)
            CTxdStore_RemoveRef(m_pInterface->usTextureDictionary);
        m_pInterface->usNumberOfRefs--;
    }

    // No references left?
    if (m_pInterface->usNumberOfRefs == 0 && !m_pCustomClump && !m_pCustomColModel)
    {
        // Remove the model.
        pGame->GetStreaming()->RemoveModel(m_dwModelID);
    }
}

bool CModelInfoSA::UnloadUnused()
{
    m_pInterface = GetInterface();
    if (!m_pInterface)
        return false;

    if (m_pInterface->usNumberOfRefs == 0 && !m_pCustomClump && !m_pCustomColModel)
    {
        pGame->GetStreaming()->RemoveModel(m_dwModelID);
        return true;
    }
    return false;
}

bool CModelInfoSA::IsLoaded()
{
    if (DoIsLoaded())
    {
        if (m_dwPendingInterfaceRef)
        {
            assert(m_dwReferences > 0);
            CBaseModelInfoSAInterface* pInterface = GetInterface();
            if (pInterface)
            {
                pInterface->usNumberOfRefs++;
                CTxdStore_AddRef(pInterface->usTextureDictionary);
                m_dwPendingInterfaceRef = 0;
            }
        }
        return true;
    }
    return false;
}

bool CModelInfoSA::DoIsLoaded()
{
    // return (BOOL)*(BYTE *)(ARRAY_ModelLoaded + 20*dwModelID);
    bool bLoaded = pGame->GetStreaming()->HasModelLoaded(m_dwModelID);

    const int32_t baseTxdId = pGame->GetBaseIDforTXD();
    if (baseTxdId > 0 && m_dwModelID < static_cast<DWORD>(baseTxdId))
    {
        m_pInterface = GetInterface();

        if (bLoaded)
        {
            if (!m_pInterface || !m_pInterface->pRwObject)
            {
                CStreamingInfo* pStreamInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);
                if (pStreamInfo)
                {
                    pStreamInfo->prevId = static_cast<unsigned short>(-1);
                    pStreamInfo->nextId = static_cast<unsigned short>(-1);
                    pStreamInfo->nextInImg = static_cast<unsigned short>(-1);
                    pStreamInfo->loadState = eModelLoadState::LOADSTATE_NOT_LOADED;
                }
                if (!m_pInterface)
                {
                    m_dwPendingInterfaceRef = 0;
                    m_dwReferences = 0;
                }
                return false;
            }
        }
    }
    return bLoaded;
}

unsigned short CModelInfoSA::GetFlags()
{
    CBaseModelInfoSAInterface* pInterface = ppModelInfo[m_dwModelID];
    if (!IsValidModelInfoPtr(pInterface))
        return 0;
    return pInterface->usFlags;
}

unsigned short CModelInfoSA::GetOriginalFlags()
{
    if (MapContains(ms_ModelDefaultFlagsMap, m_dwModelID))
        return MapGet(ms_ModelDefaultFlagsMap, m_dwModelID);

    CBaseModelInfoSAInterface* pInterface = ppModelInfo[m_dwModelID];
    if (!IsValidModelInfoPtr(pInterface))
        return 0;
    return pInterface->usFlags;
}

void CModelInfoSA::SetFlags(unsigned short usFlags)
{
    m_pInterface = GetInterface();
    if (!m_pInterface)
        return;

    // Save default value if not done yet
    if (!MapContains(ms_ModelDefaultFlagsMap, m_dwModelID))
        MapSet(ms_ModelDefaultFlagsMap, m_dwModelID, m_pInterface->usFlags);

    // Don't change bIsColLoaded flag
    usFlags &= 0xFF7F;                        // Disable flag in input
    usFlags |= m_pInterface->usFlags & 0x80;  // Apply current bIsColLoaded flag

    m_pInterface->usFlags = usFlags;
}

void CModelInfoSA::SetIdeFlags(unsigned int uiFlags)
{
    m_pInterface = GetInterface();
    if (!m_pInterface)
        return;

    // Save default value if not done yet
    if (!MapContains(ms_ModelDefaultFlagsMap, m_dwModelID))
        MapSet(ms_ModelDefaultFlagsMap, m_dwModelID, m_pInterface->usFlags);

    tIdeFlags ideFlags;
    ideFlags.uiFlags = uiFlags;

    // Default value is 0xC0 (bIsColLoaded + bIsBackfaceCulled)
    // But bIsColLoaded should not be changed
    m_pInterface->usFlags &= 0x80;  // Reset all flags except bIsColLoaded
    m_pInterface->bIsBackfaceCulled = true;

    // setBaseModelInfoFlags
    if (ideFlags.bDrawLast)
    {
        m_pInterface->bAlphaTransparency = true;
        m_pInterface->bAdditiveRender = true;
    }

    m_pInterface->bAdditiveRender = ideFlags.bAdditive;
    m_pInterface->bDontWriteZBuffer = ideFlags.bNoZBufferWrite;
    m_pInterface->bDontCastShadowsOn = ideFlags.bDontReceiveShadows;
    m_pInterface->bIsBackfaceCulled = !ideFlags.bDisableBackfaceCulling;

    switch (GetModelType())
    {
        case eModelInfoType::ATOMIC:
        case eModelInfoType::TIME:
        {
            // SetAtomicModelInfoFlags
            m_pInterface->bIsRoad = ideFlags.bIsRoad;

            m_pInterface->bDontCollideWithFlyer = ideFlags.bDontCollideWithFlyer;

            if (ideFlags.bFlag20)
                m_pInterface->eSpecialModelType = eModelSpecialType::CRANE;
            else if (ideFlags.bFlag24)
                m_pInterface->eSpecialModelType = eModelSpecialType::UNKNOW_1;
            else if (ideFlags.bIsBreakableStatue)
                m_pInterface->eSpecialModelType = eModelSpecialType::BREAKABLE_STATUE;
            else if (ideFlags.bIsTag)
                m_pInterface->eSpecialModelType = eModelSpecialType::TAG;
            else if (ideFlags.bIsPalm)
                m_pInterface->eSpecialModelType = eModelSpecialType::PALM;
            else if (ideFlags.bIsTree)
                m_pInterface->eSpecialModelType = eModelSpecialType::TREE;
            else if (ideFlags.bIsGarageDoor)
                m_pInterface->eSpecialModelType = eModelSpecialType::GARAGE_DOOR;
            else if (ideFlags.bIsGlassType2)
                m_pInterface->eSpecialModelType = eModelSpecialType::GLASS_2;
            else if (ideFlags.bIsGlassType1)
                m_pInterface->eSpecialModelType = eModelSpecialType::GLASS_1;
            break;
        }
        case eModelInfoType::CLUMP:
        {
            m_pInterface->bAdditiveRender = ideFlags.bFlag6;
            break;
        }
        default:
            break;
    }
}

void CModelInfoSA::SetIdeFlag(eModelIdeFlag eIdeFlag, bool bState)
{
    m_pInterface = GetInterface();
    if (!m_pInterface)
        return;

    switch (eIdeFlag)
    {
        case eModelIdeFlag::IS_ROAD:
            m_pInterface->bIsRoad = bState;
            break;
        case eModelIdeFlag::DRAW_LAST:
            m_pInterface->bAlphaTransparency = bState;
            if (bState)
                m_pInterface->bAdditiveRender = true;
            break;
        case eModelIdeFlag::ADDITIVE:
            m_pInterface->bAdditiveRender = bState;
            break;
        case eModelIdeFlag::IGNORE_LIGHTING:
            m_pInterface->bAdditiveRender = bState;
            break;
        case eModelIdeFlag::NO_ZBUFFER_WRITE:
            m_pInterface->bDontWriteZBuffer = bState;
            break;
        case eModelIdeFlag::DONT_RECEIVE_SHADOWS:
            m_pInterface->bDontCastShadowsOn = bState;
            break;
        case eModelIdeFlag::IS_GLASS_TYPE_1:
            SetModelSpecialType(eModelSpecialType::GLASS_1, bState);
            break;
        case eModelIdeFlag::IS_GLASS_TYPE_2:
            SetModelSpecialType(eModelSpecialType::GLASS_2, bState);
            break;
        case eModelIdeFlag::IS_GARAGE_DOOR:
            SetModelSpecialType(eModelSpecialType::GARAGE_DOOR, bState);
            break;
        case eModelIdeFlag::IS_TREE:
            SetModelSpecialType(eModelSpecialType::TREE, bState);
            break;
        case eModelIdeFlag::IS_PALM:
            SetModelSpecialType(eModelSpecialType::PALM, bState);
            break;
        case eModelIdeFlag::IS_TAG:
            SetModelSpecialType(eModelSpecialType::TAG, bState);
            break;
        case eModelIdeFlag::IS_BREAKABLE_STATUE:
            SetModelSpecialType(eModelSpecialType::BREAKABLE_STATUE, bState);
            break;
        case eModelIdeFlag::IS_CRANE:
            SetModelSpecialType(eModelSpecialType::CRANE, bState);
            break;
        case eModelIdeFlag::IS_DAMAGABLE:
            // Can't set
            break;
        case eModelIdeFlag::DOES_NOT_COLLIDE_WITH_FLYER:
            m_pInterface->bDontCollideWithFlyer = bState;
            break;
        case eModelIdeFlag::DISABLE_BACKFACE_CULLING:
            m_pInterface->bIsBackfaceCulled = !bState;
            break;
        default:
            break;
    }
}

bool CModelInfoSA::GetIdeFlag(eModelIdeFlag eIdeFlag)
{
    m_pInterface = GetInterface();
    if (!m_pInterface)
        return false;

    switch (eIdeFlag)
    {
        case eModelIdeFlag::IS_ROAD:
            return m_pInterface->bIsRoad;
        case eModelIdeFlag::DRAW_LAST:
            return m_pInterface->bAlphaTransparency;
        case eModelIdeFlag::ADDITIVE:
            return m_pInterface->bAdditiveRender;
        case eModelIdeFlag::IGNORE_LIGHTING:
            return m_pInterface->bAdditiveRender;
        case eModelIdeFlag::NO_ZBUFFER_WRITE:
            return m_pInterface->bDontWriteZBuffer;
        case eModelIdeFlag::DONT_RECEIVE_SHADOWS:
            return m_pInterface->bDontCastShadowsOn;
        case eModelIdeFlag::IS_GLASS_TYPE_1:
            return m_pInterface->eSpecialModelType == eModelSpecialType::GLASS_1;
        case eModelIdeFlag::IS_GLASS_TYPE_2:
            return m_pInterface->eSpecialModelType == eModelSpecialType::GLASS_2;
        case eModelIdeFlag::IS_GARAGE_DOOR:
            return m_pInterface->eSpecialModelType == eModelSpecialType::GARAGE_DOOR;
        case eModelIdeFlag::IS_TREE:
            return m_pInterface->eSpecialModelType == eModelSpecialType::TREE;
        case eModelIdeFlag::IS_PALM:
            return m_pInterface->eSpecialModelType == eModelSpecialType::PALM;
        case eModelIdeFlag::IS_TAG:
            return m_pInterface->eSpecialModelType == eModelSpecialType::TAG;
        case eModelIdeFlag::IS_BREAKABLE_STATUE:
            return m_pInterface->eSpecialModelType == eModelSpecialType::BREAKABLE_STATUE;
        case eModelIdeFlag::IS_CRANE:
            return m_pInterface->eSpecialModelType == eModelSpecialType::CRANE;
        case eModelIdeFlag::IS_DAMAGABLE:
        {
            DWORD funcAddr = m_pInterface->VFTBL->AsDamageAtomicModelInfoPtr;
            if (!SharedUtil::IsValidGtaSaPtr(funcAddr))
                return false;
            return ((bool (*)())funcAddr)();
        }
        case eModelIdeFlag::DOES_NOT_COLLIDE_WITH_FLYER:
            return m_pInterface->bDontCollideWithFlyer;
        case eModelIdeFlag::DISABLE_BACKFACE_CULLING:
            return !m_pInterface->bIsBackfaceCulled;
        default:
            return false;
    }
}

void CModelInfoSA::SetModelSpecialType(eModelSpecialType eType, bool bState)
{
    if (bState)
        m_pInterface->eSpecialModelType = eType;
    else if (m_pInterface->eSpecialModelType == eType)
        m_pInterface->eSpecialModelType = eModelSpecialType::NONE;
}

void CModelInfoSA::StaticResetFlags()
{
    for (auto iter = ms_ModelDefaultFlagsMap.begin(); iter != ms_ModelDefaultFlagsMap.end();)
    {
        CBaseModelInfoSAInterface* pInterface = ppModelInfo[iter->first];
        if (!IsValidModelInfoPtr(pInterface))
        {
            iter = ms_ModelDefaultFlagsMap.erase(iter);
            continue;
        }

        ushort usFlags = iter->second;
        usFlags &= 0xFF7F;
        usFlags |= pInterface->usFlags & 0x80;
        pInterface->usFlags = usFlags;
        iter = ms_ModelDefaultFlagsMap.erase(iter);
    }
}

CBoundingBox* CModelInfoSA::GetBoundingBox()
{
    DWORD         dwFunc = FUNC_GetBoundingBox;
    DWORD         ModelID = m_dwModelID;
    CBoundingBox* dwReturn = 0;
    // clang-format off
    __asm
    {
        push    ModelID
        call    dwFunc
        add     esp, 4
        mov     dwReturn, eax
    }
    // clang-format on
    return dwReturn;
}

bool CModelInfoSA::IsCollisionLoaded() const noexcept
{
    if (m_dwModelID >= MODELINFO_DFF_MAX)
        return false;

    const CBaseModelInfoSAInterface* pInterface = ppModelInfo[m_dwModelID];
    return IsValidModelInfoPtr(pInterface) && pInterface->pColModel != nullptr;
}

bool CModelInfoSA::IsRwObjectLoaded() const noexcept
{
    if (m_dwModelID >= MODELINFO_DFF_MAX)
        return false;

    const CBaseModelInfoSAInterface* pInterface = ppModelInfo[m_dwModelID];
    return IsValidModelInfoPtr(pInterface) && pInterface->pRwObject != nullptr;
}

void CModelInfoSA::WaitForModelFullyLoaded(std::chrono::milliseconds timeout)
{
    if (!IsValid())
        return;

    if (IsLoaded())
        return;

    const bool hasTimeout = timeout.count() > 0;
    auto       start = std::chrono::steady_clock::now();

    do
    {
        Request(hasTimeout ? NON_BLOCKING : BLOCKING, "WaitForModelFullyLoaded");

        if (!hasTimeout)
            return;

        pGame->GetStreaming()->LoadAllRequestedModels(false, "WaitForModelFullyLoaded");

        if (IsLoaded())
            return;
    } while (std::chrono::steady_clock::now() - start < timeout);
}

bool CModelInfoSA::IsValid()
{
    if (m_dwModelID >= MODELINFO_DFF_MAX && m_dwModelID < MODELINFO_TXD_MAX)
        return !pGame->GetPools()->GetTxdPool().IsFreeTextureDictonarySlot(m_dwModelID - MODELINFO_DFF_MAX);

    const int32_t baseTxdId = pGame->GetBaseIDforTXD();
    const int32_t countOfAllFileIds = pGame->GetCountOfAllFileIDs();
    if (baseTxdId > 0 && countOfAllFileIds > baseTxdId)
    {
        const DWORD baseTxdIdDw = static_cast<DWORD>(baseTxdId);
        const DWORD countDw = static_cast<DWORD>(countOfAllFileIds);
        if (m_dwModelID >= baseTxdIdDw && m_dwModelID < countDw)
            return true;
    }

    if (m_dwModelID >= MODELINFO_DFF_MAX)
        return false;

    m_pInterface = GetInterface();
    return m_pInterface != nullptr;
}

bool CModelInfoSA::IsAllocatedInArchive() const noexcept
{
    CStreamingInfo* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);
    return pStreamingInfo && pStreamingInfo->sizeInBlocks > 0;
}

float CModelInfoSA::GetDistanceFromCentreOfMassToBaseOfModel()
{
    DWORD dwModelInfo = 0;
    DWORD ModelID = m_dwModelID;
    float fReturn = 0;
    // clang-format off
    __asm
    {
        mov     eax, ModelID

        push    ecx
        mov     ecx, dword ptr[ARRAY_ModelInfo]
        mov     eax, dword ptr[ecx + eax*4]
        pop     ecx

        mov     eax, [eax+20]
        cmp     eax, 0
        jz      skip
        fld     [eax + 8]
        fchs
        fstp    fReturn
skip:
    }
    // clang-format on
    return fReturn;
}

unsigned short CModelInfoSA::GetTextureDictionaryID()
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    return pInterface ? pInterface->usTextureDictionary : 0;
}

void CModelInfoSA::SetTextureDictionaryID(unsigned short usID)
{
    m_pInterface = GetInterface();
    if (!m_pInterface)
        return;

    unsigned short usOldTxdId = m_pInterface->usTextureDictionary;
    if (usOldTxdId == usID)
        return;

    // Slot allocated (e.g. via engineRequestTXD) but TXD data isn't loaded yet.
    // For unloaded models, just record the new TXD ID; SA streaming handles refs
    // and texture resolution when it loads both the model and its TXD dependency.
    // Loaded models fall through to the ref-transfer path below. CTxdStore_AddRef
    // only touches usUsagesCount (never dereferences rwTexDictonary), and
    // BuildTxdTextureMap returns an empty map for null TXDs, making the rebind a
    // no-op. The real texture data arrives later via engineImageLinkTXD + restream.
    if (CTxdStore_GetTxd(usID) == nullptr)
    {
        if (!pGame || pGame->GetPools()->GetTxdPool().IsFreeTextureDictonarySlot(usID))
            return;

        if (!m_pInterface->pRwObject)
        {
            if (!MapContains(ms_DefaultTxdIDMap, static_cast<unsigned short>(m_dwModelID)))
                ms_DefaultTxdIDMap[static_cast<unsigned short>(m_dwModelID)] = usOldTxdId;

            m_pInterface->usTextureDictionary = usID;
            return;
        }

        // Loaded model: fall through to ref-transfer path
    }

    size_t referencesCount = m_pInterface->usNumberOfRefs;
    if (m_pInterface->pRwObject)
        referencesCount++;

    if (!MapContains(ms_DefaultTxdIDMap, static_cast<unsigned short>(m_dwModelID)))
        ms_DefaultTxdIDMap[static_cast<unsigned short>(m_dwModelID)] = usOldTxdId;

    m_pInterface->usTextureDictionary = usID;

    // Pin the new TXD before rebinding so textures remain valid during the switch
    for (size_t i = 0; i < referencesCount; i++)
        CTxdStore_AddRef(usID);

    // Rebind loaded model's material textures to the new TXD.
    // Without this, material->texture pointers would become stale when the old TXD is released.
    if (m_pInterface->pRwObject)
    {
        eModelInfoType modelType = GetModelType();
        switch (modelType)
        {
            case eModelInfoType::PED:
            case eModelInfoType::WEAPON:
            case eModelInfoType::VEHICLE:
            case eModelInfoType::CLUMP:
            case eModelInfoType::UNKNOWN:
            {
                RpClump* pGameClump = reinterpret_cast<RpClump*>(m_pInterface->pRwObject);
                if (pGame)
                {
                    CRenderWare* pRenderWare = pGame->GetRenderWare();
                    if (pRenderWare)
                        pRenderWare->RebindClumpTexturesToTxd(pGameClump, usID);
                }
                break;
            }
            case eModelInfoType::ATOMIC:
            case eModelInfoType::LOD_ATOMIC:
            case eModelInfoType::TIME:
            {
                RpAtomic* pAtomic = reinterpret_cast<RpAtomic*>(m_pInterface->pRwObject);
                if (pGame)
                {
                    CRenderWare* pRenderWare = pGame->GetRenderWare();
                    if (pRenderWare)
                        pRenderWare->RebindAtomicTexturesToTxd(pAtomic, usID);
                }
                break;
            }
            default:
                break;
        }
    }

    // Release old TXD refs after rebinding completes
    // Only release if old slot is still valid to avoid crash on stale/orphaned TXD slots
    if (CTxdStore_GetTxd(usOldTxdId) != nullptr)
    {
        for (size_t i = 0; i < referencesCount; i++)
            CTxdStore_RemoveRef(usOldTxdId);
    }
}

void CModelInfoSA::ResetTextureDictionaryID()
{
    const auto it = ms_DefaultTxdIDMap.find(static_cast<unsigned short>(m_dwModelID));
    if (it == ms_DefaultTxdIDMap.end())
    {
        return;
    }

    if (!GetInterface())
    {
        return;
    }

    const auto targetId = static_cast<unsigned short>(it->second);

    // If target TXD no longer exists, clean up stale entry and return
    if (CTxdStore_GetTxd(targetId) == nullptr)
    {
        ms_DefaultTxdIDMap.erase(it);
        return;
    }

    SetTextureDictionaryID(targetId);
    if (GetTextureDictionaryID() == targetId)
        ms_DefaultTxdIDMap.erase(it);
}

void CModelInfoSA::StaticResetTextureDictionaries()
{
    std::vector<unsigned short> modelIds;
    modelIds.reserve(ms_DefaultTxdIDMap.size());
    for (const auto& pair : ms_DefaultTxdIDMap)
        modelIds.push_back(pair.first);

    for (auto modelId : modelIds)
    {
        const auto mi = pGame->GetModelInfo(modelId);
        if (mi)
        {
            mi->ResetTextureDictionaryID();
        }
        else
        {
            ms_DefaultTxdIDMap.erase(modelId);
        }
    }
}

float CModelInfoSA::GetLODDistance()
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    return pInterface ? pInterface->fLodDistanceUnscaled : 0.0f;
}

bool CModelInfoSA::SetTime(char cHourOn, char cHourOff)
{
    m_pInterface = ppModelInfo[m_dwModelID];
    if (!IsValidModelInfoPtr(m_pInterface))
    {
        m_pInterface = nullptr;
        return false;
    }

    if (GetModelType() != eModelInfoType::TIME)
        return false;

    CTimeInfoSAInterface* pTime = &static_cast<CTimeModelInfoSAInterface*>(m_pInterface)->timeInfo;

    if (!MapContains(ms_ModelDefaultModelTimeInfo, m_dwModelID))
        MapSet(ms_ModelDefaultModelTimeInfo, m_dwModelID, *pTime);

    pTime->m_nTimeOn = cHourOn;
    pTime->m_nTimeOff = cHourOff;
    return true;
}

bool CModelInfoSA::GetTime(char& cHourOn, char& cHourOff)
{
    m_pInterface = ppModelInfo[m_dwModelID];
    if (!IsValidModelInfoPtr(m_pInterface))
    {
        m_pInterface = nullptr;
        return false;
    }

    if (GetModelType() != eModelInfoType::TIME)
        return false;

    CTimeInfoSAInterface* pTime = &static_cast<CTimeModelInfoSAInterface*>(m_pInterface)->timeInfo;

    cHourOn = pTime->m_nTimeOn;
    cHourOff = pTime->m_nTimeOff;
    return true;
}

void CModelInfoSA::StaticResetModelTimes()
{
    for (auto it = ms_ModelDefaultModelTimeInfo.begin(); it != ms_ModelDefaultModelTimeInfo.end();)
    {
        const DWORD                modelId = it->first;
        CBaseModelInfoSAInterface* pInterface = ppModelInfo[modelId];
        if (!IsValidModelInfoPtr(pInterface))
        {
            it = ms_ModelDefaultModelTimeInfo.erase(it);
            continue;
        }

        DWORD funcAddr = pInterface->VFTBL->GetModelType;
        if (!SharedUtil::IsValidGtaSaPtr(funcAddr))
        {
            it = ms_ModelDefaultModelTimeInfo.erase(it);
            continue;
        }
        const eModelInfoType modelType = ((eModelInfoType (*)())funcAddr)();
        if (modelType != eModelInfoType::TIME)
        {
            it = ms_ModelDefaultModelTimeInfo.erase(it);
            continue;
        }

        auto* pTime = &static_cast<CTimeModelInfoSAInterface*>(pInterface)->timeInfo;
        pTime->m_nTimeOn = it->second.m_nTimeOn;
        pTime->m_nTimeOff = it->second.m_nTimeOff;
        pTime->m_wOtherTimeModel = it->second.m_wOtherTimeModel;
        it = ms_ModelDefaultModelTimeInfo.erase(it);
    }
}

float CModelInfoSA::GetOriginalLODDistance()
{
    // Return default LOD distance value (if doesn't exist, LOD distance hasn't been changed)
    if (MapContains(ms_ModelDefaultLodDistanceMap, m_dwModelID))
        return MapGet(ms_ModelDefaultLodDistanceMap, m_dwModelID);

    CBaseModelInfoSAInterface* pInterface = ppModelInfo[m_dwModelID];
    if (IsValidModelInfoPtr(pInterface))
        return pInterface->fLodDistanceUnscaled;

    return 0.0f;
}

void CModelInfoSA::SetLODDistance(float fDistance, bool bOverrideMaxDistance)
{
#if 0
    // fLodDistanceUnscaled values:
    //
    // With the draw distance setting in GTA SP options menu set to maximum:
    //      0 - 170     roughly correlates to a LOD distance of 0 - 300 game units
    //      170 - 480   sets the LOD distance to 300 game units and has a negative effect on the alpha fade-in
    //      490 - 1e7   sets the LOD distance to 300 game units and removes the alpha fade-in completely
    //
    // With the draw distance setting in GTA SP options menu set to minimum:
    //      0 - 325     roughly correlates to a LOD distance of 0 - 300 game units
    //      340 - 960   sets the LOD distance to 300 game units and has a negative effect on the alpha fade-in
    //      1000 - 1e7  sets the LOD distance to 300 game units and removes the alpha fade-in completely
    //
    // So, to ensure the maximum draw distance with a working alpha fade-in, fLodDistanceUnscaled has to be
    // no more than: 325 - (325-170) * draw_distance_setting
    //
    if (!bOverrideMaxDistance) {
        // Change GTA draw distance value from 0.925 to 1.8 into 0 to 1
        float fDrawDistanceSetting = UnlerpClamped(0.925f, CSettingsSA().GetDrawDistance(), 1.8f);

        // Calc max setting allowed for fLodDistanceUnscaled to preserve alpha fade-in
        float fMaximumValue = Lerp(325.f, fDrawDistanceSetting, 170.f);

        // Ensure fDistance is in range
        fDistance = std::min(fDistance, fMaximumValue);
    }
#endif
    if (!bOverrideMaxDistance)
    {
        // Limit to 325.f as it goes horrible after that
        fDistance = std::min(fDistance, 325.f);
    }

    m_pInterface = GetInterface();
    if (m_pInterface)
    {
        // Save default value if not done yet
        if (!MapContains(ms_ModelDefaultLodDistanceMap, m_dwModelID))
            MapSet(ms_ModelDefaultLodDistanceMap, m_dwModelID, m_pInterface->fLodDistanceUnscaled);
        m_pInterface->fLodDistanceUnscaled = fDistance;
    }
}

void CModelInfoSA::StaticResetLodDistances()
{
    for (auto iter = ms_ModelDefaultLodDistanceMap.begin(); iter != ms_ModelDefaultLodDistanceMap.end();)
    {
        CBaseModelInfoSAInterface* pInterface = ppModelInfo[iter->first];
        if (!IsValidModelInfoPtr(pInterface))
        {
            iter = ms_ModelDefaultLodDistanceMap.erase(iter);
            continue;
        }

        pInterface->fLodDistanceUnscaled = iter->second;
        iter = ms_ModelDefaultLodDistanceMap.erase(iter);
    }
}

void CModelInfoSA::RestreamIPL()
{
    // IPLs should not contain peds, weapons, vehicles and vehicle upgrades
    if (m_dwModelID > 611 && (m_dwModelID < 1000 || m_dwModelID > 1193))
    {
        constexpr std::size_t kMaxPendingTxdIDs = 1000;
        if (ms_RestreamTxdIDMap.size() >= kMaxPendingTxdIDs)
            return;

        auto txdId = GetTextureDictionaryID();
        if (txdId == 0)
            return;

        MapSet(ms_RestreamTxdIDMap, txdId, 0);
    }
}

// Helper to call entity's DeleteRwObject virtual method
static void DeleteEntityRwObject(CEntitySAInterface* pEntity)
{
    // clang-format off
    __asm
    {
        mov ecx, pEntity
        mov eax, [ecx]
        call dword ptr [eax+20h]
    }
    // clang-format on
}

void CModelInfoSA::StaticFlushPendingRestreamIPL()
{
    if (ms_RestreamTxdIDMap.empty())
        return;

    // This function restreams all instances of the model *that are from the default SA world (ipl)*.
    // In other words, it does not affect elements created by MTA.
    // It's mostly a reimplementation of SA's DeleteAllRwObjects, except that it filters by model ID.

    reinterpret_cast<void (*)()>(FUNC_FlushRequestList)();

    std::unordered_set<unsigned short> processedTxdIDs;
    std::unordered_set<unsigned short> pendingTxdIDs;

    // Models to unload - includes processed entities, timed-out, and unprocessed TXD models
    std::unordered_set<unsigned short> modelsToUnload;

    // Sector array constants
    constexpr int kStreamSectorCount = 2 * NUM_StreamSectorRows * NUM_StreamSectorCols;
    constexpr int kRepeatSectorCount = NUM_StreamRepeatSectorRows * NUM_StreamRepeatSectorCols;
    constexpr int kRepeatSectorStride = 3;  // StreamRepeatSectors uses stride of 3, we access element [2]

    // Helper to validate entity vtable - checks if DeleteRwObject points to expected address
    auto isValidEntity = [](CEntitySAInterface* pEntity) -> bool
    {
        constexpr std::size_t kDeleteRwObjectVtblOffset = 8;
        constexpr std::size_t kExpectedDeleteRwObject = 0x00534030;
        auto*                 vtbl = static_cast<std::size_t*>(pEntity->GetVTBL());
        return vtbl[kDeleteRwObjectVtblOffset] == kExpectedDeleteRwObject;
    };

    // Process entities from a sector list
    // Note: validateVtable should be true for StreamSectors but false for StreamRepeatSectors
    auto processSectorList = [&](DWORD* pSectorEntry, bool validateVtable, int sectorIndex)
    {
        while (pSectorEntry)
        {
            auto* pEntity = reinterpret_cast<CEntitySAInterface*>(pSectorEntry[0]);
            if (!pEntity)
            {
                pSectorEntry = reinterpret_cast<DWORD*>(pSectorEntry[1]);
                continue;
            }

            // Vtable validation for StreamSectors
            if (validateVtable && !isValidEntity(pEntity))
            {
                OutputDebugString(SString("Entity 0x%08x (with model %d) at ARRAY_StreamSectors[%d,%d] is invalid\n", pEntity, pEntity->m_nModelIndex,
                                          sectorIndex / 2 % NUM_StreamSectorRows, sectorIndex / 2 / NUM_StreamSectorCols));
                pSectorEntry = reinterpret_cast<DWORD*>(pSectorEntry[1]);
                continue;
            }

            auto* pModelInfo = pGame->GetModelInfo(pEntity->m_nModelIndex);
            if (!pModelInfo)
            {
                pSectorEntry = reinterpret_cast<DWORD*>(pSectorEntry[1]);
                continue;
            }

            auto txdID = pModelInfo->GetTextureDictionaryID();
            if (MapContains(ms_RestreamTxdIDMap, txdID))
            {
                if (!pEntity->bStreamingDontDelete && !pEntity->bImBeingRendered)
                {
                    DeleteEntityRwObject(pEntity);
                    processedTxdIDs.insert(txdID);
                    modelsToUnload.insert(pEntity->m_nModelIndex);
                }
                else
                {
                    pendingTxdIDs.insert(txdID);
                }
            }

            pSectorEntry = reinterpret_cast<DWORD*>(pSectorEntry[1]);
        }
    };

    // Process StreamSectors (skip null entries for efficiency)
    for (int i = 0; i < kStreamSectorCount; i++)
    {
        auto* pSectorEntry = reinterpret_cast<DWORD*>(reinterpret_cast<DWORD**>(ARRAY_StreamSectors)[i]);
        if (pSectorEntry)
            processSectorList(pSectorEntry, true, i);
    }

    // Process StreamRepeatSectors (skip null entries for efficiency)
    for (int i = 0; i < kRepeatSectorCount; i++)
    {
        auto* pSectorEntry = reinterpret_cast<DWORD*>(reinterpret_cast<DWORD**>(ARRAY_StreamRepeatSectors)[kRepeatSectorStride * i + 2]);
        if (pSectorEntry)
            processSectorList(pSectorEntry, false, i);
    }

    // Determine which TXD IDs had no entities found at all (buildings not yet streamed in)
    std::unordered_set<unsigned short> unprocessedTxdIDs;
    for (const auto& entry : ms_RestreamTxdIDMap)
    {
        if (!processedTxdIDs.count(entry.first) && !pendingTxdIDs.count(entry.first))
            unprocessedTxdIDs.insert(entry.first);
    }

    // Only remove fully processed TXD IDs from the map
    // Keep: pendingTxdIDs (entities being rendered) + unprocessedTxdIDs (no entities found yet)
    for (auto txdID : processedTxdIDs)
    {
        if (!pendingTxdIDs.count(txdID))
            ms_RestreamTxdIDMap.erase(txdID);
    }

    // Increment retry counter ONLY for pending TXD IDs (entities being rendered)
    // Don't increment for: unprocessed (will be erased below)
    // Note: processedTxdIDs that are also in pendingTxdIDs stay in map and need counter incremented
    constexpr int                      kMaxRetryFrames = 300;  // ~5 seconds at 60fps
    std::unordered_set<unsigned short> timedOutTxdIDs;
    for (auto it = ms_RestreamTxdIDMap.begin(); it != ms_RestreamTxdIDMap.end();)
    {
        // Skip if unprocessed - those will be erased below after model unload attempt
        if (unprocessedTxdIDs.count(it->first))
        {
            ++it;
            continue;
        }

        it->second++;  // Increment retry counter for pending TXD IDs
        if (it->second > kMaxRetryFrames)
        {
            // Timed out - entity was always being rendered. Force unload the models.
            timedOutTxdIDs.insert(it->first);
            it = ms_RestreamTxdIDMap.erase(it);
        }
        else
            ++it;
    }

    // Force unload models for timed-out TXD IDs (entities that were always being rendered)
    // This is needed because we can't delete RwObject while entity is being rendered,
    // but we still need to refresh the textures eventually.
    if (!timedOutTxdIDs.empty())
    {
        const auto maxModelId = static_cast<DWORD>(pGame->GetBaseIDforTXD());

        for (DWORD modelId = 612; modelId < 1000; modelId++)
        {
            auto* pModelInfo = pGame->GetModelInfo(modelId);
            if (!pModelInfo)
                continue;
            auto txdId = pModelInfo->GetTextureDictionaryID();
            if (txdId != 0 && timedOutTxdIDs.count(txdId))
                modelsToUnload.insert(static_cast<unsigned short>(modelId));
        }

        for (DWORD modelId = 1194; modelId < maxModelId; modelId++)
        {
            auto* pModelInfo = pGame->GetModelInfo(modelId);
            if (!pModelInfo)
                continue;
            auto txdId = pModelInfo->GetTextureDictionaryID();
            if (txdId != 0 && timedOutTxdIDs.count(txdId))
                modelsToUnload.insert(static_cast<unsigned short>(modelId));
        }
    }

    // For unprocessed TXD IDs (no entities visible), try to unload the actual models
    // This handles the cases where models are loaded with old textures but not yet streamed as entities
    if (!unprocessedTxdIDs.empty())
    {
        const auto maxModelId = static_cast<DWORD>(pGame->GetBaseIDforTXD());

        auto tryQueueModelUnload = [&](DWORD modelId)
        {
            auto* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(modelId);
            if (!pStreamingInfo || pStreamingInfo->loadState == eModelLoadState::LOADSTATE_NOT_LOADED)
                return;

            auto* pModelInfo = pGame->GetModelInfo(modelId);
            if (!pModelInfo)
                return;

            auto modelTxdId = pModelInfo->GetTextureDictionaryID();
            if (modelTxdId == 0 || !unprocessedTxdIDs.count(modelTxdId))
                return;

            auto* pInterface = pModelInfo->GetInterface();
            if (pInterface && pInterface->usNumberOfRefs == 0)
                modelsToUnload.insert(static_cast<unsigned short>(modelId));
        };

        // Building model ranges matching RestreamIPL filter: > 611 && (< 1000 || > 1193)
        for (DWORD modelId = 612; modelId < 1000; modelId++)
            tryQueueModelUnload(modelId);

        for (DWORD modelId = 1194; modelId < maxModelId; modelId++)
            tryQueueModelUnload(modelId);

        // Clear unprocessed TXD IDs - we've done what we can
        for (auto txdID : unprocessedTxdIDs)
            ms_RestreamTxdIDMap.erase(txdID);
    }

    // Unload models to force texture re-binding on reload
    for (auto modelId : modelsToUnload)
    {
        pGame->GetStreaming()->RemoveModel(modelId);
        CStreamingInfo* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(modelId);
        if (pStreamingInfo)
            pStreamingInfo->loadState = eModelLoadState::LOADSTATE_NOT_LOADED;
    }
}

void CModelInfoSA::ModelAddRef(EModelRequestType requestType, const char* szTag)
{
    // Are we not loaded?
    if (!IsLoaded())
    {
        // Request it. Wait for it to load if we're asked to.
        if (pGame && pGame->IsASyncLoadingEnabled())
            Request(requestType, szTag);
        else
            Request(BLOCKING, szTag);
    }

    // Increment the references.
    if (m_dwReferences == 0)
    {
        assert(!m_dwPendingInterfaceRef);
        if (IsLoaded())
        {
            m_pInterface = ppModelInfo[m_dwModelID];
            if (IsValidModelInfoPtr(m_pInterface))
            {
                m_pInterface->usNumberOfRefs++;
                CTxdStore_AddRef(m_pInterface->usTextureDictionary);
            }
            else
                m_pInterface = nullptr;
        }
        else
            m_dwPendingInterfaceRef = 1;
    }

    m_dwReferences++;
}

int CModelInfoSA::GetRefCount()
{
    return static_cast<int>(m_dwReferences);
}

void CModelInfoSA::RemoveRef(bool bRemoveExtraGTARef)
{
    // Decrement the references
    if (m_dwReferences > 0)
        m_dwReferences--;

    if (m_dwReferences == 0 && m_dwPendingInterfaceRef)
    {
        m_dwPendingInterfaceRef = 0;
        return;
    }

    // Handle extra ref if requested
    if (bRemoveExtraGTARef)
    {
        // Remove ref added by GTA.
        CBaseModelInfoSAInterface* pInterface = GetInterface();
        if (pInterface && pInterface->usNumberOfRefs > 1)
        {
            DWORD dwFunction = FUNC_RemoveRef;
            // clang-format off
            __asm
            {
                mov     ecx, pInterface
                call    dwFunction
            }
            // clang-format on
        }
    }

    // Unload it if 0 references left and we're not CJ model.
    // And if we're loaded.
    if (m_dwReferences == 0 && m_dwModelID != 0 && IsLoaded())
    {
        Remove();
    }
}

void CModelInfoSA::SetAlphaTransparencyEnabled(bool bEnabled)
{
    m_pInterface = GetInterface();
    if (m_pInterface)
    {
        if (!MapContains(ms_ModelDefaultAlphaTransparencyMap, m_dwModelID))
        {
            MapSet(ms_ModelDefaultAlphaTransparencyMap, m_dwModelID, (BYTE)(m_pInterface->bAlphaTransparency));
        }
        m_pInterface->bAlphaTransparency = bEnabled;
    }
}

bool CModelInfoSA::IsAlphaTransparencyEnabled()
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    return pInterface ? pInterface->bAlphaTransparency : false;
}

void CModelInfoSA::StaticResetAlphaTransparencies()
{
    for (auto iter = ms_ModelDefaultAlphaTransparencyMap.begin(); iter != ms_ModelDefaultAlphaTransparencyMap.end();)
    {
        CBaseModelInfoSAInterface* pInterface = ppModelInfo[iter->first];
        if (!IsValidModelInfoPtr(pInterface))
        {
            iter = ms_ModelDefaultAlphaTransparencyMap.erase(iter);
            continue;
        }

        pInterface->bAlphaTransparency = iter->second;
        iter = ms_ModelDefaultAlphaTransparencyMap.erase(iter);
    }
}

void CModelInfoSA::ResetAlphaTransparency()
{
    m_pInterface = GetInterface();
    if (m_pInterface)
    {
        BYTE* pbEnabled = MapFind(ms_ModelDefaultAlphaTransparencyMap, m_dwModelID);
        if (pbEnabled)
        {
            m_pInterface->bAlphaTransparency = *pbEnabled;
            MapRemove(ms_ModelDefaultAlphaTransparencyMap, m_dwModelID);
        }
    }
}

short CModelInfoSA::GetAvailableVehicleMod(unsigned short usUpgrade)
{
    short sreturn = -1;
    if (usUpgrade >= 1000 && usUpgrade <= 1193)
    {
        DWORD ModelID = m_dwModelID;
        // clang-format off
        __asm
        {
            mov     eax, ModelID

            push    ecx
            mov ecx, dword ptr[ARRAY_ModelInfo]
            mov     eax, dword ptr[ecx + eax*4]
            pop     ecx


            movsx   edx, usUpgrade;
            mov     ax, [eax+edx*2+0x2D6]
            mov     sreturn, ax
        }
        // clang-format on
    }
    return sreturn;
}

bool CModelInfoSA::IsUpgradeAvailable(eVehicleUpgradePosn posn)
{
    bool  bRet = false;
    DWORD ModelID = m_dwModelID;
    // clang-format off
    __asm
    {
        mov     eax, ModelID
        mov ecx, dword ptr[ARRAY_ModelInfo]
        mov     ecx, dword ptr[ecx + eax*4]

        mov     eax, posn
        mov     ecx, [ecx+0x5C]
        shl     eax, 5
        push    esi
        mov     esi, [ecx+eax+0D0h]
        xor     edx, edx
        test    esi, esi
        setnl   dl
        mov     al, dl
        pop     esi

        mov     bRet, al
    }
    // clang-format on
    return bRet;
}

void CModelInfoSA::SetCustomCarPlateText(const char* szText)
{
    char* szStoredText;
    DWORD ModelID = m_dwModelID;
    // clang-format off
    __asm
    {
        push    ecx
        mov     ecx, ModelID

        push    eax
        mov     eax, dword ptr[ARRAY_ModelInfo]
        mov     ecx, dword ptr[eax + ecx*4]
        pop     eax

        add     ecx, 40
        mov     szStoredText, ecx
        pop     ecx
    }
    // clang-format on

    if (szText) strncpy(szStoredText, szText, 8);
    else szStoredText[0] = '\0';
}

unsigned int CModelInfoSA::GetNumRemaps()
{
    DWORD        dwFunc = FUNC_CVehicleModelInfo__GetNumRemaps;
    DWORD        ModelID = m_dwModelID;
    unsigned int uiReturn = 0;
    // clang-format off
    __asm
    {
        mov     ecx, ModelID

        push    eax
        mov     eax, dword ptr[ARRAY_ModelInfo]
        mov     ecx, dword ptr[eax + ecx*4]
        pop     eax

        call    dwFunc
        mov     uiReturn, eax
    }
    // clang-format on
    return uiReturn;
}

void* CModelInfoSA::GetVehicleSuspensionData()
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    if (!pInterface)
    {
        Request(BLOCKING, "GetVehicleSuspensionData");
        pInterface = GetInterface();
        if (!pInterface)
            return nullptr;
    }

    CColModelSAInterface* pColModel = pInterface->pColModel;
    if (!pColModel)
        return nullptr;

    CColDataSA* pColData = nullptr;
    if (!SafeReadColData(pColModel, &pColData))
    {
        AddReportLog(5554, SString("GetVehicleSuspensionData: ColData read failed for model %u", m_dwModelID), 10);
        return nullptr;
    }

    if (!pColData)
    {
        unsigned short slot;
        if (!SafeReadColSlot(pColModel, &slot))
        {
            AddReportLog(5554, SString("GetVehicleSuspensionData: ColSlot read failed for model %u", m_dwModelID), 10);
            return nullptr;
        }
        const DWORD colId = static_cast<DWORD>(RESOURCE_ID_COL + slot);
        pGame->GetStreaming()->RequestModel(colId, 0x16);
        pGame->GetStreaming()->LoadAllRequestedModels(true, "GetVehicleSuspensionData");

        if (!SafeReadColData(pColModel, &pColData))
        {
            AddReportLog(5554, SString("GetVehicleSuspensionData: ColData read failed post-load for model %u", m_dwModelID), 10);
            return nullptr;
        }
        if (!pColData)
            return nullptr;
    }

    void* pLines = nullptr;
    if (!SafeReadSuspLines(pColData, &pLines))
    {
        AddReportLog(5554, SString("GetVehicleSuspensionData: SuspLines read failed for model %u", m_dwModelID), 10);
        return nullptr;
    }
    return pLines;
}

void* CModelInfoSA::SetVehicleSuspensionData(void* pSuspensionLines)
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    if (!pInterface)
    {
        Request(BLOCKING, "SetVehicleSuspensionData");
        pInterface = GetInterface();
        if (!pInterface)
            return nullptr;
    }

    CColModelSAInterface* pColModel = pInterface->pColModel;
    if (!pColModel)
        return nullptr;

    CColDataSA* pColData = nullptr;
    if (!SafeReadColData(pColModel, &pColData))
    {
        AddReportLog(5555, SString("SetVehicleSuspensionData: ColData read failed for model %u", m_dwModelID), 10);
        return nullptr;
    }

    if (!pColData)
    {
        unsigned short slot;
        if (!SafeReadColSlot(pColModel, &slot))
        {
            AddReportLog(5555, SString("SetVehicleSuspensionData: ColSlot read failed for model %u", m_dwModelID), 10);
            return nullptr;
        }
        const DWORD colId = static_cast<DWORD>(RESOURCE_ID_COL + slot);
        pGame->GetStreaming()->RequestModel(colId, 0x16);
        pGame->GetStreaming()->LoadAllRequestedModels(true, "SetVehicleSuspensionData");

        if (!SafeReadColData(pColModel, &pColData))
        {
            AddReportLog(5555, SString("SetVehicleSuspensionData: ColData read failed post-load for model %u", m_dwModelID), 10);
            return nullptr;
        }
        if (!pColData)
            return nullptr;
    }

    void* pOld = nullptr;
    if (!SafeSwapSuspLines(pColData, pSuspensionLines, &pOld))
    {
        AddReportLog(5555, SString("SetVehicleSuspensionData: SuspLines swap failed for model %u", m_dwModelID), 10);
        return nullptr;
    }
    return pOld;
}

CVector CModelInfoSA::GetVehicleExhaustFumesPosition()
{
    return GetVehicleDummyPosition(VehicleDummies::EXHAUST);
}

void CModelInfoSA::SetVehicleExhaustFumesPosition(const CVector& vecPosition)
{
    return SetVehicleDummyPosition(VehicleDummies::EXHAUST, vecPosition);
}

bool CModelInfoSA::GetVehicleDummyPositions(std::array<CVector, static_cast<std::size_t>(VehicleDummies::VEHICLE_DUMMY_COUNT)>& positions) const
{
    if (!IsVehicle())
        return false;

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(ppModelInfo[m_dwModelID]);
    if (!IsValidModelInfoPtr(pVehicleModel) || !pVehicleModel->pVisualInfo)
        return false;

    CVector* dummyPositions = pVehicleModel->pVisualInfo->vecDummies;
    std::copy(dummyPositions, dummyPositions + positions.size(), positions.begin());
    return true;
}

CVector CModelInfoSA::GetVehicleDummyDefaultPosition(VehicleDummies eDummy)
{
    if (!IsVehicle())
        return CVector();

    auto dummyIter = ms_ModelDefaultDummiesPosition.find(m_dwModelID);

    if (dummyIter != ms_ModelDefaultDummiesPosition.end())
    {
        auto positionIter = dummyIter->second.find(eDummy);

        if (positionIter != dummyIter->second.end())
        {
            return positionIter->second;
        }
    }

    ModelAddRef(BLOCKING, "GetVehicleDummyDefaultPosition");

    auto modelInfo = reinterpret_cast<CVehicleModelInfoSAInterface*>(GetInterface());
    if (!modelInfo || !modelInfo->pVisualInfo)
    {
        RemoveRef();
        return CVector();
    }
    CVector vec = modelInfo->pVisualInfo->vecDummies[(std::size_t)eDummy];

    RemoveRef();

    return vec;
}

CVector CModelInfoSA::GetVehicleDummyPosition(VehicleDummies eDummy)
{
    if (!IsVehicle())
        return CVector();

    // Request model load right now if not loaded yet (#9897)
    if (!IsLoaded())
        Request(BLOCKING, "GetVehicleDummyPosition");

    auto* pVehicleModel = static_cast<CVehicleModelInfoSAInterface*>(GetInterface());
    if (!pVehicleModel || !pVehicleModel->pVisualInfo)
        return CVector();

    return pVehicleModel->pVisualInfo->vecDummies[(std::size_t)eDummy];
}

void CModelInfoSA::SetVehicleDummyPosition(VehicleDummies eDummy, const CVector& vecPosition)
{
    if (!IsVehicle())
        return;

    // Request model load right now if not loaded yet (#9897)
    if (!IsLoaded())
        Request(BLOCKING, "SetVehicleDummyPosition");

    auto* pVehicleModel = static_cast<CVehicleModelInfoSAInterface*>(GetInterface());
    if (!pVehicleModel || !pVehicleModel->pVisualInfo)
        return;

    // Store default position in map
    auto iter = ms_ModelDefaultDummiesPosition.find(m_dwModelID);
    if (iter == ms_ModelDefaultDummiesPosition.end())
    {
        ms_ModelDefaultDummiesPosition.insert({m_dwModelID, std::map<VehicleDummies, CVector>()});
        // Increment this model references count, so we don't unload it before we have a chance to reset the positions
        pVehicleModel->usNumberOfRefs++;
        CTxdStore_AddRef(pVehicleModel->usTextureDictionary);
    }

    if (ms_ModelDefaultDummiesPosition[m_dwModelID].find(eDummy) == ms_ModelDefaultDummiesPosition[m_dwModelID].end())
    {
        ms_ModelDefaultDummiesPosition[m_dwModelID][eDummy] = pVehicleModel->pVisualInfo->vecDummies[(std::size_t)eDummy];
    }

    // Set dummy position
    pVehicleModel->pVisualInfo->vecDummies[static_cast<std::size_t>(eDummy)] = vecPosition;
}

void CModelInfoSA::ResetVehicleDummies(bool bRemoveFromDummiesMap)
{
    if (!IsVehicle())
        return;

    auto iter = ms_ModelDefaultDummiesPosition.find(m_dwModelID);
    if (iter == ms_ModelDefaultDummiesPosition.end())
        return;  // Early out in case the model doesn't have any dummies modified

    auto* pVehicleModel = static_cast<CVehicleModelInfoSAInterface*>(GetInterface());
    if (!pVehicleModel || !pVehicleModel->pVisualInfo)
    {
        Request(BLOCKING, "ResetVehicleDummies");
        pVehicleModel = static_cast<CVehicleModelInfoSAInterface*>(GetInterface());
        if (!pVehicleModel || !pVehicleModel->pVisualInfo)
        {
            if (bRemoveFromDummiesMap)
                ms_ModelDefaultDummiesPosition.erase(iter);
            return;
        }
    }

    for (const auto& dummy : iter->second)
    {
        pVehicleModel->pVisualInfo->vecDummies[static_cast<std::size_t>(dummy.first)] = dummy.second;
    }

    if (pVehicleModel->usNumberOfRefs > 0)
    {
        if (CTxdStore_GetTxd(pVehicleModel->usTextureDictionary) != nullptr)
            CTxdStore_RemoveRef(pVehicleModel->usTextureDictionary);
        pVehicleModel->usNumberOfRefs--;
    }

    if (bRemoveFromDummiesMap)
        ms_ModelDefaultDummiesPosition.erase(iter);
}

void CModelInfoSA::ResetAllVehicleDummies()
{
    CGame* game = g_pCore->GetGame();
    for (auto it = ms_ModelDefaultDummiesPosition.begin(); it != ms_ModelDefaultDummiesPosition.end();)
    {
        CModelInfo* modelInfo = game->GetModelInfo(it->first);
        if (!modelInfo || !modelInfo->IsVehicle())
        {
            it = ms_ModelDefaultDummiesPosition.erase(it);
            continue;
        }

        auto* pModelInfoSA = static_cast<CModelInfoSA*>(modelInfo);
        auto* pVehicleModel = static_cast<CVehicleModelInfoSAInterface*>(pModelInfoSA->GetInterface());
        if (!pVehicleModel || !pVehicleModel->pVisualInfo)
        {
            pModelInfoSA->Request(BLOCKING, "ResetAllVehicleDummies");
            pVehicleModel = static_cast<CVehicleModelInfoSAInterface*>(pModelInfoSA->GetInterface());
            if (!pVehicleModel || !pVehicleModel->pVisualInfo)
            {
                it = ms_ModelDefaultDummiesPosition.erase(it);
                continue;
            }
        }

        for (const auto& dummy : it->second)
            pVehicleModel->pVisualInfo->vecDummies[static_cast<std::size_t>(dummy.first)] = dummy.second;

        if (pVehicleModel->usNumberOfRefs > 0)
        {
            if (CTxdStore_GetTxd(pVehicleModel->usTextureDictionary) != nullptr)
                CTxdStore_RemoveRef(pVehicleModel->usTextureDictionary);
            pVehicleModel->usNumberOfRefs--;
        }

        it = ms_ModelDefaultDummiesPosition.erase(it);
    }
}

float CModelInfoSA::GetVehicleWheelSize(ResizableVehicleWheelGroup eWheelGroup)
{
    if (!IsVehicle())
        return 0.0f;

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(GetInterface());
    if (!pVehicleModel)
        return 0.0f;

    switch (eWheelGroup)
    {
        case ResizableVehicleWheelGroup::FRONT_AXLE:
            return pVehicleModel->fWheelSizeFront;
        case ResizableVehicleWheelGroup::REAR_AXLE:
            return pVehicleModel->fWheelSizeRear;
    }

    return 0.0f;
}

void CModelInfoSA::SetVehicleWheelSize(ResizableVehicleWheelGroup eWheelGroup, float fWheelSize)
{
    if (!IsVehicle())
        return;

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(GetInterface());
    if (!pVehicleModel)
        return;

    // Store default wheel sizes in map
    if (!MapFind(ms_VehicleModelDefaultWheelSizes, m_dwModelID))
        MapSet(ms_VehicleModelDefaultWheelSizes, m_dwModelID, std::make_pair(pVehicleModel->fWheelSizeFront, pVehicleModel->fWheelSizeRear));

    switch (eWheelGroup)
    {
        case ResizableVehicleWheelGroup::FRONT_AXLE:
            pVehicleModel->fWheelSizeFront = fWheelSize;
            break;
        case ResizableVehicleWheelGroup::REAR_AXLE:
            pVehicleModel->fWheelSizeRear = fWheelSize;
            break;
        case ResizableVehicleWheelGroup::ALL_WHEELS:
            pVehicleModel->fWheelSizeFront = fWheelSize;
            pVehicleModel->fWheelSizeRear = fWheelSize;
            break;
    }
}

void CModelInfoSA::ResetVehicleWheelSizes(std::pair<float, float>* defaultSizes)
{
    if (!IsVehicle())
        return;

    std::pair<float, float>* sizesPair = defaultSizes ? defaultSizes : MapFind(ms_VehicleModelDefaultWheelSizes, m_dwModelID);

    // Default values not found in map
    if (!sizesPair)
        return;

    auto* pVehicleModel = static_cast<CVehicleModelInfoSAInterface*>(GetInterface());
    if (!pVehicleModel)
        return;

    pVehicleModel->fWheelSizeFront = sizesPair->first;
    pVehicleModel->fWheelSizeRear = sizesPair->second;

    if (!defaultSizes)
        MapRemove(ms_VehicleModelDefaultWheelSizes, m_dwModelID);
}

void CModelInfoSA::ResetAllVehiclesWheelSizes()
{
    CGame* game = g_pCore->GetGame();
    for (auto it = ms_VehicleModelDefaultWheelSizes.begin(); it != ms_VehicleModelDefaultWheelSizes.end();)
    {
        CModelInfo* modelInfo = game->GetModelInfo(it->first);
        if (!modelInfo || !modelInfo->IsVehicle())
        {
            it = ms_VehicleModelDefaultWheelSizes.erase(it);
            continue;
        }

        auto* pModelInfoSA = static_cast<CModelInfoSA*>(modelInfo);
        auto* pVehicleModel = static_cast<CVehicleModelInfoSAInterface*>(pModelInfoSA->GetInterface());
        if (!pVehicleModel)
        {
            it = ms_VehicleModelDefaultWheelSizes.erase(it);
            continue;
        }

        pVehicleModel->fWheelSizeFront = it->second.first;
        pVehicleModel->fWheelSizeRear = it->second.second;
        it = ms_VehicleModelDefaultWheelSizes.erase(it);
    }
}

bool CModelInfoSA::SetCustomModel(RpClump* pClump)
{
    if (!pClump)
        return false;

    if (!IsLoaded())
    {
        // Wait for the game to eventually stream-in the model and then try to replace it (via MakeCustomModel).
        m_pCustomClump = pClump;
        return true;
    }

    bool success = false;

    switch (GetModelType())
    {
        case eModelInfoType::PED:
            success = pGame->GetRenderWare()->ReplacePedModel(pClump, static_cast<unsigned short>(m_dwModelID));
            break;
        case eModelInfoType::WEAPON:
            success = pGame->GetRenderWare()->ReplaceWeaponModel(pClump, static_cast<unsigned short>(m_dwModelID));
            break;
        case eModelInfoType::VEHICLE:
            // ReplaceVehicleModele handles collision preservation internally
            success = pGame->GetRenderWare()->ReplaceVehicleModel(pClump, static_cast<unsigned short>(m_dwModelID));
            break;
        case eModelInfoType::ATOMIC:
        case eModelInfoType::LOD_ATOMIC:
        case eModelInfoType::TIME:
            success = pGame->GetRenderWare()->ReplaceAllAtomicsInModel(pClump, static_cast<unsigned short>(m_dwModelID));
            break;
        case eModelInfoType::CLUMP:
            success = pGame->GetRenderWare()->ReplaceClumpModel(pClump, static_cast<unsigned short>(m_dwModelID));
            break;
        case eModelInfoType::UNKNOWN:
            // Weapon models (321-372) may return UNKNOWN type during streaming. Using ReplaceAllAtomicsInModel
            // for weapons would skip CWeaponModelInfo::SetClump, leaving the frame plugin's m_modelInfo NULL,
            // which crashes in CVisibilityPlugins::RenderWeaponCB due to nullptr deref.
            if (m_dwModelID >= 321 && m_dwModelID <= 372)
                success = pGame->GetRenderWare()->ReplaceWeaponModel(pClump, static_cast<unsigned short>(m_dwModelID));
            else
                success = pGame->GetRenderWare()->ReplaceAllAtomicsInModel(pClump, static_cast<unsigned short>(m_dwModelID));
            break;
        default:
            AddReportLog(8634, SString("SetCustomModel: Unhandled model type %d for model %u", static_cast<int>(GetModelType()),
                                       static_cast<unsigned int>(m_dwModelID)));
            break;
    }

    if (success)
    {
        m_pCustomClump = pClump;

        // Rebind texture pointers in the GAME's clump to current TXD textures.
        // ReplaceModel clones the input clump, so we must rebind the ACTUAL clump the game is using.
        // This is needed because the TXD may contain replacement textures (via engineImportTXD),
        // but the DFF's materials still point to old/original textures from when it was loaded.
        // Without this fix, shader texture replacement fails on custom DFF models.
        eModelInfoType modelType = GetModelType();
        switch (modelType)
        {
            case eModelInfoType::PED:
            case eModelInfoType::WEAPON:
            case eModelInfoType::VEHICLE:
            case eModelInfoType::CLUMP:
            case eModelInfoType::UNKNOWN:
            {
                RpClump* pGameClump = reinterpret_cast<RpClump*>(GetRwObject());
                if (pGameClump && pGame)
                {
                    CRenderWare* pRenderWare = pGame->GetRenderWare();
                    if (pRenderWare)
                        pRenderWare->RebindClumpTexturesToTxd(pGameClump, GetTextureDictionaryID());
                }
                break;
            }
            case eModelInfoType::ATOMIC:
            case eModelInfoType::LOD_ATOMIC:
            case eModelInfoType::TIME:
            {
                RpAtomic* pAtomic = reinterpret_cast<RpAtomic*>(GetRwObject());
                if (pAtomic && pGame)
                {
                    CRenderWare* pRenderWare = pGame->GetRenderWare();
                    if (pRenderWare)
                        pRenderWare->RebindAtomicTexturesToTxd(pAtomic, GetTextureDictionaryID());
                }
                break;
            }
            default:
                break;
        }
    }
    else
    {
        m_pCustomClump = nullptr;
    }

    return success;
}

void CModelInfoSA::RestoreOriginalModel()
{
    // Are we loaded?
    if (IsLoaded())
    {
        pGame->GetStreaming()->RemoveModel(m_dwModelID);
    }
    // Reset the stored custom vehicle clump
    m_pCustomClump = nullptr;
}

void CModelInfoSA::ClearCustomModel()
{
    m_pCustomClump = nullptr;
}

void CModelInfoSA::SetColModel(CColModel* pColModel)
{
    if (!pColModel)
        return;

    // Grab the interfaces
    CColModelSAInterface* pColModelInterface = pColModel->GetInterface();
    if (!pColModelInterface)
        return;

    m_pInterface = ppModelInfo[m_dwModelID];
    if (IsValidModelInfoPtr(m_pInterface) && m_pCustomColModel == pColModel && m_pInterface->pColModel == pColModelInterface)
        return;

    // Store the col model we set
    m_pCustomColModel = pColModel;

    // Do the following only if we're loaded
    m_pInterface = ppModelInfo[m_dwModelID];

    if (IsValidModelInfoPtr(m_pInterface))
    {
        if (!m_pOriginalColModelInterface)
        {
            m_pOriginalColModelInterface = m_pInterface->pColModel;
            m_originalFlags = GetOriginalFlags();
            if (m_pOriginalColModelInterface)
            {
                unsigned short slot;
                if (!SafeReadColSlot(m_pOriginalColModelInterface, &slot))
                {
                    AddReportLog(5556, SString("SetColModel: pColModel access failed for model %u", m_dwModelID), 10);
                    m_pOriginalColModelInterface = nullptr;
                }
                else
                {
                    m_usColSlot = slot;
                }
            }
        }

        // Set collision slot to 0 (the "generic" slot) for custom collision models.
        // Slot 0 is always allocated in CColStore::Initialise(). Using an unallocated
        // slot ID (like the previous 0xA9) causes CColStore::AddRef to access garbage
        // memory since native GTA doesn't validate slot existence before pool indexing.
        pColModelInterface->m_sphere.m_collisionSlot = 0;

        CBaseModelInfo_SetColModel(m_pInterface, pColModelInterface, true);
        CColAccel_addCacheCol(m_dwModelID, pColModelInterface);

        // SetColModel sets bDoWeOwnTheColModel if the last parameter is truthy
        m_pInterface->bDoWeOwnTheColModel = false;
        m_pInterface->bIsColLoaded = false;

        // Fix random foliage on custom collisions by calling CPlantMgr::SetPlantFriendlyFlagInAtomicMI
        (reinterpret_cast<void(__cdecl*)(CBaseModelInfoSAInterface*)>(0x5DB650))(m_pInterface);

        // Set some lighting for this collision if not already present
        CColDataSA* pColData = pColModelInterface->m_data;

        if (pColData)
        {
            for (uint i = 0; i < pColData->m_numTriangles; i++)
            {
                CColTriangleSA* pTriangle = pColData->m_triangles + i;

                if (pTriangle->m_lighting.night == 0 && pTriangle->m_lighting.day == 0)
                {
                    pTriangle->m_lighting.night = 1;
                    pTriangle->m_lighting.day = 12;
                }
            }
        }
    }
}

void CModelInfoSA::RestoreColModel()
{
    if (!m_pOriginalColModelInterface || !m_pCustomColModel)
    {
        m_pCustomColModel = nullptr;
        m_pOriginalColModelInterface = nullptr;
        m_originalFlags = 0;
        return;
    }

    m_pInterface = ppModelInfo[m_dwModelID];
    if (!IsValidModelInfoPtr(m_pInterface))
    {
        m_pInterface = nullptr;
        m_pCustomColModel = nullptr;
        m_pOriginalColModelInterface = nullptr;
        m_originalFlags = 0;
        return;
    }

    // Restore original collision model and flags
    if (m_pInterface && m_pOriginalColModelInterface && m_pCustomColModel)
    {
        CBaseModelInfo_SetColModel(m_pInterface, m_pOriginalColModelInterface, true);
        CColAccel_addCacheCol(m_dwModelID, m_pInterface->pColModel);

        m_pInterface->usFlags = m_originalFlags;

        CColDataSA*    pColData = nullptr;
        unsigned short slot = 0xFFFF;
        if (m_pInterface->pColModel)
        {
            if (!SafeReadColData(m_pInterface->pColModel, &pColData))
            {
                AddReportLog(5559, SString("RestoreColModel: ColData read failed for model %u", m_dwModelID), 10);
            }
            else if (!pColData && m_dwReferences > 1)
            {
                if (SafeReadColSlot(m_pInterface->pColModel, &slot))
                    pGame->GetStreaming()->RemoveModel(RESOURCE_ID_COL + slot);
                else
                    AddReportLog(5559, SString("RestoreColModel: ColSlot read failed for model %u", m_dwModelID), 10);
            }
        }
    }

    m_pCustomColModel = nullptr;
    m_pOriginalColModelInterface = nullptr;
    m_originalFlags = 0;
}

void CModelInfoSA::MakeCustomModel()
{
    // We have a custom model?
    if (m_pCustomClump)
    {
        // Store and clear m_pCustomClump BEFORE calling SetCustomModel to prevent recursive calls.
        // SetCustomModel may trigger LoadAllRequestedModels which can recursively call MakeCustomModel
        // on the same model (via the streaming hook) if the custom DFF lacks embedded collision.
        RpClump* pClumpToSet = m_pCustomClump;
        m_pCustomClump = nullptr;

        if (!SetCustomModel(pClumpToSet))
        {
            // SetCustomModel failed, restore the custom clump for retry on next stream-in
            m_pCustomClump = pClumpToSet;
        }
        else
        {
            // Preserve the custom clump pointer for restream/retry paths
            m_pCustomClump = pClumpToSet;
            // Note: SetCustomModel now handles RebindClumpTexturesToTxd internally after successful replacement
        }
    }

    // Custom collision model is not NULL and it's different from the original?
    if (m_pCustomColModel)
    {
        SetColModel(m_pCustomColModel);
    }
}

void CModelInfoSA::AddColRef()
{
    CColModelSAInterface* originalColModel = nullptr;

    if (m_pOriginalColModelInterface && m_pCustomColModel)
        originalColModel = m_pOriginalColModelInterface;
    else
    {
        CBaseModelInfoSAInterface* pInterface = GetInterface();
        if (pInterface)
            originalColModel = pInterface->pColModel;
        else
            AddReportLog(5552, SString("AddColRef called with null/invalid interface for model %u", m_dwModelID), 10);
    }

    unsigned short slot = m_usColSlot;
    if (originalColModel)
    {
        unsigned short readSlot;
        if (SafeReadColSlot(originalColModel, &readSlot))
        {
            slot = readSlot;
            m_usColSlot = slot;
        }
        else
        {
            AddReportLog(5557, SString("AddColRef: pColModel access failed for model %u", m_dwModelID), 10);
        }
    }

    if (slot == 0xFFFF || slot > 0xFF)
        return;

    if (pGame)
    {
        auto* pColStore = pGame->GetCollisionStore();
        if (pColStore)
        {
            // Validate slot exists in pool before calling native AddRef.
            // GTA SA's CColStore::AddRef (0x4107A0) doesn't validate slot existence,
            // it directly indexes the pool causing crashes on unallocated slots.
            const CollisionSlot colSlot = static_cast<CollisionSlot>(slot);
            if (pColStore->IsValidSlot(colSlot))
            {
                pColStore->AddRef(colSlot);
                ++m_colRefCount;
            }
        }
    }
}

void CModelInfoSA::RemoveColRef()
{
    if (m_colRefCount == 0)
        return;

    CColModelSAInterface* originalColModel = nullptr;

    if (m_pOriginalColModelInterface && m_pCustomColModel)
        originalColModel = m_pOriginalColModelInterface;
    else
    {
        CBaseModelInfoSAInterface* pInterface = GetInterface();
        if (pInterface)
            originalColModel = pInterface->pColModel;
    }

    unsigned short slot = 0xFFFF;
    if (originalColModel)
    {
        if (!SafeReadColSlot(originalColModel, &slot))
        {
            AddReportLog(5558, SString("RemoveColRef: pColModel access failed for model %u", m_dwModelID), 10);
            slot = m_usColSlot;
        }
    }
    else
    {
        slot = m_usColSlot;
    }

    if (slot == 0xFFFF || slot > 0xFF)
        return;

    if (!pGame)
        return;

    auto* pColStore = pGame->GetCollisionStore();
    if (!pColStore)
        return;

    // Validate slot exists in pool before calling native RemoveRef.
    // GTA SA's CColStore::RemoveRef (0x4107D0) doesn't validate slot existence.
    const CollisionSlot colSlot = static_cast<CollisionSlot>(slot);
    if (pColStore->IsValidSlot(colSlot))
    {
        pColStore->RemoveRef(colSlot);
        if (m_colRefCount > 0)
            --m_colRefCount;
    }
    if (m_colRefCount == 0)
        m_usColSlot = 0xFFFF;
}

void CModelInfoSA::GetVoice(short* psVoiceType, short* psVoiceID)
{
    CPedModelInfoSAInterface* pInterface = GetPedModelInfoInterface();
    if (!pInterface)
    {
        if (psVoiceType)
            *psVoiceType = 0;
        if (psVoiceID)
            *psVoiceID = 0;
        return;
    }

    if (psVoiceType)
        *psVoiceType = pInterface->sVoiceType;
    if (psVoiceID)
        *psVoiceID = pInterface->sFirstVoice;
}

void CModelInfoSA::GetVoice(const char** pszVoiceType, const char** pszVoice)
{
    short sVoiceType, sVoiceID;
    GetVoice(&sVoiceType, &sVoiceID);
    if (pszVoiceType)
        *pszVoiceType = CPedSoundSA::GetVoiceTypeNameFromID(sVoiceType);
    if (pszVoice)
        *pszVoice = CPedSoundSA::GetVoiceNameFromID(sVoiceType, sVoiceID);
}

void CModelInfoSA::SetVoice(short sVoiceType, short sVoiceID)
{
    CPedModelInfoSAInterface* pInterface = GetPedModelInfoInterface();
    if (!pInterface)
        return;

    pInterface->sVoiceType = sVoiceType;
    pInterface->sFirstVoice = sVoiceID;
    pInterface->sLastVoice = sVoiceID;
    pInterface->sNextVoice = sVoiceID;
}

void CModelInfoSA::SetVoice(const char* szVoiceType, const char* szVoice)
{
    short sVoiceType = CPedSoundSA::GetVoiceTypeIDFromName(szVoiceType);
    if (sVoiceType < 0)
        return;
    short sVoiceID = CPedSoundSA::GetVoiceIDFromName(sVoiceType, szVoice);
    if (sVoiceID < 0)
        return;
    SetVoice(sVoiceType, sVoiceID);
}

void CModelInfoSA::CopyStreamingInfoFromModel(ushort usBaseModelID)
{
    CStreamingInfo* pBaseModelStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(usBaseModelID);
    CStreamingInfo* pTargetModelStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);

    if (!pBaseModelStreamingInfo || !pTargetModelStreamingInfo)
        return;

    *pTargetModelStreamingInfo = CStreamingInfo{};
    pTargetModelStreamingInfo->archiveId = pBaseModelStreamingInfo->archiveId;
    pTargetModelStreamingInfo->offsetInBlocks = pBaseModelStreamingInfo->offsetInBlocks;
    pTargetModelStreamingInfo->sizeInBlocks = pBaseModelStreamingInfo->sizeInBlocks;
}

void CModelInfoSA::MakePedModel(char* szTexture)
{
    CPedModelInfoSAInterface* pInterface = new CPedModelInfoSAInterface();

    ppModelInfo[m_dwModelID] = pInterface;

    pGame->GetStreaming()->RequestSpecialModel(m_dwModelID, szTexture, 0);
}

void CModelInfoSA::MakeObjectModel(ushort usBaseID)
{
    CBaseModelInfoSAInterface* pBaseObjectInfo = ppModelInfo[usBaseID];
    if (!IsValidModelInfoPtr(pBaseObjectInfo))
    {
        ppModelInfo[m_dwModelID] = nullptr;
        m_pInterface = nullptr;
        m_dwParentID = 0;
        ClearModelDefaults(m_dwModelID);

        CStreamingInfo* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);
        if (pStreamingInfo)
            *pStreamingInfo = CStreamingInfo{};
        return;
    }

    CBaseModelInfoSAInterface* pNewInterface = new CBaseModelInfoSAInterface();
    MemCpyFast(pNewInterface, pBaseObjectInfo, sizeof(CBaseModelInfoSAInterface));
    pNewInterface->usNumberOfRefs = 0;
    pNewInterface->pRwObject = nullptr;
    pNewInterface->usUnknown = 65535;
    pNewInterface->usDynamicIndex = 65535;

    ppModelInfo[m_dwModelID] = pNewInterface;

    m_dwParentID = usBaseID;
    CopyStreamingInfoFromModel(usBaseID);
}

void CModelInfoSA::MakeObjectDamageableModel(std::uint16_t baseModel)
{
    CDamageableModelInfoSAInterface* pBaseObjectInfo = static_cast<CDamageableModelInfoSAInterface*>(ppModelInfo[baseModel]);
    if (!IsValidModelInfoPtr(pBaseObjectInfo))
    {
        ppModelInfo[m_dwModelID] = nullptr;
        m_pInterface = nullptr;
        m_dwParentID = 0;
        ClearModelDefaults(m_dwModelID);

        CStreamingInfo* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);
        if (pStreamingInfo)
            *pStreamingInfo = CStreamingInfo{};
        return;
    }

    CDamageableModelInfoSAInterface* pNewInterface = new CDamageableModelInfoSAInterface();
    MemCpyFast(pNewInterface, pBaseObjectInfo, sizeof(CDamageableModelInfoSAInterface));
    pNewInterface->usNumberOfRefs = 0;
    pNewInterface->pRwObject = nullptr;
    pNewInterface->usUnknown = 65535;
    pNewInterface->usDynamicIndex = 65535;
    pNewInterface->m_damagedAtomic = nullptr;

    ppModelInfo[m_dwModelID] = pNewInterface;

    m_dwParentID = baseModel;
    CopyStreamingInfoFromModel(baseModel);
}

void CModelInfoSA::MakeTimedObjectModel(ushort usBaseID)
{
    CTimeModelInfoSAInterface* pBaseObjectInfo = static_cast<CTimeModelInfoSAInterface*>(ppModelInfo[usBaseID]);
    if (!IsValidModelInfoPtr(pBaseObjectInfo))
    {
        ppModelInfo[m_dwModelID] = nullptr;
        m_pInterface = nullptr;
        m_dwParentID = 0;
        ClearModelDefaults(m_dwModelID);

        CStreamingInfo* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);
        if (pStreamingInfo)
            *pStreamingInfo = CStreamingInfo{};
        return;
    }

    CTimeModelInfoSAInterface* pNewInterface = new CTimeModelInfoSAInterface();
    MemCpyFast(pNewInterface, pBaseObjectInfo, sizeof(CTimeModelInfoSAInterface));
    pNewInterface->usNumberOfRefs = 0;
    pNewInterface->pRwObject = nullptr;
    pNewInterface->usUnknown = 65535;
    pNewInterface->usDynamicIndex = 65535;
    pNewInterface->timeInfo.m_wOtherTimeModel = 0;

    ppModelInfo[m_dwModelID] = pNewInterface;

    m_dwParentID = usBaseID;
    CopyStreamingInfoFromModel(usBaseID);
}

void CModelInfoSA::MakeClumpModel(ushort usBaseID)
{
    CBaseModelInfoSAInterface* pBaseObjectInfo = ppModelInfo[usBaseID];
    if (!IsValidModelInfoPtr(pBaseObjectInfo))
    {
        ppModelInfo[m_dwModelID] = nullptr;
        m_pInterface = nullptr;
        m_dwParentID = 0;
        ClearModelDefaults(m_dwModelID);

        CStreamingInfo* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);
        if (pStreamingInfo)
            *pStreamingInfo = CStreamingInfo{};
        return;
    }

    CClumpModelInfoSAInterface* pNewInterface = new CClumpModelInfoSAInterface();
    MemCpyFast(pNewInterface, pBaseObjectInfo, sizeof(CClumpModelInfoSAInterface));
    pNewInterface->usNumberOfRefs = 0;
    pNewInterface->pRwObject = nullptr;
    pNewInterface->usUnknown = 65535;
    pNewInterface->usDynamicIndex = 65535;

    ppModelInfo[m_dwModelID] = pNewInterface;

    m_dwParentID = usBaseID;
    CopyStreamingInfoFromModel(usBaseID);
}

void CModelInfoSA::MakeVehicleAutomobile(ushort usBaseID)
{
    CBaseModelInfoSAInterface* pBaseObjectInfo = ppModelInfo[usBaseID];
    if (!IsValidModelInfoPtr(pBaseObjectInfo))
    {
        ppModelInfo[m_dwModelID] = nullptr;
        m_pInterface = nullptr;
        m_dwParentID = 0;
        ClearModelDefaults(m_dwModelID);

        CStreamingInfo* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);
        if (pStreamingInfo)
            *pStreamingInfo = CStreamingInfo{};
        return;
    }

    CVehicleModelInfoSAInterface* pNewInterface = new CVehicleModelInfoSAInterface();
    MemCpyFast(pNewInterface, pBaseObjectInfo, sizeof(CVehicleModelInfoSAInterface));
    pNewInterface->usNumberOfRefs = 0;
    pNewInterface->pRwObject = nullptr;
    pNewInterface->pVisualInfo = nullptr;
    pNewInterface->usUnknown = 65535;
    pNewInterface->usDynamicIndex = 65535;

    ppModelInfo[m_dwModelID] = pNewInterface;

    m_dwParentID = usBaseID;
    CopyStreamingInfoFromModel(usBaseID);
}

void CModelInfoSA::DeallocateModel()
{
    CBaseModelInfoSAInterface* pInterfaceToDelete = ppModelInfo[m_dwModelID];

    if (!IsValidModelInfoPtr(pInterfaceToDelete))
    {
        if (pInterfaceToDelete)
            ppModelInfo[m_dwModelID] = nullptr;

        ms_DefaultTxdIDMap.erase(static_cast<unsigned short>(m_dwModelID));
        ms_ModelDefaultFlagsMap.erase(m_dwModelID);
        ms_ModelDefaultLodDistanceMap.erase(m_dwModelID);
        ms_ModelDefaultAlphaTransparencyMap.erase(m_dwModelID);
        ms_OriginalObjectPropertiesGroups.erase(m_dwModelID);
        ms_ModelDefaultDummiesPosition.erase(m_dwModelID);
        ms_VehicleModelDefaultWheelSizes.erase(m_dwModelID);
        ms_ModelDefaultModelTimeInfo.erase(m_dwModelID);

        m_pInterface = nullptr;
        m_dwReferences = 0;
        m_dwPendingInterfaceRef = 0;
        m_dwParentID = 0;
        m_pCustomClump = nullptr;
        m_pCustomColModel = nullptr;
        m_pOriginalColModelInterface = nullptr;
        if (m_colRefCount > 0 && m_usColSlot != 0xFFFF && m_usColSlot <= 0xFF && pGame)
        {
            auto* pColStore = pGame->GetCollisionStore();
            if (pColStore)
            {
                const CollisionSlot colSlot = static_cast<CollisionSlot>(m_usColSlot);
                if (pColStore->IsValidSlot(colSlot))
                {
                    for (std::uint32_t i = 0; i < m_colRefCount; ++i)
                        pColStore->RemoveRef(colSlot);
                }
            }
        }
        m_colRefCount = 0;
        m_usColSlot = 0xFFFF;
        m_originalFlags = 0;
        m_ModelSupportedUpgrades.Reset();
        return;
    }

    // GTA's destructors (e.g. CObject at 0x4C4BB0) access ppModelInfo[] during cleanup.
    // Block deletion while refs > 0 to avoid null pointer crash.
    if (pInterfaceToDelete->usNumberOfRefs > 0)
    {
        AddReportLog(5550, SString("Blocked DeallocateModel for model %u with %u active refs to prevent crash at 0x4C4BB0", m_dwModelID,
                                   static_cast<unsigned int>(pInterfaceToDelete->usNumberOfRefs)));

        m_pInterface = pInterfaceToDelete;

        // Clear custom model pointers to prevent use-after-free on later RemoveRef calls
        m_pCustomClump = nullptr;
        m_pCustomColModel = nullptr;
        m_pOriginalColModelInterface = nullptr;
        m_originalFlags = 0;

        // Keep m_dwReferences and TXD mapping intact - model still in use
        // Tradeoff: interface leaks until refs hit 0, model ID stays occupied
        return;
    }

    // Clear stored defaults so they don't leak to a model that reuses this ID
    ms_DefaultTxdIDMap.erase(static_cast<unsigned short>(m_dwModelID));
    ms_ModelDefaultFlagsMap.erase(m_dwModelID);
    ms_ModelDefaultLodDistanceMap.erase(m_dwModelID);
    ms_ModelDefaultAlphaTransparencyMap.erase(m_dwModelID);
    ms_OriginalObjectPropertiesGroups.erase(m_dwModelID);
    ms_ModelDefaultDummiesPosition.erase(m_dwModelID);
    ms_VehicleModelDefaultWheelSizes.erase(m_dwModelID);

    pGame->GetStreaming()->RemoveModel(m_dwModelID);

    // Capture model type and damageability BEFORE nulling the array entry.
    eModelInfoType modelType = eModelInfoType::UNKNOWN;
    bool           isDamageableAtomic = false;
    if (pInterfaceToDelete->VFTBL)
    {
        DWORD typeFunc = pInterfaceToDelete->VFTBL->GetModelType;
        if (SharedUtil::IsValidGtaSaPtr(typeFunc))
            modelType = ((eModelInfoType (*)())typeFunc)();

        if (modelType == eModelInfoType::ATOMIC || modelType == eModelInfoType::LOD_ATOMIC)
        {
            DWORD damageFunc = pInterfaceToDelete->VFTBL->AsDamageAtomicModelInfoPtr;
            if (SharedUtil::IsValidGtaSaPtr(damageFunc))
            {
                void* asDamageable = ((void* (*)())damageFunc)();
                isDamageableAtomic = (asDamageable != nullptr);
            }
        }
    }

    ms_ModelDefaultModelTimeInfo.erase(m_dwModelID);

    // Null the array entry BEFORE delete for fail-fast if anything tries to access it during deletion
    ppModelInfo[m_dwModelID] = nullptr;

    // Reset wrapper state - this object persists and may be reused for a new model
    m_pInterface = nullptr;
    m_dwReferences = 0;
    m_dwPendingInterfaceRef = 0;
    m_dwParentID = 0;
    m_pCustomClump = nullptr;
    m_pCustomColModel = nullptr;
    m_pOriginalColModelInterface = nullptr;
    if (m_colRefCount > 0 && m_usColSlot != 0xFFFF && m_usColSlot <= 0xFF && pGame)
    {
        auto* pColStore = pGame->GetCollisionStore();
        if (pColStore)
        {
            const CollisionSlot colSlot = static_cast<CollisionSlot>(m_usColSlot);
            if (pColStore->IsValidSlot(colSlot))
            {
                for (std::uint32_t i = 0; i < m_colRefCount; ++i)
                    pColStore->RemoveRef(colSlot);
            }
        }
    }
    m_colRefCount = 0;
    m_usColSlot = 0xFFFF;
    m_originalFlags = 0;
    m_ModelSupportedUpgrades.Reset();

    switch (modelType)
    {
        case eModelInfoType::VEHICLE:
            delete reinterpret_cast<CVehicleModelInfoSAInterface*>(pInterfaceToDelete);
            break;
        case eModelInfoType::PED:
            delete reinterpret_cast<CPedModelInfoSAInterface*>(pInterfaceToDelete);
            break;
        case eModelInfoType::ATOMIC:
        case eModelInfoType::LOD_ATOMIC:
            if (isDamageableAtomic)
            {
                delete reinterpret_cast<CDamageableModelInfoSAInterface*>(pInterfaceToDelete);
            }
            else
            {
                delete reinterpret_cast<CBaseModelInfoSAInterface*>(pInterfaceToDelete);
            }
            break;
        case eModelInfoType::WEAPON:
        case eModelInfoType::CLUMP:
            delete reinterpret_cast<CClumpModelInfoSAInterface*>(pInterfaceToDelete);
            break;
        case eModelInfoType::TIME:
            delete reinterpret_cast<CTimeModelInfoSAInterface*>(pInterfaceToDelete);
            break;
        default:
            AddReportLog(5551, SString("Unknown model type %d for model %u - memory leaked to prevent corruption", static_cast<int>(modelType), m_dwModelID));
            return;
    }

    CStreamingInfo* pStreamingInfo = pGame->GetStreaming()->GetStreamingInfo(m_dwModelID);
    if (pStreamingInfo)
        *pStreamingInfo = CStreamingInfo{};
}
//////////////////////////////////////////////////////////////////////////////////////////
//
// Hook for NodeNameStreamRead
//
// Ignore extra characters in dff frame name
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_NodeNameStreamRead(RwStream* stream, char* pDest, uint uiSize)
{
    // Calc sizes
    const uint uiMaxBufferSize = 24;
    uint       uiAmountToRead = std::min(uiMaxBufferSize - 1, uiSize);
    uint       uiAmountToSkip = uiSize - uiAmountToRead;

    // Read good bit
    RwStreamRead(stream, pDest, uiAmountToRead);
    pDest[uiAmountToRead] = 0;

    // Skip bad bit (this might not be required)
    if (uiAmountToSkip > 0)
        RwStreamSkip(stream, uiAmountToSkip);
}

// Hook info
#define HOOKPOS_NodeNameStreamRead  0x072FA68
#define HOOKSIZE_NodeNameStreamRead 15
DWORD                        RETURN_NodeNameStreamRead = 0x072FA77;
static void _declspec(naked) HOOK_NodeNameStreamRead()
{
    // clang-format off
    __asm
    {
        pushad
        push    edi
        push    esi
        push    ebx
        call    OnMY_NodeNameStreamRead
        add     esp, 4*3
        popad

        jmp     RETURN_NodeNameStreamRead
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CModelInfoSA::StaticSetHooks()
{
    EZHookInstall(NodeNameStreamRead);
}

// Recursive RwFrame children searching function
void CModelInfoSA::RwSetSupportedUpgrades(RwFrame* parent, DWORD dwModel)
{
    for (RwFrame* ret = parent->child; ret != NULL; ret = ret->next)
    {
        // recurse into the child
        if (ret->child != NULL)
        {
            RwSetSupportedUpgrades(ret, dwModel);
        }
        SString strName = ret->szName;
        // Spoiler
        if (strName == "ug_bonnet")
        {
            m_ModelSupportedUpgrades.m_bBonnet = true;
        }
        else if (strName == "ug_bonnet_left")
        {
            m_ModelSupportedUpgrades.m_bBonnet_Left = true;
        }
        else if (strName == "ug_bonnet_left_dam")
        {
            m_ModelSupportedUpgrades.m_bBonnet_Left_dam = true;
        }
        else if (strName == "ug_bonnet_right")
        {
            m_ModelSupportedUpgrades.m_bBonnet_Right = true;
        }
        else if (strName == "ug_bonnet_right_dam")
        {
            m_ModelSupportedUpgrades.m_bBonnet_Right_dam = true;
        }
        // Spoiler
        else if (strName == "ug_spoiler")
        {
            m_ModelSupportedUpgrades.m_bSpoiler = true;
        }
        else if (strName == "ug_spoiler_dam")
        {
            m_ModelSupportedUpgrades.m_bSpoiler_dam = true;
        }
        // Bonnet
        else if (strName == "ug_lights")
        {
            m_ModelSupportedUpgrades.m_bLamps = true;
        }
        else if (strName == "ug_lights_dam")
        {
            m_ModelSupportedUpgrades.m_bLamps_dam = true;
        }
        // Roof
        else if (strName == "ug_roof")
        {
            m_ModelSupportedUpgrades.m_bRoof = true;
        }
        // Side Skirt
        else if (strName == "ug_wing_right")
        {
            m_ModelSupportedUpgrades.m_bSideSkirt_Right = true;
        }
        // Side Skirt
        else if (strName == "ug_wing_left")
        {
            m_ModelSupportedUpgrades.m_bSideSkirt_Left = true;
        }
        // Exhaust
        else if (strName == "exhaust_ok")
        {
            m_ModelSupportedUpgrades.m_bExhaust = true;
        }
        // Front bullbars
        else if (strName == "ug_frontbullbar")
        {
            m_ModelSupportedUpgrades.m_bFrontBullbars = true;
        }
        // rear bullbars
        else if (strName == "ug_backbullbar")
        {
            m_ModelSupportedUpgrades.m_bRearBullbars = true;
        }
        // Front bumper
        else if (strName == "bump_front_dummy")
        {
            m_ModelSupportedUpgrades.m_bFrontBumper = true;
        }
        // Rear bumper
        else if (strName == "bump_rear_dummy")
        {
            m_ModelSupportedUpgrades.m_bRearBumper = true;
        }
        // Rear bumper
        else if (strName == "misc_c")
        {
            m_ModelSupportedUpgrades.m_bMisc = true;
        }
    }
}

void CModelInfoSA::InitialiseSupportedUpgrades(RpClump* pClump)
{
    m_ModelSupportedUpgrades.Reset();
    RwFrame* pFrame = RpGetFrame(pClump);
    RwSetSupportedUpgrades(pFrame, m_dwModelID);
    m_ModelSupportedUpgrades.m_bInitialised = true;
}

void CModelInfoSA::ResetSupportedUpgrades()
{
    m_ModelSupportedUpgrades.Reset();
}

void CModelInfoSA::SetObjectPropertiesGroup(unsigned short usNewGroup)
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    if (!pInterface)
        return;

    unsigned short usOrgGroup = pInterface->usDynamicIndex;
    if (usOrgGroup == usNewGroup)
        return;

    if (!MapFind(ms_OriginalObjectPropertiesGroups, m_dwModelID))
        MapSet(ms_OriginalObjectPropertiesGroups, m_dwModelID, usOrgGroup);

    pInterface->usDynamicIndex = usNewGroup;
}

unsigned short CModelInfoSA::GetObjectPropertiesGroup()
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    if (pInterface)
        return pInterface->usDynamicIndex;
    return 0;
}

void CModelInfoSA::RestoreObjectPropertiesGroup()
{
    unsigned short* usGroupInMap = MapFind(ms_OriginalObjectPropertiesGroups, m_dwModelID);
    if (!usGroupInMap)
        return;

    CBaseModelInfoSAInterface* pInterface = GetInterface();
    if (!pInterface)
    {
        MapRemove(ms_OriginalObjectPropertiesGroups, m_dwModelID);
        return;
    }

    pInterface->usDynamicIndex = *usGroupInMap;
    MapRemove(ms_OriginalObjectPropertiesGroups, m_dwModelID);
}

void CModelInfoSA::RestoreAllObjectsPropertiesGroups()
{
    for (auto it = ms_OriginalObjectPropertiesGroups.begin(); it != ms_OriginalObjectPropertiesGroups.end();)
    {
        auto* pModelInfo = pGame->GetModelInfo(it->first, true);
        if (!pModelInfo)
        {
            it = ms_OriginalObjectPropertiesGroups.erase(it);
            continue;
        }

        CBaseModelInfoSAInterface* pInterface = pModelInfo->GetInterface();
        if (!pInterface)
        {
            it = ms_OriginalObjectPropertiesGroups.erase(it);
            continue;
        }

        pInterface->usDynamicIndex = it->second;
        it = ms_OriginalObjectPropertiesGroups.erase(it);
    }
}

eModelInfoType CModelInfoSA::GetModelType()
{
    auto pInterface = GetInterface();
    if (!pInterface || !pInterface->VFTBL)
        return eModelInfoType::UNKNOWN;

    DWORD funcAddr = pInterface->VFTBL->GetModelType;
    if (!SharedUtil::IsValidGtaSaPtr(funcAddr))
        return eModelInfoType::UNKNOWN;

    return ((eModelInfoType (*)())funcAddr)();
}

bool CModelInfoSA::IsTowableBy(CModelInfo* towingModel)
{
    bool isTowable = true;

    const bool isTowTruck = towingModel->GetModel() == 525;
    const bool isTractor = towingModel->GetModel() == 531;

    if (IsTrain() || towingModel->IsTrain())
    {
        // A train is never towing other vehicles. Trains are linked by other means
        isTowable = false;
    }
    else if (isTowTruck || isTractor)
    {
        const bool isFarmTrailer = GetModel() == 610;

        // Tow truck (525) and tractor (531) can only tow certain vehicle types without issues
        if (IsBoat() || IsBike() || IsBmx())
        {
            isTowable = false;
        }
        else if (IsTrailer() && !(isTractor && isFarmTrailer))
        {
            isTowable = false;
        }
    }

    return isTowable;
}

bool CModelInfoSA::IsDamageableAtomic()
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    if (!pInterface || !pInterface->VFTBL)
        return false;

    DWORD funcAddr = pInterface->VFTBL->AsDamageAtomicModelInfoPtr;
    if (!SharedUtil::IsValidGtaSaPtr(funcAddr))
        return false;

    void* asDamageable = ((void* (*)())funcAddr)();
    return asDamageable != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CModelInfoSA::ForceUnload
//
// 1. Unload the dff from memory
// 2. Unload the associated txd from memory
// 3. Cross fingers
//
// Why do we do this?
// Player model adds (seemingly) unnecessary refs
// (Will crash if anything is actually using the model or txd)
//
// Won't work if there is a custom model replacement
//
// Returns true if model was unloaded
//
//////////////////////////////////////////////////////////////////////////////////////////
bool CModelInfoSA::ForceUnload()
{
    CBaseModelInfoSAInterface* pInterface = GetInterface();
    if (!pInterface)
        return false;

    m_dwReferences = 0;
    m_dwPendingInterfaceRef = 0;

    if (pInterface->usNumberOfRefs == 0 && pInterface->pRwObject != nullptr)
        pInterface->usNumberOfRefs++;

    DWORD dwFunction = FUNC_RemoveRef;
    uint  uiLimit = 100;
    while (pInterface->usNumberOfRefs > 0 && uiLimit--)
    {
        // clang-format off
        __asm
        {
            mov     ecx, pInterface
            call    dwFunction
        }
        // clang-format on
    }

    if (pInterface->usNumberOfRefs > 0 || pInterface->pRwObject != nullptr)
        return false;

    // If success, then remove txd
    ushort usTxdId = pInterface->usTextureDictionary;
    if (usTxdId)
        pGame->GetRenderWare()->TxdForceUnload(usTxdId, true);

    return true;
}

bool CVehicleModelInfoSAInterface::IsComponentDamageable(int componentIndex) const
{
    return pVisualInfo->m_maskComponentDamagable & (1 << componentIndex);
}
