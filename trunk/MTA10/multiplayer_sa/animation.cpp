/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/animation.cpp
*  PURPOSE:     animation modification
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

AddAnimationHandler* m_pAddAnimationHandler = NULL;
BlendAnimationHandler* m_pBlendAnimationHandler = NULL;

#define HOOKPOS_CAnimManager_AddAnimation                   0x4d3aa0
DWORD RETURN_CAnimManager_AddAnimation =                    0x4D3AAA;
#define HOOKPOS_CAnimManager_BlendAnimation                 0x4D4610
DWORD RETURN_CAnimManager_BlendAnimation =                  0x4D4617;

void HOOK_CAnimManager_AddAnimation ();
void HOOK_CAnimManager_BlendAnimation ();

void animation_init ( void )
{
    HookInstall(HOOKPOS_CAnimManager_AddAnimation, (DWORD)HOOK_CAnimManager_AddAnimation, 10 ); 
    HookInstall(HOOKPOS_CAnimManager_BlendAnimation, (DWORD)HOOK_CAnimManager_BlendAnimation, 7 ); 
}

RpClump * animationClump = NULL;
AssocGroupId animationGroup = 0;
AnimationId animationID = 0;
void _declspec(naked) HOOK_CAnimManager_AddAnimation ()
{
    _asm
    {        
        mov     eax, [esp+4]
        mov     animationClump, eax
        mov     eax, [esp+8]
        mov     animationGroup, eax
        mov     eax, [esp+12]
        mov     animationID, eax
        pushad
    }
    
    if ( m_pAddAnimationHandler  )
    {
        m_pAddAnimationHandler ( animationClump, animationGroup, animationID );
    }

    _asm
    {
        popad
        mov     eax,dword ptr [esp+0Ch] 
        mov     edx,dword ptr ds:[0B4EA34h] 
        jmp     RETURN_CAnimManager_AddAnimation
    }
}

float animationBlendDelta;
void _declspec(naked) HOOK_CAnimManager_BlendAnimation ()
{
    _asm
    {        
        mov     eax, [esp+4]
        mov     animationClump, eax
        mov     eax, [esp+8]
        mov     animationGroup, eax
        mov     eax, [esp+12]
        mov     animationID, eax
        mov     eax, [esp+16]
        mov     animationBlendDelta, eax
        pushad
    }
    
    if ( m_pBlendAnimationHandler  )
    {
        m_pBlendAnimationHandler ( animationClump, animationGroup, animationID, animationBlendDelta );
    }

    _asm
    {
        popad
        sub     esp,14h 
        mov     ecx,dword ptr [esp+18h]
        jmp     RETURN_CAnimManager_BlendAnimation
    }
}