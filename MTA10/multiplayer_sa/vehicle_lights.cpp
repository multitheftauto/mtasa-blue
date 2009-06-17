/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/vehicle_lights.cpp
*  PURPOSE:     vehicle headlight modification
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define HOOKPOS_CVehicle_DoVehicleLights                    0x6e1a60
DWORD RETURN_CVehicle_DoVehicleLights =                     0x6e1a68;

#define HOOKPOS_CAutomobile_Render                          0x6a2b10
DWORD RETURN_CAutomobile_Render =                           0x6a2b18;

#define HOOKPOS_CVehicle_DoHeadLightBeam                    0x6E13A4
DWORD RETURN_CVehicle_DoHeadLightBeam =                     0x6E13AE;

#define HOOKPOS_CVehicle_DoHeadLightEffect_1                0x6E0D01
DWORD RETURN_CVehicle_DoHeadLightEffect_1 =                 0x6E0D09;

#define HOOKPOS_CVehicle_DoHeadLightEffect_2                0x6E0DF7
DWORD RETURN_CVehicle_DoHeadLightEffect_2 =                 0x6E0DFF;

#define HOOKPOS_CVehicle_DoHeadLightReflectionTwin          0x6E170F
DWORD RETURN_CVehicle_DoHeadLightReflectionTwin =           0x6E1717;

#define HOOKPOS_CVehicle_DoHeadLightReflectionSingle        0x6E15E2
DWORD RETURN_CVehicle_DoHeadLightReflectionSingle =         0x6E15EA;

void HOOK_CAutomobile_Render ();
void HOOK_CVehicle_DoVehicleLights ();
void HOOK_CVehicle_DoHeadLightBeam ();
void HOOK_CVehicle_DoHeadLightEffect_1 ();
void HOOK_CVehicle_DoHeadLightEffect_2 ();
void HOOK_CVehicle_DoHeadLightReflectionTwin ();
void HOOK_CVehicle_DoHeadLightReflectionSingle ();

void vehicle_lights_init ( void )
{
    HookInstall(HOOKPOS_CVehicle_DoVehicleLights, (DWORD)HOOK_CVehicle_DoVehicleLights, 8 );
    HookInstall(HOOKPOS_CAutomobile_Render, (DWORD)HOOK_CAutomobile_Render, 8 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightBeam, (DWORD)HOOK_CVehicle_DoHeadLightBeam, 10 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightEffect_1, (DWORD)HOOK_CVehicle_DoHeadLightEffect_1, 8 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightEffect_2, (DWORD)HOOK_CVehicle_DoHeadLightEffect_2, 8 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightReflectionTwin, (DWORD)HOOK_CVehicle_DoHeadLightReflectionTwin, 8 );
    HookInstall(HOOKPOS_CVehicle_DoHeadLightReflectionSingle, (DWORD)HOOK_CVehicle_DoHeadLightReflectionSingle, 8 );

    // Allow turning on vehicle lights even if the engine is off
    memset ( (void *)0x6E1DBC, 0x90, 8 );

    // Fix vehicle back lights both using light state 3 (SA bug)
    *(BYTE *)0x6E1D4F = 2;
}


CVehicleSAInterface * pLightsVehicleInterface = NULL;
void _declspec(naked) HOOK_CVehicle_DoVehicleLights ()
{
    _asm
    {
        mov     pLightsVehicleInterface, ecx
        mov     al,byte ptr ds:[00C1CC18h] 
        sub     esp,3Ch 
        jmp     RETURN_CVehicle_DoVehicleLights
    }
}


CVehicleSAInterface * pRenderVehicleInterface = NULL;
void _declspec(naked) HOOK_CAutomobile_Render ()
{
    _asm
    {
        mov     pRenderVehicleInterface, ecx
        sub     esp,74h 
        mov     eax,dword ptr ds:[00B7CB84h] 
        jmp     RETURN_CAutomobile_Render
    }
}

unsigned long ulHeadLightR = 0, ulHeadLightG = 0, ulHeadLightB = 0;
void CVehicle_GetHeadLightColor ( CVehicleSAInterface * pInterface, float fR, float fG, float fB )
{
    unsigned char R = 255, G = 255, B = 255;
    CVehicle * pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD *)pInterface );
    if ( pVehicle )
    {
        RGBA color = pVehicle->GetHeadLightColor ();
        R = unsigned char ( color );
        G = unsigned char ( color >> 8 );
        B = unsigned char ( color >> 16 );
    }
    
    // Scale our color values to the defaults ..looks dodgy but its needed!
    ulHeadLightR = (unsigned char) min ( 255.0f, ( (float)R / 255.0f ) * fR );
    ulHeadLightG = (unsigned char) min ( 255.0f, ( (float)G / 255.0f ) * fG );
    ulHeadLightB = (unsigned char) min ( 255.0f, ( (float)B / 255.0f ) * fB );
}

