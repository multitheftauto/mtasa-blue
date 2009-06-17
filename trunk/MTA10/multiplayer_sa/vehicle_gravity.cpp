/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/vehicle_gravity.cpp
*  PURPOSE:     vehicle gravity modification
*  DEVELOPERS:  arc_ <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define HOOKPOS_VehicleCamStart                             0x5245B7
DWORD RETURN_VehicleCamStart =                              0x5245BD;
#define HOOKPOS_VehicleCamLookDir1                          0x524DF1
DWORD RETURN_VehicleCamLookDir1 =                           0x524DF6;
#define HOOKPOS_VehicleCamLookDir2                          0x525B0E
DWORD RETURN_VehicleCamLookDir2 =                           0x525B73;
#define HOOKPOS_VehicleCamHistory                           0x525C56
DWORD RETURN_VehicleCamHistory =                            0x525D4A;
#define HOOKPOS_VehicleCamColDetect                         0x525D8D
DWORD RETURN_VehicleCamColDetect =                          0x525D92;
#define CALL_VehicleCamUp                                   0x525E1F
#define HOOKPOS_VehicleCamEnd                               0x525E43

#define HOOKPOS_VehicleLookBehind                           0x5207E3
DWORD RETURN_VehicleLookBehind =                            0x520891;
#define CALL_VehicleLookBehindUp                            0x520E2A
#define HOOKPOS_VehicleLookAside                            0x520F70
DWORD RETURN_VehicleLookAside =                             0x520FDC;
#define CALL_VehicleLookAsideUp                             0x5211E0

#define HOOKPOS_OccupiedVehicleBurnCheck                    0x570C84
DWORD RETURN_OccupiedVehicleBurnCheck =                     0x570C8A;
#define HOOKPOS_UnoccupiedVehicleBurnCheck                  0x6A76DC
DWORD RETURN_UnoccupiedVehicleBurnCheck =                   0x6A76E4;
#define HOOKPOS_ApplyCarBlowHop                             0x6B3816
DWORD RETURN_ApplyCarBlowHop =                              0x6B3831;

#define HOOKPOS_CPhysical_ApplyGravity                      0x543081
DWORD RETURN_CPhysical_ApplyGravity =                       0x543093;

void HOOK_CPhysical_ApplyGravity ();
void HOOK_VehicleCamStart ();
void HOOK_VehicleCamLookDir1 ();
void HOOK_VehicleCamLookDir2 ();
void HOOK_VehicleCamHistory ();
void HOOK_VehicleCamColDetect ();
void HOOK_VehicleCamUp ();
void HOOK_VehicleCamEnd ();
void HOOK_VehicleLookBehind ();
void HOOK_VehicleLookAside ();
void HOOK_OccupiedVehicleBurnCheck ();
void HOOK_UnoccupiedVehicleBurnCheck ();
void HOOK_ApplyCarBlowHop ();

void vehicle_gravity_init ( void )
{
    HookInstall(HOOKPOS_VehicleCamStart, (DWORD)HOOK_VehicleCamStart, 6);
    HookInstall(HOOKPOS_VehicleCamLookDir1, (DWORD)HOOK_VehicleCamLookDir1, 5);
    HookInstall(HOOKPOS_VehicleCamLookDir2, (DWORD)HOOK_VehicleCamLookDir2, 6);
    HookInstall(HOOKPOS_VehicleCamHistory, (DWORD)HOOK_VehicleCamHistory, 6);
    HookInstall(HOOKPOS_VehicleCamColDetect, (DWORD)HOOK_VehicleCamColDetect, 5);
    HookInstall(HOOKPOS_VehicleCamEnd, (DWORD)HOOK_VehicleCamEnd, 5);
    HookInstall(HOOKPOS_VehicleLookBehind, (DWORD)HOOK_VehicleLookBehind, 6);
    HookInstall(HOOKPOS_VehicleLookAside, (DWORD)HOOK_VehicleLookAside, 6);
    HookInstall(HOOKPOS_CPhysical_ApplyGravity, (DWORD)HOOK_CPhysical_ApplyGravity, 6);
    HookInstall(HOOKPOS_OccupiedVehicleBurnCheck, (DWORD)HOOK_OccupiedVehicleBurnCheck, 6);
    HookInstall(HOOKPOS_UnoccupiedVehicleBurnCheck, (DWORD)HOOK_UnoccupiedVehicleBurnCheck, 5);
    HookInstall(HOOKPOS_ApplyCarBlowHop, (DWORD)HOOK_ApplyCarBlowHop, 6);  

    HookInstallCall ( CALL_VehicleCamUp, (DWORD)HOOK_VehicleCamUp );
    HookInstallCall ( CALL_VehicleLookBehindUp, (DWORD)HOOK_VehicleCamUp );
    HookInstallCall ( CALL_VehicleLookAsideUp, (DWORD)HOOK_VehicleCamUp );
}


