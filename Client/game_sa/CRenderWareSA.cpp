/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.cpp
 *  PURPOSE:     RenderWare mapping to Grand Theft Auto: San Andreas
 *               and miscellaneous rendering functions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#include "StdInc.h"
#include <array>
#include <CMatrix.h>
#include <core/CCoreInterface.h>
#define RWFUNC_IMPLEMENT
#include <game/RenderWareD3D.h>
#include "CColModelSA.h"
#include "CFileLoaderSA.h"
#include "CGameSA.h"
#include "CRenderWareSA.h"
#include "CRenderWareSA.ShaderMatching.h"
#include "gamesa_renderware.h"
#include "gamesa_renderware.hpp"

extern CCoreInterface* g_pCore;
extern CGameSA*        pGame;

// RwFrameForAllObjects struct and callback used to replace dynamic vehicle parts
struct SReplaceParts
{
    const char*        szName;                    // name of the part you want to replace (e.g. 'door_lf' or 'door_rf')
    unsigned char      ucIndex;                   // index counter for internal usage (0 is the 'ok' part model, 1 is the 'dam' part model)
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static RwObject* ReplacePartsCB(RwObject* object, SReplaceParts* data)
{
    if (!object) [[unlikely]]
        return object;
    
    RpAtomic* Atomic = (RpAtomic*)object;
    char      szAtomicName[16] = {0};

    // iterate through our loaded atomics
    for (unsigned int i = 0; i < data->uiReplacements; i++)
    {
        // get the correct atomic name based on the object # in our parent frame
        if (data->ucIndex == 0)
            snprintf(&szAtomicName[0], 16, "%s_ok", data->szName);
        else
            snprintf(&szAtomicName[0], 16, "%s_dam", data->szName);

        // see if we have such an atomic in our replacement atomics array
        if (strncmp(&data->pReplacements[i].szName[0], &szAtomicName[0], 16) == 0)
        {
            // if so, override the geometry
            RpAtomicSetGeometry(Atomic, data->pReplacements[i].atomic->geometry, 0);

            // and copy the matrices
            RwFrameCopyMatrix(RpGetFrame(Atomic), RpGetFrame(data->pReplacements[i].atomic));

            object = (RwObject*)data->pReplacements[i].atomic;
            data->ucIndex++;
        }
    }

    return object;
}

// RpClumpForAllAtomics callback used to add atomics to a vehicle
static bool AddAllAtomicsCB(RpAtomic* atomic, void* pClump)
{
    if (!atomic || !pClump) [[unlikely]]
        return false;
    
    RpClump* data = reinterpret_cast<RpClump*>(pClump);
    RwFrame* pFrame = RpGetFrame(data);

    // add the atomic to the frame
    RpAtomicSetFrame(atomic, pFrame);
    RpClumpAddAtomic(data, atomic);

    return true;
}

// RpClumpForAllAtomics struct and callback used to replace all wheels with a given wheel model
struct SReplaceWheels
{
    const char*        szName;                    // name of the new wheel model
    RpClump*           pClump;                    // the vehicle's clump
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool ReplaceWheelsCB(RpAtomic* atomic, void* pData)
{
    if (!atomic || !pData) [[unlikely]]
        return false;
    
    SReplaceWheels* data = reinterpret_cast<SReplaceWheels*>(pData);
    RwFrame*        Frame = RpGetFrame(atomic);

    // find our wheel atomics
    if (strncmp(&Frame->szName[0], "wheel_lf_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rf_dummy", 16) == 0 ||
        strncmp(&Frame->szName[0], "wheel_lm_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rm_dummy", 16) == 0 ||
        strncmp(&Frame->szName[0], "wheel_lb_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rb_dummy", 16) == 0)
    {
        // find a replacement atomic
        for (unsigned int i = 0; i < data->uiReplacements; i++)
        {
            // see if it's name is equal to the specified wheel
            if (strncmp(&data->pReplacements[i].szName[0], data->szName, 16) == 0)
            {
                // clone our wheel atomic
                RpAtomic* WheelAtomic = RpAtomicClone(data->pReplacements[i].atomic);
                RpAtomicSetFrame(WheelAtomic, Frame);

                // add it to the clump
                RpClumpAddAtomic(data->pClump, WheelAtomic);

                // delete the current atomic
                RpClumpRemoveAtomic(data->pClump, atomic);
                RpAtomicDestroy(atomic);  // Destroy removed atomic to prevent leak
            }
        }
    }

    return true;
}

// RpClumpForAllAtomics struct and callback used to replace all atomics for a vehicle
struct SReplaceAll
{
    RpClump*           pClump;                    // the vehicle's clump
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool ReplaceAllCB(RpAtomic* atomic, void* pData)
{
    if (!atomic || !pData) [[unlikely]]
        return false;
    
    SReplaceAll* data = reinterpret_cast<SReplaceAll*>(pData);
    RwFrame*     Frame = RpGetFrame(atomic);
    if (Frame == NULL)
        return true;

    // find a replacement atomic
    for (unsigned int i = 0; i < data->uiReplacements; i++)
    {
        // see if we can find an atomic with the same name
        if (strncmp(&data->pReplacements[i].szName[0], &Frame->szName[0], 16) == 0)
        {
            // copy the matrices
            RwFrameCopyMatrix(RpGetFrame(atomic), RpGetFrame(data->pReplacements[i].atomic));

            // set the new atomic's frame to the current one
            RpAtomicSetFrame(data->pReplacements[i].atomic, Frame);

            // override all engine and material related callbacks and pointers
            data->pReplacements[i].atomic->renderCallback = atomic->renderCallback;
            data->pReplacements[i].atomic->frame = atomic->frame;
            data->pReplacements[i].atomic->render = atomic->render;
            data->pReplacements[i].atomic->interpolator = atomic->interpolator;
            data->pReplacements[i].atomic->info = atomic->info;

            // add the new atomic to the vehicle clump
            RpClumpAddAtomic(data->pClump, data->pReplacements[i].atomic);

            // remove the current atomic
            RpClumpRemoveAtomic(data->pClump, atomic);
            RpAtomicDestroy(atomic);  // Destroy removed atomic to prevent leak
        }
    }

    return true;
}

// RpClumpForAllAtomics struct and callback used to load the atomics from a specific clump into a container
// Stores atomic pointers without transferring ownership - caller must manage lifetime
struct SLoadAtomics
{
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool LoadAtomicsCB(RpAtomic* atomic, void* pData)
{
    if (!atomic || !pData) [[unlikely]]
        return false;
    
    SLoadAtomics* data = reinterpret_cast<SLoadAtomics*>(pData);
    RwFrame*      Frame = RpGetFrame(atomic);

    if (!Frame) [[unlikely]]
        return false;

    // Add atomic to container
    data->pReplacements[data->uiReplacements].atomic = atomic;
    strncpy(&data->pReplacements[data->uiReplacements].szName[0], &Frame->szName[0], 16);

    // and increment the counter
    data->uiReplacements++;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CRenderWareSA
//
//
/////////////////////////////////////////////////////////////////////////////
CRenderWareSA::CRenderWareSA()
{
    InitRwFunctions();

    InitTextureWatchHooks();
    m_pMatchChannelManager = new CMatchChannelManager();
    m_iRenderingEntityType = TYPE_MASK_WORLD;
    m_GTAVertexShadersDisabledTimer.SetMaxIncrement(1000, true);
    m_bGTAVertexShadersEnabled = true;
}

CRenderWareSA::~CRenderWareSA()
{
    SAFE_DELETE(m_pMatchChannelManager);
}

// Reads and parses a TXD file specified by a path (szTXD)
RwTexDictionary* CRenderWareSA::ReadTXD(const SString& strFilename, const SString& buffer)
{
    // open the stream
    RwStream* streamTexture;
    RwBuffer  streamBuffer;
    if (!buffer.empty())
    {
        streamBuffer.ptr = (void*)buffer.data();
        streamBuffer.size = buffer.size();
        streamTexture = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &streamBuffer);
    }
    else
        streamTexture = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_READ, *strFilename);

