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
#include "../game_sa/CTasksSA.h"

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
void OnEnterCrashZone ( uint uiId );

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


//////////////////////////////////////////////////////////////////////////////////////////
//
// Handle CTaskSimpleCarFallOut::FinishAnimFallOutCB having wrong data
//
//
//////////////////////////////////////////////////////////////////////////////////////////
bool IsTaskSimpleCarFallOutValid( CAnimBlendAssociationSAInterface* pAnimBlendAssociation, CTaskSimpleCarFallOutSAInterface* pTask )
{
    if ( pTask->VTBL != (TaskVTBL*)VTBL_CTaskSimpleCarFallOut )
    {
        AddReportLog( 8530, SString( "IsTaskSimpleCarFallOutValid fail - pTask->VTBL: %08x", pTask->VTBL ), 5 );
        return false;
    }

    if ( pTask->pVehicle )
    {
        CVehicle* pVehicle = pGameInterface->GetPools ()->GetVehicle ( (DWORD*) pTask->pVehicle );
        if ( !pVehicle )
        {
            // Task looks valid, but vehicle is not recognised by MTA
            AddReportLog( 8531, SString( "IsTaskSimpleCarFallOutValid invalid vehicle ptr - pTask->pVehicle: %08x", pTask->pVehicle ), 5 );
            pTask->pVehicle = NULL;
            return true;
        }
    }

    return true;
}

#define HOOKPOS_CrashFix_Misc21                             0x648EE0
#define HOOKSIZE_CrashFix_Misc21                            7
DWORD RETURN_CrashFix_Misc21 =                              0x648EE7;
void _declspec(naked) HOOK_CrashFix_Misc21 ()
{
#if TEST_CRASH_FIXES
    SIMULATE_ERROR_BEGIN( 10 )
        _asm
        {
            mov     [esp+8], 0x10
        }
    SIMULATE_ERROR_END
#endif

    _asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    IsTaskSimpleCarFallOutValid
        add     esp, 4*2
        cmp     al,0
        popad
        je      cont  // Skip much code if CTaskSimpleCarFallOut is not valid

        // continue standard path
        mov     eax, [esp+8]
        mov     ecx, [eax+10h]
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

        CRASH_AVERTED( 28 )
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
        CRASH_AVERTED( 29 )
        // Skip much code
        jmp     RETURN_CrashFix_Misc29B
    }
}


////////////////////////////////////////////////////////////////////////
// Handle CAnimBlendAssociation::SetFinishCallback with invalid ecx
#define HOOKPOS_CrashFix_Misc30                             0x4CEBE8
#define HOOKSIZE_CrashFix_Misc30                            7
#define HOOKCHECK_CrashFix_Misc30                           0xC7
DWORD RETURN_CrashFix_Misc30 =                              0x4CEBEF;
DWORD RETURN_CrashFix_Misc30B =                             0x4CEBF5;
void _declspec(naked) HOOK_CrashFix_Misc30 ()
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
        // Check for incorrect pointer
        cmp     ecx, 0
        jz      cont

        // Execute replaced code
        mov     dword ptr [ecx+30h], 1
        // Continue standard path
        jmp     RETURN_CrashFix_Misc30

cont:
        CRASH_AVERTED( 30 )
        // Skip much code
        jmp     RETURN_CrashFix_Misc30B
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


struct CStreamingInfo
{
    DWORD gta_hash;
    WORD  chain_next;
    uchar flg;
    uchar archiveId;
    DWORD offsetInBlocks;
    DWORD sizeInBlocks;
    DWORD reqload;
};

CStreamingInfo* GetStreamingInfoFromModelId( uint id )
{
    CStreamingInfo* pItemInfo = (CStreamingInfo*)(0x8E4CC0);
    return pItemInfo + id;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CEntity::GetBoundRect
//
// Check if crash will occur
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CEntity_GetBoundRect( CEntitySAInterface* pEntity )
{
    ushort usModelId = pEntity->m_nModelIndex;
    CBaseModelInfoSAInterface* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelId];
    if ( !pModelInfo )
    {
        // Crash will occur at offset 00134131
        LogEvent( 814, "Model info missing", "CEntity_GetBoundRect", SString( "No info for model:%d", usModelId ), 5414 );
        CArgMap argMap;
        argMap.Set( "id", usModelId );
        argMap.Set( "reason", "info" );
        SetApplicationSetting( "diagnostics", "gta-model-fail", argMap.ToString() );
    }
    else
    {
        CColModelSAInterface* pColModel = pModelInfo->pColModel;
        if ( !pColModel )
        {
            // Crash will occur at offset 00134134
            CStreamingInfo* pStreamingInfo = GetStreamingInfoFromModelId( usModelId );
            SString strDetails( "refs:%d txd:%d RwObj:%08x bOwn:%d bColStr:%d flg:%d off:%d size:%d reqload:%d"
                                ,pModelInfo->usNumberOfRefs
                                ,pModelInfo->usTextureDictionary
                                ,pModelInfo->pRwObject
                                ,pModelInfo->bDoWeOwnTheColModel
                                ,pModelInfo->bCollisionWasStreamedWithModel
                                ,pStreamingInfo->flg
                                ,pStreamingInfo->offsetInBlocks
                                ,pStreamingInfo->sizeInBlocks
                                ,pStreamingInfo->reqload
                            );
            LogEvent( 815, "Model collision missing", "CEntity_GetBoundRect", SString( "No collision for model:%d %s", usModelId, *strDetails ), 5415 );
            CArgMap argMap;
            argMap.Set( "id", usModelId );
            argMap.Set( "reason", "collision" );
            SetApplicationSetting( "diagnostics", "gta-model-fail", argMap.ToString() );
        }
    }
}

