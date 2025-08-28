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
#include <mutex>
#include <cmath>

extern CGameSA* pGame;

static std::mutex s_cameraClipMutex;
static bool   bCameraClipObjects;
static bool   bCameraClipVehicles;

#define VAR_CameraClipVehicles              0x8A5B14
#define VAR_CameraClipDynamicObjects        0x8A5B15
#define VAR_CameraClipStaticObjects         0x8A5B16

#define HOOKPOS_Camera_CollisionDetection   0x520190
DWORD RETURN_Camera_CollisionDetection = 0x520195;
void  HOOK_Camera_CollisionDetection();

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
        try {
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
    
    // Thread-safe initialization
    {
        std::lock_guard<std::mutex> lock(s_cameraClipMutex);
        bCameraClipObjects = true;
        bCameraClipVehicles = true;
    }
    
    HookInstall(HOOKPOS_Camera_CollisionDetection, (DWORD)HOOK_Camera_CollisionDetection, 5);
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
    DWORD               dwFunc = FUNC_Restore;
    CCameraSAInterface* cameraInterface = GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
}

void CCameraSA::RestoreWithJumpCut()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    DWORD               dwFunc = 0x50BD40;
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
    dwFunc = 0x50BAB0;
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
}

/**
 * \todo Find out what the last two paramters are
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

    if (CamSwitchStyle < 0 || CamSwitchStyle > 10)
        return;

    DWORD CCamera__TakeControl = FUNC_TakeControl;
    _asm
    {
        mov ecx, cameraInterface
        push 1
        push CamSwitchStyle
        push CamMode
        push entityInterface
        call CCamera__TakeControl
    }
}

void CCameraSA::TakeControl(CVector* position, int CamSwitchStyle)
{
    if (!position)
        return;
        
    CCameraSAInterface* cameraInterface = GetInterface();
    // __thiscall
    CVector vecOffset;
    /*  vecOffset.fZ = 0.5f;
        vecOffset.fY = 0.5f;
        vecOffset.fX = 0.5f;*/
    /*  DWORD dwFunc = 0x50BEC0;
        _asm
        {
            mov ecx, cameraInterface
            lea     eax, vecOffset
            push    eax
            push    position
            call    dwFunc
        }*/

    DWORD CCamera__TakeControlNoEntity = FUNC_TakeControlNoEntity;
    _asm
        {
        mov ecx, cameraInterface
        push 1
        push CamSwitchStyle
        push position
        call CCamera__TakeControlNoEntity
        }

    DWORD dwFunc = 0x50BEC0;
    _asm
    {
        mov ecx, cameraInterface
        lea     eax, vecOffset
        push    eax
        push    position
        call    dwFunc
    }
}

// LSOD recovery
void CCameraSA::RestoreLastGoodState()
{
    CMatrix defmat;
    SetMatrix(&defmat);

    CCameraSAInterface* pCameraInterface = GetInterface();

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
        
    CMatrix_Padded* pCamMatrix = &GetInterface()->m_cameraMatrix;
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
        
    float               fOriginX = vecGunMuzzle->fX;
    float               fOriginY = vecGunMuzzle->fY;
    float               fOriginZ = vecGunMuzzle->fZ;

    if (!std::isfinite(fOriginX) || !std::isfinite(fOriginY) || !std::isfinite(fOriginZ))
        return;
        
    DWORD               dwFunc = FUNC_Find3rdPersonCamTargetVector;
    CCameraSAInterface* cameraInterface = GetInterface();

    if (!cameraInterface)
        return;
        
    _asm
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
}

float CCameraSA::Find3rdPersonQuickAimPitch()
{
    float               fReturn;
    DWORD               dwFunc = FUNC_Find3rdPersonQuickAimPitch;
    CCameraSAInterface* cameraInterface = GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        fstp    fReturn
    }
    return fReturn;
}

BYTE CCameraSA::GetActiveCam()
{
    CCameraSAInterface* cameraInterface = GetInterface();
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
    GetInterface()->m_WideScreenOn = bWidescreen;
}

bool CCameraSA::GetWidescreen()
{
    CCameraSAInterface* cameraInterface = GetInterface();
    return cameraInterface->m_WideScreenOn;
}

bool CCameraSA::IsFading()
{
    DWORD               dwFunc = FUNC_GetFading;
    CCameraSAInterface* cameraInterface = GetInterface();
    bool                bRet = false;
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        mov     bRet, al
    }
    return bRet;
}

