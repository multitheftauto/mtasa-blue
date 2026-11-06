/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCameraSA.cpp
 *  PURPOSE:     Camera rendering
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCameraSA.h"
#include "CGameSA.h"
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>

namespace
{
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kTwoPi = 6.28318530717958647692f;

    inline float WrapAngleRad(float angle) noexcept
    {
        // Wrap into [-pi, pi] using one multiplication and floor
        angle -= kTwoPi * std::floor((angle + kPi) / kTwoPi);
        if (angle <= -kPi)
            angle += kTwoPi;
        else if (angle > kPi)
            angle -= kTwoPi;
        return angle;
    }

    inline bool IsFiniteVector(const CVector& vec) noexcept
    {
        return std::isfinite(vec.fX) && std::isfinite(vec.fY) && std::isfinite(vec.fZ);
    }
}

extern CGameSA* pGame;

enum class CameraClipFlags : uint8_t
{
    Objects = 1u << 0,
    Vehicles = 1u << 1,
};

static std::atomic<uint8_t> s_cameraClipMask{static_cast<uint8_t>(CameraClipFlags::Objects) | static_cast<uint8_t>(CameraClipFlags::Vehicles)};

#define VAR_CameraClipVehicles       0x8A5B14
#define VAR_CameraClipDynamicObjects 0x8A5B15
#define VAR_CameraClipStaticObjects  0x8A5B16
#define VAR_RelVelCamCollisionVehSqr 0x8A5B18

#define HOOKPOS_Camera_CollisionDetection 0x520190
DWORD RETURN_Camera_CollisionDetection = 0x520195;
void  HOOK_Camera_CollisionDetection();

// The camera finds its target through CWorld::Players[PlayerInFocus] (FindPlayerPed, FindPlayerVehicle, FindPlayerCoors
// and FindPlayerSpeed derive from it); swapping that ped makes every camera mode work for the spectated ped
#define VAR_PlayerInFocus    0xB7CD74
#define ARRAY_PlayerInfo     0xB7CD98  // CWorld::Players; the first field of an entry is its ped
#define SIZE_PlayerInfo      0x190
#define FUNC_CCamera_Process 0x52B730

// Direct calls to CCamera::Process
#define CALL_CCamera_Process_InProcedure53BC80 0x53BCD3
#define CALL_CCamera_Process_InProcedure53BCF0 0x53BE97
#define CALL_CCamera_Process_InProcedure53BEE0 0x53C104
#define CALL_CCamera_Process_InProcedure570AAB 0x570B5D

// Called in sequence from Idle
#define FUNC_CRenderer_ConstructRenderList        0x5556E0
#define FUNC_CRenderer_PreRender                  0x553910
#define CALL_CRenderer_ConstructRenderList_InIdle 0x53E9F9
#define CALL_CRenderer_PreRender_InIdle           0x53E9FE

// CHud::DrawCrossHairs and the frame variables it checks first
#define FUNC_CHud_DrawCrossHairs          0x58E020
#define FUNC_RwRenderStateSet             0x7FC2D0
#define VAR_CTimer_FrameCounter           0xB7CB4C
#define VAR_CinematicCameraProcessedFrame 0xB6EC40

namespace
{
    CPedSAInterface* s_spectatedPed = nullptr;

    // Holds the spectated ped in the player slot for its lifetime
    class SpectatedPedScope
    {
    public:
        SpectatedPedScope() noexcept
        {
            if (!s_spectatedPed)
                return;

            const std::uint8_t playerInFocus = *reinterpret_cast<std::uint8_t*>(VAR_PlayerInFocus);
            m_slot = reinterpret_cast<CPedSAInterface**>(ARRAY_PlayerInfo + playerInFocus * SIZE_PlayerInfo);
            m_localPlayerPed = *m_slot;
            *m_slot = s_spectatedPed;
        }

        ~SpectatedPedScope() noexcept
        {
            if (m_slot)
                *m_slot = m_localPlayerPed;
        }

        SpectatedPedScope(const SpectatedPedScope&) = delete;
        SpectatedPedScope& operator=(const SpectatedPedScope&) = delete;

    private:
        CPedSAInterface** m_slot = nullptr;
        CPedSAInterface*  m_localPlayerPed = nullptr;
    };