void _cdecl CPhysical_ApplyGravity ( DWORD dwThis )
{
    DWORD dwType;
    _asm
    {
        mov ecx, dwThis
        mov eax, 0x46A2C0       // CEntity::GetType
        call eax
        mov dwType, eax
    }

    float fTimeStep = *(float *)0xB7CB5C;
    float fGravity  = *(float *)0x863984;
    if ( dwType == 2 )
    {
        // It's a vehicle, use the gravity vector
        CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)dwThis );
        if ( !pVehicle )
            return;

        CVector vecGravity, vecMoveSpeed;
        pVehicle->GetGravity ( &vecGravity );
        pVehicle->GetMoveSpeed ( &vecMoveSpeed );
        vecMoveSpeed += vecGravity * fTimeStep * fGravity;
        pVehicle->SetMoveSpeed ( &vecMoveSpeed );
    }
    else
    {
        // It's something else, apply regular downward gravity (+0x4C == m_vecMoveSpeed.fZ)
        *(float *)(dwThis + 0x4C) -= fTimeStep * fGravity;
    }
}

void _declspec(naked) HOOK_CPhysical_ApplyGravity ()
{
    _asm
    {
        push esi
        call CPhysical_ApplyGravity
        add esp, 4
        jmp RETURN_CPhysical_ApplyGravity
    }
}

// ---------------------------------------------------

void GetMatrixForGravity ( const CVector& vecGravity, CMatrix& mat )
{
    // Calculates a basis where the z axis is the inverse of the gravity
    if ( vecGravity.Length () > 0.0001f )
    {
        mat.vUp = -vecGravity;
        mat.vUp.Normalize ();
        if ( abs(mat.vUp.fX) > 0.0001f || abs(mat.vUp.fZ) > 0.0001f )
        {
            CVector y ( 0.0f, 1.0f, 0.0f );
            mat.vFront = vecGravity;
            mat.vFront.CrossProduct ( &y );
            mat.vFront.CrossProduct ( &vecGravity );
            mat.vFront.Normalize ();
        }
        else
        {
            mat.vFront = CVector ( 0.0f, 0.0f, vecGravity.fY );
        }
        mat.vRight = mat.vFront;
        mat.vRight.CrossProduct ( &mat.vUp );
    }
    else
    {
        // No gravity, use default axes
        mat.vRight = CVector ( 1.0f, 0.0f, 0.0f );
        mat.vFront = CVector ( 0.0f, 1.0f, 0.0f );
        mat.vUp    = CVector ( 0.0f, 0.0f, 1.0f );
    }
}

// ---------------------------------------------------

CMatrix gravcam_matGravity;
CMatrix gravcam_matInvertGravity;
CMatrix gravcam_matVehicleTransform;
CVector gravcam_vecVehicleVelocity;

void _cdecl VehicleCamStart ( DWORD dwCam, DWORD pVehicleInterface )
{
    // Inverse transform some things so that they match a downward pointing gravity.
    // This way SA's gravity-goes-downward assumptive code can calculate the camera
    // spherical coords correctly. Of course we restore these after the camera function
    // completes.
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( !pVehicle )
        return;

    CVector vecGravity;
    pVehicle->GetGravity ( &vecGravity );

    GetMatrixForGravity ( vecGravity, gravcam_matGravity );
    gravcam_matInvertGravity = gravcam_matGravity;
    gravcam_matInvertGravity.Invert ();

    pVehicle->GetMatrix ( &gravcam_matVehicleTransform );
    CMatrix matVehicleInverted = gravcam_matInvertGravity * gravcam_matVehicleTransform;
    matVehicleInverted.vPos = gravcam_matVehicleTransform.vPos;
    pVehicle->SetMatrix ( &matVehicleInverted );

    pVehicle->GetMoveSpeed ( &gravcam_vecVehicleVelocity );
    CVector vecVelocityInverted = gravcam_matInvertGravity * gravcam_vecVehicleVelocity;
    pVehicle->SetMoveSpeed ( &vecVelocityInverted );
}

