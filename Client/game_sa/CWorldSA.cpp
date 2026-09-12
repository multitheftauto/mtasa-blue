/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWorldSA.cpp
 *  PURPOSE:     Game world/entity logic
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <numbers>
#include <multiplayer/CMultiplayer.h>
#include <core/CCoreInterface.h>
#include "CGameSA.h"
#include "CPoolsSA.h"
#include "CWorldSA.h"
#include "CColModelSA.h"
#include "gamesa_renderware.h"
#include "CCollisionSA.h"

extern CCoreInterface* g_pCore;
extern CGameSA*        pGame;

#define ARRAY_SurfaceInfos 0xB79538

namespace
{
    extern const unsigned char aOriginalSurfaceInfo[2292];
}

CWorldSA::CWorldSA()
{
    m_pSurfaceInfo = reinterpret_cast<CSurfaceType*>(ARRAY_SurfaceInfos);

    InstallHooks();
}

CSurfaceType* CWorldSA::GetSurfaceInfo()
{
    return m_pSurfaceInfo;
}

void CWorldSA::ResetAllSurfaceInfo()
{
#if 0
    // Verification code for the embedded surface infos byte array:
    // GTA's surface infos are not embedded in the binary. You have to connect to a server
    // and then disconnect (the server MUST NOT modify the surface infos) to run the code below.
    // Checks if surface infos are byte-equal to the embedded original surface info bytes
    assert(memcmp(m_pSurfaceInfo, aOriginalSurfaceInfo, sizeof(CSurfaceType)) == 0);
#endif

    const auto iSurfaceInfo = reinterpret_cast<intptr_t>(m_pSurfaceInfo);
    const auto iOriginalSurfaceInfo = reinterpret_cast<intptr_t>(&aOriginalSurfaceInfo[0]);

    const size_t sOffset = offsetof(CSurfaceType, CSurfaceType::surfType);
    const size_t sSize = sizeof(CSurfaceType) - sOffset;

    memcpy((void*)(iSurfaceInfo + sOffset), (void*)(iOriginalSurfaceInfo + sOffset), sSize);
}

bool CWorldSA::ResetSurfaceInfo(short sSurfaceID)
{
    if (sSurfaceID >= EColSurfaceValue::DEFAULT && sSurfaceID <= EColSurfaceValue::SIZE)
    {
        const auto iSurfaceInfo = reinterpret_cast<intptr_t>(m_pSurfaceInfo);
        const auto iOriginalSurfaceInfo = reinterpret_cast<intptr_t>(&aOriginalSurfaceInfo[0]);

        const size_t sOffset = offsetof(CSurfaceType, CSurfaceType::surfType) + sizeof(SurfaceInfo_c) * sSurfaceID;
        const size_t sSize = sizeof(SurfaceInfo_c);

        memcpy((void*)(iSurfaceInfo + sOffset), (void*)(iOriginalSurfaceInfo + sOffset), sSize);
        return true;
    }

    return false;
}

DWORD CONTINUE_CWorld_FallenPeds = 0x00565CBA;
DWORD CONTINUE_CWorld_FallenCars = 0x00565E8A;

static bool IsUnderWorldWarpEnabled()
{
    return pGame && pGame->IsUnderWorldWarpEnabled();
}

static void __declspec(naked) HOOK_FallenPeds()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        call    IsUnderWorldWarpEnabled
        test    al, al
        jnz     continueWithOriginalCode
        ret

        continueWithOriginalCode:
        sub     esp, 2Ch
        push    ebx
        mov     ebx, ds:0B74490h
        jmp     CONTINUE_CWorld_FallenPeds
    }
    // clang-format on
}

static void __declspec(naked) HOOK_FallenCars()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        call    IsUnderWorldWarpEnabled
        test    al, al
        jnz     continueWithOriginalCode
        ret

        continueWithOriginalCode:
        sub     esp, 2Ch
        push    ebx
        mov     ebx, ds:0B74494h
        jmp     CONTINUE_CWorld_FallenCars
    }
    // clang-format on
}

void CWorldSA::InstallHooks()
{
    HookInstall(0x565CB0, (DWORD)HOOK_FallenPeds, 10);
    HookInstall(0x565E80, (DWORD)HOOK_FallenCars, 10);
}

void CWorldSA::Add(CEntity* pEntity, eDebugCaller CallerId)
{
    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(pEntity);

    if (pEntitySA)
    {
        CEntitySAInterface* pInterface = pEntitySA->GetInterface();
        if (pInterface->IsPlaceableVTBL())
        {
            SString strMessage("Caller: %i ", CallerId);
            LogEvent(506, "CWorld::Add ( CEntity * ) Crash", "", strMessage);
        }
        DWORD dwEntity = (DWORD)pEntitySA->GetInterface();
        DWORD dwFunction = FUNC_Add;
        // clang-format off
        __asm
        {
            push    dwEntity
            call    dwFunction
            add     esp, 4
        }
        // clang-format on
    }
}

void CWorldSA::Add(CEntitySAInterface* entityInterface, eDebugCaller CallerId)
{
    DWORD dwFunction = FUNC_Add;
    if (entityInterface->IsPlaceableVTBL())
    {
        SString strMessage("Caller: %i ", CallerId);
        LogEvent(506, "CWorld::Add ( CEntitySAInterface * ) Crash", "", strMessage);
    }
    // clang-format off
    __asm
    {
        push    entityInterface
        call    dwFunction
        add     esp, 4
    }
    // clang-format on
}

void CWorldSA::Remove(CEntity* pEntity, eDebugCaller CallerId)
{
    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(pEntity);

    if (pEntitySA)
    {
        CEntitySAInterface* pInterface = pEntitySA->GetInterface();
        if (pInterface->IsPlaceableVTBL())
        {
            SString strMessage("Caller: %i ", CallerId);
            LogEvent(507, "CWorld::Remove ( CEntity * ) Crash", "", strMessage);
        }
        DWORD dwEntity = (DWORD)pInterface;
        DWORD dwFunction = FUNC_Remove;
        // clang-format off
        __asm
        {
            push    dwEntity
            call    dwFunction
            add     esp, 4
        }
        // clang-format on
    }
}

