/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCameraSA.cpp
*  PURPOSE:     Camera rendering
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
/**
 * \todo Need to add other TakeControl functions
 */

unsigned long CCameraSA::FUNC_RwFrameGetLTM;
static bool bCameraClipObjects;
static bool bCameraClipVehicles;

#define VAR_CameraClipVehicles              0x8A5B14
#define VAR_CameraClipDynamicObjects        0x8A5B15
#define VAR_CameraClipStaticObjects         0x8A5B16

#define HOOKPOS_Camera_CollisionDetection   0x520190
DWORD RETURN_Camera_CollisionDetection =    0x520195;
void HOOK_Camera_CollisionDetection ();

CCameraSA::CCameraSA(CCameraSAInterface * cameraInterface)
{ 
    DEBUG_TRACE("CCameraSA::CCameraSA(CCameraSAInterface * cameraInterface)");
    this->internalInterface = cameraInterface;
    for(int i = 0; i<MAX_CAMS;i++)
        this->Cams[i] = new CCamSA(&this->internalInterface->Cams[i]);
    bCameraClipObjects = true;
    bCameraClipVehicles = true;
    HookInstall(HOOKPOS_Camera_CollisionDetection, (DWORD)HOOK_Camera_CollisionDetection, 5 );
}

CCameraSA::~CCameraSA ( void )
{
    for ( int i = 0; i < MAX_CAMS; i++ )
    {
        delete Cams [i];
    }
}

VOID CCameraSA::Restore()
{
    DEBUG_TRACE("VOID CCameraSA::Restore()");    
    DWORD dwFunc = FUNC_Restore;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
}