void _declspec(naked) HOOK_VehicleCamStart ()
{
    _asm
    {
        mov edi, [esi+0x21C]        // edi = camera target, i.e. the vehicle

        push edi
        push esi
        call VehicleCamStart
        add esp, 8
        jmp RETURN_VehicleCamStart
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamLookDir1 ( DWORD dwCam, DWORD pVehicleInterface )
{
    // For the same reason as in VehicleCamStart, inverse transform the camera's lookdir
    // at this point
    CVector* pvecLookDir = (CVector *)(dwCam + 0x190);
    *pvecLookDir = gravcam_matInvertGravity * (*pvecLookDir);
}

void _declspec(naked) HOOK_VehicleCamLookDir1 ()
{
    _asm
    {
        mov eax, 0x59C910       // CVector::Normalise
        call eax

        push edi
        push esi
        call VehicleCamLookDir1
        add esp, 8

        jmp RETURN_VehicleCamLookDir1
    }
}

// ---------------------------------------------------

bool _cdecl VehicleCamLookDir2 ( DWORD dwCam )
{
    // Calculates the look direction vector for the vehicle camera. This vector
    // is later multiplied by a factor and added to the vehicle position by SA
    // to obtain the final camera position.
    float fPhi   = *(float *)(dwCam + 0xBC);
    float fTheta = *(float *)(dwCam + 0xAC);

    *(CVector *)(dwCam + 0x190) = -gravcam_matGravity.vRight*cos(fPhi)*cos(fTheta) - gravcam_matGravity.vFront*sin(fPhi)*cos(fTheta) + gravcam_matGravity.vUp*sin(fTheta);

    *(float *)0x8CCEA8 = fPhi;
    return true;
}

void _declspec(naked) HOOK_VehicleCamLookDir2 ()
{
    _asm
    {
        push esi
        call VehicleCamLookDir2
        add esp, 4

        lea ebx, [esi+0x190]
        lea ebp, [esi+0x19C]
        push 4
        jmp RETURN_VehicleCamLookDir2
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamHistory ( DWORD dwCam, CVector* pvecTarget, float fTargetTheta, float fRadius, float fZoom )
{
    float fPhi = *(float *)(dwCam + 0xBC);
    CVector vecDir = -gravcam_matGravity.vRight*cos(fPhi)*cos(fTargetTheta) - gravcam_matGravity.vFront*sin(fPhi)*cos(fTargetTheta) + gravcam_matGravity.vUp*sin(fTargetTheta);
    ((CVector *)(dwCam + 0x1D8))[0] = *pvecTarget - vecDir*fRadius;
    ((CVector *)(dwCam + 0x1D8))[1] = *pvecTarget - vecDir*fZoom;
}

void _declspec(naked) HOOK_VehicleCamHistory ()
{
    _asm
    {
        push [esp+0x0+0x7C]       // zoom
        push [esp+0x4+0x2C]       // radius
        push [esp+0x8+0x14]       // targetTheta
        lea eax, [esp+0xC+0x48]
        push eax                  // pvecTarget
        push esi                  // pCam
        call VehicleCamHistory
        add esp, 0x14

        mov eax, [esp+0x24]
        jmp RETURN_VehicleCamHistory
    }
}

// ---------------------------------------------------

CVector* _cdecl VehicleCamColDetect ( CVector* pvecTarget )
{
    // Hack for collision detection placing the camera too close to the car
    // when using upside down gravity
    static CVector vecAdjustedTarget;
    if ( gravcam_matGravity.vUp.fZ < 0.0f )
    {
        vecAdjustedTarget = *pvecTarget + gravcam_matGravity.vUp;
        return &vecAdjustedTarget;
    }
    else
    {
        return pvecTarget;
    }
}

void _declspec(naked) HOOK_VehicleCamColDetect ()
{
    _asm
    {
        lea eax, [esp+0x48]
        push eax
        call VehicleCamColDetect
        add esp, 4

        push eax
        jmp RETURN_VehicleCamColDetect
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamUp ( DWORD dwCam )
{
    // Calculates the up vector for the vehicle camera.
    CVector* pvecUp = (CVector *)(dwCam + 0x1B4);
    CVector* pvecLookDir = (CVector *)(dwCam + 0x190);

    pvecLookDir->Normalize ();
    *pvecUp = *pvecLookDir;
    pvecUp->CrossProduct ( &gravcam_matGravity.vUp );
    pvecUp->CrossProduct ( pvecLookDir );
}

void _declspec(naked) HOOK_VehicleCamUp ()
{
    _asm
    {
        mov edx, ecx
        mov ecx, [ecx+0x21C]        // CCam::pTargetEntity
        mov eax, 0x46A2C0           // CEntity::GetType
        call eax

        cmp al, 2                   // Is it a vehicle?
        jz docustom
        
        mov ecx, edx
        mov eax, 0x509CE0           // CCam::GetVectorsReadyForRW
        jmp eax

docustom:
        push edx
        call VehicleCamUp
        add esp, 4
        ret
    }
}

// ---------------------------------------------------

void _cdecl VehicleCamEnd ( DWORD pVehicleInterface )
{
    // Restore the things that we inverse transformed in VehicleCamStart
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( !pVehicle )
        return;

    pVehicle->SetMatrix ( &gravcam_matVehicleTransform );
    pVehicle->SetMoveSpeed ( &gravcam_vecVehicleVelocity );
}

void _declspec(naked) HOOK_VehicleCamEnd ()
{
    _asm
    {
        push edi
        call VehicleCamEnd
        add esp, 4

        pop edi
        pop esi
        add esp, 0x68
        retn 0x14
    }
}

// ---------------------------------------------------

void _cdecl VehicleLookBehind ( DWORD dwCam, CVector* pvecEntityPos, float fDistance )
{
    // Custom calculation of the camera position when looking behind while in
    // vehicle cam mode, taking in account custom gravity
    *(CVector *)(dwCam + 0x19C) = *pvecEntityPos + (gravcam_matVehicleTransform.vFront + gravcam_matGravity.vUp*0.2f)*fDistance;
}

void _declspec(naked) HOOK_VehicleLookBehind ()
{
    _asm
    {
        push [esp+0x14]
        lea eax, [esp+4+0x1C]
        push eax
        push edi
        call VehicleLookBehind
        add esp, 0xC

        mov eax, [esp+0x1C]     // vecTemp = vecEntityPos
        mov ecx, [esp+0x1C+4]
        mov edx, [esp+0x1C+8]
        mov [esp+0x34],   eax
        mov [esp+0x34+4], ecx
        mov [esp+0x34+8], edx

        mov eax, 0xB7CD68       // IgnoreEntity
        mov [eax], ebx
        mov eax, 0xB6F028       // NumExtraIgnoreEntities
        mov [eax], 0

        mov eax, ebx            // pEntity
        jmp RETURN_VehicleLookBehind
    }
}

// ---------------------------------------------------

void _cdecl VehicleLookAside ( DWORD dwCam, CVector* pvecEntityPos, float fDirectionFactor, float fDistance )
{
    // Custom calculation of the camera position when looking left/right while in
    // vehicle cam mode, taking in account custom gravity
    *(CVector *)(dwCam + 0x19C) = *pvecEntityPos + (-gravcam_matVehicleTransform.vRight*fDirectionFactor + gravcam_matGravity.vUp*0.2f)*fDistance;
}

void _declspec(naked) HOOK_VehicleLookAside ()
{
    _asm
    {
        push [esp+0x14]
        push [esp+4+0x1C]
        lea eax, [esp+8+0x20]
        push eax
        push esi
        call VehicleLookAside
        add esp, 0x10

        lea ebp, [esi+0x19C]
        mov ecx, [esi+0x21C]
        jmp RETURN_VehicleLookAside
    }
}

// ---------------------------------------------------

float _cdecl VehicleBurnCheck ( DWORD pVehicleInterface )
{
    // To check if a vehicle is lying upside down on its roof, SA checks if the z coordinate
    // of the vehicle's up vector is negative. We replace this z by the dot product of the up vector
    // and the negated gravity vector.
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( !pVehicle )
        return 1.0f;

    CVector vecGravity;
    CMatrix matVehicle;
    pVehicle->GetGravity ( &vecGravity );
    pVehicle->GetMatrix ( &matVehicle );
    vecGravity = -vecGravity;
    return matVehicle.vUp.DotProduct ( &vecGravity );
}

void _declspec(naked) HOOK_OccupiedVehicleBurnCheck ()
{
    _asm
    {
        push eax
        call VehicleBurnCheck
        add esp, 4
        jmp RETURN_OccupiedVehicleBurnCheck
    }
}

void _declspec(naked) HOOK_UnoccupiedVehicleBurnCheck ()
{
    _asm
    {
        mov word ptr [esp+0x78], cx

        push esi
        call VehicleBurnCheck
        add esp, 4
        jmp RETURN_UnoccupiedVehicleBurnCheck
    }
}

// ---------------------------------------------------

void _cdecl ApplyVehicleBlowHop ( DWORD pVehicleInterface )
{
    // Custom application of the little jump that vehicles make when they blow up,
    // taking into account custom gravity
    CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pVehicleInterface );
    if ( !pVehicle )
        return;

    CVector vecGravity, vecVelocity;
    pVehicle->GetGravity ( &vecGravity );
    pVehicle->GetMoveSpeed ( &vecVelocity );
    vecVelocity -= vecGravity * 0.13f;
    pVehicle->SetMoveSpeed ( &vecVelocity );
}

void _declspec(naked) HOOK_ApplyCarBlowHop ()
{
    _asm
    {
        push esi
        call ApplyVehicleBlowHop
        add esp, 4

        mov dl, [esi+0x36]
        mov ecx, [esi+0x18]
        and dl, 7
        or dl, 0x28
        mov [esi+0x36], dl
        jmp RETURN_ApplyCarBlowHop
    }
}