    // check for errors
    if (streamTexture == NULL)
        return NULL;

    // TXD header id: 0x16
    // find our txd chunk (dff loads textures, so correct loading order is: txd, dff)
    if (RwStreamFindChunk(streamTexture, 0x16, NULL, NULL) == false)
    {
        RwStreamClose(streamTexture, NULL);
        return NULL;
    }

    // read the texture dictionary from our model (txd)
    RwTexDictionary* pTex = RwTexDictionaryGtaStreamRead(streamTexture);

    // close the stream
    RwStreamClose(streamTexture, NULL);

    ScriptAddedTxd(pTex);

    return pTex;
}

// Reads and parses a DFF file specified by a path (szDFF) into a CModelInfo identified by the object id (usModelID)
// bLoadEmbeddedCollisions should be true for vehicles
// Any custom TXD should be imported before this call
RpClump* CRenderWareSA::ReadDFF(const SString& strFilename, const SString& buffer, unsigned short usModelID, bool bLoadEmbeddedCollisions)
{
    // Set correct TXD as materials are processed at the same time
    if (usModelID != 0)
    {
        unsigned short usTxdId = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
        SetTextureDict(usTxdId);
    }

    // open the stream
    RwStream* streamModel;
    RwBuffer  streamBuffer;
    if (!buffer.empty())
    {
        streamBuffer.ptr = (void*)buffer.data();
        streamBuffer.size = buffer.size();
        streamModel = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &streamBuffer);
    }
    else
        streamModel = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_READ, *strFilename);

    // get the modelinfo array
    DWORD* pPool = (DWORD*)ARRAY_ModelInfo;

    // check for errors
    if (streamModel == NULL)
        return NULL;

    // DFF header id: 0x10
    // find our dff chunk
    if (RwStreamFindChunk(streamModel, 0x10, NULL, NULL) == false)
    {
        RwStreamClose(streamModel, NULL);
        return NULL;
    }

    if (bLoadEmbeddedCollisions)
    {
        // Vehicles have their collision loaded through the CollisionModel plugin, so we need to remove the current collision to prevent a memory leak.
        // This needs to be done here before reading the stream data, because plugins are read in RpClumpStreamRead.
        CModelInfo* modelInfo = pGame->GetModelInfo(usModelID);
        if (modelInfo)
        {
            if (auto* modelInfoInterface = modelInfo->GetInterface())
                ((void(__thiscall*)(CBaseModelInfoSAInterface*))0x4C4C40)(modelInfoInterface); // CBaseModelInfo::DeleteCollisionModel
        }

        // rockstar's collision hack
        // It sets the pointer CCollisionPlugin::ms_currentModel to the model info of the given vehicle in order to correctly set up the vehicle’s
        // collision during collision plugin reading (0x41B2BD).
        RpPrtStdGlobalDataSetStreamEmbedded((void*)pPool[usModelID]);

        // Call CVehicleModelInfo::UseCommonVehicleTexDicationary
        ((void(__cdecl*)())0x4C75A0)();
    }

