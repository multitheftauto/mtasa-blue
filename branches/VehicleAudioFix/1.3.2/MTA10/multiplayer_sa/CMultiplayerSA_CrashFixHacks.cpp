/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_CrashFixHacks.cpp
*  PORPOISE:    Home for the poke'n'hope crash fixes
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CPlayerPed__ProcessControl_Abort();

//
// Test macros for CrashFixes
//
#ifdef MTA_DEBUG

#define SIMULATE_ERROR_BEGIN( chance ) \
{ \
    _asm { pushad } \
    if ( rand() % 100 < (chance) ) \
    { \
        _asm popad


#define SIMULATE_ERROR_END \
        _asm pushad \
    } \
    _asm popad \
}

#endif

#define TEST_CRASH_FIXES 0


//
// Support for crash stats
//
void OnCrashAverted ( uint uiId );

void _cdecl CrashAverted ( DWORD id )
{
    OnCrashAverted ( id );
}

#define CRASH_AVERTED(id) \
    } \
    _asm pushfd \
    _asm pushad \
    _asm mov eax, id \
    _asm push eax \
    _asm call CrashAverted \
    _asm add esp, 4 \
    _asm popad \
    _asm popfd \
    _asm \
    {



////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc1                              0x5D9A6E
#define HOOKSIZE_CrashFix_Misc1                             6
DWORD RETURN_CrashFix_Misc1 =                               0x5D9A74;
void _declspec(naked) HOOK_CrashFix_Misc1 ()
{
    _asm
    {
        mov     eax,dword ptr [esp+18h]
        test    eax,eax 
        je      cont

        mov     eax,dword ptr ds:[008D12CCh] 
        mov     ecx,dword ptr [eax+esi]     // If [eax+esi] is 0, it causes a crash
        test    ecx,ecx
        jne     cont
        CRASH_AVERTED( 1 )
        xor    ecx,ecx
    cont:
        jmp     RETURN_CrashFix_Misc1
    }
}


////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc2                              0x6B18B0
#define HOOKSIZE_CrashFix_Misc2                             9
DWORD RETURN_CrashFix_Misc2 =                               0x6B18B9;
DWORD RETURN_CrashFix_Misc2B =                              0x6B3775;
void _declspec(naked) HOOK_CrashFix_Misc2 ()
{
    _asm
    {
        test    eax,eax 
        je      cont        // Skip much code if eax is zero (vehicle has no colmodel)

        mov     eax,dword ptr [eax+2Ch] 

        test    eax,eax 
        je      cont        // Skip much code if eax is zero (colmodel has no coldata)

        mov     ebx,dword ptr [eax+10h] 

        test    ebx,ebx 
        je      cont        // Skip much code if ebx is zero (coldata has no suspension lines)

        mov     cl,byte ptr [esi+429h]
        jmp     RETURN_CrashFix_Misc2
    cont:
        CRASH_AVERTED( 2 )
        jmp     RETURN_CrashFix_Misc2B
    }
}


////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc3                              0x645FD9
#define HOOKSIZE_CrashFix_Misc3                             6
DWORD RETURN_CrashFix_Misc3 =                               0x645FDF;
void _declspec(naked) HOOK_CrashFix_Misc3 ()
{
    _asm
    {
        test    ecx,ecx 
        je      cont        // Skip much code if ecx is zero (ped has no something)

        mov     edx,dword ptr [ecx+384h]
        jmp     RETURN_CrashFix_Misc3
    cont:
        CRASH_AVERTED( 3 )
        jmp     CPlayerPed__ProcessControl_Abort
    }
}


////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc4                              0x4F02D2
#define HOOKSIZE_CrashFix_Misc4                             5
DWORD RETURN_CrashFix_Misc4 =                               0x4F02D7;
DWORD RETURN_CrashFix_Misc4B =                              0x4F0B07;
void _declspec(naked) HOOK_CrashFix_Misc4 ()
{
    _asm
    {
        test    ecx,ecx 
        je      cont        // Skip much code if ecx is zero (avoid divide by zero in soundmanager::service)

        cdq  
        idiv    ecx  
        add     edx, ebp  
        jmp     RETURN_CrashFix_Misc4
    cont:
        CRASH_AVERTED( 4 )
        jmp     RETURN_CrashFix_Misc4B
    }
}


////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc5                              0x5DF949
#define HOOKSIZE_CrashFix_Misc5                             7
DWORD RETURN_CrashFix_Misc5 =                               0x5DF950;
DWORD RETURN_CrashFix_Misc5B =                              0x5DFCC4;
void _declspec(naked) HOOK_CrashFix_Misc5 ()
{
    _asm
    {
        mov     edi, dword ptr [ecx*4+0A9B0C8h]
        mov     edi, dword ptr [edi+5Ch]     
        test    edi, edi 
        je      cont        // Skip much code if edi is zero (ped has no model)

        mov     edi, dword ptr [ecx*4+0A9B0C8h]
        jmp     RETURN_CrashFix_Misc5
    cont:
        CRASH_AVERTED( 5 )
        pop edi
        jmp     RETURN_CrashFix_Misc5B
    }
}


////////////////////////////////////////////////////////////////////////
// #5465 2/2
#define HOOKPOS_CrashFix_Misc6                              0x4D1750
#define HOOKSIZE_CrashFix_Misc6                             5
DWORD RETURN_CrashFix_Misc6 =                               0x4D1755;
DWORD RETURN_CrashFix_Misc6B =                              0x4D1A44;
void _declspec(naked) HOOK_CrashFix_Misc6 ()
{
    _asm
    {
        test    ecx, ecx 
        je      cont        // Skip much code if ecx is zero (ped has no anim something)

        mov     eax, dword ptr [ecx+10h]
        test    eax, eax
        jmp     RETURN_CrashFix_Misc6
    cont:
        CRASH_AVERTED( 6 )
        jmp     RETURN_CrashFix_Misc6B
    }
}


////////////////////////////////////////////////////////////////////////
// #5466
#define HOOKPOS_CrashFix_Misc7                              0x417BF8
#define HOOKSIZE_CrashFix_Misc7                             5
DWORD RETURN_CrashFix_Misc7 =                               0x417BFD;
DWORD RETURN_CrashFix_Misc7B =                              0x417BFF;
void _declspec(naked) HOOK_CrashFix_Misc7 ()
{
    _asm
    {
        test    ecx, ecx 
        je      cont        // Skip much code if ecx is zero (no colmodel)

        mov     esi, dword ptr [ecx+2Ch] 
        test    esi, esi
        jmp     RETURN_CrashFix_Misc7
    cont:
        CRASH_AVERTED( 7 )
        jmp     RETURN_CrashFix_Misc7B
    }
}


////////////////////////////////////////////////////////////////////////
// #5468  1/3
#define HOOKPOS_CrashFix_Misc8                              0x73485D
#define HOOKSIZE_CrashFix_Misc8                             5
DWORD RETURN_CrashFix_Misc8 =                               0x734862;
DWORD RETURN_CrashFix_Misc8B =                              0x734871;
void _declspec(naked) HOOK_CrashFix_Misc8 ()
{
    _asm
    {
        test    ecx, ecx 
        je      cont        // Skip much code if ecx is zero (no 2d effect plugin)

        mov     ecx, dword ptr [edx+ecx] 
        test    ecx, ecx 
        jmp     RETURN_CrashFix_Misc8
    cont:
        CRASH_AVERTED( 8 )
        jmp     RETURN_CrashFix_Misc8B
    }
}


////////////////////////////////////////////////////////////////////////
// #5468  2/3
#define HOOKPOS_CrashFix_Misc9                              0x738B64
#define HOOKSIZE_CrashFix_Misc9                             6
DWORD RETURN_CrashFix_Misc9 =                               0x738B6A;
DWORD RETURN_CrashFix_Misc9B =                              0x73983A;
void _declspec(naked) HOOK_CrashFix_Misc9 ()
{
    _asm
    {
        test    esi, esi 
        je      cont        // Skip much code if esi is zero (invalid projectile)

        mov     eax, dword ptr [esi+40h] 
        test    ah, 1
        jmp     RETURN_CrashFix_Misc9
    cont:
        CRASH_AVERTED( 9 )
        jmp     RETURN_CrashFix_Misc9B
    }
}


////////////////////////////////////////////////////////////////////////
// #5468  3/3
#define HOOKPOS_CrashFix_Misc10                             0x5334FE
#define HOOKSIZE_CrashFix_Misc10                            6
DWORD RETURN_CrashFix_Misc10 =                              0x533504;
DWORD RETURN_CrashFix_Misc10B =                             0x533539;
void _declspec(naked) HOOK_CrashFix_Misc10 ()
{
    _asm
    {
        cmp     ecx, 0x80
        jb      cont  // Skip much code if ecx is small (invalid vector pointer)

        mov     edx, dword ptr [ecx] 
        mov     dword ptr [esp], edx
        jmp     RETURN_CrashFix_Misc10
    cont:
        CRASH_AVERTED( 10 )
        mov     ecx, dword ptr [esp+1Ch]
        mov     dword ptr [ecx],0
        mov     dword ptr [ecx+4],0
        mov     dword ptr [ecx+8],0
        jmp     RETURN_CrashFix_Misc10B
    }
}


////////////////////////////////////////////////////////////////////////
// #5576
#define HOOKPOS_CrashFix_Misc11                             0x4D2C62
#define HOOKSIZE_CrashFix_Misc11                            5
DWORD RETURN_CrashFix_Misc11 =                              0x4D2C67;
DWORD RETURN_CrashFix_Misc11B =                             0x4D2E03;
void _declspec(naked) HOOK_CrashFix_Misc11 ()
{
    _asm
    {
        test    ecx, ecx 
        je      cont  // Skip much code if ecx is zero (invalid anim somthing)

        mov     eax, dword ptr [ecx+10h] 
        test    eax, eax 
        jmp     RETURN_CrashFix_Misc11
    cont:
        CRASH_AVERTED( 11 )
        jmp     RETURN_CrashFix_Misc11B
    }
}


////////////////////////////////////////////////////////////////////////
// #5530
#define HOOKPOS_CrashFix_Misc12                             0x4D41C5
#define HOOKSIZE_CrashFix_Misc12                            5
DWORD RETURN_CrashFix_Misc12 =                              0x4D41CA;
DWORD RETURN_CrashFix_Misc12B =                             0x4D4222;
void _declspec(naked) HOOK_CrashFix_Misc12 ()
{
    _asm
    {
        test    edi, edi 
        je      cont  // Skip much code if edi is zero (invalid anim somthing)

        mov     al, byte ptr [edi+0Bh] 
        test    al, al 
        jmp     RETURN_CrashFix_Misc12
    cont:
        CRASH_AVERTED( 12 )
        jmp     RETURN_CrashFix_Misc12B
    }
}


////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc13                             0x4D464E
#define HOOKSIZE_CrashFix_Misc13                            6
DWORD RETURN_CrashFix_Misc13 =                              0x4D4654;
DWORD RETURN_CrashFix_Misc13B =                             0x4D4764;
void _declspec(naked) HOOK_CrashFix_Misc13 ()
{
    _asm
    {
        cmp     eax, 0x2480
        jb      cont  // Skip much code if eax is less than 0x480 (invalid anim)

        mov     al, byte ptr [eax+0Ah] 
        shr     al, 5
        jmp     RETURN_CrashFix_Misc13
    cont:
        CRASH_AVERTED( 13 )
        jmp     RETURN_CrashFix_Misc13B
    }
}


////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc14                             0x4DD4B5
#define HOOKSIZE_CrashFix_Misc14                            6
DWORD RETURN_CrashFix_Misc14 =                              0x4DD4BB;
void _declspec(naked) HOOK_CrashFix_Misc14 ()
{
    _asm
    {
        mov     eax, dword ptr ds:[0BD00F8h]
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero ( Audio event volumes table not initialized )

        sub     esp, 0D4h
        jmp     RETURN_CrashFix_Misc14
    cont:
        CRASH_AVERTED( 14 )
        add     esp, 12
        retn    12
    }
}


////////////////////////////////////////////////////////////////////////
void _cdecl DoWait ( HANDLE hHandle )
{
    DWORD dwWait = 4000;
    DWORD dwResult = WaitForSingleObject ( hHandle, dwWait );
    if ( dwResult == WAIT_TIMEOUT )
    {
        AddReportLog ( 6211, SString ( "WaitForSingleObject timed out with %08x and %dms", hHandle, dwWait ) );
        // This thread lock bug in GTA will have to be fixed one day.
        // Until then, a 5 second freeze should be long enough for the loading thread to have finished it's job.
#if 0
        _wassert ( _CRT_WIDE("\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            ")
         , _CRT_WIDE(__FILE__), __LINE__);
#endif
        dwResult = WaitForSingleObject ( hHandle, 1000 );
    }
}

// hook info
#define HOOKPOS_FreezeFix_Misc15_US                         0x156CDAE
#define HOOKSIZE_FreezeFix_Misc15_US                        6
#define HOOKPOS_FreezeFix_Misc15_EU                         0x156CDEE
#define HOOKSIZE_FreezeFix_Misc15_EU                        6
DWORD RETURN_FreezeFix_Misc15_US =                          0x156CDB4;
DWORD RETURN_FreezeFix_Misc15_EU =                          0x156CDF4;
DWORD RETURN_FreezeFix_Misc15_BOTH =                        0;
void _declspec(naked) HOOK_FreezeFix_Misc15 ()
{
    _asm
    {
        pop eax
        pop edx
        pushad

        push eax
        call DoWait
        add esp, 4

        popad
        jmp     RETURN_FreezeFix_Misc15_BOTH
    }
}


////////////////////////////////////////////////////////////////////////
// Handle RpAnimBlendClumpGetFirstAssociation returning NULL
#define HOOKPOS_CrashFix_Misc16                             0x5E5815
#define HOOKSIZE_CrashFix_Misc16                            6
DWORD RETURN_CrashFix_Misc16 =                              0x5E581B;
void _declspec(naked) HOOK_CrashFix_Misc16 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     eax, 0
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero ( RpAnimBlendClumpGetFirstAssociation returns NULL )

        // continue standard path
        movsx   ecx, word ptr [eax+2Ch]
        xor     edi, edi
        jmp     RETURN_CrashFix_Misc16

    cont:
        CRASH_AVERTED( 16 )
        add     esp, 96
        retn
    }
}


