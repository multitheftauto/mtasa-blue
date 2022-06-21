/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCameraSA.cpp
 *  PURPOSE:     Camera rendering
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
/**
 * \todo Need to add other TakeControl functions
 */

unsigned long CCameraSA::FUNC_RwFrameGetLTM;
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
    DEBUG_TRACE("CCameraSA::CCameraSA(CCameraSAInterface * cameraInterface)");
    this->internalInterface = cameraInterface;
    for (int i = 0; i < MAX_CAMS; i++)
        this->Cams[i] = new CCamSA(&this->internalInterface->Cams[i]);
    bCameraClipObjects = true;
    bCameraClipVehicles = true;
    HookInstall(HOOKPOS_Camera_CollisionDetection, (DWORD)HOOK_Camera_CollisionDetection, 5);
}

CCameraSA::~CCameraSA()
{
    for (int i = 0; i < MAX_CAMS; i++)
    {
        delete Cams[i];
    }
}

VOID CCameraSA::Restore()
{
    DEBUG_TRACE("VOID CCameraSA::Restore()");

    // CCamera::Restore
    ((void(__thiscall*)(CCameraSAInterface*))FUNC_Restore)(GetInterface());
}

VOID CCameraSA::RestoreWithJumpCut()
{
    DEBUG_TRACE("VOID CCameraSA::RestoreWithJumpCut()");

    // CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString
    ((void(__thiscall*)(CCameraSAInterface*))0x50BD40)(GetInterface());

    // CCamera::RestoreWithJumpCut
    ((void(__thiscall*)(CCameraSAInterface*))0x50BAB0)(GetInterface()); 
}

/**
 * \todo Find out what the last two paramters are
 */
VOID CCameraSA::TakeControl(CEntity* entity, eCamMode CamMode, int CamSwitchStyle)
{
    DEBUG_TRACE("VOID CCameraSA::TakeControl(CEntity * entity, eCamMode CamMode, int CamSwitchStyle)");

    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(entity);
    if (!pEntitySA)
        return;

    // CCamera::TakeControl
    ((void(__thiscall*)(CCameraSAInterface*, CEntitySAInterface*, short, short, int))FUNC_TakeControl)(GetInterface(), pEntitySA->GetInterface(), CamMode,
                                                                                                       CamSwitchStyle, SCRIPT_CAM_CONTROL);
}

VOID CCameraSA::TakeControl(CVector* position, int CamSwitchStyle)
{
    DEBUG_TRACE("VOID CCameraSA::TakeControl(CVector * position, int CamSwitchStyle)");

    CVector vecOffset;

    // CCamera::TakeControlNoEntity
    ((void(__thiscall*)(CCameraSAInterface*, CVector&, int, int))FUNC_TakeControlNoEntity)(GetInterface(), *position, CamSwitchStyle, SCRIPT_CAM_CONTROL);

    // CCamera::SetCamPositionForFixedMode
    ((void(__thiscall*)(CCameraSAInterface*, CVector&, CVector&))0x50BEC0)(GetInterface(), *position, vecOffset);
}

// vecOffset: an offset from 0,0,0
// vecLookAt: an offset from 0,0,0