    // read the clump with all its extensions
    RpClump* pClump = RpClumpStreamRead(streamModel);

    if (bLoadEmbeddedCollisions)
    {
        // reset collision hack
        RpPrtStdGlobalDataSetStreamEmbedded(nullptr);

        // Call CVehicleModelInfo::StopUsingCommonVehicleTexDicationary
        ((void(__cdecl*)())0x4C75C0)();
    }

    // close the stream
    RwStreamClose(streamModel, NULL);

    return pClump;
}

//
// Returns list of atomics inside a clump
//
void CRenderWareSA::GetClumpAtomicList(RpClump* pClump, std::vector<RpAtomic*>& outAtomicList)
{
    RpClumpForAllAtomics(
        pClump,
        [](RpAtomic* pAtomic, void* pData) {
            reinterpret_cast<std::vector<RpAtomic*>*>(pData)->push_back(pAtomic);
            return true;
        },
        &outAtomicList);
}

//
// Returns true if the clump geometry sort of matches
//
// ClumpA vs ClumpB(or)AtomicB
//
bool CRenderWareSA::DoContainTheSameGeometry(RpClump* pClumpA, RpClump* pClumpB, RpAtomic* pAtomicB)
{
    // Fast check if comparing one atomic
    if (pAtomicB)
    {
        RpGeometry* pGeometryA = ((RpAtomic*)((pClumpA->atomics.root.next) - 0x8))->geometry;
        RpGeometry* pGeometryB = pAtomicB->geometry;
        return pGeometryA == pGeometryB;
    }

    // Get atomic list from both sides
    std::vector<RpAtomic*> atomicListA;
    std::vector<RpAtomic*> atomicListB;
    GetClumpAtomicList(pClumpA, atomicListA);
    if (pClumpB)
        GetClumpAtomicList(pClumpB, atomicListB);
    if (pAtomicB)
        atomicListB.push_back(pAtomicB);

    // Count geometries that exist in both sides
    std::set<RpGeometry*> geometryListA;
    for (uint i = 0; i < atomicListA.size(); i++)
        MapInsert(geometryListA, atomicListA[i]->geometry);

    uint uiInBoth = 0;
    for (uint i = 0; i < atomicListB.size(); i++)
        if (MapContains(geometryListA, atomicListB[i]->geometry))
            uiInBoth++;

    // If less than 50% match then assume it is not the same
    if (uiInBoth * 2 < atomicListB.size() || atomicListB.size() == 0)
        return false;

    return true;
}

// Replaces a vehicle/weapon/ped model
bool CRenderWareSA::ReplaceModel(RpClump* pNew, unsigned short usModelID, DWORD dwSetClumpFunction)
{
    auto CVehicleModelInfo_CVehicleStructure_Destructor = (void(__thiscall*)(CVehicleModelVisualInfoSAInterface * pThis))0x4C7410;
    auto CVehicleModelInfo_CVehicleStructure_release = (void(__cdecl*)(CVehicleModelVisualInfoSAInterface * pThis))0x4C9580;
    auto CBaseModelInfo_SetClump = (void(__thiscall*)(CBaseModelInfoSAInterface * pThis, RpClump * clump)) dwSetClumpFunction;

    CModelInfo* pModelInfo = pGame->GetModelInfo(usModelID);
    if (pModelInfo)
    {
        RpClump* pOldClump = (RpClump*)pModelInfo->GetRwObject();
        if (pOldClump != pNew && !DoContainTheSameGeometry(pNew, pOldClump, NULL))
        {
            if (pModelInfo->IsVehicle())
            {
                pModelInfo->ResetSupportedUpgrades();
            }

            // Make new clump container for the model geometry
            // Clone twice as the geometry render order seems to be reversed each time it is cloned.
            RpClump* pTemp = RpClumpClone(pNew);
            RpClump* pNewClone = RpClumpClone(pTemp);
            RpClumpDestroy(pTemp);

            // Calling CVehicleModelInfo::SetClump() allocates a new CVehicleStructure.
            // So let's delete the old one first to avoid CPool<CVehicleStructure> depletion.
            if (dwSetClumpFunction == FUNC_LoadVehicleModel)
            {
                auto pVehicleModelInfoInterface = (CVehicleModelInfoSAInterface*)pModelInfo->GetInterface();
                if (pVehicleModelInfoInterface->pVisualInfo)
                {
                    auto pVisualInfo = pVehicleModelInfoInterface->pVisualInfo;
                    CVehicleModelInfo_CVehicleStructure_Destructor(pVisualInfo);
                    CVehicleModelInfo_CVehicleStructure_release(pVisualInfo);
                    pVehicleModelInfoInterface->pVisualInfo = nullptr;
                }
            }

            CBaseModelInfoSAInterface* pModelInfoInterface = pModelInfo->GetInterface();
            CBaseModelInfo_SetClump(pModelInfoInterface, pNewClone);
            
            // Re-fetch interface pointer after SetClump (may relocate/change)
            pModelInfoInterface = pModelInfo->GetInterface();
            
            // Fix for custom DFF without embedded collision:
            // SetClump clears pColModel when DFF has no collision data, but vehicles need collision from .col pool.
            // Solution: Remove + Request + Load to restore pool-managed collision from data/vehicles.col
            if (dwSetClumpFunction == FUNC_LoadVehicleModel && !pModelInfoInterface->pColModel)
            {
                pGame->GetStreaming()->RemoveModel(usModelID);
                pGame->GetStreaming()->RequestModel(usModelID, 0x16);
                pGame->GetStreaming()->LoadAllRequestedModels(false, "CRenderWareSA::ReplaceVehicleModel");
                // Re-fetch interface pointer after model reload
                pModelInfoInterface = pModelInfo->GetInterface();
            }
            
            RpClumpDestroy(pOldClump);
        }
    }

    return true;
}

// Replaces a vehicle model
bool CRenderWareSA::ReplaceVehicleModel(RpClump* pNew, unsigned short usModelID)
{
    return ReplaceModel(pNew, usModelID, FUNC_LoadVehicleModel);
}

// Replaces a weapon model
bool CRenderWareSA::ReplaceWeaponModel(RpClump* pNew, unsigned short usModelID)
{
    return ReplaceModel(pNew, usModelID, FUNC_LoadWeaponModel);
}

// Replaces a ped model
bool CRenderWareSA::ReplacePedModel(RpClump* pNew, unsigned short usModelID)
{
    // NOTE(botder): The game logic requires the animation hierarchy to be present (read: it's not a corrupt model),
    // otherwise it will crash (offset 0x3c51a8).
    if (!GetAnimHierarchyFromClump(pNew))
    {
        LogEvent(851, "Model not replaced", "CRenderWareSA::ReplacePedModel", SString("No anim hierarchy for ped model:%d", usModelID), 5421);
        return false;
    }

    return ReplaceModel(pNew, usModelID, FUNC_LoadPedModel);
}

// Reads and parses a COL file (versions 1-4: COLL, COL2, COL3, COL4)
CColModel* CRenderWareSA::ReadCOL(const SString& buffer)
{
    // Validate minimum buffer size
    if (buffer.size() < sizeof(ColModelFileHeader) + 16) [[unlikely]]
        return nullptr;

    const auto& header = *reinterpret_cast<const ColModelFileHeader*>(buffer.data());

    // Validate version field contains valid COL magic number
    // Version is 4-char fixed string (not null-terminated): "COLL", "COL2", "COL3", "COL4"
    constexpr std::array<std::array<char, 4>, 4> validVersions = {{
        {'C', 'O', 'L', 'L'},
        {'C', 'O', 'L', '2'},
        {'C', 'O', 'L', '3'},
        {'C', 'O', 'L', '4'}
    }};
    
    const bool isValidVersion = std::any_of(validVersions.begin(), validVersions.end(),
        [&header](const auto& valid) { 
            return std::equal(valid.begin(), valid.end(), header.version);
        });

    if (!isValidVersion) [[unlikely]]
    {
        // Explicitly limit to 4 characters
        AddReportLog(8622, SString("ReadCOL: Invalid version '%c%c%c%c' - expected COLL, COL2, COL3, or COL4",
            header.version[0], header.version[1], header.version[2], header.version[3]));
        return nullptr;
    }

    // Ensure name field is null-terminated to prevent buffer overrun
    const auto* nameEnd = static_cast<const char*>(std::memchr(header.name, '\0', sizeof(header.name)));
    if (!nameEnd) [[unlikely]]
    {
        AddReportLog(8623, "ReadCOL: Name field not null-terminated, may be truncated");
        return nullptr;
    }

    // Buffer is not modified by us, but GTA's functions expect non-const
    auto* pModelData = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(buffer.data())) + sizeof(ColModelFileHeader);