void CWorldSA::Remove(CEntitySAInterface* entityInterface, eDebugCaller CallerId)
{
    if (entityInterface->IsPlaceableVTBL())
    {
        SString strMessage("Caller: %i ", CallerId);
        LogEvent(507, "CWorld::Remove ( CEntitySAInterface * ) Crash", "", strMessage);
    }
    DWORD dwFunction = FUNC_Remove;
    // clang-format off
    __asm
    {
        push    entityInterface
        call    dwFunction
        add     esp, 4

    /*  mov     ecx, entityInterface
        mov     esi, [ecx]
        push    1
        call    dword ptr [esi+8]*/
    }
    // clang-format on
}

void CWorldSA::RemoveReferencesToDeletedObject(CEntitySAInterface* entity)
{
    DWORD dwFunc = FUNC_RemoveReferencesToDeletedObject;
    DWORD dwEntity = (DWORD)entity;
    // clang-format off
    __asm
    {
        push    dwEntity
        call    dwFunc
        add     esp, 4
    }
    // clang-format on
}

void ConvertMatrixToEulerAngles(const CMatrix_Padded& matrixPadded, float& fX, float& fY, float& fZ)
{
    // Convert the given matrix to a padded matrix
    // CMatrix_Padded matrixPadded ( Matrix );

    // Grab its pointer and call gta's func
    const CMatrix_Padded* pMatrixPadded = &matrixPadded;
    DWORD                 dwFunc = FUNC_CMatrix__ConvertToEulerAngles;

    float* pfX = &fX;
    float* pfY = &fY;
    float* pfZ = &fZ;
    int    iUnknown = 21;
    // clang-format off
    __asm
    {
        push    iUnknown
            push    pfZ
            push    pfY
            push    pfX
            mov     ecx, pMatrixPadded
            call    dwFunc
    }
    // clang-format on
}

auto CWorldSA::ProcessLineAgainstMesh(CEntitySAInterface* targetEntity, CVector start, CVector end) -> SProcessLineOfSightMaterialInfoResult
{
    assert(targetEntity);

    SProcessLineOfSightMaterialInfoResult ret;

    struct Context
    {
        float               minHitDistSq{};          //< [squared] hit distance from the line segment's origin
        CVector             originOS, endOS, dirOS;  //< Line origin, end and dir [in object space]
        CMatrix             entMat, entInvMat;       //< The hit entity's matrix, and it's inverse
        RpTriangle*         hitTri{};                //< The triangle hit
        RpAtomic*           hitAtomic{};             //< The atomic of the hit triangle's geometry
        RpGeometry*         hitGeo{};                //< The geometry of the hit triangle
        CVector             hitBary{};               //< Barycentric coordinates [on the hit triangle] of the hit
        CVector             hitPosOS{};              //< Hit position in object space
        CEntitySAInterface* entity{};                //< The hit entity
    } c = {};

    c.entity = targetEntity;

    if (!c.entity->m_pRwObject)
    {
        return ret;  // isValid will be false in this case
    }

    // Get matrix, and it's inverse
    if (c.entity->matrix)
        c.entity->matrix->ConvertToMatrix(c.entMat);
    else
    {
        c.entMat.SetPosition(c.entity->m_transform.m_translate);
        c.entMat.SetRotation(CVector{0.0f, 0.0f, c.entity->m_transform.m_heading});
    }
    c.entInvMat = c.entMat.Inverse();

    // ...to transform the line origin and end into object space
    c.originOS = c.entInvMat.TransformVector(start);
    c.endOS = c.entInvMat.TransformVector(end);
    c.dirOS = c.endOS - c.originOS;
    c.minHitDistSq = c.dirOS.LengthSquared();  // By setting it to this value we avoid collisions that would be detected after the line segment
    // [but are still ont the ray]

    // Do raycast against the DFF to get hit position material UV and name
    // This is very slow
    // Perhaps we could parallelize it somehow? [OpenMP?]
    const auto ProcessOneAtomic = [](RpAtomic* a, void* data)
    {
        Context* const c = static_cast<Context*>(data);
        RwFrame* const f = RpAtomicGetFrame(a);

        const auto GetFrameCMatrix = [](RwFrame* f)
        {
            CMatrix out;
            pGame->GetRenderWare()->RwMatrixToCMatrix(*RwFrameGetMatrix(f), out);
            return out;
        };

        // Atomic not visible
        if (!a->renderCallback || !(a->object.object.flags & 0x04 /*rpATOMICRENDER*/))
        {
            return true;
        }

        // Sometimes atomics have no geometry [I don't think that should be possible, but okay]
        RpGeometry* const geo = a->geometry;
        if (!geo)
        {
            return true;
        }

        // Calculate transformation by traversing the hierarchy from the bottom (this frame) -> top (root frame)
        CMatrix localToObjTransform{};
        for (RwFrame* i = f; i && i != i->root; i = RwFrameGetParent(i))
        {
            localToObjTransform = GetFrameCMatrix(i) * localToObjTransform;
        }
        const CMatrix objToLocalTransform = localToObjTransform.Inverse();

        const auto ObjectToLocalSpace = [&](const CVector& in) { return objToLocalTransform.TransformVector(in); };

        // Transform from object space, into local (the frame's) space
        const CVector localOrigin = ObjectToLocalSpace(c->originOS);
        const CVector localEnd = ObjectToLocalSpace(c->endOS);

#if 0
            if (!CCollisionSA::TestLineSphere(
                CColLineSA{localOrigin, 0.f, localEnd, 0.f},
                reinterpret_cast<CColSphereSA&>(*RpAtomicGetBoundingSphere(a)) // Fine for now
            )) {
                return true; // Line segment doesn't touch bsp
            }
#endif
        const CVector localDir = localEnd - localOrigin;

        const CVector* const verts = reinterpret_cast<CVector*>(geo->morph_target->verts);  // It's fine, trust me bro
        for (auto i = geo->triangles_size; i-- > 0;)
        {
            RpTriangle* const tri = &geo->triangles[i];

            // Process the line against the triangle
            CVector hitBary, hitPos;
            if (!localOrigin.IntersectsSegmentTriangle(localDir, verts[tri->verts[0]], verts[tri->verts[1]], verts[tri->verts[2]], &hitPos, &hitBary))
            {
                continue;  // No intersection at all
            }

            // Intersection, check if it's closer than the previous one
            const float hitDistSq = (hitPos - localOrigin).LengthSquared();
            if (c->minHitDistSq > hitDistSq)
            {
                c->minHitDistSq = hitDistSq;
                c->hitGeo = geo;
                c->hitAtomic = a;
                c->hitTri = tri;
                c->hitBary = hitBary;
                c->hitPosOS = localToObjTransform.TransformVector(hitPos);  // Transform back into object space
            }
        }

        return true;
    };

    if (c.entity->m_pRwObject->object.type == 2 /*rpCLUMP*/)
    {
        RpClumpForAllAtomics(c.entity->m_pRwObject, ProcessOneAtomic, &c);
    }
    else
    {  // Object is a single atomic, so process directly
        ProcessOneAtomic(reinterpret_cast<RpAtomic*>(c.entity->m_pRwObject), &c);
    }

    if (ret.valid = c.hitGeo != nullptr)
    {
        // Now, calculate texture UV, etc based on the hit [if we've hit anything at all]
        // Since we have the barycentric coords of the hit, calculating it is easy
        ret.uv = {};

        // Index of the UV set to use
        const int uvSetIdx = 0;

        // Check if texcoords exist (some models only have colored mesh without textures)
        if (c.hitGeo->texcoords[uvSetIdx])
        {
            for (int i = 0; i < 3; i++)
            {
                // Vertex's UV position
                RwTextureCoordinates* const vtxUV = &c.hitGeo->texcoords[uvSetIdx][c.hitTri->verts[i]];

                // Now, just interpolate
                ret.uv += CVector2D{vtxUV->u, vtxUV->v} * c.hitBary[i];
            }
        }
        else
        {
            // No texture coords available, use barycentric coords as UV fallback
            ret.uv = CVector2D{c.hitBary.fX, c.hitBary.fY};
        }

        // Find out material texture name
        // For some reason this is sometimes null
        if (c.hitGeo->materials.materials && c.hitGeo->materials.materials[c.hitTri->materialId])
        {
            RwTexture* const tex = c.hitGeo->materials.materials[c.hitTri->materialId]->texture;
            ret.textureName = tex ? tex->name : nullptr;
        }
        else
        {
            ret.textureName = nullptr;
        }

        RwFrame* const hitFrame = RpAtomicGetFrame(c.hitAtomic);
        ret.frameName = hitFrame ? hitFrame->szName : nullptr;

        // Get hit position in world space
        ret.hitPos = c.entMat.TransformVector(c.hitPosOS);
    }

    return ret;
}