VOID CCameraSA::RestoreWithJumpCut()
{
    DEBUG_TRACE("VOID CCameraSA::RestoreWithJumpCut()");
    CCameraSAInterface * cameraInterface = this->GetInterface();
    DWORD dwFunc = 0x50BD40;
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
VOID CCameraSA::TakeControl(CEntity * entity, eCamMode CamMode, int CamSwitchStyle)
{
    DEBUG_TRACE("VOID CCameraSA::TakeControl(CEntity * entity, eCamMode CamMode, int CamSwitchStyle)");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( entity );
    if ( !pEntitySA ) return;

    CEntitySAInterface * entityInterface = pEntitySA->GetInterface();
    CCameraSAInterface * cameraInterface = this->GetInterface();
    // __thiscall

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

VOID CCameraSA::TakeControl(CVector * position, int CamSwitchStyle)
{
    DEBUG_TRACE("VOID CCameraSA::TakeControl(CVector * position, int CamSwitchStyle)");
    CCameraSAInterface * cameraInterface = this->GetInterface();
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

// vecOffset: an offset from 0,0,0
// vecLookAt: an offset from 0,0,0

VOID CCameraSA::TakeControlAttachToEntity(CEntity * TargetEntity, CEntity * AttachEntity, 
                                          CVector * vecOffset, CVector * vecLookAt, 
                                          float fTilt, int CamSwitchStyle)
{
    DEBUG_TRACE("VOID CCameraSA::TakeControlAttachToEntity(CEntity * TargetEntity, CEntity * AttachEntity, CVector * vecOffset, CVector * vecLookAt, float fTilt, int CamSwitchStyle)");
    char szDebug[255] = {'\0'};
    CEntitySAInterface * targetEntityInterface = 0;
    CEntitySAInterface * attachEntityInterface = 0;

    if(TargetEntity)
    {
        CEntitySA* pTargetEntitySA = dynamic_cast < CEntitySA* > ( TargetEntity );
        if ( pTargetEntitySA )
            targetEntityInterface = pTargetEntitySA->GetInterface ();
    }

    if(AttachEntity)
    {
        CEntitySA* pAttachEntitySA = dynamic_cast < CEntitySA* > ( AttachEntity );
        if ( pAttachEntitySA )
            attachEntityInterface = pAttachEntitySA->GetInterface ();
    }
    
    if(!attachEntityInterface)
        return;

    CCameraSAInterface * cameraInterface = this->GetInterface();
    // __thiscall

    //  void    TakeControlAttachToEntity(CEntity* NewTarget, CEntity* AttachEntity, 
    //  CVector& vecOffset, CVector& vecLookAt, float fTilt, short CamSwitchStyle, 
    //  int WhoIsTakingControl=SCRIPT_CAM_CONTROL);
    DWORD CCamera__TakeControlAttachToEntity = FUNC_TakeControlAttachToEntity;

    
    _asm 
    {
        mov ecx, cameraInterface
        push 1
        push CamSwitchStyle
        push fTilt
        push vecLookAt
        push vecOffset
        push attachEntityInterface
        push targetEntityInterface
        call CCamera__TakeControlAttachToEntity
    }
}

CMatrix * CCameraSA::GetMatrix ( CMatrix * matrix )
{
    DEBUG_TRACE("CMatrix * CCameraSA::GetMatrix ( CMatrix * matrix )");
    //_asm int 3
    //CCameraSAInterface * pCamInterface = this->GetInterface();
    CMatrix_Padded * pCamMatrix = &this->GetInterface()->m_cameraMatrix; // ->Placeable.matrix;
    if ( pCamMatrix )
    {
        MemCpy (&matrix->vFront,     &pCamMatrix->vFront,    sizeof(CVector));
        MemCpy (&matrix->vPos,           &pCamMatrix->vPos,          sizeof(CVector));
        MemCpy (&matrix->vUp,            &pCamMatrix->vUp,           sizeof(CVector));
        MemCpy (&matrix->vRight,         &pCamMatrix->vRight,            sizeof(CVector));   
    }
    else
    {
        MemSet (matrix, 0, sizeof(CMatrix));
    }
    return matrix;
}

VOID CCameraSA::SetMatrix ( CMatrix * matrix )
{
    DEBUG_TRACE("VOID CCameraSA::SetMatrix ( CMatrix * matrix )");
    CMatrix_Padded * pCamMatrix = this->GetInterface()->Placeable.matrix;
    if ( pCamMatrix )
    {
        MemCpy (&pCamMatrix->vFront,     &matrix->vFront,    sizeof(CVector));
        MemCpy (&pCamMatrix->vPos,           &matrix->vPos,          sizeof(CVector));
        MemCpy (&pCamMatrix->vUp,            &matrix->vUp,           sizeof(CVector));
        MemCpy (&pCamMatrix->vRight,         &matrix->vRight,            sizeof(CVector));
    }   
}

VOID CCameraSA::SetCamPositionForFixedMode ( CVector * vecPosition, CVector * vecUpOffset )
{
    DWORD dwFunc = FUNC_SetCamPositionForFixedMode;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    vecUpOffset
        push    vecPosition
        call    dwFunc
    }
}

VOID CCameraSA::Find3rdPersonCamTargetVector ( FLOAT fDistance, CVector * vecGunMuzzle, CVector * vecSource, CVector * vecTarget )
{
    DEBUG_TRACE("VOID CCameraSA::Find3rdPersonCamTargetVector ( FLOAT fDistance, CVector * vecGunMuzzle, CVector * vecSource, CVector * vecTarget )");
    FLOAT fOriginX = vecGunMuzzle->fX;
    FLOAT fOriginY = vecGunMuzzle->fY;
    FLOAT fOriginZ = vecGunMuzzle->fZ;
    DWORD dwFunc = FUNC_Find3rdPersonCamTargetVector;
    CCameraSAInterface * cameraInterface = this->GetInterface();
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

float CCameraSA::Find3rdPersonQuickAimPitch ( void )
{
    DEBUG_TRACE("float CCameraSA::Find3rdPersonQuickAimPitch ( void )");
    float fReturn;
    DWORD dwFunc = FUNC_Find3rdPersonQuickAimPitch;
    CCameraSAInterface * cameraInterface = this->GetInterface();
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
    DEBUG_TRACE("BYTE CCameraSA::GetActiveCam()");
    CCameraSAInterface * cameraInterface = this->GetInterface();
    return cameraInterface->ActiveCam;
}

CCam * CCameraSA::GetCam(BYTE bCameraID)
{
    DEBUG_TRACE("CCam * CCameraSA::GetCam(BYTE bCameraID)");
    
    if(bCameraID<MAX_CAMS)
        return Cams[bCameraID];

    return NULL;
}


CCam* CCameraSA::GetCam ( CCamSAInterface* camInterface )
{
    for ( int i = 0; i < MAX_CAMS; i++ )
    {
        if ( Cams [i] && Cams [i]->GetInterface () == camInterface )
        {
            return Cams [i];
        }
    }

    return NULL;
}


VOID CCameraSA::SetWidescreen(BOOL bWidescreen)
{
    DEBUG_TRACE("VOID CCameraSA::SetWidescreen(BOOL bWidescreen)");
    CCameraSAInterface * cameraInterface = this->GetInterface();
    if(bWidescreen == FALSE)
        cameraInterface->m_WideScreenOn = false;
    else
        cameraInterface->m_WideScreenOn = true;
}

BOOL CCameraSA::GetWidescreen()
{
    DEBUG_TRACE("BOOL CCameraSA::GetWidescreen()");
    CCameraSAInterface * cameraInterface = this->GetInterface();
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
    DWORD dwFunc = FUNC_TryToStartNewCamMode;
    bool bReturn = false;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    dwCamMode
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CCameraSA::ConeCastCollisionResolve(CVector *pPos, CVector *pLookAt, CVector *pDest, float rad, float minDist, float *pDist)
{
    DWORD dwFunc = FUNC_ConeCastCollisionResolve;
    bool bReturn = false;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    pDist
        push    minDist
        push    rad
        push    pDest
        push    pLookAt
        push    pPos
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CCameraSA::VectorTrackLinear ( CVector * pTo, CVector * pFrom, float time, bool bSmoothEnds )
{
    DWORD dwFunc = FUNC_VectorTrackLinear;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    bSmoothEnds
        push    time
        push    pFrom
        push    pTo
        call    dwFunc
    }
}

bool CCameraSA::IsFading ( void )
{
    DWORD dwFunc = FUNC_GetFading;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    bool bRet = false;
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        mov     bRet, al
    }
    return bRet;
}

int CCameraSA::GetFadingDirection ( void )
{
    DWORD dwFunc = FUNC_GetFadingDirection;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    int dwRet = false;
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        mov     dwRet, eax
    }
    return dwRet;
}

void CCameraSA::Fade ( float fFadeOutTime, int iOutOrIn )
{
    DWORD dwFunc = FUNC_Fade;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    iOutOrIn
        push    fFadeOutTime
        call    dwFunc
    }
}

void CCameraSA::SetFadeColor ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    DWORD dwFunc = FUNC_SetFadeColour;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    DWORD dwRed = ucRed;
    DWORD dwGreen = ucGreen;
    DWORD dwBlue = ucBlue;
    _asm
    {
        mov     ecx, cameraInterface
        push    dwBlue
        push    dwGreen
        push    dwRed
        call    dwFunc
    }
}