////////////////////////////////////////////////////////////////////////
// Handle RwFrameSetStaticPluginsSize having NULL member at 0x90
#define HOOKPOS_CrashFix_Misc17_US                          0x7F120E
#define HOOKSIZE_CrashFix_Misc17_US                         6
#define HOOKPOS_CrashFix_Misc17_EU                          0x7F124E
#define HOOKSIZE_CrashFix_Misc17_EU                         6
DWORD RETURN_CrashFix_Misc17_US =                           0x7F1214;
DWORD RETURN_CrashFix_Misc17_EU =                           0x7F1254;
DWORD RETURN_CrashFix_Misc17_BOTH =                         0;
DWORD RETURN_CrashFix_Misc17B_US =                          0x7F1236;
DWORD RETURN_CrashFix_Misc17B_EU =                          0x7F1276;
DWORD RETURN_CrashFix_Misc17B_BOTH =                        0;
void _declspec(naked) HOOK_CrashFix_Misc17 ()
{
    _asm
    {
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero

        // continue standard path
        mov     eax, [eax+90h]
        jmp     RETURN_CrashFix_Misc17_BOTH

    cont:
        CRASH_AVERTED( 17 )
        jmp     RETURN_CrashFix_Misc17B_BOTH
    }
}


////////////////////////////////////////////////////////////////////////
// Handle GetWheelPosition having wrong data
#define HOOKPOS_CrashFix_Misc18                             0x4C7DAD
#define HOOKSIZE_CrashFix_Misc18                            7
DWORD RETURN_CrashFix_Misc18 =                              0x4C7DB4;
void _declspec(naked) HOOK_CrashFix_Misc18 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     ebp, 0
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        cmp     ebp, 0
        je      cont  // Skip much code if ebp is zero

        // continue standard path
        mov         edx,dword ptr [ebp+40h] 
        mov         eax,dword ptr [esp+10h] 
        jmp     RETURN_CrashFix_Misc18

    cont:
        CRASH_AVERTED( 18 )
        mov         edx,0 
        mov         eax,dword ptr [esp+10h]
        mov         dword ptr [eax],edx 
        mov         dword ptr [eax+4],edx 
        pop         esi  
        mov         dword ptr [eax+8],edx 
        pop         ebp  
        ret         0Ch  
    }
}