bool CWorldSA::ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** CollisionEntity,
                                  const SLineOfSightFlags flags, SLineOfSightBuildingResult* pBuildingResult, SProcessLineOfSightMaterialInfoResult* outMatInfo)
{
    DWORD dwPadding[100];  // stops the function missbehaving and overwriting the return address
    dwPadding[0] = 0;      // prevent the warning and eventual compiler optimizations from removing it

    CColPointSA*          pColPointSA = new CColPointSA();
    CColPointSAInterface* pColPointSAInterface = pColPointSA->GetInterface();

    // DWORD targetEntity;
    CEntitySAInterface* targetEntity = NULL;
    bool                bReturn = false;

    DWORD dwFunc = FUNC_ProcessLineOfSight;
    // bool bCheckBuildings = true,                 bool bCheckVehicles = true,     bool bCheckPeds = true,
    // bool bCheckObjects = true,                   bool bCheckDummies = true,      bool bSeeThroughStuff = false,
    // bool bIgnoreSomeObjectsForCamera = false,    bool bShootThroughStuff = false
    MemPutFast<BYTE>(VAR_CWorld_bIncludeCarTires, flags.bCheckCarTires);

    // clang-format off
    __asm
    {
        push    flags.bShootThroughStuff
        push    flags.bIgnoreSomeObjectsForCamera
        push    flags.bSeeThroughStuff
        push    flags.bCheckDummies
        push    flags.bCheckObjects
        push    flags.bCheckPeds
        push    flags.bCheckVehicles
        push    flags.bCheckBuildings
        lea     eax, targetEntity
        push    eax
        push    pColPointSAInterface
        push    vecEnd
        push    vecStart
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x30
    }
    // clang-format on

    MemPutFast<BYTE>(VAR_CWorld_bIncludeCarTires, 0);

    // Building info needed?
    if (pBuildingResult)
    {
        CPoolsSA* pPools = ((CPoolsSA*)pGame->GetPools());
        if (pPools)
        {
            if (targetEntity && targetEntity->nType == ENTITY_TYPE_BUILDING)
            {
                pBuildingResult->bValid = true;
                pBuildingResult->usModelID = targetEntity->m_nModelIndex;
                if (targetEntity->m_pLod)
                    pBuildingResult->usLODModelID = targetEntity->m_pLod->m_nModelIndex;
                else
                    pBuildingResult->usLODModelID = 0;

                pBuildingResult->pInterface = targetEntity;
                pBuildingResult->vecPosition = targetEntity->m_transform.m_translate;
                if (targetEntity->matrix)
                {
                    CVector& vecRotation = pBuildingResult->vecRotation;
                    ConvertMatrixToEulerAngles(*targetEntity->matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ);
                    vecRotation = -vecRotation;
                }
            }
            if (targetEntity && targetEntity->nType == ENTITY_TYPE_OBJECT)
            {
                pBuildingResult->bValid = true;
                pBuildingResult->usModelID = targetEntity->m_nModelIndex;
                if (targetEntity->m_pLod)
                    pBuildingResult->usLODModelID = targetEntity->m_pLod->m_nModelIndex;
                else
                    pBuildingResult->usLODModelID = 0;

                pBuildingResult->pInterface = targetEntity;
                if (targetEntity->matrix)
                {
                    pBuildingResult->vecPosition = targetEntity->matrix->vPos;
                    CVector& vecRotation = pBuildingResult->vecRotation;
                    ConvertMatrixToEulerAngles(*targetEntity->matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ);
                    vecRotation = -vecRotation;
                }
                else
                    pBuildingResult->vecPosition = targetEntity->m_transform.m_translate;
            }
        }
    }

    if (CollisionEntity)
    {
        CPools* pPools = pGame->GetPools();
        if (pPools)
        {
            if (targetEntity)
            {
                *CollisionEntity = pPools->GetEntity((DWORD*)targetEntity);
            }
        }
    }

    if (outMatInfo)
    {
        outMatInfo->valid = false;
        if (targetEntity)
        {
            // There might not be a texture hit info result as the collision model differs from the mesh itself.
            // This is completely normal as collisions models are meant to be simplified
            // compared to the mesh
            *outMatInfo = ProcessLineAgainstMesh(targetEntity, *vecStart, *vecEnd);
        }
    }

    if (colCollision)
        *colCollision = pColPointSA;
    else
        pColPointSA->Destroy();

    return bReturn;
}