    // Replaces the CCamera::Process calls (thiscall; ecx arrives as the first fastcall argument)
    void __fastcall ProcessCameraAsSpectator(CCameraSAInterface* camera, void*) noexcept
    {
        const SpectatedPedScope spectatedPed;
        reinterpret_cast<void(__thiscall*)(CCameraSAInterface*)>(FUNC_CCamera_Process)(camera);
    }

    // Same swap for the steps that also depend on the player: the bumper camera vehicle pass (SetupEntityVisibility),
    // level of detail and mirrors (ConstructRenderList), shadow and headlight coronas (vehicle PreRender)
    void __cdecl ConstructRenderListAsSpectator() noexcept
    {
        const SpectatedPedScope spectatedPed;
        reinterpret_cast<void(__cdecl*)()>(FUNC_CRenderer_ConstructRenderList)();
    }

    void __cdecl PreRenderAsSpectator() noexcept
    {
        const SpectatedPedScope spectatedPed;
        reinterpret_cast<void(__cdecl*)()>(FUNC_CRenderer_PreRender)();
    }
}  // namespace

CCameraSA::CCameraSA(CCameraSAInterface* cameraInterface)
{
    if (!cameraInterface)
    {
        internalInterface = nullptr;
        // Initialize all camera pointers to null
        for (int i = 0; i < MAX_CAMS; i++)
            Cams[i] = nullptr;
        return;
    }

    internalInterface = cameraInterface;

    for (int i = 0; i < MAX_CAMS; i++)
    {
        try
        {
            Cams[i] = new CCamSA(&internalInterface->Cams[i]);
        }
        catch (...)
        {
            // Clean up on failure
            for (int j = 0; j < i; j++)
            {
                delete Cams[j];
                Cams[j] = nullptr;
            }
            internalInterface = nullptr;
            throw;
        }
    }

    s_cameraClipMask.store(static_cast<uint8_t>(CameraClipFlags::Objects) | static_cast<uint8_t>(CameraClipFlags::Vehicles), std::memory_order_relaxed);

    HookInstall(HOOKPOS_Camera_CollisionDetection, (DWORD)HOOK_Camera_CollisionDetection, 5);

    constexpr std::array<std::uint32_t, 4> processCallSites{CALL_CCamera_Process_InProcedure53BC80, CALL_CCamera_Process_InProcedure53BCF0,
                                                            CALL_CCamera_Process_InProcedure53BEE0, CALL_CCamera_Process_InProcedure570AAB};
    for (const std::uint32_t callSite : processCallSites)
        HookInstallCall(callSite, reinterpret_cast<DWORD>(&ProcessCameraAsSpectator));

    HookInstallCall(CALL_CRenderer_ConstructRenderList_InIdle, reinterpret_cast<DWORD>(&ConstructRenderListAsSpectator));
    HookInstallCall(CALL_CRenderer_PreRender_InIdle, reinterpret_cast<DWORD>(&PreRenderAsSpectator));
}

CCameraSA::~CCameraSA()
{
    for (int i = 0; i < MAX_CAMS; i++)
    {
        if (Cams[i])
        {
            delete Cams[i];
            Cams[i] = nullptr;
        }
    }
}

void CCameraSA::Restore()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;

    DWORD dwFunc = FUNC_Restore;
    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
    // clang-format on
}

void CCameraSA::RestoreWithJumpCut()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;
    DWORD dwFunc = 0x50BD40;
    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
    // clang-format on
    dwFunc = 0x50BAB0;
    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
    // clang-format on
}

/**
 * \todo Find out what the last two parameters are
 */
void CCameraSA::TakeControl(CEntity* entity, eCamMode CamMode, int CamSwitchStyle)
{
    if (!entity)
        return;

    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(entity);
    if (!pEntitySA)
        return;

    CEntitySAInterface* entityInterface = pEntitySA->GetInterface();
    if (!entityInterface)
        return;

    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;

    if (CamSwitchStyle < 0)
        CamSwitchStyle = 0;
    else if (CamSwitchStyle > 10)
        CamSwitchStyle = 10;

    DWORD CCamera__TakeControl = FUNC_TakeControl;
    // clang-format off
    __asm
    {
        mov ecx, cameraInterface
        push 1
        push CamSwitchStyle
        push CamMode
        push entityInterface
        call CCamera__TakeControl
    }
    // clang-format on
}