float CCameraSA::GetCameraRotation ( void )
{
    return *(float *)VAR_CameraRotation;
}

RwMatrix * CCameraSA::GetLTM ( void )
{
    DWORD frame = *(DWORD *)(((DWORD)this->GetInterface()->m_pRwCamera) + 4);
    DWORD dwReturn;
    _asm
    {
        push    frame
        call    FUNC_RwFrameGetLTM
        add     esp, 4
        mov     dwReturn, eax
    }
    return (RwMatrix *)dwReturn;
}

CEntity * CCameraSA::GetTargetEntity ( void )
{
    CEntitySAInterface * pInterface = this->GetInterface()->pTargetEntity;
    CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
    CEntity * pReturn = NULL;

    if ( pPools && pInterface )
    {
        switch ( pInterface->nType )
        {
            case ENTITY_TYPE_PED:
                pReturn = (CEntity*)(pPools->GetPed((DWORD *)pInterface));
                break;
            case ENTITY_TYPE_VEHICLE:
                pReturn = (CEntity*)(pPools->GetVehicle((DWORD *)pInterface));
                break;
            case ENTITY_TYPE_OBJECT:
                pReturn = (CEntity*)(pPools->GetObject ((DWORD *)pInterface));
                break;
            default:
                break;
        }
    }
    return pReturn;
}

void CCameraSA::SetCameraClip ( bool bObjects, bool bVehicles )
{
    bCameraClipObjects = bObjects;
    bCameraClipVehicles = bVehicles;
}


void _cdecl DoCameraCollisionDetectionPokes ()
{
    if ( !bCameraClipObjects )
    {
        MemPut < char > ( VAR_CameraClipDynamicObjects, 0 );  //         *(char*)VAR_CameraClipDynamicObjects = 0;
        MemPut < char > ( VAR_CameraClipStaticObjects, 0 );  //         *(char*)VAR_CameraClipStaticObjects = 0;
    }
    else
        MemPut < char > ( VAR_CameraClipStaticObjects, 1 );  //         *(char*)VAR_CameraClipStaticObjects = 1;

    if ( !bCameraClipVehicles )
        MemPut < char > ( VAR_CameraClipVehicles, 0 );  //         *(char*)VAR_CameraClipVehicles = 0;
}

void _declspec(naked) HOOK_Camera_CollisionDetection ()
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

