/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CModelInfoSA.cpp
 *  PURPOSE:     Entity model information handler
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

extern CGameSA* pGame;

CBaseModelInfoSAInterface** CModelInfoSAInterface::ms_modelInfoPtrs = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;
CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;

std::map<unsigned short, int>                                         CModelInfoSA::ms_RestreamTxdIDMap;
std::map<DWORD, float>                                                CModelInfoSA::ms_ModelDefaultLodDistanceMap;
std::map<DWORD, BYTE>                                                 CModelInfoSA::ms_ModelDefaultAlphaTransparencyMap;
std::unordered_map<std::uint32_t, std::map<eVehicleDummies, CVector>> CModelInfoSA::ms_ModelDefaultDummiesPosition;
std::map<CTimeInfoSAInterface*, CTimeInfoSAInterface*>                CModelInfoSA::ms_ModelDefaultModelTimeInfo;
std::unordered_map<DWORD, unsigned short>                             CModelInfoSA::ms_OriginalObjectPropertiesGroups;
std::unordered_map<DWORD, std::pair<float, float>>                    CModelInfoSA::ms_VehicleModelDefaultWheelSizes;

static constexpr uintptr_t vftable_CVehicleModelInfo = 0x85C5C8u;

CModelInfoSA::CModelInfoSA()
{
    m_pInterface = NULL;
    this->m_dwModelID = 0xFFFFFFFF;
    m_dwReferences = 0;
    m_dwPendingInterfaceRef = 0;
    m_pOriginalColModelInterface = NULL;
    m_pCustomClump = NULL;
    m_pCustomColModel = NULL;
    m_bAddedRefForCollision = false;
}

CModelInfoSA::CModelInfoSA(DWORD dwModelID)
{
    this->m_dwModelID = dwModelID;
    m_pInterface = ppModelInfo[m_dwModelID];
    m_dwReferences = 0;
    m_dwPendingInterfaceRef = 0;
    m_pOriginalColModelInterface = NULL;
    m_pCustomClump = NULL;
    m_pCustomColModel = NULL;
    m_bAddedRefForCollision = false;
}

CBaseModelInfoSAInterface* CModelInfoSA::GetInterface()
{
    return m_pInterface = ppModelInfo[m_dwModelID];
}

BOOL CModelInfoSA::IsBoat()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsBoat ( )");
    DWORD dwFunction = FUNC_IsBoatModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsCar()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsCar ( )");
    DWORD dwFunction = FUNC_IsCarModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsTrain()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsTrain ( )");
    DWORD dwFunction = FUNC_IsTrainModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsHeli()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsHeli ( )");
    DWORD dwFunction = FUNC_IsHeliModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsPlane()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsPlane ( )");
    DWORD dwFunction = FUNC_IsPlaneModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsBike()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsBike ( )");
    DWORD dwFunction = FUNC_IsBikeModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsFakePlane()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsFakePlane ( )");
    DWORD dwFunction = FUNC_IsFakePlaneModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsMonsterTruck()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsMonsterTruck ( )");
    DWORD dwFunction = FUNC_IsMonsterTruckModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsQuadBike()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsQuadBike ( )");
    DWORD dwFunction = FUNC_IsQuadBikeModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsBmx()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsBmx ( )");
    DWORD dwFunction = FUNC_IsBmxModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsTrailer()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsTrailer ( )");
    DWORD dwFunction = FUNC_IsTrailerModel;
    DWORD ModelID = m_dwModelID;
    BYTE  bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BYTE CModelInfoSA::GetVehicleType()
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsVehicle ( )");
    // This function will return a vehicle type for vehicles or 0xFF on failure
    DWORD dwFunction = FUNC_IsVehicleModelType;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = -1;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return bReturn;
}

bool CModelInfoSA::IsVehicle() const
{
    // NOTE(botder): This is from CModelInfo::IsVehicleModelType
    if (m_dwModelID >= 20000)
        return false;

    // NOTE(botder): m_pInterface might be a nullptr here, we can't use it
    CBaseModelInfoSAInterface* model = ppModelInfo[m_dwModelID];
    return model != nullptr && reinterpret_cast<intptr_t>(model->VFTBL) == vftable_CVehicleModelInfo;
}

bool CModelInfoSA::IsPlayerModel()
{
    return (GetInterface() && GetInterface()->pColModel && GetInterface()->pColModel == (CColModelSAInterface*)VAR_CTempColModels_ModelPed1);
}

BOOL CModelInfoSA::IsUpgrade()
{
    return m_dwModelID >= 1000 && m_dwModelID <= 1193;
}

char* CModelInfoSA::GetNameIfVehicle()
{
    DEBUG_TRACE("char * CModelInfoSA::GetNameIfVehicle ( )");
    //  if(this->IsVehicle())
    //  {
    DWORD dwModelInfo = (DWORD)ARRAY_ModelInfo;
    DWORD dwFunc = FUNC_CText_Get;
    DWORD ModelID = m_dwModelID;
    DWORD dwReturn = 0;

        _asm
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
    return (char*)dwReturn;
    //  }
    //  return NULL;
}