// Hook info
#define HOOKPOS_CEntity_GetBoundRect                      0x53412A
#define HOOKSIZE_CEntity_GetBoundRect                     7
#define HOOKCHECK_CEntity_GetBoundRect                    0x8B
DWORD RETURN_CEntity_GetBoundRect =                       0x534131;
void _declspec(naked) HOOK_CEntity_GetBoundRect()
{
    _asm
    {
        pushad
        push    esi
        call    OnMY_CEntity_GetBoundRect
        add     esp, 4*1
        popad

        // Continue replaced code
        mov     ecx,dword ptr [eax*4+0A9B0C8h] 
        jmp     RETURN_CEntity_GetBoundRect
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle_AddUpgrade
//
// Record some variables in case crash occurs
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVehicle_AddUpgrade_Pre( CVehicleSAInterface* pVehicle, int iUpgradeId, int iFrame )
{
    CArgMap argMap;
    argMap.Set( "vehid", pVehicle->m_nModelIndex );
    argMap.Set( "upgid", iUpgradeId );
    argMap.Set( "frame", iFrame );
    SetApplicationSetting( "diagnostics", "gta-upgrade-fail", argMap.ToString() );
}

void OnMY_CVehicle_AddUpgrade_Post( void )
{
    SetApplicationSetting( "diagnostics", "gta-upgrade-fail", "" );
}

// Hook info
#define HOOKPOS_CVehicle_AddUpgrade                      0x6DFA20
#define HOOKSIZE_CVehicle_AddUpgrade                     6
#define HOOKCHECK_CVehicle_AddUpgrade                    0x51
DWORD RETURN_CVehicle_AddUpgrade =                       0x6DFA26;
void _declspec(naked) HOOK_CVehicle_AddUpgrade()
{
    _asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    ecx
        call    OnMY_CVehicle_AddUpgrade_Pre
        add     esp, 4*3
        popad

        push    [esp+4*2]
        push    [esp+4*2]
        call inner

        pushad
        call    OnMY_CVehicle_AddUpgrade_Post
        popad
        retn    8
inner:
        push    ecx
        push    ebx
        mov     ebx, [esp+16]
        jmp     RETURN_CVehicle_AddUpgrade
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Train crossings: Detach barrier from post (to be able to create objects 1373 and 1374 separately)
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CObject_Destructor_TrainCrossing_Check 0x59F7A8
#define HOOKPOS_CObject_ProcessTrainCrossingBehavior1 0x5A0C34
#define HOOKPOS_CObject_ProcessTrainCrossingBehavior2 0x5A0C54
#define RETURN_CObject_Destructor_TrainCrossing_Check 0x59F7AD
#define RETURN_CObject_Destructor_TrainCrossing_Invalid 0x59F811
#define RETURN_CObject_ProcessTrainCrossingBehavior1_Check 0x5A0C39
#define RETURN_CObject_ProcessTrainCrossingBehavior2_Check 0x5A0C59
#define RETURN_CObject_ProcessTrainCrossingBehavior_Invalid 0x5A0CBD

DWORD TrainCrossingFix_ReturnAddress, TrainCrossingFix_InvalidReturnAddress;
template <DWORD ReturnAddress, DWORD InvalidReturnAddress>
void _declspec(naked) HOOK_TrainCrossingBarrierCrashFix ()
{
    // We cannot use template parameters directly in inline assembly; the following instructions don't modify registers
    TrainCrossingFix_ReturnAddress = ReturnAddress;
    TrainCrossingFix_InvalidReturnAddress = InvalidReturnAddress;

    _asm
    {
        test eax, eax // Check if pLinkedBarrierPost exists
        jz jmp_invalid // Skip the barrier stuff
        mov ecx, [eax+14h] // Execute replaced code
        test ecx, ecx
        jmp TrainCrossingFix_ReturnAddress

jmp_invalid:
        jmp TrainCrossingFix_InvalidReturnAddress
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// GTA doesn't reset the furniture object counter, so do it manually everytime before GTA furnishes an interior (Interior_c::Init)
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_ResetFurnitureObjectCounter 0x593BF0
#define HOOKSIZE_ResetFurnitureObjectCounter 6
DWORD RETURN_ResetFurnitureObjectCounter = 0x593BF6;
void _declspec(naked) HOOK_ResetFurnitureObjectCounter ()
{
    *(int*)0xBB3A18 = 0; // InteriorManager_c::ms_objectCounter

    _asm
    {
        // original instruction
        mov eax, fs:[0]
        jmp RETURN_ResetFurnitureObjectCounter
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CVolumetricShadowMgr_Render
//
// Custom models can cause problems for volumetric shadows.
// Record when volumetric shadows are being rendered so we can disable them if a crash occurs.
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVolumetricShadowMgr_Render_Pre( void )
{
    OnEnterCrashZone( 1 );
}

void OnMY_CVolumetricShadowMgr_Render_Post( void )
{
    OnEnterCrashZone( 0 );
}

// Hook info
#define HOOKPOS_CVolumetricShadowMgr_Render                 0x7113B0
#define HOOKSIZE_CVolumetricShadowMgr_Render                8
#define HOOKCHECK_CVolumetricShadowMgr_Render               0x83
DWORD RETURN_CVolumetricShadowMgr_Render =                  0x7113B8;
void _declspec(naked) HOOK_CVolumetricShadowMgr_Render()
{
    _asm
    {
        pushad
        call    OnMY_CVolumetricShadowMgr_Render_Pre
        popad

        call inner

        pushad
        call    OnMY_CVolumetricShadowMgr_Render_Post
        popad
        retn

inner:
        // Replaced code
        sub     esp, 18h  
        mov     ecx, 0A9AE00h 
        jmp     RETURN_CVolumetricShadowMgr_Render
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CVolumetricShadowMgr_Update
//
// Custom models can cause problems for volumetric shadows.
// Record when volumetric shadows are being updated so we can disable them if a crash occurs.
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVolumetricShadowMgr_Update_Pre( void )
{
    OnEnterCrashZone( 2 );
}

void OnMY_CVolumetricShadowMgr_Update_Post( void )
{
    OnEnterCrashZone( 0 );
}

// Hook info
#define HOOKPOS_CVolumetricShadowMgr_Update                 0x711D90
#define HOOKSIZE_CVolumetricShadowMgr_Update                5
#define HOOKCHECK_CVolumetricShadowMgr_Update               0xB9
DWORD RETURN_CVolumetricShadowMgr_Update =                  0x711D95;
void _declspec(naked) HOOK_CVolumetricShadowMgr_Update()
{
    _asm
    {
        pushad
        call    OnMY_CVolumetricShadowMgr_Update_Pre
        popad

        push    [esp+4*1]
        call inner
        add     esp, 4*1

        pushad
        call    OnMY_CVolumetricShadowMgr_Update_Post
        popad
        retn

inner:
        // Replaced code
        mov     ecx, 0A9AE00h  
        jmp     RETURN_CVolumetricShadowMgr_Update
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CAnimManager_CreateAnimAssocGroups
//
// A model (usually 7) has to be loaded to avoid a crash
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CAnimManager_CreateAnimAssocGroups( uint uiModelId )
{
    CModelInfo* pModelInfo = pGameInterface->GetModelInfo( uiModelId );
    if ( pModelInfo->GetInterface()->pRwObject == NULL )
    {
        // Crash will occur at offset 00349b7b
        LogEvent( 816, "Model not loaded", "CAnimManager_CreateAnimAssocGroups", SString( "No RwObject for model:%d", uiModelId ), 5416 );
        CArgMap argMap;
        argMap.Set( "id", uiModelId );
        argMap.Set( "reason", "createanim" );
        SetApplicationSetting( "diagnostics", "gta-model-fail", argMap.ToString() );
    }
}


// Hook info
#define HOOKPOS_CAnimManager_CreateAnimAssocGroups                 0x4D3D52
#define HOOKSIZE_CAnimManager_CreateAnimAssocGroups                5
#define HOOKCHECK_CAnimManager_CreateAnimAssocGroups               0x8B
DWORD RETURN_CAnimManager_CreateAnimAssocGroups =                  0x4D3D59;
void _declspec(naked) HOOK_CAnimManager_CreateAnimAssocGroups()
{
    _asm
    {
        pushad
        push    eax
        call    OnMY_CAnimManager_CreateAnimAssocGroups
        add     esp, 4*1
        popad

        // Replaced code
        mov     eax, 0x0A9B0C8[eax*4] 
        jmp     RETURN_CAnimManager_CreateAnimAssocGroups
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Something called from CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask
//
// Accessing a temporally not existing vehicle
// (seems to happen when the driver is slower being thrown out than the jacker enters the vehicle)
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask   0x6485AC
#define HOOKSIZE_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask  6
DWORD RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask =    0x6485B2;
DWORD RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask_Invalid = 0x6485E1;
void OnMY_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask()
{
    LogEvent( 817, "CTaskComplexCarSlowBeDraggedOut", "", "CTaskComplexCarSlowBeDraggedOut race condition" );
}

void _declspec(naked) HOOK_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask()
{
    _asm
    {
        test eax, eax
        jz invalid_vehicle

        mov ecx, [eax+460h]
        jmp RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask

    invalid_vehicle:
        pushad
        call OnMY_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask
        popad
        jmp RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask_Invalid
    }
}


////////////////////////////////////////////////////////////////////////
//
// OnMY_printf
//
// GTA outputs stuff via printf which we can use to help diagnose problems
//
////////////////////////////////////////////////////////////////////////
void _cdecl OnMY_printf ( DWORD dwCalledFrom, const char* szMessage )
{
    SString strMessage = SStringX( szMessage ).Replace( "\n", "" );

    // Ignore unimportant messages
    if ( strMessage == "Initialised SoundManager" || strMessage == "%f" )
    {
        return;
    }

    SString strContext( "GTALOG Called from 0x%08x", dwCalledFrom );
    LogEvent ( 6311, "printf", strContext, strMessage, 6311 );

    // Check for known issues
    if ( strMessage == "Error subrastering" )
    {
        // Couldn't create render target for CPostEffects
        BrowseToSolution ( "error-subrastering", EXIT_GAME_FIRST | ASK_GO_ONLINE, _( "Problem with graphics driver" ) );
    }
    else
    if ( strMessage == "Too many objects without modelinfo structures" )
    {
        // Corrupted gta3.img - TODO: Inform user one day
    }
}

// hook info
#define HOOKPOS_printf_US                         0x821982
#define HOOKSIZE_printf_US                        7
#define HOOKCHECK_printf_US                       0x6A
#define HOOKPOS_printf_EU                         0x8219C2
#define HOOKSIZE_printf_EU                        7
#define HOOKCHECK_printf_EU                       0x6A
DWORD RETURN_printf_US =                          0x821989;
DWORD RETURN_printf_EU =                          0x8219C9;
DWORD RETURN_printf_BOTH =                        0;
void _declspec(naked) HOOK_printf ()
{
    _asm
    {
        pushad
        push    [esp+32+4*1]
        push    [esp+32+4*1]
        call    OnMY_printf
        add     esp, 4*2
        popad

        // Replaced code
        push    10h
        push    887DC0h
        jmp     RETURN_printf_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for CrashFixHacks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_CrashFixHacks ( void )
{
    EZHookInstall ( CrashFix_Misc1 );
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
    EZHookInstallChecked ( CrashFix_Misc30 );
    EZHookInstall ( CClumpModelInfo_GetFrameFromId );
    EZHookInstallChecked ( CEntity_GetBoundRect );
    EZHookInstallChecked ( CVehicle_AddUpgrade );
    EZHookInstall ( ResetFurnitureObjectCounter );
    EZHookInstallChecked ( CVolumetricShadowMgr_Render );
    EZHookInstallChecked ( CVolumetricShadowMgr_Update );
    EZHookInstallChecked ( CAnimManager_CreateAnimAssocGroups );
    EZHookInstall ( CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask );
    EZHookInstallChecked ( printf );

    // Install train crossing crashfix (the temporary variable is required for the template logic)
    void (*temp)() = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_Destructor_TrainCrossing_Check, RETURN_CObject_Destructor_TrainCrossing_Invalid>;
    HookInstall ( HOOKPOS_CObject_Destructor_TrainCrossing_Check, (DWORD)temp, 5 );
    temp = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_ProcessTrainCrossingBehavior1_Check, RETURN_CObject_ProcessTrainCrossingBehavior_Invalid>;
    HookInstall ( HOOKPOS_CObject_ProcessTrainCrossingBehavior1, (DWORD)temp, 5 );
    temp = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_ProcessTrainCrossingBehavior2_Check, RETURN_CObject_ProcessTrainCrossingBehavior_Invalid>;
    HookInstall ( HOOKPOS_CObject_ProcessTrainCrossingBehavior2, (DWORD)temp, 5 );
}