CEntity* CWorldSA::TestSphereAgainstWorld(const CVector& sphereCenter, float radius, CEntity* ignoredEntity, bool checkBuildings, bool checkVehicles,
                                          bool checkPeds, bool checkObjects, bool checkDummies, bool cameraIgnore, STestSphereAgainstWorldResult& result)
{
    auto entity = ((CEntitySAInterface * (__cdecl*)(CVector, float, CEntitySAInterface*, bool, bool, bool, bool, bool, bool))
                       FUNC_CWorld_TestSphereAgainstWorld)(sphereCenter, radius, ignoredEntity ? ignoredEntity->GetInterface() : nullptr, checkBuildings,
                                                           checkVehicles, checkPeds, checkObjects, checkDummies, cameraIgnore);
    if (!entity)
        return nullptr;

    result.collisionDetected = true;
    result.modelID = entity->m_nModelIndex;
    if (entity->matrix)
    {
        result.entityPosition = entity->matrix->vPos;
        ConvertMatrixToEulerAngles(*entity->matrix, result.entityRotation.fX, result.entityRotation.fY, result.entityRotation.fZ);
    }
    else
    {
        result.entityPosition = entity->m_transform.m_translate;
        result.entityRotation.fX = result.entityRotation.fY = 0.0f;
        result.entityRotation.fZ = entity->m_transform.m_heading * (180.0f / std::numbers::pi);
    }
    result.entityRotation = -result.entityRotation;
    result.lodID = entity->m_pLod ? entity->m_pLod->m_nModelIndex : 0;
    result.type = static_cast<eEntityType>(entity->nType);

    return pGame->GetPools()->GetEntity(reinterpret_cast<DWORD*>(entity));
}

void CWorldSA::IgnoreEntity(CEntity* pEntity)
{
    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(pEntity);

    if (pEntitySA)
        MemPutFast<DWORD>(VAR_IgnoredEntity, (DWORD)pEntitySA->GetInterface());
    else
        MemPutFast<DWORD>(VAR_IgnoredEntity, 0);
}

float CWorldSA::FindGroundZFor3DPosition(CVector* vecPosition)
{
    DWORD dwFunc = FUNC_FindGroundZFor3DCoord;
    float fReturn = 0;
    float fX = vecPosition->fX;
    float fY = vecPosition->fY;
    float fZ = vecPosition->fZ;
    // clang-format off
    __asm
    {
        push    0
        push    0
        push    fZ
        push    fY
        push    fX
        call    dwFunc
        fstp    fReturn
        add     esp, 0x14
    }
    // clang-format on
    return fReturn;
}

float CWorldSA::FindRoofZFor3DCoord(CVector* pvecPosition, bool* pbOutResult)
{
    auto CWorld_FindRoofZFor3DCoord = (float(__cdecl*)(float, float, float, bool*))0x569750;
    return CWorld_FindRoofZFor3DCoord(pvecPosition->fX, pvecPosition->fY, pvecPosition->fZ, pbOutResult);
}

bool CWorldSA::IsLineOfSightClear(const CVector* vecStart, const CVector* vecEnd, const SLineOfSightFlags flags)
{
    DWORD dwFunc = FUNC_IsLineOfSightClear;
    bool  bReturn = false;
    // bool bCheckBuildings = true, bool bCheckVehicles = true, bool bCheckPeds = true,
    // bool bCheckObjects = true, bool bCheckDummies = true, bool bSeeThroughStuff = false,
    // bool bIgnoreSomeObjectsForCamera = false

    // clang-format off
    __asm
    {
        push    flags.bIgnoreSomeObjectsForCamera
        push    flags.bSeeThroughStuff
        push    flags.bCheckDummies
        push    flags.bCheckObjects
        push    flags.bCheckPeds
        push    flags.bCheckVehicles
        push    flags.bCheckBuildings
        push    vecEnd
        push    vecStart
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x24
    }
    // clang-format on
    return bReturn;
}

bool CWorldSA::HasCollisionBeenLoaded(CVector* vecPosition)
{
    DWORD dwFunc = FUNC_HasCollisionBeenLoaded;
    bool  bRet = false;
    // clang-format off
    __asm
    {
        push    0
        push    vecPosition
        call    dwFunc
        mov     bRet, al
        add     esp, 8
    }
    // clang-format on
    return bRet;
}

DWORD CWorldSA::GetCurrentArea()
{
    return *(DWORD*)VAR_currArea;
}

void CWorldSA::SetCurrentArea(DWORD dwArea)
{
    MemPutFast<DWORD>(VAR_currArea, dwArea);

    DWORD dwFunc = FUNC_RemoveBuildingsNotInArea;
    // clang-format off
    __asm
    {
        push    dwArea
        call    dwFunc
        add     esp, 4
    }
    // clang-format on
}

void CWorldSA::SetJetpackMaxHeight(float fHeight)
{
    MemPut<float>(VAR_fJetpackMaxHeight, fHeight);
}

float CWorldSA::GetJetpackMaxHeight()
{
    return *(float*)(VAR_fJetpackMaxHeight);
}

void CWorldSA::SetAircraftMaxHeight(float fHeight)
{
    g_pCore->GetMultiplayer()->SetAircraftMaxHeight(fHeight);
}

float CWorldSA::GetAircraftMaxHeight()
{
    return g_pCore->GetMultiplayer()->GetAircraftMaxHeight();
}

void CWorldSA::SetAircraftMaxVelocity(float fVelocity)
{
    g_pCore->GetMultiplayer()->SetAircraftMaxVelocity(fVelocity);
}

float CWorldSA::GetAircraftMaxVelocity()
{
    return g_pCore->GetMultiplayer()->GetAircraftMaxVelocity();
}