void CCameraSA::TakeControl(CVector* position, int CamSwitchStyle)
{
    if (!position)
        return;

    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;

    if (CamSwitchStyle < 0)
        CamSwitchStyle = 0;
    else if (CamSwitchStyle > 10)
        CamSwitchStyle = 10;
    // __thiscall
    CVector vecOffset;
    /*  vecOffset.fZ = 0.5f;
        vecOffset.fY = 0.5f;
        vecOffset.fX = 0.5f;*/
    /*  DWORD dwFunc = 0x50BEC0;
        // clang-format off
        __asm
        {
            mov ecx, cameraInterface
            lea     eax, vecOffset
            push    eax
            push    position
            call    dwFunc
        }*/
    // clang-format on

    DWORD CCamera__TakeControlNoEntity = FUNC_TakeControlNoEntity;
    // clang-format off
    __asm
        {
        mov ecx, cameraInterface
        push 1
        push CamSwitchStyle
        push position
        call CCamera__TakeControlNoEntity
        }
    // clang-format on

    DWORD dwFunc = 0x50BEC0;
    // clang-format off
    __asm
    {
        mov ecx, cameraInterface
        lea     eax, vecOffset
        push    eax
        push    position
        call    dwFunc
    }
    // clang-format on
}

// LSOD recovery
void CCameraSA::RestoreLastGoodState()
{
    CMatrix defmat;
    SetMatrix(&defmat);

    CCameraSAInterface* pCameraInterface = GetInterface();

    if (!pCameraInterface)
        return;

    pCameraInterface->m_CameraAverageSpeed = 0;
    pCameraInterface->m_CameraSpeedSoFar = 0;
    pCameraInterface->m_PreviousCameraPosition = CVector();
    pCameraInterface->m_vecGameCamPos = CVector();
    pCameraInterface->m_cameraMatrix.SetFromMatrixSkipPadding(CMatrix());
    pCameraInterface->m_cameraMatrixOld.SetFromMatrixSkipPadding(CMatrix());
    pCameraInterface->m_viewMatrix.SetFromMatrixSkipPadding(CMatrix());
    pCameraInterface->m_matInverse.SetFromMatrixSkipPadding(CMatrix());
    pCameraInterface->m_vecBottomFrustumNormal = CVector(0, -1, -1);
    pCameraInterface->m_vecTopFrustumNormal = CVector(-1, -1, 0);

    for (uint i = 0; i < MAX_CAMS; i++)
    {
        CCamSA* pCam = Cams[i];
        if (!pCam)
            continue;

        CCamSAInterface* pCamInterface = pCam->GetInterface();
        if (!pCamInterface)
            continue;

        pCamInterface->m_fAlphaSpeedOverOneFrame = 0;
        pCamInterface->m_fBetaSpeedOverOneFrame = 0;
        pCamInterface->m_fTrueBeta = 1;
        pCamInterface->m_fTrueAlpha = 1;
        pCamInterface->m_fVerticalAngle = 1;
        pCamInterface->m_fHorizontalAngle = 1;
        pCamInterface->BetaSpeed = 0;
        pCamInterface->SpeedVar = 0;

        pCamInterface->m_cvecSourceSpeedOverOneFrame = CVector(0, 0, 0);
        pCamInterface->m_cvecTargetSpeedOverOneFrame = CVector(0, 0, 0);
        pCamInterface->Front = CVector(1, 0, 0);
        pCamInterface->Source = CVector(1, 0, 0);
        pCamInterface->SourceBeforeLookBehind = CVector(1, 0, 0);
        pCamInterface->Up = CVector(0, 0, 1);
        for (uint i = 0; i < CAM_NUM_TARGET_HISTORY; i++)
            pCamInterface->m_aTargetHistoryPos[i] = CVector(1, 0, 0);
    }
}

CMatrix* CCameraSA::GetMatrix(CMatrix* matrix)
{
    if (!matrix)
        return nullptr;

    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
    {
        *matrix = CMatrix();
        return matrix;
    }

    CMatrix_Padded* pCamMatrix = &cameraInterface->m_cameraMatrix;
    if (pCamMatrix)
    {
        matrix->vFront = pCamMatrix->vFront;
        matrix->vPos = pCamMatrix->vPos;
        matrix->vUp = pCamMatrix->vUp;
        matrix->vRight = pCamMatrix->vRight;

        if (!IsValidMatrix(*matrix))
        {
            RestoreLastGoodState();
            pCamMatrix->ConvertToMatrix(*matrix);
        }
    }
    else
    {
        *matrix = CMatrix();
    }
    return matrix;
}