////////////////////////////////////////////////////////////////////////
// Handle RwFrameCloneHierarchy having wrong data
#define HOOKPOS_CrashFix_Misc19_US                          0x7F0BF7
#define HOOKSIZE_CrashFix_Misc19_US                         6
#define HOOKPOS_CrashFix_Misc19_EU                          0x7F0C37
#define HOOKSIZE_CrashFix_Misc19_EU                         6
DWORD RETURN_CrashFix_Misc19_US =                           0x7F0BFD;
DWORD RETURN_CrashFix_Misc19_EU =                           0x7F0C3D;
DWORD RETURN_CrashFix_Misc19_BOTH =                         0;
DWORD RETURN_CrashFix_Misc19B_US =                          0x7F0C20;
DWORD RETURN_CrashFix_Misc19B_EU =                          0x7F0C60;
DWORD RETURN_CrashFix_Misc19B_BOTH =                        0;
void _declspec(naked) HOOK_CrashFix_Misc19 ()
{
    _asm
    {
        cmp     esi, 0
        je      cont  // Skip much code if esi is zero

        // continue standard path
        mov     eax, [esi+98h] 
        jmp     RETURN_CrashFix_Misc19_BOTH

    cont:
        CRASH_AVERTED( 19 )
        mov         edx,dword ptr [ecx+98h] 
        test        edx,edx
        jmp     RETURN_CrashFix_Misc19B_BOTH
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CPlaceable::RemoveMatrix having wrong data
#define HOOKPOS_CrashFix_Misc20                             0x54F3B0
#define HOOKSIZE_CrashFix_Misc20                            6
DWORD RETURN_CrashFix_Misc20 =                              0x54F3B6;
void _declspec(naked) HOOK_CrashFix_Misc20 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     ecx, 0
        }
    SIMULATE_ERROR_END
#endif
    _asm
    {
        cmp     ecx, 0
        je      cont        // Skip much code if ecx is zero

        // continue standard path
        sub     esp, 10h 
        mov     eax, [ecx+14h] 
        jmp     RETURN_CrashFix_Misc20

    cont:
        CRASH_AVERTED( 20 )
        retn
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CTaskSimpleCarFallOut::FinishAnimFallOutCB having wrong data
#define HOOKPOS_CrashFix_Misc21                             0x648EF6
#define HOOKSIZE_CrashFix_Misc21                            6
DWORD RETURN_CrashFix_Misc21 =                              0x648EFC;
void _declspec(naked) HOOK_CrashFix_Misc21 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     ecx, 0x10
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        cmp     ecx, 0x480
        jb      cont  // Skip much code if ecx is low

        // continue standard path
        mov     edx, [ecx+590h]
        jmp     RETURN_CrashFix_Misc21

    cont:
        CRASH_AVERTED( 21 )
        retn
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CAnimBlendAssociation::Init having wrong data
#define HOOKPOS_CrashFix_Misc22                             0x4CEF08
#define HOOKSIZE_CrashFix_Misc22                            6
DWORD RETURN_CrashFix_Misc22 =                              0x4CEF25;
void _declspec(naked) HOOK_CrashFix_Misc22 ()
{
    _asm
    {
        mov         edx,dword ptr [edi+0Ch] 
    }

#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     edx, 0x10
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        cmp     edx, 0x480
        jb      altcode  // Fill output with zeros if edx is low

        // do standard code
    lp1:
        mov         edx,dword ptr [edi+0Ch] 
        mov         edx,dword ptr [edx+eax*4] 
        mov         ebx,dword ptr [esi+10h] 
        mov         dword ptr [ebx+ecx+10h],edx 
        mov         edx,dword ptr [esi+10h] 
        mov         dword ptr [edx+ecx+14h],esi 
        movsx       edx,word ptr [esi+0Ch] 
        inc         eax  
        add         ecx,18h 
        cmp         eax,edx 
        jl          lp1 
        jmp     RETURN_CrashFix_Misc22

        // do alternate code
    altcode:
        CRASH_AVERTED( 22 )
        mov     edx,0
        mov         ebx,dword ptr [esi+10h] 
        mov         dword ptr [ebx+ecx+10h],edx 
        mov         dword ptr [ebx+ecx+14h],edx 
        movsx       edx,word ptr [esi+0Ch] 
        inc         eax  
        add         ecx,18h 
        cmp         eax,edx 
        jl          altcode 
        jmp     RETURN_CrashFix_Misc22
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CVehicleAnimGroup::ComputeAnimDoorOffsets having wrong door index
#define HOOKPOS_CrashFix_Misc23                             0x6E3D10
#define HOOKSIZE_CrashFix_Misc23                            7
DWORD RETURN_CrashFix_Misc23 =                              0x6E3D17;
void _declspec(naked) HOOK_CrashFix_Misc23 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     edx,0xffff0000
            mov     [esp+8], edx
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        // Ensure door index is reasonable
        mov     edx, [esp+8]
        cmp     edx,16
        jb      ok

        // zero if out of range
        mov     edx,0
        mov     [esp+8], edx
        CRASH_AVERTED( 23 )

    ok:
        // continue standard path
        mov     edx, [esp+8]
        lea     eax, [edx+edx*2]
        jmp     RETURN_CrashFix_Misc23
    }
}


////////////////////////////////////////////////////////////////////////
// Handle _RwFrameForAllChildren being called with NULL
#define HOOKPOS_CrashFix_Misc24_US                          0x7F0DC8
#define HOOKSIZE_CrashFix_Misc24_US                         6
#define HOOKPOS_CrashFix_Misc24_EU                          0x7F0E08
#define HOOKSIZE_CrashFix_Misc24_EU                         6
DWORD RETURN_CrashFix_Misc24_US =                           0x7F0DCE;
DWORD RETURN_CrashFix_Misc24_EU =                           0x7F0E0E;
DWORD RETURN_CrashFix_Misc24_BOTH =                         0;
void _declspec(naked) HOOK_CrashFix_Misc24 ()
{
    _asm
    {
        cmp     ebp, 0x480
        jb      cont  // Skip code if ebp is low

        // continue standard path
        mov     eax, [ebp+98h]
        jmp     RETURN_CrashFix_Misc24_BOTH

    cont:
        CRASH_AVERTED( 24 )
        mov     ebp, 0
        mov     eax, 0
        jmp     RETURN_CrashFix_Misc24_BOTH
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CTaskSimpleCarOpenDoorFromOutside::FinishAnimCarOpenDoorFromOutsideCB having zero pointer to vehicle
#define HOOKPOS_CrashFix_Misc25                             0x646026
#define HOOKSIZE_CrashFix_Misc25                            5
DWORD RETURN_CrashFix_Misc25 =                              0x64602B;
void _declspec(naked) HOOK_CrashFix_Misc25 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     eax, 0
            mov     [esi+0x10], eax
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        // Check for zero pointer to vehicle
        mov     eax, [esi+0x10]
        cmp     eax, 0
        jz      fix

        // Continue standard path
        lea     eax,[esp+10h]
        push    eax
        jmp     RETURN_CrashFix_Misc25

    fix:
        CRASH_AVERTED( 25 )
        // Do special thing
        pop     esi
        pop     ecx
        retn
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CShotInfo::Update having invalid item
#define HOOKPOS_CrashFix_Misc26                             0x739FA0
#define HOOKSIZE_CrashFix_Misc26                            6
DWORD RETURN_CrashFix_Misc26 =                              0x739FA6;
void _declspec(naked) HOOK_CrashFix_Misc26 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     ebx, 130h
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        // Check for incorrect pointer
        cmp     ebx, 130h
        jz      fix

        // Continue standard path
        mov     edi,dword ptr [ebx+ebp*4] 
        dec     ebp  
        test    edi,edi 
        jmp     RETURN_CrashFix_Misc26

    fix:
        CRASH_AVERTED( 26 )
        // Do special thing
        mov     edi, 0 
        dec     ebp  
        test    edi,edi 
        jmp     RETURN_CrashFix_Misc26
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CTaskComplexDieInCar::ControlSubTask ped with no vehicle
#define HOOKPOS_CrashFix_Misc27                             0x6377FB
#define HOOKSIZE_CrashFix_Misc27                            7
DWORD RETURN_CrashFix_Misc27 =                              0x637802;
void _declspec(naked) HOOK_CrashFix_Misc27 ()
{
    _asm
    {
        // Execute replaced code
        cmp     byte ptr [edi+484h], 2
        je      cont

        // Check if veh pointer is zero
        mov     ecx, [edi+58Ch]
        test    ecx, ecx
        jne     cont
        CRASH_AVERTED( 27 )

cont:
        // Continue standard path
        jmp     RETURN_CrashFix_Misc27
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CObject::ProcessGarageDoorBehaviour object with no dummy
#define HOOKPOS_CrashFix_Misc28                             0x44A4FD
#define HOOKSIZE_CrashFix_Misc28                            6
DWORD RETURN_CrashFix_Misc28 =                              0x44A503;
DWORD RETURN_CrashFix_Misc28B =                             0x44A650;
void _declspec(naked) HOOK_CrashFix_Misc28 ()
{
    _asm
    {
        // Execute replaced code
        mov     eax, [esi+170h]
    }

#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 50 )
        _asm
        {
            mov     eax, 0
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        // Check if dummy pointer is zero
        test    eax, eax
        jne     cont

        // Skip much code
        jmp     RETURN_CrashFix_Misc28B

cont:
        // Continue standard path
        jmp     RETURN_CrashFix_Misc28
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CAEPCBankLoader::IsSoundBankLoaded with invalid argument
#define HOOKPOS_CrashFix_Misc29                             0x4E022C
#define HOOKSIZE_CrashFix_Misc29                            5
DWORD RETURN_CrashFix_Misc29 =                              0x4E0231;
DWORD RETURN_CrashFix_Misc29B =                             0x4E0227;
void _declspec(naked) HOOK_CrashFix_Misc29 ()
{
    _asm
    {
        // Execute replaced code
        movsx   eax,word ptr [esp+8]
    }

#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     eax, 0xFFFFFFFF
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        // Check word being -1
        cmp     al, 0xffff
        jz      cont

        // Continue standard path
        jmp     RETURN_CrashFix_Misc29

cont:
        // Skip much code
        jmp     RETURN_CrashFix_Misc29B
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CClumpModelInfo::GetFrameFromId
//
//////////////////////////////////////////////////////////////////////////////////////////
RwFrame* OnMY_CClumpModelInfo_GetFrameFromId_Post ( RwFrame* pFrameResult, DWORD _ebx, DWORD _esi, DWORD _edi, DWORD calledFrom, RpClump* pClump, int id )
{
    if ( pFrameResult )
        return pFrameResult;

    // Don't check frame if call can legitimately return NULL
    if ( calledFrom == 0x6D308F       // CVehicle::SetWindowOpenFlag
        || calledFrom == 0x6D30BF     // CVehicle::ClearWindowOpenFlag
        || calledFrom == 0x4C7DDE     // CVehicleModelInfo::GetOriginalCompPosition
        || calledFrom == 0x4C96BD )   // CVehicleModelInfo::CreateInstance
        return NULL;

    // Ignore external calls
    if ( calledFrom < 0x401000 || calledFrom > 0x801000 )
        return NULL;

    // Now we have a NULL frame which will cause a crash
    // (Probably due to a custom vehicle with missing frames)

    // See if we can get the model id for reporting
    int iModelId = 0;
    DWORD pVehicle = NULL;

    if ( calledFrom == 0x6D3847 )       // CVehicle::AddReplacementUpgrade
        pVehicle = _ebx;
    else
    if ( calledFrom == 0x6DFA61         // CVehicle::AddUpgrade
        || calledFrom == 0x6D3A62 )     // CVehicle::GetReplacementUpgrade
        pVehicle = _edi;
    else
    if ( calledFrom == 0x06AC740        // CAutomobile::PreRender (Forklift)
        || calledFrom == 0x6D39F3       // CVehicle::RemoveReplacementUpgrade
        || calledFrom == 0x6D3A32 )     // CVehicle::RemoveReplacementUpgrade2
        pVehicle = _esi;

    if ( pVehicle > 0x1000 )
        iModelId = ((CVehicleSAInterface*)pVehicle)->m_nModelIndex;

    // Need a valid frame to replace the missing one - Find nearest other id     
    for ( uint i = 2 ; i < 40 ; i++ )
    {
        RwFrame* pNewFrameResult = NULL;
        uint uiNewId = id + ( i / 2 ) * ( ( i & 1 ) ? -1 : 1 );
        DWORD dwFunc = 0x4C53C0;    // CClumpModelInfo::GetFrameFromId
        _asm
        {
            push    uiNewId
            push    pClump
            call    dwFunc
            add     esp, 8
            mov     pNewFrameResult,eax
        }

        if ( pNewFrameResult )
        {
            SString strMsg ( "No frame for vehicle:%d  frameId:%d  (replaced with:%d  calledfrom:%08x)", iModelId, id, uiNewId, calledFrom );
            AddReportLog ( 5412, SString ( "GetFrameFromId - %s", *strMsg ) );
            LogEvent ( 5412, "Model frame warning", "GetFrameFromId", strMsg );
            return pNewFrameResult;
        }
    }

    // Couldn't find a replacement frame id
    SString strMsg ( "No frame for vehicle:%d  frameId:%d  (calledfrom:%08x)", iModelId, id, calledFrom );
    LogEvent ( 5413, "Model frame error", "GetFrameFromId", strMsg );

    return NULL;
}

// Hook info
#define HOOKPOS_CClumpModelInfo_GetFrameFromId                      0x4C53C0
#define HOOKSIZE_CClumpModelInfo_GetFrameFromId                     7
DWORD RETURN_CClumpModelInfo_GetFrameFromId =                       0x4C53C7;
void _declspec(naked) HOOK_CClumpModelInfo_GetFrameFromId ()
{
    _asm
    {
        push    [esp+4*2]
        push    [esp+4*2]
        call    inner
        add     esp, 4*2

        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    edi
        push    esi
        push    ebx
        push    eax
        call    OnMY_CClumpModelInfo_GetFrameFromId_Post
        mov     [esp+0],eax
        add     esp, 4*7
        popad
        mov     eax,[esp-32-4*7]
        retn

inner:
        sub     esp,8 
        mov     eax,dword ptr [esp+10h] 
        jmp     RETURN_CClumpModelInfo_GetFrameFromId
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for CrashFixHacks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_CrashFixHacks ( void )
{
    //EZHookInstall ( CrashFix_Misc1 );
    EZHookInstall ( CrashFix_Misc2 );
    //EZHookInstall ( CrashFix_Misc3 );
    EZHookInstall ( CrashFix_Misc4 );
    EZHookInstall ( CrashFix_Misc5 );
    EZHookInstall ( CrashFix_Misc6 );
    EZHookInstall ( CrashFix_Misc7 );
    EZHookInstall ( CrashFix_Misc8 );
    EZHookInstall ( CrashFix_Misc9 );
    EZHookInstall ( CrashFix_Misc10 );
    EZHookInstall ( CrashFix_Misc11 );
    //EZHookInstall ( CrashFix_Misc12 );
    EZHookInstall ( CrashFix_Misc13 );
    EZHookInstall ( CrashFix_Misc14 );
    EZHookInstall ( FreezeFix_Misc15 );
    EZHookInstall ( CrashFix_Misc16 );
    //EZHookInstall ( CrashFix_Misc17 );
    EZHookInstall ( CrashFix_Misc18 );
    //EZHookInstall ( CrashFix_Misc19 );
    EZHookInstall ( CrashFix_Misc20 );
    EZHookInstall ( CrashFix_Misc21 );
    EZHookInstall ( CrashFix_Misc22 );
    EZHookInstall ( CrashFix_Misc23 );
    EZHookInstall ( CrashFix_Misc24 );
    EZHookInstall ( CrashFix_Misc25 );
    EZHookInstall ( CrashFix_Misc26 );
    EZHookInstall ( CrashFix_Misc27 );
    EZHookInstall ( CrashFix_Misc28 );
    EZHookInstall ( CrashFix_Misc29 );
    EZHookInstall ( CClumpModelInfo_GetFrameFromId );
}