    // Create a new CColModel
    auto* pColModel = new CColModelSA();

    // Load appropriate collision version
    switch (header.version[3])
    {
        case 'L':
            LoadCollisionModel(pModelData, pColModel->GetInterface(), nullptr);
            break;
        case '2':
            LoadCollisionModelVer2(pModelData, header.size - 0x18, pColModel->GetInterface(), nullptr);
            break;
        case '3':
            LoadCollisionModelVer3(pModelData, header.size - 0x18, pColModel->GetInterface(), nullptr);
            break;
        case '4':
            // COL4 format has same structure as COL3 with one extra uint32 field in header
            // Must use Ver4 loader for correct offset calculations
            LoadCollisionModelVer4(pModelData, header.size - 0x18, pColModel->GetInterface(), nullptr);
            break;
        default:
            // Should never reach here due to validation above
            delete pColModel;
            return nullptr;
    }

    return pColModel;
}

// Loads all atomics from a clump into a container struct and returns the number of atomics it loaded
unsigned int CRenderWareSA::LoadAtomics(RpClump* pClump, RpAtomicContainer* pAtomics)
{
    if (!pClump || !pAtomics) [[unlikely]]
        return 0;
    
    // iterate through all atomics in the clump
    SLoadAtomics data = {0};
    data.pReplacements = pAtomics;
    RpClumpForAllAtomics(pClump, LoadAtomicsCB, &data);

    // return the number of atomics that were added to the container
    return data.uiReplacements;
}