void CCameraSA::SetMatrix(CMatrix* matrix)
{
    if (!matrix)
        return;

    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;

    CMatrix_Padded* pCamMatrix = &cameraInterface->m_cameraMatrix;
    if (pCamMatrix)
    {
        pCamMatrix->vFront = matrix->vFront;
        pCamMatrix->vPos = matrix->vPos;
        pCamMatrix->vUp = matrix->vUp;
        pCamMatrix->vRight = matrix->vRight;
    }
}

void CCameraSA::Find3rdPersonCamTargetVector(float fDistance, CVector* vecGunMuzzle, CVector* vecSource, CVector* vecTarget)
{
    if (!vecGunMuzzle || !vecSource || !vecTarget)
        return;

    // Validate float parameter to prevent NaN/infinity issues
    if (!std::isfinite(fDistance) || fDistance < 0.0f)
        return;

    float fOriginX = vecGunMuzzle->fX;
    float fOriginY = vecGunMuzzle->fY;
    float fOriginZ = vecGunMuzzle->fZ;

    if (!std::isfinite(fOriginX) || !std::isfinite(fOriginY) || !std::isfinite(fOriginZ))
        return;

    DWORD               dwFunc = FUNC_Find3rdPersonCamTargetVector;
    CCameraSAInterface* cameraInterface = GetInterface();

    if (!cameraInterface)
        return;

    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        push    vecTarget
        push    vecSource
        push    fOriginZ
        push    fOriginY
        push    fOriginX
        push    fDistance
        call    dwFunc
    }
    // clang-format on
}

float CCameraSA::Find3rdPersonQuickAimPitch()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return 0.0f;

    float fReturn;
    DWORD dwFunc = FUNC_Find3rdPersonQuickAimPitch;
    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        fstp    fReturn
    }
    // clang-format on
    return fReturn;
}

BYTE CCameraSA::GetActiveCam()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return 0;
    return cameraInterface->ActiveCam;
}

CCam* CCameraSA::GetCam(BYTE bCameraID)
{
    if (bCameraID < MAX_CAMS)
        return Cams[bCameraID];

    return NULL;
}

CCam* CCameraSA::GetCam(CCamSAInterface* camInterface)
{
    for (int i = 0; i < MAX_CAMS; i++)
    {
        if (Cams[i] && Cams[i]->GetInterface() == camInterface)
        {
            return Cams[i];
        }
    }

    return NULL;
}

void CCameraSA::SetWidescreen(bool bWidescreen)
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;
    cameraInterface->m_WideScreenOn = bWidescreen;
}

bool CCameraSA::GetWidescreen()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return false;
    return cameraInterface->m_WideScreenOn;
}

bool CCameraSA::IsFading()
{
    DWORD               dwFunc = FUNC_GetFading;
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return false;
    bool bRet = false;
    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        mov     bRet, al
    }
    // clang-format on
    return bRet;
}

int CCameraSA::GetFadingDirection()
{
    DWORD               dwFunc = FUNC_GetFadingDirection;
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return 0;
    int dwRet = false;
    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        mov     dwRet, eax
    }
    // clang-format on
    return dwRet;
}

void CCameraSA::Fade(float fFadeOutTime, int iOutOrIn)
{
    if (!std::isfinite(fFadeOutTime))
        return;

    if (fFadeOutTime < 0.0f)
        fFadeOutTime = 0.0f;
    else if (fFadeOutTime > 60.0f)
        fFadeOutTime = 60.0f;

    if (iOutOrIn < 0)
        iOutOrIn = 0;
    else if (iOutOrIn > 1)
        iOutOrIn = 1;

    DWORD               dwFunc = FUNC_Fade;
    CCameraSAInterface* cameraInterface = GetInterface();

    if (!cameraInterface)
        return;

    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        push    iOutOrIn
        push    fFadeOutTime
        call    dwFunc
    }
    // clang-format on
}