int CCameraSA::GetFadingDirection()
{
    DWORD               dwFunc = FUNC_GetFadingDirection;
    CCameraSAInterface* cameraInterface = GetInterface();
    int                 dwRet = false;
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        mov     dwRet, eax
    }
    return dwRet;
}

void CCameraSA::Fade(float fFadeOutTime, int iOutOrIn)
{
    if (!std::isfinite(fFadeOutTime) || fFadeOutTime < 0.0f || fFadeOutTime > 60.0f)
        return;
        
    if (iOutOrIn < 0 || iOutOrIn > 1)
        return;
        
    DWORD               dwFunc = FUNC_Fade;
    CCameraSAInterface* cameraInterface = GetInterface();
    
    if (!cameraInterface)
        return;
        
    _asm
    {
        mov     ecx, cameraInterface
        push    iOutOrIn
        push    fFadeOutTime
        call    dwFunc
    }
}

void CCameraSA::SetFadeColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    DWORD               dwFunc = FUNC_SetFadeColour;
    CCameraSAInterface* cameraInterface = GetInterface();
    DWORD               dwRed = ucRed;
    DWORD               dwGreen = ucGreen;
    DWORD               dwBlue = ucBlue;
    _asm
    {
        mov     ecx, cameraInterface
        push    dwBlue
        push    dwGreen
        push    dwRed
        call    dwFunc
    }
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
    return ((RwMatrix*(_cdecl*)(void*))0x7F0990)(cameraInterface->m_pRwCamera->object.object.parent);
}

CEntity* CCameraSA::GetTargetEntity()
{
    CEntitySAInterface* pInterface = GetInterface()->pTargetEntity;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        return pPools->GetEntity((DWORD*)pInterface);
    }
    return nullptr;
}

void CCameraSA::SetCameraClip(bool bObjects, bool bVehicles)
{
    // Thread-safe access to static variables
    std::lock_guard<std::mutex> lock(s_cameraClipMutex);
    bCameraClipObjects = bObjects;
    bCameraClipVehicles = bVehicles;
}

void CCameraSA::GetCameraClip(bool& bObjects, bool& bVehicles)
{
    // Thread-safe access to static variables
    std::lock_guard<std::mutex> lock(s_cameraClipMutex);
    bObjects = bCameraClipObjects;
    bVehicles = bCameraClipVehicles;
}

__declspec(noinline) void _cdecl DoCameraCollisionDetectionPokes()
{
    if (!bCameraClipObjects)
    {
        MemPutFast<char>(VAR_CameraClipDynamicObjects, 0);
        MemPutFast<char>(VAR_CameraClipStaticObjects, 0);
    }
    else
        MemPutFast<char>(VAR_CameraClipStaticObjects, 1);

    if (!bCameraClipVehicles)
        MemPutFast<char>(VAR_CameraClipVehicles, 0);
}

void _declspec(naked) HOOK_Camera_CollisionDetection()
{
    _asm
    {
        pushad
        call DoCameraCollisionDetectionPokes
        popad
        sub         esp,24h
        push        ebx
        push        ebp
        jmp         RETURN_Camera_CollisionDetection
    }
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

void CCameraSA::SetShakeForce(float fShakeForce)
{
    CCameraSAInterface* pCameraInterface = GetInterface();
    pCameraInterface->m_fCamShakeForce = fShakeForce;
}

float CCameraSA::GetShakeForce()
{
    CCameraSAInterface* pCameraInterface = GetInterface();
    return pCameraInterface->m_fCamShakeForce;
}

void CCameraSA::ShakeCamera(float radius, float x, float y, float z) noexcept
{
    CCameraSAInterface* cameraInterface = GetInterface();
    if (radius <= 0.0f)
        return ResetShakeCamera();

    using ShakeCamera_t = void(__thiscall*)(CCameraSAInterface*, float radius, float x, float y, float z);
    ((ShakeCamera_t)FUNC_ShakeCam)(cameraInterface, radius, x, y, z);
}

void CCameraSA::ResetShakeCamera() noexcept
{
    GetInterface()->m_fCamShakeForce = 0.0f;
}

std::uint8_t CCameraSA::GetTransitionState() const
{
    return GetInterface()->m_uiTransitionState;
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