// Replaces all atomics for a specific model
typedef struct
{
    unsigned short usTxdID;
    RpClump*       pClump;
} SAtomicsReplacer;
bool AtomicsReplacer(RpAtomic* pAtomic, void* data)
{
    SAtomicsReplacer* pData = reinterpret_cast<SAtomicsReplacer*>(data);
    SRelatedModelInfo relatedModelInfo = {0};
    relatedModelInfo.pClump = pData->pClump;
    relatedModelInfo.bDeleteOldRwObject = true;
    CFileLoader_SetRelatedModelInfoCB(pAtomic, &relatedModelInfo);

    // The above function adds a reference to the model's TXD by either
    // calling CAtomicModelInfo::SetAtomic or CDamagableModelInfo::SetDamagedAtomic. Remove it again.
    CTxdStore_RemoveRef(pData->usTxdID);
    return true;
}

bool CRenderWareSA::ReplaceAllAtomicsInModel(RpClump* pNew, unsigned short usModelID)
{
    if (!pNew) [[unlikely]]
        return false;
    
    CModelInfo* pModelInfo = pGame->GetModelInfo(usModelID);

    if (pModelInfo)
    {
        RpAtomic* pOldAtomic = (RpAtomic*)pModelInfo->GetRwObject();

        if (reinterpret_cast<RpClump*>(pOldAtomic) != pNew && !DoContainTheSameGeometry(pNew, NULL, pOldAtomic))
        {
            // Clone the clump that's to be replaced (FUNC_AtomicsReplacer removes the atomics from the source clump)
            RpClump* pCopy = RpClumpClone(pNew);
            if (!pCopy) [[unlikely]]
            {
                AddReportLog(8624, SString("ReplaceAllAtomicsInModel: RpClumpClone failed for model %d", usModelID));
                return false;
            }

            // Replace the atomics
            SAtomicsReplacer data;
            data.usTxdID = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
            data.pClump = pCopy;

            MemPutFast<DWORD>((DWORD*)DWORD_AtomicsReplacerModelID, usModelID);
            RpClumpForAllAtomics(pCopy, AtomicsReplacer, &data);

            // Get rid of the now empty copied clump
            RpClumpDestroy(pCopy);
        }
    }

    return true;
}

// Replaces all atomics in a vehicle
void CRenderWareSA::ReplaceAllAtomicsInClump(RpClump* pDst, RpAtomicContainer* pAtomics, unsigned int uiAtomics)
{
    SReplaceAll data;
    data.pClump = pDst;
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    RpClumpForAllAtomics(pDst, ReplaceAllCB, &data);
}

// Replaces the wheels in a vehicle
void CRenderWareSA::ReplaceWheels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szWheel)
{
    // get the clump's frame
    RwFrame* pFrame = RpGetFrame(pClump);

    SReplaceWheels data;
    data.pClump = pClump;
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    data.szName = szWheel;
    RpClumpForAllAtomics(pClump, ReplaceWheelsCB, &data);
}

// Repositions an atomic
void CRenderWareSA::RepositionAtomic(RpClump* pDst, RpClump* pSrc, const char* szName)
{
    RwFrame* pDstFrame = RpGetFrame(pDst);
    RwFrame* pSrcFrame = RpGetFrame(pSrc);
    RwFrameCopyMatrix(RwFrameFindFrame(pDstFrame, szName), RwFrameFindFrame(pSrcFrame, szName));
}

// Adds the atomics from a source clump (pSrc) to a destination clump (pDst)
void CRenderWareSA::AddAllAtomics(RpClump* pDst, RpClump* pSrc)
{
    RpClumpForAllAtomics(pSrc, AddAllAtomicsCB, pDst);
}

// Replaces dynamic parts of the vehicle (models that have two different versions: 'ok' and 'dam'), such as doors
// szName should be without the part suffix (e.g. 'door_lf' or 'door_rf', and not 'door_lf_dummy')
bool CRenderWareSA::ReplacePartModels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szName)
{
    // get the part's dummy name
    char szDummyName[16] = {0};
    snprintf(&szDummyName[0], 16, "%s_dummy", szName);

    // get the clump's frame
    RwFrame* pFrame = RpGetFrame(pClump);

    // get the part's dummy frame
    RwFrame* pPart = RwFrameFindFrame(pFrame, &szDummyName[0]);
    if (pPart == NULL)
        return false;

    // now replace all the objects in the frame
    SReplaceParts data = {0};
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    data.szName = szName;
    RwFrameForAllObjects(pPart, ReplacePartsCB, &data);

    return true;
}

// Replaces a CColModel for a specific object identified by the object id (usModelID)
void CRenderWareSA::ReplaceCollisions(CColModel* pCol, unsigned short usModelID)
{
    DWORD*        pPool = (DWORD*)ARRAY_ModelInfo;
    CColModelSA*  pColModel = (CColModelSA*)pCol;
    CModelInfoSA* pModelInfoSA = (CModelInfoSA*)(pGame->GetModelInfo(usModelID));

    // Apply some low-level hacks (copies the old col area and sets a flag)
    DWORD pColModelInterface = (DWORD)pColModel->GetInterface();
    DWORD pOldColModelInterface = *((DWORD*)pPool[usModelID] + 20);
    MemOrFast<BYTE>(pPool[usModelID] + 0x13, 8);
    MemPutFast<BYTE>(pColModelInterface + 40, *((BYTE*)(pOldColModelInterface + 40)));

    // TODO: It seems that on entering the game, when this function is executed, the modelinfo array for this
    // model is still zero, leading to a crash!
    [[maybe_unused]] const bool modelLoaded = pModelInfoSA->IsLoaded();
}