void CWorldSA::SetOcclusionsEnabled(bool bEnabled)
{
    if (!bEnabled)
    {
        MemPut<BYTE>(FUNC_COcclusion_ProcessBeforeRendering, 0xC3);  // retn
        MemPutFast<int>(VAR_COcclusion_NumActiveOccluders, 0);
        MemCpy((void*)CALL_CCullZones_FindTunnelAttributesForCoors, "\xB8\x80\x28\x00\x00", 5);  // mov eax, 0x2880
    }
    else
    {
        MemPut<BYTE>(FUNC_COcclusion_ProcessBeforeRendering, 0x51);                              // Standard value
        MemCpy((void*)CALL_CCullZones_FindTunnelAttributesForCoors, "\xE8\xDE\x82\x1D\x00", 5);  // call 0x72D9F0
    }
}

bool CWorldSA::GetOcclusionsEnabled()
{
    if (*(BYTE*)FUNC_COcclusion_ProcessBeforeRendering == 0x51)  // Is standard value ?
        return true;
    return false;
}

namespace
{
    // The three angles arrive as rotZ, rotY, rotX: the seventh argument is handed to CMatrix::SetRotateZOnly
    // (0x59B0E0) and the ninth to SetRotateXOnly (0x59B060), the same order the occl IPL sections use.
    using COcclusion_AddOne_t = void(__cdecl*)(float fCentreX, float fCentreY, float fCentreZ, float fSizeX, float fSizeY, float fSizeZ, float fRotZ,
                                               float fRotY, float fRotX, int iFlag, char cInterior);

    // Stored as int16 in quarter units; the extents are truncated to whole units first
    constexpr float OCCLUDER_MAX_COORD = 8000.0f;
    constexpr float OCCLUDER_MIN_SIZE = 1.0f;

    // Byte angles of 360/256 degrees
    constexpr float OCCLUDER_ANGLE_STEP = 360.0f / 256.0f;

    // COcclusion::AddOne normalises the angles with two unbounded loops, so an out of range angle
    // never returns. It then truncates to the byte angle grid after two float constants that sit
    // just below their true values, so half a step is added to make that a rounding.
    float NormaliseOccluderAngle(float fDegrees)
    {
        fDegrees = std::fmod(fDegrees, 360.0f);
        if (fDegrees < 0.0f)
            fDegrees += 360.0f;
        return fDegrees + OCCLUDER_ANGLE_STEP / 2.0f;
    }

    bool IsFiniteVector(const CVector& vec)
    {
        return std::isfinite(vec.fX) && std::isfinite(vec.fY) && std::isfinite(vec.fZ);
    }
}  // namespace

void CWorldSA::CaptureOccluderBaseline()
{
    if (m_bOccluderBaselineTaken)
        return;

    const uint uiCount = *(uint*)VAR_COcclusion_NumOccluders;
    const uint uiInteriorCount = *(uint*)VAR_COcclusion_NumInteriorOccluders;

    m_OccluderBaseline.resize(uiCount * COCCLUSION_ENTRY_SIZE);
    if (!m_OccluderBaseline.empty())
        MemCpyFast(m_OccluderBaseline.data(), (void*)ARRAY_COcclusion_Occluders, m_OccluderBaseline.size());

    m_InteriorOccluderBaseline.resize(uiInteriorCount * COCCLUSION_ENTRY_SIZE);
    if (!m_InteriorOccluderBaseline.empty())
        MemCpyFast(m_InteriorOccluderBaseline.data(), (void*)ARRAY_COcclusion_InteriorOccluders, m_InteriorOccluderBaseline.size());

    MemCpyFast(m_OccluderBaselineHeads, (void*)VAR_COcclusion_ListHeads, sizeof(m_OccluderBaselineHeads));
    m_bOccluderBaselineTaken = true;
}

void CWorldSA::RebuildOccluders()
{
    CaptureOccluderBaseline();

    // Put the map back exactly as it was loaded, heads included, then let the engine thread the
    // lists itself by replaying every scripted occluder through its own AddOne.
    if (!m_OccluderBaseline.empty())
        MemCpyFast((void*)ARRAY_COcclusion_Occluders, m_OccluderBaseline.data(), m_OccluderBaseline.size());
    if (!m_InteriorOccluderBaseline.empty())
        MemCpyFast((void*)ARRAY_COcclusion_InteriorOccluders, m_InteriorOccluderBaseline.data(), m_InteriorOccluderBaseline.size());

    MemPutFast<uint>(VAR_COcclusion_NumOccluders, m_OccluderBaseline.size() / COCCLUSION_ENTRY_SIZE);
    MemPutFast<uint>(VAR_COcclusion_NumInteriorOccluders, m_InteriorOccluderBaseline.size() / COCCLUSION_ENTRY_SIZE);
    MemCpyFast((void*)VAR_COcclusion_ListHeads, m_OccluderBaselineHeads, sizeof(m_OccluderBaselineHeads));

    const auto pfnAddOne = (COcclusion_AddOne_t)FUNC_COcclusion_AddOne;
    for (const SScriptedOccluder& occluder : m_ScriptedOccluders)
    {
        pfnAddOne(occluder.vecPosition.fX, occluder.vecPosition.fY, occluder.vecPosition.fZ, occluder.vecSize.fX, occluder.vecSize.fY, occluder.vecSize.fZ,
                  occluder.vecRotation.fZ, occluder.vecRotation.fY, occluder.vecRotation.fX, 0, occluder.bInterior ? 1 : 0);
    }

    RestartOccluderListWalk();
}

// The far list is walked sixteen entries per frame from a persistent cursor, so an entry pushed onto
// its head could wait a full sweep before it is looked at. Clearing the cursor the way COcclusion::Init
// does makes the next walk start from the head, and the newest occluder is the first thing it sees.
void CWorldSA::RestartOccluderListWalk()
{
    MemPutFast<std::uint16_t>(VAR_COcclusion_ListHeads + 8, 0xFFFF);
    MemPutFast<std::uint16_t>(VAR_COcclusion_ListHeads + 12, 0xFFFF);
}