void CCameraSA::SetFadeColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    DWORD               dwFunc = FUNC_SetFadeColour;
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;
    DWORD dwRed = ucRed;
    DWORD dwGreen = ucGreen;
    DWORD dwBlue = ucBlue;
    // clang-format off
    __asm
    {
        mov     ecx, cameraInterface
        push    dwBlue
        push    dwGreen
        push    dwRed
        call    dwFunc
    }
    // clang-format on
}

float CCameraSA::GetCameraRotation()
{
    return *(float*)VAR_CameraRotation;
}

RwMatrix* CCameraSA::GetLTM()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return nullptr;

    if (!cameraInterface->m_pRwCamera)
        return nullptr;

    if (!cameraInterface->m_pRwCamera->object.object.parent)
        return nullptr;

    // RwFrameGetLTM
    return ((RwMatrix * (_cdecl*)(void*))0x7F0990)(cameraInterface->m_pRwCamera->object.object.parent);
}

CEntity* CCameraSA::GetTargetEntity()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return nullptr;

    if (!pGame)
        return nullptr;

    CEntitySAInterface* pInterface = cameraInterface->pTargetEntity;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        if (!pPools)
            return nullptr;

        return pPools->GetEntity((DWORD*)pInterface);
    }
    return nullptr;
}

void CCameraSA::SetCameraClip(bool bObjects, bool bVehicles)
{
    uint8_t newMask = 0;
    if (bObjects)
        newMask |= static_cast<uint8_t>(CameraClipFlags::Objects);
    if (bVehicles)
        newMask |= static_cast<uint8_t>(CameraClipFlags::Vehicles);
    s_cameraClipMask.store(newMask, std::memory_order_relaxed);
}

void CCameraSA::ResetCameraClip()
{
    s_cameraClipMask.store(static_cast<uint8_t>(CameraClipFlags::Objects) | static_cast<uint8_t>(CameraClipFlags::Vehicles), std::memory_order_relaxed);
}

void CCameraSA::GetCameraClip(bool& bObjects, bool& bVehicles)
{
    const uint8_t mask = s_cameraClipMask.load(std::memory_order_relaxed);
    bObjects = (mask & static_cast<uint8_t>(CameraClipFlags::Objects)) != 0;
    bVehicles = (mask & static_cast<uint8_t>(CameraClipFlags::Vehicles)) != 0;
}

// At speed, relax camera collision against dynamic (script-created) objects only.
// Static world geometry always keeps collision so default GTA world/buildings still block the camera.
// When the camera target is another player's vehicle, use that vehicle's speed rather than the local player
static void ApplyVehicleSpeedCameraClip()
{
    // Static-world clip stays on regardless of speed.
    MemPutFast<char>(VAR_CameraClipStaticObjects, 1);

    using FindPlayerVehicle_t = void*(__cdecl*)(int playerId, bool bIncludeRemote);
    auto FindPlayerVehicle = reinterpret_cast<FindPlayerVehicle_t>(0x56E0D0);

    void* pVehicle = nullptr;

    // Check the camera's actual target entity first: when spectating another player who is driving,
    // the camera target is their vehicle, not the local player's.
    CCamera* pCamera = pGame ? pGame->GetCamera() : nullptr;
    if (pCamera)
    {
        CEntity* pTargetEntity = pCamera->GetTargetEntity();
        if (pTargetEntity && pTargetEntity->GetEntityType() == ENTITY_TYPE_VEHICLE)
        {
            pVehicle = pTargetEntity->GetInterface();
        }
    }

    // Fall back to local player's vehicle if the camera isn't targeting a vehicle.
    if (!pVehicle)
    {
        pVehicle = FindPlayerVehicle(-1, false);
    }

    // No vehicle to derive speed from: restore stock defaults so the camera collides with everything.
    if (!pVehicle)
    {
        MemPutFast<float>(VAR_RelVelCamCollisionVehSqr, 1.0f);
        MemPutFast<char>(VAR_CameraClipDynamicObjects, 1);
        return;
    }

    // Apply camera clipping for dynamic objects
    // CPhysicalSAInterface::m_vecLinearVelocity at offset 0x44 (CVector: 3 floats)
    float* pSpeed = reinterpret_cast<float*>(static_cast<char*>(pVehicle) + 0x44);
    float  speedSq = pSpeed[0] * pSpeed[0] + pSpeed[1] * pSpeed[1] + pSpeed[2] * pSpeed[2];
    bool   slow = speedSq <= (0.2f * 0.2f);

    MemPutFast<float>(VAR_RelVelCamCollisionVehSqr, slow ? 0.1f : 1.0f);
    MemPutFast<char>(VAR_CameraClipDynamicObjects, slow ? 1 : 0);
}