// Destroys a DFF instance
void CRenderWareSA::DestroyDFF(RpClump* pClump)
{
    if (pClump)
        RpClumpDestroy(pClump);
}

// Destroys a TXD instance
void CRenderWareSA::DestroyTXD(RwTexDictionary* pTXD)
{
    if (pTXD)
        RwTexDictionaryDestroy(pTXD);
}

// Destroys a texture instance
void CRenderWareSA::DestroyTexture(RwTexture* pTex)
{
    if (pTex)
    {
        ScriptRemovedTexture(pTex);
        RwTextureDestroy(pTex);
    }
}

void CRenderWareSA::RwTexDictionaryRemoveTexture(RwTexDictionary* pTXD, RwTexture* pTex)
{
    if (!pTex || !pTXD)
        return;
        
    if (pTex->txd != pTXD)
        return;

    pTex->TXDList.next->prev = pTex->TXDList.prev;
    pTex->TXDList.prev->next = pTex->TXDList.next;
    pTex->txd = NULL;
}

short CRenderWareSA::CTxdStore_GetTxdRefcount(unsigned short usTxdID)
{
    return *(short*)(*(*(DWORD**)0xC8800C) + 0xC * usTxdID + 4);
}

bool CRenderWareSA::RwTexDictionaryContainsTexture(RwTexDictionary* pTXD, RwTexture* pTex)
{
    // Avoid crashes with freed/invalid textures and TXDs
    if (!pTex || !pTXD)
        return false;

    // Prevent crash when texture/TXD has been freed but pointer still exists
    if (!SharedUtil::IsReadablePointer(pTex, sizeof(RwTexture)))
        return false;
    
    if (!SharedUtil::IsReadablePointer(pTXD, sizeof(RwTexDictionary)))
        return false;
    
    return pTex->txd == pTXD;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::TxdForceUnload
//
// 1. Unload txd from memory
// 2. Cross fingers
//
// Why do we do this?
// Player model adds (seemingly) unnecessary refs
// (Will crash if anything is actually using the txd)
//
// Handles custom txd replacements
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::TxdForceUnload(ushort usTxdId, bool bDestroyTextures)
{
    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);
    if (!pTxd)
        return;

    // We can abandon the textures instead of destroy. It might be safer, but will cause a memory leak
    if (bDestroyTextures)
    {
        // Unref the textures
        std::vector<RwTexture*> textureList;
        pGame->GetRenderWareSA()->GetTxdTextures(textureList, pTxd);
        for (std::vector<RwTexture*>::iterator iter = textureList.begin(); iter != textureList.end(); iter++)
        {
            RwTexture* pTexture = *iter;
            while (pTexture->refs > 1)
                RwTextureDestroy(pTexture);
            RwTextureDestroy(pTexture);
        }
    }

    // Need to have at least one ref for RemoveRef to work correctly
    if (CTxdStore_GetNumRefs(usTxdId) == 0)
    {
        CTxdStore_AddRef(usTxdId);
    }

    while (CTxdStore_GetNumRefs(usTxdId) > 0)
    {
        CTxdStore_RemoveRef(usTxdId);
    }
}

namespace
{
    struct TextureMapping
    {
        const char* externalName;
        const char* internalName;
    };

