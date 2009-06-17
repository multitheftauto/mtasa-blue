/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/entity_alpha.cpp
*  PURPOSE:     entity alpha modification
*  DEVELOPERS:  Ryden <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define HOOKPOS_CVehicle_SetupRender        0x6D6512
#define HOOKPOS_CVehicle_ResetAfterRender   0x6D0E3E
#define HOOKPOS_CObject_Render              0x59F1ED
DWORD FUNC_CEntity_Render =                 0x534310;
   
void HOOK_CVehicle_SetupRender ();
void HOOK_CVehicle_ResetAfterRender();
void HOOK_CObject_Render ();

void entity_alpha_init ( void )
{
    HookInstall(HOOKPOS_CVehicle_SetupRender, (DWORD)HOOK_CVehicle_SetupRender, 5);
    HookInstall(HOOKPOS_CVehicle_ResetAfterRender, (DWORD)HOOK_CVehicle_ResetAfterRender, 5);
    HookInstall(HOOKPOS_CObject_Render, (DWORD)HOOK_CObject_Render, 5);
	
    // Stop CPlayerPed::ProcessControl from calling CVisibilityPlugins::SetClumpAlpha
    memset ( (void*)0x5E8E84, 0x90, 5 );

    // Stop CVehicle::UpdateClumpAlpha from calling CVisibilityPlugins::SetClumpAlpha
    memset ( (void*)0x6D29CB, 0x90, 5 );
}


/**
 ** Per-entity alpha
 **/
DWORD dwEAEG = 0;
static DWORD dwAlphaEntity = 0;
static bool bEntityHasAlpha = false;
static unsigned char ucCurrentAlpha [ 1024 ];
static unsigned char* pCurAlpha = ucCurrentAlpha;

static void SetEntityAlphaHooked ( DWORD dwEntity, DWORD dwCallback, DWORD dwAlpha )
{
    if ( dwEntity )
    {
        // Alpha setting of SetRwObjectAlpha function is done by
        // iterating all materials of a clump and its atoms, and
        // calling a given callback. We temporarily overwrite that
        // callback with our own callback and then restore it.
        *(DWORD *)(0x5332A2) = dwCallback;
        *(DWORD *)(0x5332F3) = dwCallback;

        // Call SetRwObjectAlpha
        DWORD dwFunc = FUNC_SetRwObjectAlpha;
        _asm
        {
            mov     ecx, dwEntity
            push    dwAlpha
            call    dwFunc
        }

        // Restore the GTA callbacks
        *(DWORD *)(0x5332A2) = (DWORD)(0x533280);
        *(DWORD *)(0x5332F3) = (DWORD)(0x533280);
    }
}

static RpMaterial* HOOK_GetAlphaValues ( RpMaterial* pMaterial, unsigned char ucAlpha )
{
    *pCurAlpha = pMaterial->color.a;
    pCurAlpha++;

    return pMaterial;
}
static RpMaterial* HOOK_SetAlphaValues ( RpMaterial* pMaterial, unsigned char ucAlpha )
{
    pMaterial->color.a = static_cast < unsigned char > ( (float)(pMaterial->color.a) * (float)ucAlpha / 255.0f );

    return pMaterial;
}
static RpMaterial* HOOK_RestoreAlphaValues ( RpMaterial* pMaterial, unsigned char ucAlpha )
{
    pMaterial->color.a = *pCurAlpha;
    pCurAlpha++;

    return pMaterial;
}

static void GetAlphaAndSetNewValues ( unsigned char ucAlpha )
{
    if ( ucAlpha < 255 )
    {
        bEntityHasAlpha = true;
        pCurAlpha = ucCurrentAlpha;
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_GetAlphaValues, 0 );
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_SetAlphaValues, ucAlpha );
    }
    else
        bEntityHasAlpha = false;
}
static void RestoreAlphaValues ()
{
    if ( bEntityHasAlpha )
    {
        pCurAlpha = ucCurrentAlpha;
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_RestoreAlphaValues, 0 );
    }
}


/**
 ** Vehicles
 **/
static RpAtomic* CVehicle_EAEG ( RpAtomic* pAtomic, void* )
{
    RwFrame* pFrame = ((RwFrame*)(((RwObject *)(pAtomic))->parent));
    if ( pFrame )
    {
        switch ( pFrame->szName[0] )
        {
            case '\0': case 'h': break;
            default:
                DWORD dwFunc = (DWORD)0x533290;
                DWORD dwAtomic = (DWORD)pAtomic;
                _asm
                {
                    push    0
                    push    dwAtomic
                    call    dwFunc
                    add     esp, 0x8
                }
        }
    }

    return pAtomic;
}

static void SetVehicleAlpha ( )
{
    CVehicleSAInterface* pInterface = ((CVehicleSAInterface *)dwAlphaEntity);
    unsigned char ucAlpha = pInterface->m_pVehicle->GetAlpha ();

    if ( ucAlpha < 255 )
        GetAlphaAndSetNewValues ( ucAlpha );
    else if ( dwEAEG && pInterface->m_pVehicle->GetModelIndex() == 0x20A )
    {
        bEntityHasAlpha = true;
        pCurAlpha = ucCurrentAlpha;
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_GetAlphaValues, 0 );
        *(DWORD *)(0x5332D6) = (DWORD)CVehicle_EAEG;
        SetEntityAlphaHooked ( dwAlphaEntity, (DWORD)HOOK_SetAlphaValues, 0 );
        *(DWORD *)(0x5332D6) = 0x533290;
    }
    else
        bEntityHasAlpha = false;
}

static DWORD dwCVehicle_SetupRender_ret = 0x6D6517;
void _declspec(naked) HOOK_CVehicle_SetupRender()
{
    _asm
    {
        mov     dwAlphaEntity, esi
        pushad
    }

    SetVehicleAlpha ( );

    _asm
    {
        popad
        add     esp, 0x8
        test    eax, eax
        jmp     dwCVehicle_SetupRender_ret
    }
}

static DWORD dwCVehicle_ResetAfterRender_ret = 0x6D0E43;
void _declspec(naked) HOOK_CVehicle_ResetAfterRender ()
{
    _asm
    {
        pushad
    }

    RestoreAlphaValues ();

    _asm
    {
        popad
        add     esp, 0x0C
        test    eax, eax
        jmp     dwCVehicle_ResetAfterRender_ret
    }
}


/**
 ** Objects
 **/
static void SetObjectAlpha ()
{
    bEntityHasAlpha = false;

    if ( dwAlphaEntity )
    {
        CObject* pObject = pGameInterface->GetPools()->GetObject ( (DWORD *)dwAlphaEntity );
        if ( pObject )
        {
            GetAlphaAndSetNewValues ( pObject->GetAlpha () );
        }
    }
}

DWORD dwCObjectRenderRet = 0;
void _declspec(naked) HOOK_CObject_PostRender ()
{
    _asm
    {
        pushad
    }

    RestoreAlphaValues ( );

    _asm
    {
        popad
        mov         edx, dwCObjectRenderRet
        jmp         edx
    }
}

// Note: This hook is also called for world objects (light poles, wooden fences, etc).
void _declspec(naked) HOOK_CObject_Render ()
{
    _asm
    {
        mov         dwAlphaEntity, ecx
        pushad 
    }

    SetObjectAlpha ( );

    _asm
    {
        popad
        mov         edx, [esp]
        mov         dwCObjectRenderRet, edx
        mov         edx, HOOK_CObject_PostRender
        mov         [esp], edx
        jmp         FUNC_CEntity_Render
    }
}