static void _cdecl DoCameraCollisionDetectionPokes()
{
    const uint8_t mask = s_cameraClipMask.load(std::memory_order_relaxed);

    // Objects clip on = GTA default (speed-dependent dynamic, always-on static); off = force off.
    if (mask & static_cast<uint8_t>(CameraClipFlags::Objects))
        ApplyVehicleSpeedCameraClip();
    else
    {
        MemPutFast<char>(VAR_CameraClipDynamicObjects, 0);
        MemPutFast<char>(VAR_CameraClipStaticObjects, 0);
    }

    // Vehicles clip on = GTA default (always on); off = force off.
    if (mask & static_cast<uint8_t>(CameraClipFlags::Vehicles))
        MemPutFast<char>(VAR_CameraClipVehicles, 1);
    else
        MemPutFast<char>(VAR_CameraClipVehicles, 0);
}

static void __declspec(naked) HOOK_Camera_CollisionDetection()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    DoCameraCollisionDetectionPokes
        popad

        sub     esp, 24h
        push    ebx
        push    ebp
        jmp     RETURN_Camera_CollisionDetection
    }
    // clang-format on
}

BYTE CCameraSA::GetCameraVehicleViewMode()
{
    return *(BYTE*)VAR_VehicleCameraView;
}

BYTE CCameraSA::GetCameraPedViewMode()
{
    return *(BYTE*)VAR_PedCameraView;
}

void CCameraSA::SetCameraVehicleViewMode(BYTE dwCamMode)
{
    MemPutFast<BYTE>(VAR_VehicleCameraView, dwCamMode);
}

void CCameraSA::SetCameraPedViewMode(BYTE dwCamMode)
{
    MemPutFast<BYTE>(VAR_PedCameraView, dwCamMode);
}

// The game's camera follows the ped like the local player (see SpectatedPedScope); null restores the local player.
// Always restores, since TakeControl turns the view mode cycling and the cinematic camera off
void CCameraSA::SetSpectatedPed(CPed* ped, bool smoothTransition)
{
    if (!GetInterface())
        return;

    auto* const pedSA = dynamic_cast<CPedSA*>(ped);
    s_spectatedPed = pedSA ? pedSA->GetPedInterface() : nullptr;

    // Restore picks its target through FindPlayerPed and FindPlayerVehicle, so the slot must hold the ped
    const SpectatedPedScope spectatedPed;
    if (smoothTransition)
        Restore();
    else
        RestoreWithJumpCut();
}

// The slot must not keep a ped that is about to be destroyed
void CCameraSA::ForgetSpectatedPed() noexcept
{
    s_spectatedPed = nullptr;
}

bool CCameraSA::IsSpectatedPedInVehicle() const noexcept
{
    return s_spectatedPed && s_spectatedPed->pedFlags.bInVehicle && s_spectatedPed->pVehicle;
}