uint CModelInfoSA::GetAnimFileIndex()
{
    DWORD dwFunc = m_pInterface->VFTBL->GetAnimFileIndex;
    DWORD dwThis = (DWORD)m_pInterface;
    uint  uiReturn = 0;
    if (dwFunc)
    {
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
            mov     uiReturn, eax
        }
    }
    return uiReturn;
}

VOID CModelInfoSA::Request(EModelRequestType requestType, const char* szTag)
{
    DEBUG_TRACE("VOID CModelInfoSA::Request( BOOL bAndLoad, BOOL bWaitForLoad )");
    // don't bother loading it if it already is
    if (IsLoaded())
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
        pGame->GetStreaming()->LoadAllRequestedModels(true, szTag);
        if (!IsLoaded())
        {
            // Try 3 more times, final time without high priority flag
            int iCount = 0;
            while (iCount++ < 10 && !IsLoaded())
            {
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

VOID CModelInfoSA::Remove()
{
    DEBUG_TRACE("VOID CModelInfoSA::Remove ( )");

    // Don't remove if GTA refers to it somehow.
    // Or we'll screw up SA's map for example.

    m_pInterface = ppModelInfo[m_dwModelID];

    // Remove ref added for collision
    if (m_bAddedRefForCollision)
    {
        m_bAddedRefForCollision = false;
        if (m_pInterface->usNumberOfRefs > 0)
            m_pInterface->usNumberOfRefs--;
    }

    // Remove our reference
    if (m_pInterface->usNumberOfRefs > 0)
        m_pInterface->usNumberOfRefs--;

    // No references left?
    if (m_pInterface->usNumberOfRefs == 0)
    {
        // We have a custom model?
        if (m_pCustomClump)
        {
            // Mark us as unloaded. We manage the clump unloading.
            // BYTE *ModelLoaded = (BYTE*)ARRAY_ModelLoaded;
            // ModelLoaded[(m_dwModelID+m_dwModelID*4)<<2] = 0;
        }
        else
        {
            // Make our collision model original again before we unload.
            RestoreColModel();

            // Remove the model.
            DWORD dwFunction = FUNC_RemoveModel;
            DWORD ModelID = m_dwModelID;
            _asm
            {
                push    ModelID
                call    dwFunction
                add     esp, 4
            }
        }
    }
}

BYTE CModelInfoSA::GetLevelFromPosition(CVector* vecPosition)
{
    DEBUG_TRACE("BYTE CModelInfoSA::GetLevelFromPosition ( CVector * vecPosition )");
    DWORD dwFunction = FUNC_GetLevelFromPosition;
    BYTE  bReturn = 0;
    _asm
    {
        push    vecPosition
        call    dwFunction
        add     esp, 4
        mov     bReturn, al
    }
    return bReturn;
}

BOOL CModelInfoSA::IsLoaded()
{
    if (DoIsLoaded())
    {
        if (m_dwPendingInterfaceRef)
        {
            assert(m_dwReferences > 0);
            m_pInterface = ppModelInfo[m_dwModelID];
            m_pInterface->usNumberOfRefs++;
            m_dwPendingInterfaceRef = 0;
        }
        return true;
    }
    return false;
}

BOOL CModelInfoSA::DoIsLoaded()
{
    DEBUG_TRACE("BOOL CModelInfoSA::DoIsLoaded ( )");

    // return (BOOL)*(BYTE *)(ARRAY_ModelLoaded + 20*dwModelID);
    BOOL bLoaded = pGame->GetStreaming()->HasModelLoaded(m_dwModelID);

    if (m_dwModelID < pGame->GetBaseIDforTXD())
    {
        m_pInterface = ppModelInfo[m_dwModelID];

        if (bLoaded)
        {
            // Check rw object is there
            if (!m_pInterface || !m_pInterface->pRwObject)
                return false;
        }
    }
    return bLoaded;
}

BYTE CModelInfoSA::GetFlags()
{
    DWORD dwFunc = FUNC_GetModelFlags;
    DWORD ModelID = m_dwModelID;
    BYTE  bFlags = 0;
    _asm
    {
        push    ModelID
        call    dwFunc
        add     esp, 4
        mov     bFlags, al
    }
    return bFlags;
}

CBoundingBox* CModelInfoSA::GetBoundingBox()
{
    DWORD         dwFunc = FUNC_GetBoundingBox;
    DWORD         ModelID = m_dwModelID;
    CBoundingBox* dwReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunc
        add     esp, 4
        mov     dwReturn, eax
    }
    return dwReturn;
}

bool CModelInfoSA::IsValid()
{
    if (m_dwModelID >= pGame->GetBaseIDforTXD() && m_dwModelID < pGame->GetCountOfAllFileIDs())
        return true;

    if (!ppModelInfo[m_dwModelID])
        return false;

    return true;
}

bool CModelInfoSA::IsAllocatedInArchive()
{
    return pGame->GetStreaming()->GetStreamingInfoFromModelId(m_dwModelID)->sizeInBlocks > 0;
}

float CModelInfoSA::GetDistanceFromCentreOfMassToBaseOfModel()
{
    DWORD dwModelInfo = 0;
    DWORD ModelID = m_dwModelID;
    FLOAT fReturn = 0;
    _asm {
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
    return fReturn;
}

unsigned short CModelInfoSA::GetTextureDictionaryID()
{
    m_pInterface = ppModelInfo[m_dwModelID];
    if (m_pInterface)
        return m_pInterface->usTextureDictionary;

    return 0;
}

void CModelInfoSA::SetTextureDictionaryID(unsigned short usID)
{
    m_pInterface = ppModelInfo[m_dwModelID];
    if (m_pInterface)
        m_pInterface->usTextureDictionary = usID;
}

float CModelInfoSA::GetLODDistance()
{
    m_pInterface = ppModelInfo[m_dwModelID];
    if (m_pInterface)
        return m_pInterface->fLodDistanceUnscaled;

    return 0.0f;
}

bool CModelInfoSA::SetTime(char cHourOn, char cHourOff)
{
    m_pInterface = ppModelInfo[m_dwModelID];
    if (!m_pInterface)
        return false;

    if (GetModelType() != eModelInfoType::TIME)
        return false;

    CTimeInfoSAInterface* pTime = &static_cast<CTimeModelInfoSAInterface*>(m_pInterface)->timeInfo;

    if (!MapContains(ms_ModelDefaultModelTimeInfo, pTime))
        MapSet(ms_ModelDefaultModelTimeInfo, pTime, new CTimeInfoSAInterface(pTime->m_nTimeOn, pTime->m_nTimeOff, pTime->m_wOtherTimeModel));

    pTime->m_nTimeOn = cHourOn;
    pTime->m_nTimeOff = cHourOff;
    return true;
}

bool CModelInfoSA::GetTime(char& cHourOn, char& cHourOff)
{
    m_pInterface = ppModelInfo[m_dwModelID];
    if (!m_pInterface)
        return false;

    if (GetModelType() != eModelInfoType::TIME)
        return false;

    CTimeInfoSAInterface* pTime = &static_cast<CTimeModelInfoSAInterface*>(m_pInterface)->timeInfo;

    cHourOn = pTime->m_nTimeOn;
    cHourOff = pTime->m_nTimeOff;
    return true;
}

void CModelInfoSA::StaticResetModelTimes()
{
    for (auto const& x : ms_ModelDefaultModelTimeInfo)
    {
        x.first->m_nTimeOn = x.second->m_nTimeOn;
        x.first->m_nTimeOff = x.second->m_nTimeOff;
    }
    ms_ModelDefaultModelTimeInfo.clear();
}

float CModelInfoSA::GetOriginalLODDistance()
{
    // Return default LOD distance value (if doesn't exist, LOD distance hasn't been changed)
    if (MapContains(ms_ModelDefaultLodDistanceMap, m_dwModelID))
        return MapGet(ms_ModelDefaultLodDistanceMap, m_dwModelID);

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
    if (!bOverrideMaxDistance) {
        // Limit to 325.f as it goes horrible after that
        fDistance = std::min(fDistance, 325.f);
    }

    m_pInterface = ppModelInfo[m_dwModelID];
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
    // Restore default values
    for (std::map<DWORD, float>::const_iterator iter = ms_ModelDefaultLodDistanceMap.begin(); iter != ms_ModelDefaultLodDistanceMap.end(); ++iter)
    {
        CBaseModelInfoSAInterface* pInterface = ppModelInfo[iter->first];
        if (pInterface)
            pInterface->fLodDistanceUnscaled = iter->second;
    }

    ms_ModelDefaultLodDistanceMap.clear();
}

void CModelInfoSA::RestreamIPL()
{
    // IPLs should not contain peds, weapons, vehicles and vehicle upgrades
    if (m_dwModelID > 611 && (m_dwModelID < 1000 || m_dwModelID > 1193))
        MapSet(ms_RestreamTxdIDMap, GetTextureDictionaryID(), 0);
}

void CModelInfoSA::StaticFlushPendingRestreamIPL()
{
    if (ms_RestreamTxdIDMap.empty())
        return;
    // This function restreams all instances of the model *that are from the default SA world (ipl)*.
    // In other words, it does not affect elements created by MTA.
    // It's mostly a reimplementation of SA's DeleteAllRwObjects, except that it filters by model ID.

    ((void (*)())FUNC_FlushRequestList)();

    std::set<unsigned short> removedModels;

    for (int i = 0; i < 2 * NUM_StreamSectorRows * NUM_StreamSectorCols; i++)
    {
        DWORD* pSectorEntry = ((DWORD**)ARRAY_StreamSectors)[i];
        while (pSectorEntry)
        {
            CEntitySAInterface* pEntity = (CEntitySAInterface*)pSectorEntry[0];

            // Possible bug - pEntity seems to be invalid here occasionally
            if (pEntity->vtbl->DeleteRwObject != 0x00534030)
            {
                // Log info
                OutputDebugString(SString("Entity 0x%08x (with model %d) at ARRAY_StreamSectors[%d,%d] is invalid\n", pEntity, pEntity->m_nModelIndex,
                                          i / 2 % NUM_StreamSectorRows, i / 2 / NUM_StreamSectorCols));
                // Assert in debug
                #if MTA_DEBUG
                assert(pEntity->vtbl->DeleteRwObject == 0x00534030);
                #endif
                pSectorEntry = (DWORD*)pSectorEntry[1];
                continue;
            }

            if (MapContains(ms_RestreamTxdIDMap, pGame->GetModelInfo(pEntity->m_nModelIndex)->GetTextureDictionaryID()))
            {
                if (!pEntity->bStreamingDontDelete && !pEntity->bImBeingRendered)
                {
                    _asm
                    {
                        mov ecx, pEntity
                        mov eax, [ecx]
                        call dword ptr [eax+20h]
                    }
                    removedModels.insert(pEntity->m_nModelIndex);
                }
            }

            pSectorEntry = (DWORD*)pSectorEntry[1];
        }
    }

    for (int i = 0; i < NUM_StreamRepeatSectorRows * NUM_StreamRepeatSectorCols; i++)
    {
        DWORD* pSectorEntry = ((DWORD**)ARRAY_StreamRepeatSectors)[3 * i + 2];
        while (pSectorEntry)
        {
            CEntitySAInterface* pEntity = (CEntitySAInterface*)pSectorEntry[0];
            if (MapContains(ms_RestreamTxdIDMap, pGame->GetModelInfo(pEntity->m_nModelIndex)->GetTextureDictionaryID()))
            {
                if (!pEntity->bStreamingDontDelete && !pEntity->bImBeingRendered)
                {
                    _asm
                    {
                        mov ecx, pEntity
                        mov eax, [ecx]
                        call dword ptr [eax+20h]
                    }
                    removedModels.insert(pEntity->m_nModelIndex);
                }
            }
            pSectorEntry = (DWORD*)pSectorEntry[1];
        }
    }

    ms_RestreamTxdIDMap.clear();

    std::set<unsigned short>::iterator it;
    for (it = removedModels.begin(); it != removedModels.end(); it++)
    {
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(*it);
        pGame->GetStreaming()->GetStreamingInfoFromModelId(*it)->loadState = 0;
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
            m_pInterface->usNumberOfRefs++;
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
        if (m_pInterface->usNumberOfRefs > 1)
        {
            DWORD                      dwFunction = FUNC_RemoveRef;
            CBaseModelInfoSAInterface* pInterface = m_pInterface;
            _asm
            {
                mov     ecx, pInterface
                call    dwFunction
            }
        }
    }

    // Unload it if 0 references left and we're not CJ model.
    // And if we're loaded.
    if (m_dwReferences == 0 && m_dwModelID != 0 && IsLoaded())
    {
        Remove();
    }
}

void CModelInfoSA::SetAlphaTransparencyEnabled(BOOL bEnabled)
{
    m_pInterface = ppModelInfo[m_dwModelID];
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
    m_pInterface = ppModelInfo[m_dwModelID];
    if (m_pInterface)
    {
        return m_pInterface->bAlphaTransparency;
    }
    return false;
}

void CModelInfoSA::StaticResetAlphaTransparencies()
{
    for (std::map<DWORD, BYTE>::const_iterator iter = ms_ModelDefaultAlphaTransparencyMap.begin(); iter != ms_ModelDefaultAlphaTransparencyMap.end(); iter++)
    {
        CBaseModelInfoSAInterface* pInterface = ppModelInfo[iter->first];
        if (pInterface)
        {
            pInterface->bAlphaTransparency = iter->second;
        }
    }

    ms_ModelDefaultAlphaTransparencyMap.clear();
}

void CModelInfoSA::ResetAlphaTransparency()
{
    m_pInterface = ppModelInfo[m_dwModelID];
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
        _asm
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
    }
    return sreturn;
}

bool CModelInfoSA::IsUpgradeAvailable(eVehicleUpgradePosn posn)
{
    bool  bRet = false;
    DWORD ModelID = m_dwModelID;
    _asm
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
    return bRet;
}

void CModelInfoSA::SetCustomCarPlateText(const char* szText)
{
    char* szStoredText;
    DWORD ModelID = m_dwModelID;
    _asm
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

    if (szText) strncpy(szStoredText, szText, 8);
    else szStoredText[0] = 0;
}

unsigned int CModelInfoSA::GetNumRemaps()
{
    DWORD        dwFunc = FUNC_CVehicleModelInfo__GetNumRemaps;
    DWORD        ModelID = m_dwModelID;
    unsigned int uiReturn = 0;
    _asm
    {
        mov     ecx, ModelID

        push    eax
        mov     eax, dword ptr[ARRAY_ModelInfo]
        mov     ecx, dword ptr[eax + ecx*4]
        pop     eax

        call    dwFunc
        mov     uiReturn, eax
    }
    return uiReturn;
}

void* CModelInfoSA::GetVehicleSuspensionData()
{
    return GetInterface()->pColModel->pColData->pSuspensionLines;
}

void* CModelInfoSA::SetVehicleSuspensionData(void* pSuspensionLines)
{
    CColDataSA* pColData = GetInterface()->pColModel->pColData;
    void*       pOrigSuspensionLines = pColData->pSuspensionLines;
    pColData->pSuspensionLines = pSuspensionLines;
    return pOrigSuspensionLines;
}

CVector CModelInfoSA::GetVehicleExhaustFumesPosition()
{
    return GetVehicleDummyPosition(eVehicleDummies::EXHAUST);
}

void CModelInfoSA::SetVehicleExhaustFumesPosition(const CVector& vecPosition)
{
    return SetVehicleDummyPosition(eVehicleDummies::EXHAUST, vecPosition);
}

bool CModelInfoSA::GetVehicleDummyPositions(std::array<CVector, VEHICLE_DUMMY_COUNT>& positions) const
{
    if (!IsVehicle())
        return false;

    CVector* dummyPositions = reinterpret_cast<CVehicleModelInfoSAInterface*>(m_pInterface)->pVisualInfo->vecDummies;
    std::copy(dummyPositions, dummyPositions + positions.size(), positions.begin());
    return true;
}

CVector CModelInfoSA::GetVehicleDummyDefaultPosition(eVehicleDummies eDummy)
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

    if (!IsLoaded())
        Request(BLOCKING, "GetVehicleDummyDefaultPosition");

    auto modelInfo = reinterpret_cast<CVehicleModelInfoSAInterface*>(m_pInterface);
    return modelInfo->pVisualInfo->vecDummies[eDummy];
}

CVector CModelInfoSA::GetVehicleDummyPosition(eVehicleDummies eDummy)
{
    if (!IsVehicle())
        return CVector();

    // Request model load right now if not loaded yet (#9897)
    if (!IsLoaded())
        Request(BLOCKING, "GetVehicleDummyPosition");

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(m_pInterface);
    return pVehicleModel->pVisualInfo->vecDummies[eDummy];
}

void CModelInfoSA::SetVehicleDummyPosition(eVehicleDummies eDummy, const CVector& vecPosition)
{
    if (!IsVehicle())
        return;

    // Request model load right now if not loaded yet (#9897)
    if (!IsLoaded())
        Request(BLOCKING, "SetVehicleDummyPosition");

    // Store default position in map
    auto iter = ms_ModelDefaultDummiesPosition.find(m_dwModelID);
    if (iter == ms_ModelDefaultDummiesPosition.end())
    {
        ms_ModelDefaultDummiesPosition.insert({m_dwModelID, std::map<eVehicleDummies, CVector>()});
        // Increment this model references count, so we don't unload it before we have a chance to reset the positions
        m_pInterface->usNumberOfRefs++;
    }

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(m_pInterface);
    if (ms_ModelDefaultDummiesPosition[m_dwModelID].find(eDummy) == ms_ModelDefaultDummiesPosition[m_dwModelID].end())
    {
        ms_ModelDefaultDummiesPosition[m_dwModelID][eDummy] = pVehicleModel->pVisualInfo->vecDummies[eDummy];
    }

    // Set dummy position
    pVehicleModel->pVisualInfo->vecDummies[eDummy] = vecPosition;
}

void CModelInfoSA::ResetVehicleDummies(bool bRemoveFromDummiesMap)
{
    if (!IsVehicle())
        return;

    auto iter = ms_ModelDefaultDummiesPosition.find(m_dwModelID);
    if (iter == ms_ModelDefaultDummiesPosition.end())
        return; // Early out in case the model doesn't have any dummies modified

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(m_pInterface);
    for (const auto& dummy : ms_ModelDefaultDummiesPosition[m_dwModelID])
    {
        if (pVehicleModel->pVisualInfo != nullptr)
            pVehicleModel->pVisualInfo->vecDummies[dummy.first] = dummy.second;
    }
    // Decrement reference counter, since we reverted all position changes, the model can be safely unloaded
    pVehicleModel->usNumberOfRefs--;

    if (bRemoveFromDummiesMap)
        ms_ModelDefaultDummiesPosition.erase(m_dwModelID);
}

void CModelInfoSA::ResetAllVehicleDummies()
{
    CGame* game = g_pCore->GetGame();
    for (auto& info : ms_ModelDefaultDummiesPosition) {
        CModelInfo* modelInfo = game->GetModelInfo(info.first);
        if (modelInfo)
            modelInfo->ResetVehicleDummies(false);
    }

    ms_ModelDefaultDummiesPosition.clear();
}

float CModelInfoSA::GetVehicleWheelSize(eResizableVehicleWheelGroup eWheelGroup)
{
    if (!IsVehicle())
        return 0.0f;

    // Request model load right now if not loaded yet
    if (!IsLoaded())
        Request(BLOCKING, "GetVehicleWheelSize");

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(m_pInterface);
    switch (eWheelGroup)
    {
        case eResizableVehicleWheelGroup::FRONT_AXLE:
            return pVehicleModel->fWheelSizeFront;
        case eResizableVehicleWheelGroup::REAR_AXLE:
            return pVehicleModel->fWheelSizeRear;
    }

    return 0.0f;
}

void CModelInfoSA::SetVehicleWheelSize(eResizableVehicleWheelGroup eWheelGroup, float fWheelSize)
{
    if (!IsVehicle())
        return;

    // Request model load right now if not loaded yet
    if (!IsLoaded())
        Request(BLOCKING, "SetVehicleWheelSize");

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(m_pInterface);

    // Store default wheel sizes in map
    if (!MapFind(ms_VehicleModelDefaultWheelSizes, m_dwModelID))
        MapSet(ms_VehicleModelDefaultWheelSizes, m_dwModelID, std::make_pair(pVehicleModel->fWheelSizeFront, pVehicleModel->fWheelSizeRear));

    switch (eWheelGroup)
    {
        case eResizableVehicleWheelGroup::FRONT_AXLE:
            pVehicleModel->fWheelSizeFront = fWheelSize;
            break;
        case eResizableVehicleWheelGroup::REAR_AXLE:
            pVehicleModel->fWheelSizeRear = fWheelSize;
            break;
        case eResizableVehicleWheelGroup::ALL_WHEELS:
            pVehicleModel->fWheelSizeFront = fWheelSize;
            pVehicleModel->fWheelSizeRear = fWheelSize;
            break;
    }
}

void CModelInfoSA::ResetVehicleWheelSizes(std::pair<float, float>* defaultSizes)
{
    if (!IsVehicle())
        return;

    std::pair<float, float>* sizesPair;
    if (!defaultSizes)
    {
        sizesPair = MapFind(ms_VehicleModelDefaultWheelSizes, m_dwModelID);
        MapRemove(ms_VehicleModelDefaultWheelSizes, m_dwModelID);
    }
    else
    {
        sizesPair = defaultSizes;
    }

    // Default values not found in map
    if (!sizesPair)
        return;

    auto pVehicleModel = reinterpret_cast<CVehicleModelInfoSAInterface*>(m_pInterface);
    pVehicleModel->fWheelSizeFront = sizesPair->first;
    pVehicleModel->fWheelSizeRear = sizesPair->second;
}

void CModelInfoSA::ResetAllVehiclesWheelSizes()
{
    CGame* game = g_pCore->GetGame();
    for (auto& info : ms_VehicleModelDefaultWheelSizes)
    {
        CModelInfo* modelInfo = game->GetModelInfo(info.first);
        if (modelInfo)
        {
            modelInfo->ResetVehicleWheelSizes(&info.second);
        }
    }

    ms_VehicleModelDefaultWheelSizes.clear();
}

void CModelInfoSA::SetCustomModel(RpClump* pClump)
{
    // Error
    if (pClump == NULL)
        return;

    // Store the custom clump
    m_pCustomClump = pClump;

    // Replace the model if we're loaded.
    if (IsLoaded())
    {
        switch (GetModelType())
        {
            case eModelInfoType::PED:
                return pGame->GetRenderWare()->ReplacePedModel(pClump, static_cast<unsigned short>(m_dwModelID));
            case eModelInfoType::WEAPON:
                return pGame->GetRenderWare()->ReplaceWeaponModel(pClump, static_cast<unsigned short>(m_dwModelID));
            case eModelInfoType::VEHICLE:
                return pGame->GetRenderWare()->ReplaceVehicleModel(pClump, static_cast<unsigned short>(m_dwModelID));
            case eModelInfoType::ATOMIC:
            case eModelInfoType::LOD_ATOMIC:
            case eModelInfoType::TIME:
                return pGame->GetRenderWare()->ReplaceAllAtomicsInModel(pClump, static_cast<unsigned short>(m_dwModelID));
        }
    }
}

void CModelInfoSA::RestoreOriginalModel()
{
    // Are we loaded?
    if (IsLoaded())
    {
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(static_cast<unsigned short>(m_dwModelID));
    }

    // Reset the stored custom vehicle clump
    m_pCustomClump = NULL;
}

void CModelInfoSA::SetColModel(CColModel* pColModel)
{
    // Grab the interfaces
    CColModelSAInterface* pColModelInterface = pColModel->GetInterface();

    if (!m_bAddedRefForCollision)
    {
        // Prevent this model from unloading while we have custom collision
        ModelAddRef(BLOCKING, "for collision");
        m_bAddedRefForCollision = true;
    }

    // Should always be loaded at this point

    // Skip setting if already done
    if (m_pCustomColModel == pColModel)
        return;

    // Store the col model we set
    m_pCustomColModel = pColModel;

    // Do the following only if we're loaded
    m_pInterface = ppModelInfo[m_dwModelID];
    if (m_pInterface)
    {
        // If no collision model has been set before, store the original in case we want to restore it
        if (!m_pOriginalColModelInterface)
            m_pOriginalColModelInterface = m_pInterface->pColModel;

        // Apply some low-level hacks
        pColModelInterface->level = 0xA9;

        // Call SetColModel
        DWORD dwFunc = FUNC_SetColModel;
        DWORD ModelID = m_dwModelID;
        _asm
        {
            mov     ecx, ModelID

            push    eax
            mov     eax, dword ptr[ARRAY_ModelInfo]
            mov     ecx, dword ptr[eax + ecx*4]
            pop     eax

            push    1
            push    pColModelInterface
            call    dwFunc
        }

        // FUNC_SetColModel resets bDoWeOwnTheColModel
        m_pInterface->bDoWeOwnTheColModel = false;
        m_pInterface->bCollisionWasStreamedWithModel = false;

        // public: static void __cdecl CColAccel::addCacheCol(int, class CColModel const &)
        DWORD func = 0x5B2C20;
        _asm
        {
            push    pColModelInterface
            push    ModelID
            call    func
            add     esp, 8
        }

        // Set some lighting for this collision if not already present
        CColDataSA* pColData = pColModelInterface->pColData;
        if (pColData)
        {
            for (uint i = 0; i < pColData->numColTriangles; i++)
            {
                CColTriangleSA* pTriangle = pColData->pColTriangles + i;
                if (pTriangle->lighting.night == 0 && pTriangle->lighting.day == 0)
                {
                    pTriangle->lighting.night = 1;
                    pTriangle->lighting.day = 12;
                }
            }
        }
    }
}

void CModelInfoSA::RestoreColModel()
{
    // Are we loaded?
    m_pInterface = ppModelInfo[m_dwModelID];
    if (m_pInterface)
    {
        // We only have to store if the collision model was set
        // Also only if we have a col model set
        if (m_pOriginalColModelInterface && m_pCustomColModel)
        {
            DWORD dwFunc = FUNC_SetColModel;
            DWORD dwOriginalColModelInterface = (DWORD)m_pOriginalColModelInterface;
            DWORD ModelID = m_dwModelID;
            _asm
            {
                mov     ecx, ModelID

                push    eax
                mov     eax, dword ptr[ARRAY_ModelInfo]
                mov     ecx, dword ptr[eax + ecx*4]
                pop     eax

                push    1
                push    dwOriginalColModelInterface
                call    dwFunc
            }

            // public: static void __cdecl CColAccel::addCacheCol(int, class CColModel const &)
            DWORD func = 0x5B2C20;
            _asm
            {
                push    dwOriginalColModelInterface
                push    ModelID
                call    func
                add     esp, 8
            }
            // (IJs) Document this function some time
        }
    }

    // We currently have no custom model loaded
    m_pCustomColModel = NULL;

    // Remove ref added for collision
    if (m_bAddedRefForCollision)
    {
        m_bAddedRefForCollision = false;
        RemoveRef();
    }
}

void CModelInfoSA::MakeCustomModel()
{
    // We have a custom model?
    if (m_pCustomClump)
    {
        SetCustomModel(m_pCustomClump);
    }

    // Custom collision model is not NULL and it's different from the original?
    if (m_pCustomColModel)
    {
        SetColModel(m_pCustomColModel);
    }
}

void CModelInfoSA::GetVoice(short* psVoiceType, short* psVoiceID)
{
    if (psVoiceType)
        *psVoiceType = GetPedModelInfoInterface()->sVoiceType;
    if (psVoiceID)
        *psVoiceID = GetPedModelInfoInterface()->sFirstVoice;
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
    GetPedModelInfoInterface()->sVoiceType = sVoiceType;
    GetPedModelInfoInterface()->sFirstVoice = sVoiceID;
    GetPedModelInfoInterface()->sLastVoice = sVoiceID;
    GetPedModelInfoInterface()->sNextVoice = sVoiceID;
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
    CStreamingInfo* pBaseModelStreamingInfo = pGame->GetStreaming()->GetStreamingInfoFromModelId(usBaseModelID);
    CStreamingInfo* pTargetModelStreamingInfo = pGame->GetStreaming()->GetStreamingInfoFromModelId(m_dwModelID);

    pTargetModelStreamingInfo->Reset();
    pTargetModelStreamingInfo->archiveId = pBaseModelStreamingInfo->archiveId;
    pTargetModelStreamingInfo->offsetInBlocks = pBaseModelStreamingInfo->offsetInBlocks;
    pTargetModelStreamingInfo->sizeInBlocks = pBaseModelStreamingInfo->sizeInBlocks;
}

void CModelInfoSA::MakePedModel(char* szTexture)
{
    // Create a new CPedModelInfo
    CPedModelInfoSA pedModelInfo;
    ppModelInfo[m_dwModelID] = (CBaseModelInfoSAInterface*)pedModelInfo.GetPedModelInfoInterface();
    // Load our texture
    pGame->GetStreaming()->RequestSpecialModel(m_dwModelID, szTexture, 0);
}

void CModelInfoSA::MakeObjectModel(ushort usBaseID)
{
    CBaseModelInfoSAInterface* m_pInterface = new CBaseModelInfoSAInterface();

    CBaseModelInfoSAInterface* pBaseObjectInfo = ppModelInfo[usBaseID];
    MemCpyFast(m_pInterface, pBaseObjectInfo, sizeof(CBaseModelInfoSAInterface));
    m_pInterface->usNumberOfRefs = 0;
    m_pInterface->pRwObject = nullptr;
    m_pInterface->usUnknown = 65535;
    m_pInterface->usDynamicIndex = 65535;

    ppModelInfo[m_dwModelID] = m_pInterface;

    m_dwParentID = usBaseID;
    CopyStreamingInfoFromModel(usBaseID);
}

void CModelInfoSA::MakeVehicleAutomobile(ushort usBaseID)
{
    CVehicleModelInfoSAInterface* m_pInterface = new CVehicleModelInfoSAInterface();

    CBaseModelInfoSAInterface* pBaseObjectInfo = (CBaseModelInfoSAInterface*)ppModelInfo[usBaseID];
    MemCpyFast(m_pInterface, pBaseObjectInfo, sizeof(CVehicleModelInfoSAInterface));
    m_pInterface->usNumberOfRefs = 0;
    m_pInterface->pRwObject = nullptr;
    m_pInterface->pVisualInfo = nullptr;
    m_pInterface->usUnknown = 65535;
    m_pInterface->usDynamicIndex = 65535;

    ppModelInfo[m_dwModelID] = m_pInterface;

    m_dwParentID = usBaseID;
    CopyStreamingInfoFromModel(usBaseID);
}

void CModelInfoSA::DeallocateModel(void)
{
    Remove();
    ppModelInfo[m_dwModelID] = nullptr;
    pGame->GetStreaming()->GetStreamingInfoFromModelId(m_dwModelID)->Reset();
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
#define HOOKPOS_NodeNameStreamRead                         0x072FA68
#define HOOKSIZE_NodeNameStreamRead                        15
DWORD RETURN_NodeNameStreamRead = 0x072FA77;
void _declspec(naked) HOOK_NodeNameStreamRead()
{
    _asm
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
    unsigned short usOrgGroup = GetObjectPropertiesGroup();
    if (usOrgGroup == usNewGroup)
        return;

    if (!MapFind(ms_OriginalObjectPropertiesGroups, m_dwModelID))
        MapSet(ms_OriginalObjectPropertiesGroups, m_dwModelID, usOrgGroup);

    GetInterface()->usDynamicIndex = usNewGroup;
}

unsigned short CModelInfoSA::GetObjectPropertiesGroup()
{
    unsigned short usGroup = GetInterface()->usDynamicIndex;
    if (usGroup == 0xFFFF)
        usGroup = 0;

    return usGroup;
}

void CModelInfoSA::RestoreObjectPropertiesGroup()
{
    unsigned short* usGroupInMap = MapFind(ms_OriginalObjectPropertiesGroups, m_dwModelID);
    if (usGroupInMap)
    {
        GetInterface()->usDynamicIndex = *usGroupInMap;
        MapRemove(ms_OriginalObjectPropertiesGroups, m_dwModelID);
    }
}

void CModelInfoSA::RestoreAllObjectsPropertiesGroups()
{
    for (const auto& pair : ms_OriginalObjectPropertiesGroups)
    {
        pGame->GetModelInfo(pair.first)->GetInterface()->usDynamicIndex = pair.second;
    }
    ms_OriginalObjectPropertiesGroups.clear();
}

eModelInfoType CModelInfoSA::GetModelType()
{
    return ((eModelInfoType(*)())m_pInterface->VFTBL->GetModelType)();
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
    CBaseModelInfoSAInterface* pModelInfoSAInterface = GetInterface();

    // Need to have at least one ref to delete pRwObject
    if (pModelInfoSAInterface->usNumberOfRefs == 0 && pModelInfoSAInterface->pRwObject != NULL)
    {
        pModelInfoSAInterface->usNumberOfRefs++;
    }

    // Keep removing refs from the model until is it gone
    uint uiLimit = 100;
    while (pModelInfoSAInterface->usNumberOfRefs > 0 && uiLimit--)
    {
        RemoveRef();
    }

    // Did it work?
    if (pModelInfoSAInterface->usNumberOfRefs > 0 || pModelInfoSAInterface->pRwObject != NULL)
        return false;

    // If success, then remove txd
    ushort usTxdId = GetTextureDictionaryID();
    if (usTxdId)
        pGame->GetRenderWare()->TxdForceUnload(usTxdId, true);

    return true;
}
