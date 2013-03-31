/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.pipeline.cpp
*  PURPOSE:     RenderWare pipeline (OS implementation) management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Nasty pools which limit rendering.
CEnvMapMaterialPool **ppEnvMapMaterialPool = (CEnvMapMaterialPool**)0x00C02D28;
CEnvMapAtomicPool **ppEnvMapAtomicPool = (CEnvMapAtomicPool**)0x00C02D2C;
CSpecMapMaterialPool **ppSpecMapMaterialPool = (CSpecMapMaterialPool**)0x00C02D30;

/*=========================================================
    HOOK_InitDeviceSystem

    Purpose:
        Sets up the atomic pipeline, prepares the device information
        and initializes the material, atomic and specular material
        custom pools.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005DA020
=========================================================*/
static int __cdecl HOOK_InitDeviceSystem( void )
{
    RwPipeline *defPipeline;

    // Pah, might analyze those chunks way later :P
    // I am here because of the pools!
    __asm
    {
        // Initialize pipelines
        mov eax,0x005D9F80
        call eax
        mov defPipeline,eax
    }

    if ( !defPipeline )
        return 0;

    *(RwPipeline**)0x00C02D24 = defPipeline;

    int iReturn;    // did you know that "iRet" is a reserved value in VC++ ASM?

    __asm
    {
        // Initialize device information
        mov eax,0x005D8980
        call eax
        mov iReturn,eax
    }

    if ( !iReturn )
        return 0;

    // Set some NULL pointers
    memset( (void*)0x00C02CB0, 0, sizeof(void*) * 26 );

    // Finally initialize the pools!
    *ppEnvMapMaterialPool = new CEnvMapMaterialPool;
    *ppEnvMapAtomicPool = new CEnvMapAtomicPool;
    *ppSpecMapMaterialPool = new CSpecMapMaterialPool;
    return 1;
}

void RenderWarePipeline_Init( void )
{
    HookInstall( 0x005DA020, (DWORD)HOOK_InitDeviceSystem, 5 );
}

void RenderWarePipeline_Shutdown( void )
{
}