// MTA skips CHud::Draw while following another entity, which drops the crosshair; draws it as the spectated ped
// under the conditions of CHud::Draw (no widescreen, no cinematic camera this frame)
void CCameraSA::DrawSpectatedPedCrossHairs()
{
    const CCameraSAInterface* const cameraInterface = GetInterface();
    if (!cameraInterface || !s_spectatedPed || cameraInterface->m_WideScreenOn)
        return;

    if (*reinterpret_cast<std::uint32_t*>(VAR_CinematicCameraProcessedFrame) == *reinterpret_cast<std::uint32_t*>(VAR_CTimer_FrameCounter))
        return;

    // RenderWare render states and values in the order CHud::Draw sets them (0x58FB1E)
    const auto              setRenderState = reinterpret_cast<int(__cdecl*)(std::uint32_t, std::uint32_t)>(FUNC_RwRenderStateSet);
    constexpr std::uint32_t textureRaster = 1, textureAddress = 2, shadeMode = 7, textureFilter = 9, srcBlend = 10, destBlend = 11, vertexAlphaEnable = 12,
                            fogEnable = 14, alphaTestFunction = 29, alphaTestFunctionRef = 30;
    constexpr std::uint32_t filterNearest = 1, filterLinear = 2, blendSrcAlpha = 5, blendInvSrcAlpha = 6, addressClamp = 3, shadeFlat = 1, alphaTestGreater = 5;
    setRenderState(textureFilter, filterNearest);
    setRenderState(vertexAlphaEnable, 0);
    setRenderState(fogEnable, 0);
    setRenderState(srcBlend, blendSrcAlpha);
    setRenderState(destBlend, blendInvSrcAlpha);
    setRenderState(vertexAlphaEnable, 1);
    setRenderState(textureFilter, filterLinear);
    setRenderState(textureAddress, addressClamp);
    setRenderState(textureRaster, 0);
    setRenderState(shadeMode, shadeFlat);
    setRenderState(alphaTestFunction, alphaTestGreater);
    setRenderState(alphaTestFunctionRef, 0);

    const SpectatedPedScope spectatedPed;
    reinterpret_cast<void(__cdecl*)()>(FUNC_CHud_DrawCrossHairs)();
}

void CCameraSA::SetShakeForce(float fShakeForce)
{
    CCameraSAInterface* pCameraInterface = GetInterface();
    if (!pCameraInterface)
        return;
    pCameraInterface->m_fCamShakeForce = fShakeForce;
}

float CCameraSA::GetShakeForce()
{
    CCameraSAInterface* pCameraInterface = GetInterface();
    if (!pCameraInterface)
        return 0.0f;
    return pCameraInterface->m_fCamShakeForce;
}

void CCameraSA::ShakeCamera(float radius, float x, float y, float z) noexcept
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;
    if (radius <= 0.0f)
        return ResetShakeCamera();

    using ShakeCamera_t = void(__thiscall*)(CCameraSAInterface*, float radius, float x, float y, float z);
    ((ShakeCamera_t)FUNC_ShakeCam)(cameraInterface, radius, x, y, z);
}

void CCameraSA::ResetShakeCamera() noexcept
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return;
    cameraInterface->m_fCamShakeForce = 0.0f;
}

std::uint8_t CCameraSA::GetTransitionState() const
{
    const CCameraSAInterface* cameraInterface = GetInterface();
    return cameraInterface ? cameraInterface->m_uiTransitionState : 0;
}

bool CCameraSA::IsInTransition() const
{
    return GetTransitionState() != 0;
}

float CCameraSA::GetTransitionFOV() const
{
    CCameraSAInterface* cameraInterface = GetInterface();
    return cameraInterface ? cameraInterface->FOVDuringInter : DEFAULT_FOV;
}

bool CCameraSA::GetTransitionMatrix(CMatrix& matrix) const
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface || !IsInTransition())
        return false;

    CVector source = cameraInterface->SourceDuringInter;
    CVector target = cameraInterface->TargetDuringInter;
    CVector up = cameraInterface->UpDuringInter;

    if (!IsFiniteVector(source) || !IsFiniteVector(target) || !IsFiniteVector(up))
        return false;

    CVector forward = target - source;
    if (forward.Length() < FLOAT_EPSILON)
        forward = CVector(0.0f, 1.0f, 0.0f);
    else
        forward.Normalize();

    CVector right = CVector(forward.fY, -forward.fX, 0.0f);
    if (right.Length() < FLOAT_EPSILON)
        right = CVector(1.0f, 0.0f, 0.0f);
    else
        right.Normalize();

    CVector correctedUp = right;
    correctedUp.CrossProduct(&forward);
    correctedUp.Normalize();

    matrix.vPos = source;
    matrix.vFront = forward;
    matrix.vRight = -right;
    matrix.vUp = correctedUp;
    matrix.OrthoNormalize(CMatrix::AXIS_FRONT, CMatrix::AXIS_UP);

    return true;
}

bool CCameraSA::IsSphereVisible(CVector* center, float radius) const
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (!cameraInterface)
        return false;

    return ((bool(__thiscall*)(CCameraSAInterface*, CVector*, float))0x420D40)(cameraInterface, center, radius);
}
