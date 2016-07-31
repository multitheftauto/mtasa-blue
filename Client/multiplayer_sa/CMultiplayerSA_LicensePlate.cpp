/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_LicensePlate.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "..\game_sa\gamesa_renderware.h"

namespace
{
    struct SPlateInfo
    {
        std::vector < RpMaterial* > plateMaterialList;
    };

    std::map < CVehicleModelInfoSAInterface*, SPlateInfo >  ms_ModelPlateMaterialsMap;
    std::vector < RwTexture* >                              ms_SavedTextureList;
    CVehicleModelInfoSAInterface*                           ms_pProcessingVehicleModelInfo = NULL;
    RwTexture*                                              ms_pLastGeneratedPlateText = NULL;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::CustomCarPlate_BeforeRenderingStart
//
// Swap custom plate textures (GTA only does one, we do the rest here)
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CAutomobile_CustomCarPlate_BeforeRenderingStart( CVehicleSAInterface* pVehicle, CVehicleModelInfoSAInterface* pVehicleModelInfo )
{
    if ( !pVehicle->m_pCustomPlateTexture )
        return;

    SPlateInfo* pPlateInfo = MapFind( ms_ModelPlateMaterialsMap, pVehicleModelInfo );
    if ( pPlateInfo && !pPlateInfo->plateMaterialList.empty() )
    {
        // Copy custom plate texture (for this vehicle) into the model 'carplate' materials
        for ( uint i = 0 ; i < pPlateInfo->plateMaterialList.size() ; i++ )
        {
            RpMaterial* pMaterial = pPlateInfo->plateMaterialList[i];
            // Only do replace if it's not the material that GTA is handling
            if ( pMaterial != pVehicleModelInfo->pPlateMaterial )
            {
                RwTexture* pOldTexture = pMaterial->texture;
                pOldTexture->refs++;
                ms_SavedTextureList.push_back( pOldTexture );
                RpMaterialSetTexture( pMaterial, pVehicle->m_pCustomPlateTexture );
            }
            else
                ms_SavedTextureList.push_back( NULL );
        }
    }
}

// Hook info
#define HOOKPOS_CAutomobile_CustomCarPlate_BeforeRenderingStart                         0x006A2F00
#define HOOKSIZE_CAutomobile_CustomCarPlate_BeforeRenderingStart                        7
DWORD RETURN_CAutomobile_CustomCarPlate_BeforeRenderingStart =                          0x006A2F07;
void _declspec(naked) HOOK_CAutomobile_CustomCarPlate_BeforeRenderingStart()
{
    _asm
    {
        pushad
        push    [esp+32+4*1]
        push    ecx
        call    OnMY_CAutomobile_CustomCarPlate_BeforeRenderingStart
        add     esp, 4*2
        popad

        mov     eax, [esp+4]
        mov     eax, [eax+24h]
        jmp     RETURN_CAutomobile_CustomCarPlate_BeforeRenderingStart
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::CustomCarPlate_AfterRenderingStop
//
// Restore custom plate textures
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CAutomobile_CustomCarPlate_AfterRenderingStop( CVehicleModelInfoSAInterface* pVehicleModelInfo )
{
    if ( ms_SavedTextureList.empty() )
        return;

    SPlateInfo* pPlateInfo = MapFind( ms_ModelPlateMaterialsMap, pVehicleModelInfo );
    if ( pPlateInfo )
    {
        // Restore custom plate texture (for this vehicle) from the model 'carplate' materials
        for ( uint i = 0 ; i < pPlateInfo->plateMaterialList.size() ; i++ )
        {
            RpMaterial* pMaterial = pPlateInfo->plateMaterialList[i];
            RwTexture* pOldTexture = ms_SavedTextureList[i];
            // Only do restore if it's not the one that GTA handled
            if ( pMaterial != pVehicleModelInfo->pPlateMaterial )
            {
                assert( pOldTexture );
                RpMaterialSetTexture( pMaterial, pOldTexture );
                RwTextureDestroy( pOldTexture );
            }
            else
                assert( pOldTexture == NULL );
        }
        ms_SavedTextureList.clear();
    }
}

// Hook info
#define HOOKPOS_CAutomobile_CustomCarPlate_AfterRenderingStop                         0x006A2F30
#define HOOKSIZE_CAutomobile_CustomCarPlate_AfterRenderingStop                        7
DWORD RETURN_CAutomobile_CustomCarPlate_AfterRenderingStop =                          0x006A2F37;
void _declspec(naked) HOOK_CAutomobile_CustomCarPlate_AfterRenderingStop()
{
    _asm
    {
        pushad
        push    [esp+32+4*1]
        call    OnMY_CAutomobile_CustomCarPlate_AfterRenderingStop
        add     esp, 4*1
        popad

        mov     eax, [esp+4]
        mov     eax, [eax+24h]
        jmp     RETURN_CAutomobile_CustomCarPlate_AfterRenderingStop
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CCustomCarPlateMgr::SetupMaterialPlateTexture
//
// Save plate material for using later (in BeforeRenderingStart())
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CCustomCarPlateMgr_SetupMaterialPlateTexture( struct RpMaterial * a, char * b, unsigned char c )
{
    if ( ms_pProcessingVehicleModelInfo )
    {
        SPlateInfo& info = MapGet( ms_ModelPlateMaterialsMap, ms_pProcessingVehicleModelInfo );
        info.plateMaterialList.push_back( a );
    }
}

// Hook info
#define HOOKPOS_CCustomCarPlateMgr_SetupMaterialPlateTexture                         0x006FE020
#define HOOKSIZE_CCustomCarPlateMgr_SetupMaterialPlateTexture                        5
DWORD RETURN_CCustomCarPlateMgr_SetupMaterialPlateTexture =                          0x006FE025;
void _declspec(naked) HOOK_CCustomCarPlateMgr_SetupMaterialPlateTexture()
{
    _asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    [esp+32+4*3]
        call    OnMY_CCustomCarPlateMgr_SetupMaterialPlateTexture
        add     esp, 4*3
        popad

        push    edi
        mov     edi, [esp+8]
        jmp     RETURN_CCustomCarPlateMgr_SetupMaterialPlateTexture
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicleModelInfo::SetCarCustomPlate
//
// Prepare to receive list of plate materials for this vehicle type
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVehicleModelInfo_SetCarCustomPlate( CVehicleModelInfoSAInterface* pVehicleModelInfo )
{
    assert( !ms_pLastGeneratedPlateText );
    ms_pProcessingVehicleModelInfo = pVehicleModelInfo;
    SPlateInfo& info = MapGet( ms_ModelPlateMaterialsMap, ms_pProcessingVehicleModelInfo );
    info.plateMaterialList.clear();
}

void OnMY_CVehicleModelInfo_SetCarCustomPlate_Post( void )
{
    // Done
    ms_pProcessingVehicleModelInfo = NULL;
    ms_pLastGeneratedPlateText = NULL;
}

// Hook info
#define HOOKPOS_CVehicleModelInfo_SetCarCustomPlate                         0x004C9450
#define HOOKSIZE_CVehicleModelInfo_SetCarCustomPlate                        6
DWORD RETURN_CVehicleModelInfo_SetCarCustomPlate =                          0x004C9456;
void _declspec(naked) HOOK_CVehicleModelInfo_SetCarCustomPlate()
{
    _asm
    {
        pushad
        push    ecx
        call    OnMY_CVehicleModelInfo_SetCarCustomPlate
        add     esp, 4*1
        popad

        call inner

        pushad
        call    OnMY_CVehicleModelInfo_SetCarCustomPlate_Post
        popad
        retn
inner:

        sub     esp, 10h
        push    esi
        mov     esi, ecx
        jmp     RETURN_CVehicleModelInfo_SetCarCustomPlate
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CCustomCarPlateMgr::CreatePlateTexture
//
// Optional optimization.
// Prevent GTA from generating a new texture for each occurrence of 'carplate' in a model
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CCustomCarPlateMgr_CreatePlateTexture                         0x006FDEA0
#define HOOKSIZE_CCustomCarPlateMgr_CreatePlateTexture                        5
DWORD RETURN_CCustomCarPlateMgr_CreatePlateTexture =                          0x006FDEA5;
void _declspec(naked) HOOK_CCustomCarPlateMgr_CreatePlateTexture()
{
    _asm
    {
        // Skip our code if not processing a VehicleModelInfo
        cmp     ms_pProcessingVehicleModelInfo, 0
        jz      inner

        // Use LastGeneratedPlateTexture if we have one
        mov      eax, ms_pLastGeneratedPlateText
        test     eax, eax
        jnz      fin

        // Call original
        push    [esp+4*2]
        push    [esp+4*2]
        call    inner
        add     esp, 4*2

        // Save generated texture
        mov     ms_pLastGeneratedPlateText,eax
fin:
        retn

inner:
        push    ebx
        mov     bl, [esp+0x0c]
        jmp     RETURN_CCustomCarPlateMgr_CreatePlateTexture
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_LicensePlate
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_LicensePlate ( void )
{
    EZHookInstall ( CAutomobile_CustomCarPlate_BeforeRenderingStart );
    EZHookInstall ( CAutomobile_CustomCarPlate_AfterRenderingStop );
    EZHookInstall ( CCustomCarPlateMgr_SetupMaterialPlateTexture );
    EZHookInstall ( CVehicleModelInfo_SetCarCustomPlate );
    EZHookInstall ( CCustomCarPlateMgr_CreatePlateTexture );
}