void CWorldSA::GetOccluderCapacity(bool bInterior, uint& uiOutUsed, uint& uiOutFree)
{
    CaptureOccluderBaseline();

    const size_t sizeBaseline = bInterior ? m_InteriorOccluderBaseline.size() : m_OccluderBaseline.size();
    const uint   uiMax = bInterior ? COCCLUSION_MAX_INTERIOR_OCCLUDERS : COCCLUSION_MAX_OCCLUDERS;
    const uint   uiVanilla = sizeBaseline / COCCLUSION_ENTRY_SIZE;
    const uint   uiScriptable = uiMax > uiVanilla ? uiMax - uiVanilla : 0;

    uiOutUsed = std::count_if(m_ScriptedOccluders.begin(), m_ScriptedOccluders.end(),
                              [bInterior](const SScriptedOccluder& occluder) { return occluder.bInterior == bInterior; });
    uiOutFree = uiScriptable > uiOutUsed ? uiScriptable - uiOutUsed : 0;
}

bool CWorldSA::AddOccluder(const CVector& vecPosition, const CVector& vecSize, const CVector& vecRotation, bool bInterior, void* pChangeSource, uint& uiOutId)
{
    if (!IsFiniteVector(vecPosition) || !IsFiniteVector(vecSize) || !IsFiniteVector(vecRotation))
        return false;

    if (std::fabs(vecPosition.fX) > OCCLUDER_MAX_COORD || std::fabs(vecPosition.fY) > OCCLUDER_MAX_COORD || std::fabs(vecPosition.fZ) > OCCLUDER_MAX_COORD)
        return false;

    if (vecSize.fX < OCCLUDER_MIN_SIZE || vecSize.fY < OCCLUDER_MIN_SIZE || vecSize.fZ < OCCLUDER_MIN_SIZE)
        return false;

    if (vecSize.fX > OCCLUDER_MAX_COORD || vecSize.fY > OCCLUDER_MAX_COORD || vecSize.fZ > OCCLUDER_MAX_COORD)
        return false;

    // AddOne returns nothing and drops the call when the array is full, so the room has to be
    // checked here or the caller would be told about an occluder that does not exist.
    uint uiUsed, uiFree;
    GetOccluderCapacity(bInterior, uiUsed, uiFree);
    if (uiFree == 0)
        return false;

    SScriptedOccluder occluder;
    occluder.uiId = m_uiNextOccluderId++;
    occluder.vecPosition = vecPosition;
    occluder.vecSize = vecSize;
    occluder.vecRotation = CVector(NormaliseOccluderAngle(vecRotation.fX), NormaliseOccluderAngle(vecRotation.fY), NormaliseOccluderAngle(vecRotation.fZ));
    occluder.bInterior = bInterior;
    occluder.pChangeSource = pChangeSource;
    m_ScriptedOccluders.push_back(occluder);

    // AddOne only ever pushes onto the head of the far list, so a new occluder can go straight in;
    // only removal has to rebuild. The count growing is the only sign that it kept the box.
    const DWORD dwCountAddress = bInterior ? VAR_COcclusion_NumInteriorOccluders : VAR_COcclusion_NumOccluders;
    const uint  uiCountBefore = *(uint*)dwCountAddress;
    const auto  pfnAddOne = (COcclusion_AddOne_t)FUNC_COcclusion_AddOne;
    pfnAddOne(occluder.vecPosition.fX, occluder.vecPosition.fY, occluder.vecPosition.fZ, occluder.vecSize.fX, occluder.vecSize.fY, occluder.vecSize.fZ,
              occluder.vecRotation.fZ, occluder.vecRotation.fY, occluder.vecRotation.fX, 0, occluder.bInterior ? 1 : 0);
    if (*(uint*)dwCountAddress != uiCountBefore + 1)
    {
        m_ScriptedOccluders.pop_back();
        return false;
    }
    RestartOccluderListWalk();

    uiOutId = occluder.uiId;
    return true;
}

bool CWorldSA::RemoveOccluder(uint uiId, void* pChangeSource)
{
    const auto iter =
        std::find_if(m_ScriptedOccluders.begin(), m_ScriptedOccluders.end(), [uiId](const SScriptedOccluder& occluder) { return occluder.uiId == uiId; });
    if (iter == m_ScriptedOccluders.end())
        return false;

    // A resource can only remove the occluders it created itself
    if (pChangeSource && iter->pChangeSource != pChangeSource)
        return false;

    m_ScriptedOccluders.erase(iter);
    RebuildOccluders();
    return true;
}

void CWorldSA::UndoOccluderChanges(void* pChangeSource)
{
    if (m_ScriptedOccluders.empty())
        return;

    if (pChangeSource)
    {
        const size_t sizeBefore = m_ScriptedOccluders.size();
        m_ScriptedOccluders.erase(std::remove_if(m_ScriptedOccluders.begin(), m_ScriptedOccluders.end(),
                                                 [pChangeSource](const SScriptedOccluder& occluder) { return occluder.pChangeSource == pChangeSource; }),
                                  m_ScriptedOccluders.end());
        if (m_ScriptedOccluders.size() == sizeBefore)
            return;
    }
    else
    {
        m_ScriptedOccluders.clear();
    }

    RebuildOccluders();
}

void CWorldSA::FindWorldPositionForRailTrackPosition(float fRailTrackPosition, int iTrackId, CVector* pOutVecPosition)
{
    DWORD dwFunc = FUNC_CWorld_FindPositionForTrackPosition;  // __cdecl

    // clang-format off
    __asm
    {
        push pOutVecPosition
        push iTrackId
        push fRailTrackPosition
        call dwFunc
        add  esp, 3*4
    }
    // clang-format on
}

int CWorldSA::FindClosestRailTrackNode(const CVector& vecPosition, uchar& ucOutTrackId, float& fOutRailDistance)
{
    // Original function @ 0x6F7550
    int           iNodeId = -1;
    float         fMinDistance = 99999.898f;
    int*          aNumTrackNodes = (int*)ARRAY_NumRailTrackNodes;
    SRailNodeSA** aTrackNodes = (SRailNodeSA**)ARRAY_RailTrackNodePointers;
    uchar         ucDesiredTrackId = ucOutTrackId;

    if (ucDesiredTrackId >= NUM_RAILTRACKS)
        ucDesiredTrackId = 0xFF;

    for (uchar ucTrackId = 0; ucTrackId < NUM_RAILTRACKS; ++ucTrackId)
    {
        if ((ucDesiredTrackId == 0xFF || ucTrackId == ucDesiredTrackId) && aNumTrackNodes[ucTrackId] > 0)
        {
            for (int i = 0; i < aNumTrackNodes[ucTrackId]; ++i)
            {
                SRailNodeSA& railNode = aTrackNodes[ucTrackId][i];

                float fDistance = sqrtf(powf(vecPosition.fZ - railNode.sZ * 0.125f, 2) + powf(vecPosition.fY - railNode.sY * 0.125f, 2) +
                                        powf(vecPosition.fX - railNode.sX * 0.125f, 2));
                if (fDistance < fMinDistance)
                {
                    fMinDistance = fDistance;
                    iNodeId = i;
                    ucOutTrackId = ucTrackId;
                }
            }
        }
    }

    // Read rail distance
    if (iNodeId != -1)
    {
        fOutRailDistance = aTrackNodes[ucOutTrackId][iNodeId].sRailDistance * 3.33333334f;
    }

    return iNodeId;
}