    constexpr std::array<TextureMapping, 2> kTextureMappings = {{
        {"remap", "#emap"},
        {"white", "@hite"}
    }};
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetInternalTextureName
//
// Maps external texture names (e.g. "remap") to internal GTA:SA names (e.g. "#emap")
// Returns original name if no mapping exists
//
////////////////////////////////////////////////////////////////
const char* CRenderWareSA::GetInternalTextureName(const char* szExternalName)
{
    if (!szExternalName)
        return nullptr;

    for (const auto& mapping : kTextureMappings)
    {
        if (_stricmp(szExternalName, mapping.externalName) == 0)
            return mapping.internalName;
    }
    return szExternalName;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetExternalTextureName
//
// Maps internal GTA:SA names (e.g. "#emap") to external texture names (e.g. "remap")
// Returns original name if no mapping exists
//
////////////////////////////////////////////////////////////////
const char* CRenderWareSA::GetExternalTextureName(const char* szInternalName)
{
    if (!szInternalName)
        return nullptr;

    for (const auto& mapping : kTextureMappings)
    {
        if (_stricmp(szInternalName, mapping.internalName) == 0)
            return mapping.externalName;
    }
    return szInternalName;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTXDIDForModelID
//
// Get a TXD ID associated with the model ID
//
////////////////////////////////////////////////////////////////
ushort CRenderWareSA::GetTXDIDForModelID(ushort usModelID)
{
    if (usModelID >= pGame->GetBaseIDforTXD() && usModelID < pGame->GetBaseIDforCOL())
    {
        // Get global TXD ID instead
        return usModelID - pGame->GetBaseIDforTXD();
    }
    else
    {
        // Get the CModelInfo's TXD ID

        // Ensure valid
        if (usModelID >= pGame->GetBaseIDforTXD() || !((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID])
            return 0;

        return ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTextureNames
//
// Get list of texture names associated with the model
// Will try to load the model if needed
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetModelTextureNames(std::vector<SString>& outNameList, ushort usModelId)
{
    outNameList.clear();

    // Special case for CJ
    if (usModelId == 0)
    {
        outNameList.push_back("CJ");
        return;
    }

    ushort usTxdId = GetTXDIDForModelID(usModelId);

    if (usTxdId == 0)
        return;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd(usTxdId);

    bool bLoadedModel = false;
    if (!pTXD)
    {
        // Try model load
        bLoadedModel = true;
        pGame->GetModelInfo(usModelId)->Request(BLOCKING, "CRenderWareSA::GetModelTextureNames");
        pTXD = CTxdStore_GetTxd(usTxdId);
        
        // Revalidate TXD pointer after load - it may still be NULL or have been GC'd
        if (!pTXD)
        {
            if (bLoadedModel)
                ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
            return;
        }
    }

    std::vector<RwTexture*> textureList;
    GetTxdTextures(textureList, pTXD);

    for (RwTexture* pTexture : textureList)
    {
        // Fix for #emap corruption:
        // Some textures (like 'remap') are internally renamed to start with '#' (e.g. '#emap') by SA.
        // This causes issues when scripts try to access them by their original name.
        // We detect this case and return the expected name 'remap' instead.
        outNameList.push_back(GetExternalTextureName(pTexture->name));
    }

    if (bLoadedModel)
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTextures
//
// Get textures associated with the model
// Will try to load the model if needed
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::GetModelTextures(std::vector<std::tuple<std::string, CPixels>>& outTextureList, ushort usModelId, std::vector<SString> vTextureNames)
{
    outTextureList.clear();

    ushort usTxdId = GetTXDIDForModelID(usModelId);

    if (usTxdId == 0)
        return false;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd(usTxdId);

    bool bLoadedModel = false;
    if (!pTXD)
    {
        // Try model load
        bLoadedModel = true;
        pGame->GetModelInfo(usModelId)->Request(BLOCKING, "CRenderWareSA::GetModelTextures");
        pTXD = CTxdStore_GetTxd(usTxdId);
        
        // Revalidate TXD pointer after load - it may still be NULL or have been GC'd
        if (!pTXD)
        {
            if (bLoadedModel)
                ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
            return false;
        }
    }

    std::vector<RwTexture*> rwTextureList;
    GetTxdTextures(rwTextureList, pTXD);

    // If texture list is empty after enumeration
    if (rwTextureList.empty())
    {
        if (bLoadedModel)
            ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
        return false;
    }

    // If any texture names specified in vTextureNames, we should only return these
    const bool bExcludeTextures = !vTextureNames.empty();

    for (RwTexture* pTexture : rwTextureList)
    {
        SString strTextureName = GetExternalTextureName(pTexture->name);

        bool    bValidTexture = false;

        if (bExcludeTextures)
        {
            for (const auto& str : vTextureNames)
            {
                if (WildcardMatchI(strTextureName, str))
                {
                    bValidTexture = true;
                }
            }
        }
        else
            bValidTexture = true;

        if (bValidTexture)
        {
            RwD3D9Raster* pD3DRaster = (RwD3D9Raster*)(&pTexture->raster->renderResource);
            CPixels       texture;
            g_pCore->GetGraphics()->GetPixelsManager()->GetTexturePixels(pD3DRaster->texture, texture);
            outTextureList.emplace_back(strTextureName, std::move(texture));
        }
    }

    if (bLoadedModel)
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);

    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
// If TXD must already be loaded
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures(std::vector<RwTexture*>& outTextureList, ushort usTxdId)
{
    if (usTxdId == 0)
        return;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd(usTxdId);

    if (!pTXD)
        return;

    if (!SharedUtil::IsReadablePointer(pTXD, sizeof(*pTXD)))
        return;

    GetTxdTextures(outTextureList, pTXD);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
// Get textures from a TXD
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures(std::vector<RwTexture*>& outTextureList, RwTexDictionary* pTXD)
{
    if (!pTXD)
        return;

    // Validate TXD structure is readable (includes textures member)
    if (!SharedUtil::IsReadablePointer(pTXD, sizeof(*pTXD)))
        return;

    // Validate the linked list structure to prevent crash at 0x007F374A
    // The crash occurs when next pointer is invalid and gets deref'd during iteration
    RwListEntry* firstNode = pTXD->textures.root.next;
    if (!SharedUtil::IsReadablePointer(firstNode, sizeof(RwListEntry)))
        return;

    // Check for empty list (next points back to root - valid case)
    if (firstNode == &pTXD->textures.root)
        return;  // Empty TXD is valid

    // Validate the first texture node structure
    // The texture pointer is at (node - offsetof(RwTexture, TXDList))
    // which is (node - 8) since TXDList is at offset 8 in RwTexture
    RwTexture* firstTexture = (RwTexture*)((char*)firstNode - 8);
    if (!SharedUtil::IsReadablePointer(firstTexture, sizeof(RwTexture)))
        return;

    // Validate that first node's next pointer is also readable
    // This catches most corruption cases where the list is broken
    if (!SharedUtil::IsReadablePointer(firstNode->next, sizeof(RwListEntry)))
        return;

    constexpr std::size_t kMaxReasonableTextures = 8192;
    if (outTextureList.size() >= kMaxReasonableTextures)
    {
        LogEvent(852, "Texture enumeration aborted", "CRenderWareSA::GetTxdTextures",
                 SString("Texture list already contains %zu textures (limit: %zu)", outTextureList.size(), kMaxReasonableTextures), 5422);
        return;
    }

    if (outTextureList.empty())
        outTextureList.reserve(16);

    RwTexDictionaryForAllTextures(pTXD, StaticGetTextureCB, &outTextureList);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StaticGetTextureCB
//
// Callback used in GetTxdTextures
// Returns false to stop enumeration if limits are hit
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::StaticGetTextureCB(RwTexture* texture, std::vector<RwTexture*>* pTextureList)
{
    // Fast null check before any heavy validation
    if (!texture || !pTextureList)
        return false;

    // Prevent excessive allocations from corrupted TXDs
    constexpr std::size_t kMaxReasonableTextures = 8192;
    if (pTextureList->size() >= kMaxReasonableTextures)
        return false;            // Stop enumeration

    // Note: We don't validate readability here for performance reasons.
    // The upfront validation in GetTxdTextures catches the first two nodes,
    // and the SA function crashes before calling this callback if the
    // linked list is corrupted mid-iteration. If we reach here with a bad
    // pointer, we'll crash in push_back, but that's acceptable vs the cost
    // of VirtualQuery on every texture in every TXD.
    
    pTextureList->push_back(texture);
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTextureName
//
// Only called by CRenderItemManager::GetVisibleTextureNames ?
//
////////////////////////////////////////////////////////////////
const char* CRenderWareSA::GetTextureName(CD3DDUMMY* pD3DData)
{
    STexInfo** ppTexInfo = MapFind(m_D3DDataTexInfoMap, pD3DData);
    if (ppTexInfo)
        return (*ppTexInfo)->strTextureName;
    if (!pD3DData)
        return FAKE_NAME_NO_TEXTURE;
    return "";
}

//
// CFastHashMap functions
//
CD3DDUMMY* GetEmptyMapKey(CD3DDUMMY**)
{
    return FAKE_D3DTEXTURE_EMPTY_KEY;
}

CD3DDUMMY* GetDeletedMapKey(CD3DDUMMY**)
{
    return FAKE_D3DTEXTURE_DELETED_KEY;
}

RwFrame* CRenderWareSA::GetFrameFromName(RpClump* pRoot, SString strName)
{
    return RwFrameFindFrame(RpGetFrame(pRoot), strName);
}

void CRenderWareSA::CMatrixToRwMatrix(const CMatrix& mat, RwMatrix& rwOutMatrix)
{
    rwOutMatrix.right = (RwV3d&)mat.vRight;
    rwOutMatrix.up = (RwV3d&)mat.vFront;
    rwOutMatrix.at = (RwV3d&)mat.vUp;
    rwOutMatrix.pos = (RwV3d&)mat.vPos;
}

void CRenderWareSA::RwMatrixToCMatrix(const RwMatrix& rwMatrix, CMatrix& matOut)
{
    matOut.vRight = (CVector&)rwMatrix.right;
    matOut.vFront = (CVector&)rwMatrix.up;
    matOut.vUp = (CVector&)rwMatrix.at;
    matOut.vPos = (CVector&)rwMatrix.pos;
}

void CRenderWareSA::RwMatrixGetRotation(const RwMatrix& rwMatrix, CVector& vecOutRotation)
{
    CMatrix matTemp;
    RwMatrixToCMatrix(rwMatrix, matTemp);
    vecOutRotation = matTemp.GetRotation();
}

void CRenderWareSA::RwMatrixSetRotation(RwMatrix& rwInOutMatrix, const CVector& vecRotation)
{
    CMatrix matTemp;
    RwMatrixToCMatrix(rwInOutMatrix, matTemp);
    matTemp.SetRotation(vecRotation);
    rwInOutMatrix.right = (RwV3d&)matTemp.vRight;
    rwInOutMatrix.up = (RwV3d&)matTemp.vFront;
    rwInOutMatrix.at = (RwV3d&)matTemp.vUp;
}

void CRenderWareSA::RwMatrixGetPosition(const RwMatrix& rwMatrix, CVector& vecOutPosition)
{
    vecOutPosition = (CVector&)rwMatrix.pos;
}

void CRenderWareSA::RwMatrixSetPosition(RwMatrix& rwInOutMatrix, const CVector& vecPosition)
{
    rwInOutMatrix.pos = (RwV3d&)vecPosition;
}

void CRenderWareSA::RwMatrixGetScale(const RwMatrix& rwMatrix, CVector& vecOutScale)
{
    CMatrix matTemp;
    RwMatrixToCMatrix(rwMatrix, matTemp);
    vecOutScale = matTemp.GetScale();
}

void CRenderWareSA::RwMatrixSetScale(RwMatrix& rwInOutMatrix, const CVector& vecScale)
{
    CMatrix matTemp;
    RwMatrixToCMatrix(rwInOutMatrix, matTemp);
    matTemp.SetScale(vecScale);
    rwInOutMatrix.right = (RwV3d&)matTemp.vRight;
    rwInOutMatrix.up = (RwV3d&)matTemp.vFront;
    rwInOutMatrix.at = (RwV3d&)matTemp.vUp;
}
