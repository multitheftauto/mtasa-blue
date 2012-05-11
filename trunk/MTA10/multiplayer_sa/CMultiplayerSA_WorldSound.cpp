/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    WorldSoundHandler*          pWorldSoundHandler  = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
class CAEAudioEntity;

class CAESound
{
public:
    ushort  usGroup;    //+0
    ushort  usIndex;    //+2
    CAEAudioEntity*         pAudioEntity;   //+4
    CEntitySAInterface*     pGameEntity;    //+8    Either a player or NULL
    int     a;          //+c  = 3
    float   b;          //+10 = -1.f
    float   c;          //+14 = -50.f
    float   fVolume;    //+18 = 1.f
    float   fPitch;     //+1c = 1.f
    uint    d;          //+20
    CVector vec1;       //+24 = Position of player?
    CVector vec2;       //+30 = Position of player?
    int     e;          //+3c = 1,621         set from framecounter, no set pos if non zero
    int     f;          //+40 = 300           time in milliseconds
    int     g;          //+44 = 300           set from framecounter
    float   h;          //+48 = 2997.3567f    start something?
    float   j;          //+4c = 2997.3567f    current something?
    float   k;          //+50 = 1.0f
    ushort  l;          //+54 = 31488
    ushort  m;          //+56 = 1005
    int     n;          //+58 = 1
    int     o;          //+5C = 0
    float   p;          //+60 = -100.f
    float   q;          //+64 = 1.f
    ushort  r;          //+68 = 0         (1 on stop)
    ushort  s;          //+6a = 114       
};


////////////////////////////////////////////////////////////////////////////////////////////////
// Return true to skip sound
bool _cdecl OnCAESoundManagerRequestNewSound ( CAESound* pAESound )
{
    if ( pWorldSoundHandler )
    {
        SSFXParams tParams = SSFXParams( pAESound->fVolume, pAESound->fPitch );
        if ( !pWorldSoundHandler ( pAESound->usGroup, pAESound->usIndex, &tParams ) )
        {
            // Mute the sound if not required.
            pAESound->fVolume = 0.f;

            // Return false added because *SOME* SFX need it and will not be muted e.g. weapons and explosions.
            if ( pAESound->usGroup == 5 || ( pAESound->usGroup == 4 && pAESound->usIndex <= 4 ) )
            {
                return false;
            }
            // We could return false here instead to avoid playing the sound altogether,
            // but some sound effects will keep restarting (car engine, fire etc)
            return true;
        }
        pAESound->fPitch = tParams.m_fPitch;
        pAESound->fVolume = tParams.m_fVolume;
    }
    return true;
}


// Hook info
#define HOOKPOS_CAESoundManagerRequestNewSound       0x4EFB10
#define HOOKSIZE_CAESoundManagerRequestNewSound      5
DWORD RETURN_CAESoundManagerRequestNewSound =        0x4EFB15;
void _declspec(naked) HOOK_CAESoundManagerRequestNewSound()
{
    _asm
    {
        pushad
        push    [esp+32+4*1]
        call    OnCAESoundManagerRequestNewSound
        add     esp, 4*1
        cmp     al, 0
        jz      skip
        popad

        // Continue with standard code
        push    esi
        push    edi
        xor     esi, esi
        jmp     RETURN_CAESoundManagerRequestNewSound
       
skip:   // Skip playing sound
        popad
        xor     eax, eax
        retn    4
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Set handlers
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetWorldSoundHandler ( WorldSoundHandler * pHandler )
{
    pWorldSoundHandler = pHandler;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for WorldSound
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_WorldSound ( void )
{
    // Make room for hook
    MemPut < BYTE > ( 0x4EFB15 + 0, 0x0F );     // movsx   eax, si
    MemPut < BYTE > ( 0x4EFB15 + 1, 0xBF );
    MemPut < BYTE > ( 0x4EFB15 + 2, 0xC6 );

    MemPut < BYTE > ( 0x4EFB15 + 3, 0x6B );     // imul    eax, 74h
    MemPut < BYTE > ( 0x4EFB15 + 4, 0xC0 );
    MemPut < BYTE > ( 0x4EFB15 + 5, 0x74 );

    MemPut < BYTE > ( 0x4EFB29, 0xEB );     // Move jump forward one byte

    EZHookInstall ( CAESoundManagerRequestNewSound );
}