bool CWorldSA::CalculateImpactPosition(const CVector& vecInputStart, CVector& vecInputEnd)
{
    // get our position
    CVector vecStart = vecInputStart;
    // get our end position by projecting forward a few velocities more
    CVector vecEnd = vecInputEnd;
    // grab the difference between our reported and actual end position
    CVector diff = vecEnd - vecStart;
    // normalize our difference
    diff.Normalize();
    // project forward another unit
    vecEnd = vecEnd + diff * 1;
    // create a variable to store our collision data
    CColPoint* pColPoint;
    // create a variable to store our collision entity
    CEntity* pCollisionEntity;

    // flags
    SLineOfSightFlags flags;
    flags.bCheckCarTires = false;
    flags.bIgnoreSomeObjectsForCamera = true;
    flags.bCheckBuildings = true;
    flags.bCheckPeds = true;
    flags.bCheckObjects = true;
    flags.bCheckDummies = true;

    // Include dead peds
    MemPutFast<DWORD>(0xB7CD71, 1);

    SLineOfSightBuildingResult result;
    // process forward another 1 unit
    if (ProcessLineOfSight(&vecStart, &vecEnd, &pColPoint, &pCollisionEntity, flags, &result))
    {
        // set our collision position
        vecInputEnd = pColPoint->GetPosition();

        // destroy our colshape
        pColPoint->Destroy();

        // reset include dead peds
        MemPutFast<DWORD>(0xB7CD71, 0);
        return true;
    }
    // Include dead peds
    MemPutFast<DWORD>(0xB7CD71, 0);
    return false;
}