RwVertex * pHeadLightVerts = NULL;
unsigned int uiHeadLightNumVerts = 0;
void CVehicle_DoHeadLightBeam ()
{    
    // 255, 255, 255
    CVehicle_GetHeadLightColor ( pRenderVehicleInterface, 255.0f, 255.0f, 255.0f );

    for ( unsigned int i = 0 ; i < uiHeadLightNumVerts ; i++ )
    {        
        unsigned char alpha = ( unsigned char ) ( pHeadLightVerts [ i ].color >> 24 );
        pHeadLightVerts [ i ].color = COLOR_ARGB ( alpha, ulHeadLightR, ulHeadLightG, ulHeadLightB );
    }
}

void _declspec(naked) HOOK_CVehicle_DoHeadLightBeam ()
{
    _asm
    {
        mov     eax, [esp]
        mov     pHeadLightVerts, eax
        mov     eax, [esp+4]
        mov     uiHeadLightNumVerts, eax
    }

    CVehicle_DoHeadLightBeam ();
    *(int *)0xC4B950 = 5;
    
    _asm
    {
        jmp     RETURN_CVehicle_DoHeadLightBeam
    }
}

DWORD dwCCoronas_RegisterCorona = 0x6FC580;
void _declspec(naked) HOOK_CVehicle_DoHeadLightEffect_1 ()
{
    // 160, 160, 140
    _asm pushad

    CVehicle_GetHeadLightColor ( pLightsVehicleInterface, 160.0f, 160.0f, 140.0f );

    _asm
    {
        popad
        mov     eax, ulHeadLightR
        mov     [esp+8], eax
        mov     eax, ulHeadLightG
        mov     [esp+12], eax
        mov     eax, ulHeadLightB
        mov     [esp+16], eax
        //mov     eax, ulHeadLightA
        //mov     [esp+20], eax

        call    dwCCoronas_RegisterCorona 
        add     esp,54h
        jmp     RETURN_CVehicle_DoHeadLightEffect_1
    }
}


void _declspec(naked) HOOK_CVehicle_DoHeadLightEffect_2 ()
{
    // 160, 160, 140
    _asm pushad

    CVehicle_GetHeadLightColor ( pLightsVehicleInterface, 160.0f, 160.0f, 140.0f );

    _asm
    {
        popad
        mov     eax, ulHeadLightR
        mov     [esp+8], eax
        mov     eax, ulHeadLightG
        mov     [esp+12], eax
        mov     eax, ulHeadLightB
        mov     [esp+16], eax
        //mov     eax, ulHeadLightA
        //mov     [esp+20], eax

        call    dwCCoronas_RegisterCorona 
        add     esp, 54h
        jmp     RETURN_CVehicle_DoHeadLightEffect_2
    }
}


DWORD dwCShadows_StoreCarLightShadow = 0x70C500;
void _declspec(naked) HOOK_CVehicle_DoHeadLightReflectionTwin ()
{
    // 45, 45, 45
    _asm pushad
 
    CVehicle_GetHeadLightColor ( pLightsVehicleInterface, 45.0f, 45.0f, 45.0f );

    _asm
    {
        popad
        mov     eax, ulHeadLightR
        mov     [esp+32], eax
        mov     eax, ulHeadLightG
        mov     [esp+36], eax
        mov     eax, ulHeadLightB
        mov     [esp+40], eax

        call    dwCShadows_StoreCarLightShadow
        add     esp, 4Ch
        jmp     RETURN_CVehicle_DoHeadLightReflectionTwin
    }    
}


void _declspec(naked) HOOK_CVehicle_DoHeadLightReflectionSingle ()
{
    // 45, 45, 45
    __asm pushad

    CVehicle_GetHeadLightColor ( pLightsVehicleInterface, 45.0f, 45.0f, 45.0f );

    _asm
    {
        popad
        mov     eax, ulHeadLightR
        mov     [esp+32], eax
        mov     eax, ulHeadLightG
        mov     [esp+36], eax
        mov     eax, ulHeadLightB
        mov     [esp+40], eax

        call    dwCShadows_StoreCarLightShadow
        add     esp, 30h
        jmp     RETURN_CVehicle_DoHeadLightReflectionSingle
    }
}