VOID CCameraSA::TakeControlAttachToEntity(CEntity* TargetEntity, CEntity* AttachEntity, CVector* vecOffset, CVector* vecLookAt, float fTilt, int CamSwitchStyle)
{
    DEBUG_TRACE(
        "VOID CCameraSA::TakeControlAttachToEntity(CEntity * TargetEntity, CEntity * AttachEntity, CVector * vecOffset, CVector * vecLookAt, float fTilt, int "
        "CamSwitchStyle)");
    char                szDebug[255] = {'\0'};
    CEntitySAInterface* targetEntityInterface = 0;
    CEntitySAInterface* attachEntityInterface = 0;

    if (TargetEntity)
    {
        CEntitySA* pTargetEntitySA = dynamic_cast<CEntitySA*>(TargetEntity);
        if (pTargetEntitySA)
            targetEntityInterface = pTargetEntitySA->GetInterface();
    }

    if (AttachEntity)
    {
        CEntitySA* pAttachEntitySA = dynamic_cast<CEntitySA*>(AttachEntity);
        if (pAttachEntitySA)
            attachEntityInterface = pAttachEntitySA->GetInterface();
    }

    if (!attachEntityInterface)
        return;

    //  void    TakeControlAttachToEntity(CEntity* NewTarget, CEntity* AttachEntity,
    //  CVector& vecOffset, CVector& vecLookAt, float fTilt, short CamSwitchStyle,
    //  int WhoIsTakingControl=SCRIPT_CAM_CONTROL);

    // CCamera::TakeControlAttachToEntity
    ((void(__thiscall*)(CCameraSAInterface*, CEntitySAInterface*, CEntitySAInterface*, CVector&, CVector&, float, short, int))FUNC_TakeControlAttachToEntity)(
        GetInterface(), targetEntityInterface, attachEntityInterface, *vecOffset, *vecLookAt, fTilt, CamSwitchStyle, SCRIPT_CAM_CONTROL);
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
    DEBUG_TRACE("CMatrix * CCameraSA::GetMatrix ( CMatrix * matrix )");
    // CCameraSAInterface * pCamInterface = this->GetInterface();
    CMatrix_Padded* pCamMatrix = &this->GetInterface()->m_cameraMatrix;            // ->Placeable.matrix;
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

VOID CCameraSA::SetMatrix(CMatrix* matrix)
{
    DEBUG_TRACE("VOID CCameraSA::SetMatrix ( CMatrix * matrix )");
    CMatrix_Padded* pCamMatrix = this->GetInterface()->Placeable.matrix;
    if (pCamMatrix)
    {
        pCamMatrix->vFront = matrix->vFront;
        pCamMatrix->vPos = matrix->vPos;
        pCamMatrix->vUp = matrix->vUp;
        pCamMatrix->vRight = matrix->vRight;
    }
}

VOID CCameraSA::SetCamPositionForFixedMode(CVector* vecPosition, CVector* vecUpOffset)
{
    // CCamera::SetCamPositionForFixedMode
    ((void(__thiscall*)(CCameraSAInterface*, CVector&, CVector&))FUNC_SetCamPositionForFixedMode)(GetInterface(), *vecPosition, *vecUpOffset);
}

VOID CCameraSA::Find3rdPersonCamTargetVector(FLOAT fDistance, CVector* vecGunMuzzle, CVector* vecSource, CVector* vecTarget)
{
    DEBUG_TRACE("VOID CCameraSA::Find3rdPersonCamTargetVector ( FLOAT fDistance, CVector * vecGunMuzzle, CVector * vecSource, CVector * vecTarget )");

    // CCamera::Find3rdPersonCamTargetVector
    ((void(__thiscall*)(CCameraSAInterface*, float, float, float, float, CVector&, CVector&))FUNC_Find3rdPersonCamTargetVector)(
        GetInterface(), fDistance, vecGunMuzzle->fX, vecGunMuzzle->fY, vecGunMuzzle->fZ, *vecSource, *vecTarget);
}

float CCameraSA::Find3rdPersonQuickAimPitch()
{
    DEBUG_TRACE("float CCameraSA::Find3rdPersonQuickAimPitch ( void )");

    // CCamera::Find3rdPersonQuickAimPitch
    return ((float(__thiscall*)(CCameraSAInterface*))FUNC_Find3rdPersonQuickAimPitch)(GetInterface());
}

BYTE CCameraSA::GetActiveCam()
{
    DEBUG_TRACE("BYTE CCameraSA::GetActiveCam()");
    CCameraSAInterface* cameraInterface = this->GetInterface();
    return cameraInterface->ActiveCam;
}

CCam* CCameraSA::GetCam(BYTE bCameraID)
{
    DEBUG_TRACE("CCam * CCameraSA::GetCam(BYTE bCameraID)");

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

VOID CCameraSA::SetWidescreen(BOOL bWidescreen)
{
    DEBUG_TRACE("VOID CCameraSA::SetWidescreen(BOOL bWidescreen)");
    CCameraSAInterface* cameraInterface = this->GetInterface();
    if (bWidescreen == FALSE)
        cameraInterface->m_WideScreenOn = false;
    else
        cameraInterface->m_WideScreenOn = true;
}

BOOL CCameraSA::GetWidescreen()
{
    DEBUG_TRACE("BOOL CCameraSA::GetWidescreen()");
    CCameraSAInterface* cameraInterface = this->GetInterface();
    return cameraInterface->m_WideScreenOn;
}
/**
 * \todo Rewrite these functions to use m_nCarZoom presumeably
 */
float CCameraSA::GetCarZoom()
{
    DEBUG_TRACE("float CCameraSA::GetCarZoom()");
    /*  CCameraSAInterface * cameraInterface = this->GetInterface();
        char szDebug[255] = {'\0'};
        sprintf(szDebug, "%d", (DWORD)&cameraInterface->CarZoomIndicator - (DWORD)cameraInterface);
        OutputDebugString(szDebug);
        return cameraInterface->CarZoomIndicator;   */
    return NULL;
}
/**
 * \todo Check if this ever works?
 */
VOID CCameraSA::SetCarZoom(float fCarZoom)
{
    DEBUG_TRACE("VOID CCameraSA::SetCarZoom(float fCarZoom)");
    /*CCameraSAInterface * cameraInterface = this->GetInterface();
    cameraInterface->CarZoomIndicator = fCarZoom;   */
}

/**
 * \todo does this need an enum?
 * This eventually calls TakeControl if its a valid cam mode at that time
 */
bool CCameraSA::TryToStartNewCamMode(DWORD dwCamMode)
{
    DEBUG_TRACE("VOID CCameraSA::TryToStartNewCamMode(DWORD dwCamMode)");
    /*
    DWORD               dwFunc = FUNC_TryToStartNewCamMode;
    bool                bReturn = false;
    CCameraSAInterface* cameraInterface = this->GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    dwCamMode
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;*/
    return false;
}

bool CCameraSA::ConeCastCollisionResolve(CVector* pPos, CVector* pLookAt, CVector* pDest, float rad, float minDist, float* pDist)
{
    // CCamera::ConeCastCollisionResolve
    return ((bool(__thiscall*)(CCameraSAInterface*, CVector&, CVector&, CVector&, float, float, float*))FUNC_ConeCastCollisionResolve)(
        GetInterface(), *pPos, *pLookAt, *pDest, rad, minDist, pDist);
}

void CCameraSA::VectorTrackLinear(CVector* pTo, CVector* pFrom, float time, bool bSmoothEnds)
{
    // CCamera::VectorTrackLinear
    ((void(__thiscall*)(CCameraSAInterface*, CVector*, CVector*, float, bool))FUNC_VectorTrackLinear)(GetInterface(), pTo, pFrom, time, bSmoothEnds);
}

bool CCameraSA::IsFading()
{
    // CCamera::GetFading
    return ((bool(__thiscall*)(CCameraSAInterface*))FUNC_GetFading)(GetInterface());
}

int CCameraSA::GetFadingDirection()
{
    // CCamera::GetFadingDirection
    return ((int(__thiscall*)(CCameraSAInterface*))FUNC_GetFadingDirection)(GetInterface());
}

void CCameraSA::Fade(float fFadeOutTime, int iOutOrIn)
{
    // CCamera::Fade
    ((void(__thiscall*)(CCameraSAInterface*, float, int))FUNC_Fade)(GetInterface(), fFadeOutTime, iOutOrIn);
}

void CCameraSA::SetFadeColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    // CCamera::SetFadeColour
    ((void(__thiscall*)(CCameraSAInterface*, unsigned char, unsigned char, unsigned char))FUNC_SetFadeColour)(GetInterface(), ucRed, ucGreen, ucBlue);
}

float CCameraSA::GetCameraRotation()
{
    return *(float*)VAR_CameraRotation;
}

RwMatrix* CCameraSA::GetLTM()
{
    // RwFrameGetLTM
    return ((RwMatrix*(_cdecl*)(void*))FUNC_RwFrameGetLTM)(GetInterface()->m_pRwCamera->object.object.parent);
}

CEntity* CCameraSA::GetTargetEntity()
{
    CEntitySAInterface* pInterface = this->GetInterface()->pTargetEntity;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        return pPools->GetEntity((DWORD*)pInterface);
    }
    return nullptr;
}

void CCameraSA::SetCameraClip(bool bObjects, bool bVehicles)
{
    bCameraClipObjects = bObjects;
    bCameraClipVehicles = bVehicles;
}

void CCameraSA::GetCameraClip(bool& bObjects, bool& bVehicles)
{
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

VOID CCameraSA::SetCameraVehicleViewMode(BYTE dwCamMode)
{
    MemPutFast<BYTE>(VAR_VehicleCameraView, dwCamMode);
}

VOID CCameraSA::SetCameraPedViewMode(BYTE dwCamMode)
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