namespace
{
    const unsigned char aOriginalSurfaceInfo[2292] = {
        0x00, 0x00, 0xc0, 0x40, 0x66, 0x66, 0x66, 0x40, 0x00, 0x00, 0x90, 0x40, 0xcd, 0xcc, 0x4c, 0x40, 0x00, 0x00, 0x40, 0x40, 0x33, 0x33, 0x33, 0x40, 0x66,
        0x66, 0x66, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x60, 0x40, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
        0x90, 0x40, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0xc0, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x80, 0x3f, 0xcd, 0xcc, 0x4c,
        0x40, 0x00, 0x00, 0x60, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x40, 0x40,
        0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x33, 0x33, 0x33, 0x40, 0x00,
        0x00, 0x00, 0x40, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x3f, 0x0a, 0xe7, 0x00, 0x00, 0x22, 0x01,
        0x00, 0x00, 0x40, 0x06, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x22, 0x01, 0x20, 0x48, 0x40, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x22, 0x01, 0x80,
        0x08, 0x40, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x09, 0xc0, 0x06, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x22, 0x01, 0x60, 0x48,
        0x00, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x08, 0x00, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x23, 0x09, 0x80, 0xc1, 0x04,
        0x22, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x22, 0x01, 0x80, 0x48, 0x00, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x00, 0x00, 0x06,
        0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0a, 0x80, 0x02, 0x82, 0x0a, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0a, 0x80, 0x02, 0x82, 0x0a, 0x00,
        0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0a, 0x80, 0x82, 0x82, 0x02, 0x02, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0a, 0x80, 0x02, 0x82, 0x0a, 0x00, 0x00,
        0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0a, 0x80, 0x02, 0x82, 0x0a, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0a, 0x80, 0x82, 0x82, 0x02, 0x02, 0x00, 0x0a,
        0xd8, 0x00, 0x00, 0x0b, 0x0a, 0x80, 0x02, 0x02, 0x0a, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0a, 0x80, 0x02, 0x02, 0x0a, 0x00, 0x00, 0x0a, 0x28,
        0x00, 0x00, 0x03, 0x0a, 0x04, 0x02, 0x00, 0x08, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x23, 0x01, 0x84, 0x01, 0x00, 0x04, 0x00, 0x00, 0x0a, 0xd8, 0x00,
        0x00, 0x03, 0x0a, 0x80, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x03, 0x0a, 0x80, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x00, 0x81, 0x00, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x21, 0x02, 0x00, 0x81, 0x80, 0x06, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x03,
        0x0b, 0x00, 0x01, 0x00, 0x0a, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x0c, 0x0a, 0x00, 0x81, 0x08, 0x02, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x29, 0x02,
        0x00, 0x81, 0x08, 0x22, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x02, 0x80, 0x80, 0x10, 0x22, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00,
        0xc0, 0x10, 0x22, 0x00, 0x00, 0x0a, 0x32, 0x00, 0x00, 0x14, 0x4a, 0x00, 0xa0, 0xa0, 0x12, 0x00, 0x00, 0x0a, 0x32, 0x00, 0x00, 0x14, 0x4a, 0x00, 0xa0,
        0xa0, 0x12, 0x00, 0x00, 0x0a, 0x32, 0x00, 0x00, 0x12, 0x0a, 0x00, 0xa0, 0xa0, 0x12, 0x00, 0x00, 0x0a, 0x32, 0x00, 0x00, 0x14, 0x4a, 0x00, 0xa0, 0xa0,
        0x12, 0x00, 0x00, 0x0a, 0x32, 0x00, 0x00, 0x14, 0x4a, 0x00, 0xa0, 0xa0, 0x12, 0x00, 0x00, 0x0a, 0x32, 0x00, 0x00, 0x14, 0x4a, 0x02, 0xa0, 0x20, 0x12,
        0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x02, 0x01, 0x02, 0x48, 0x00, 0x12, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x21, 0x01, 0x00, 0x49, 0x00, 0x06, 0x00,
        0x00, 0x0a, 0x00, 0x00, 0x00, 0x21, 0x01, 0x00, 0x01, 0x00, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x21, 0x01, 0x00, 0x01, 0x00, 0x06, 0x00, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x05, 0x8a, 0x00, 0x01, 0x01, 0x82, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x05, 0x8a, 0x01, 0x01, 0x01, 0x82, 0x00, 0x00, 0x0a,
        0x00, 0x00, 0x00, 0x03, 0x0a, 0x80, 0x80, 0x80, 0x02, 0x02, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x0a, 0x80, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x0a, 0xe7,
        0x00, 0x00, 0x02, 0x03, 0x00, 0x02, 0x00, 0x42, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x02, 0x03, 0x00, 0x02, 0x00, 0x42, 0x00, 0x00, 0x0a, 0x00, 0x00,
        0x00, 0x02, 0x03, 0x00, 0x02, 0x00, 0x42, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x11, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00,
        0x21, 0x11, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x21, 0x11, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x21,
        0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x31, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00,
        0x00, 0x02, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x31, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00,
        0x02, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0x02,
        0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x0a, 0xe7, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
        0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x0a,
        0x00, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x21, 0x31, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0a, 0xd8,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0a, 0xe7, 0x00,
        0x00, 0x21, 0x11, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00,
        0x21, 0x0c, 0x00, 0x10, 0x00, 0x12, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x21, 0x03, 0x00, 0x10, 0x00, 0x42, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02,
        0x03, 0x10, 0x00, 0x00, 0x42, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x14, 0x42, 0x00, 0xa0, 0xa0, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x14, 0x42,
        0x80, 0xa0, 0xa0, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x14, 0x42, 0x00, 0xa0, 0xa0, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x14, 0x42, 0x00,
        0xa0, 0xa0, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x14, 0x42, 0x00, 0xa0, 0x20, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x14, 0x42, 0x02, 0xa0,
        0x20, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x00, 0x82, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x00, 0x82,
        0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x00, 0x82, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x04, 0x80, 0x00, 0x88, 0x07,
        0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x04, 0x80, 0x00, 0x88, 0x07, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x23, 0x04, 0x80, 0x00, 0x08, 0x07, 0x00,
        0x00, 0x0a, 0x00, 0x00, 0x00, 0x23, 0x04, 0x80, 0x00, 0x84, 0x07, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x08, 0x03, 0x02, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x08, 0x07, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x0a,
        0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x0a, 0x00,
        0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x0a, 0x00, 0x00,
        0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
        0x12, 0xca, 0x00, 0x00, 0x21, 0x83, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x12, 0xca, 0x00, 0x00, 0x21, 0x83, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x12,
        0xca, 0x00, 0x00, 0x21, 0x83, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x12, 0xca, 0x00, 0x00, 0x21, 0x83, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x12, 0xca,
        0x00, 0x00, 0x21, 0x83, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00,
        0x00, 0x00, 0x43, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,
        0x00, 0x43, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00,
        0x03, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x43,
        0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x23, 0x01, 0x00, 0x80, 0x00, 0x23, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x80, 0x00, 0x0b, 0x00,
        0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x80, 0x02, 0x03, 0x02, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x80, 0x02, 0x03, 0x02, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x80, 0x82, 0x03, 0x02, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x80, 0x02, 0x03, 0x02, 0x00, 0x0a,
        0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x82, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x82, 0x03, 0x02, 0x00, 0x0a, 0x00,
        0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x82, 0x03, 0x02, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x82, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00,
        0x00, 0x0b, 0x04, 0x80, 0x80, 0x82, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x82, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
        0x0b, 0x04, 0x80, 0x80, 0x82, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x82, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b,
        0x04, 0x80, 0x80, 0x08, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x04, 0x80, 0x80, 0x00, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x04,
        0x00, 0x80, 0x08, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x01, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00,
        0x00, 0x00, 0x07, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x88, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x00, 0x81,
        0x08, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x08, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x13, 0x04, 0x80, 0x80, 0x00,
        0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0b, 0x04, 0x80, 0x80, 0x08, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x0b,
        0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x13, 0x00,
        0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x0a,
        0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x0a, 0x00,
        0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00,
        0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x03, 0x0a, 0x00, 0x80, 0x00, 0x03, 0x02, 0x00, 0x0a, 0xd8, 0x00, 0x00,
        0x0b, 0x0c, 0x80, 0x80, 0x80, 0x0b, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0c, 0x80, 0x80, 0x80, 0x0b, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b,
        0x0c, 0x80, 0x80, 0x80, 0x0b, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0c, 0x80, 0x80, 0x80, 0x0b, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0c,
        0x80, 0x80, 0x80, 0x0b, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0c, 0x80, 0x00, 0x80, 0x0b, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0c, 0x80,
        0x00, 0x80, 0x0b, 0x00, 0x00, 0x0a, 0xd8, 0x00, 0x00, 0x0b, 0x0c, 0x80, 0x00, 0x80, 0x0b, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00,
        0x00, 0x83, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0x01, 0x00, 0x00, 0x00,
        0x83, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x40,
        0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0a, 0xe7, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x0a, 0x00,
        0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x10, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x10, 0x00, 0x00, 0x02, 0x01, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x02, 0x03, 0x10, 0x00, 0x00, 0x42, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x22, 0x01, 0x10, 0x10, 0x00, 0x02, 0x01, 0x00, 0x0a,
        0x00, 0x00, 0x00, 0x22, 0x01, 0x10, 0x10, 0x00, 0x06, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x04, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x00,
        0x00, 0x00, 0x22, 0x01, 0x10, 0x10, 0x00, 0x02, 0x01, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x22, 0x01, 0x10, 0x10, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x00, 0x00,
        0x00, 0x02, 0x04, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
        0x22, 0x01, 0x10, 0x10, 0x00, 0x02, 0x01, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x03, 0x10, 0x10, 0x00, 0x42, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02,
        0x03, 0x10, 0x10, 0x00, 0x42, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x03, 0x10, 0x10, 0x00, 0x42, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x22, 0x01,
        0x10, 0x10, 0x00, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x04, 0x10, 0x10, 0x00, 0x0a, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x10,
        0x10, 0x00, 0x12, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x01, 0x90, 0x01, 0x00, 0x22, 0x00, 0x00};
    static_assert(sizeof(CSurfaceType) == sizeof(aOriginalSurfaceInfo), "invalid size of aOriginalSurfaceInfo");
}  // namespace
