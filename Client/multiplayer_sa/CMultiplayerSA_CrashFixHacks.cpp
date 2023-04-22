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
#include <game/CAnimManager.h>
#include "../game_sa/CTrainSA.h"
#include "../game_sa/CTasksSA.h"
#include "../game_sa/CAnimBlendSequenceSA.h"
#include "../game_sa/CAnimBlendHierarchySA.h"
#include "../game_sa/TaskBasicSA.h"
#include "../game_sa/CFxSystemBPSA.h"
#include "../game_sa/CFxSystemSA.h"

extern CCoreInterface* g_pCore;

void CPlayerPed__ProcessControl_Abort();

//
// Support for crash stats
//
void OnCrashAverted(uint uiId);
void OnEnterCrashZone(uint uiId);

void _declspec(naked) CrashAverted()
{
    _asm
    {
        pushfd
        pushad
        push    [esp+4+32+4*1]
        call    OnCrashAverted
        add     esp, 4
        popad
        popfd
        retn    4
    }
}

////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc1                              0x5D9A6E
#define HOOKSIZE_CrashFix_Misc1                             6
DWORD RETURN_CrashFix_Misc1 = 0x5D9A74;
void _declspec(naked) HOOK_CrashFix_Misc1()
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
        push    1
        call    CrashAverted
        xor    ecx,ecx
    cont:
        jmp     RETURN_CrashFix_Misc1
    }
}

////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc2                              0x6B18B0
#define HOOKSIZE_CrashFix_Misc2                             9
DWORD RETURN_CrashFix_Misc2 = 0x6B18B9;
DWORD RETURN_CrashFix_Misc2B = 0x6B3775;
void _declspec(naked) HOOK_CrashFix_Misc2()
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
        push    2
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc2B
    }
}

////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc3                              0x645FD9
#define HOOKSIZE_CrashFix_Misc3                             6
DWORD RETURN_CrashFix_Misc3 = 0x645FDF;
void _declspec(naked) HOOK_CrashFix_Misc3()
{
    _asm
    {
        test    ecx,ecx
        je      cont        // Skip much code if ecx is zero (ped has no something)

        mov     edx,dword ptr [ecx+384h]
        jmp     RETURN_CrashFix_Misc3
    cont:
        push    3
        call    CrashAverted
        jmp     CPlayerPed__ProcessControl_Abort
    }
}

////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc4                              0x4F02D2
#define HOOKSIZE_CrashFix_Misc4                             5
DWORD RETURN_CrashFix_Misc4 = 0x4F02D7;
DWORD RETURN_CrashFix_Misc4B = 0x4F0B07;
void _declspec(naked) HOOK_CrashFix_Misc4()
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
        push    4
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc4B
    }
}

////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc5                              0x5DF949
#define HOOKSIZE_CrashFix_Misc5                             7
DWORD RETURN_CrashFix_Misc5 = 0x5DF950;
DWORD RETURN_CrashFix_Misc5B = 0x5DFCC4;
void _declspec(naked) HOOK_CrashFix_Misc5()
{
    _asm {
        mov edi, dword ptr[ARRAY_ModelInfo]
        mov     edi, dword ptr [ecx*4+edi]
        mov     edi, dword ptr [edi+5Ch]
        test    edi, edi
        je      cont            // Skip much code if edi is zero (ped has no model)

        mov edi, dword ptr[ARRAY_ModelInfo]
        mov     edi, dword ptr [ecx*4+edi]
        jmp     RETURN_CrashFix_Misc5
    cont:
        push    5
        call    CrashAverted
        pop edi
        jmp     RETURN_CrashFix_Misc5B
    }
}

////////////////////////////////////////////////////////////////////////
// #5465 2/2
#define HOOKPOS_CrashFix_Misc6                              0x4D1750
#define HOOKSIZE_CrashFix_Misc6                             5
DWORD RETURN_CrashFix_Misc6 = 0x4D1755;
DWORD RETURN_CrashFix_Misc6B = 0x4D1A44;
void _declspec(naked) HOOK_CrashFix_Misc6()
{
    _asm
    {
        test    ecx, ecx
        je      cont        // Skip much code if ecx is zero (ped has no anim something)

        mov     eax, dword ptr [ecx+10h]
        test    eax, eax
        jmp     RETURN_CrashFix_Misc6
    cont:
        push    6
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc6B
    }
}

////////////////////////////////////////////////////////////////////////
// #5466
#define HOOKPOS_CrashFix_Misc7                              0x417BF8
#define HOOKSIZE_CrashFix_Misc7                             5
DWORD RETURN_CrashFix_Misc7 = 0x417BFD;
DWORD RETURN_CrashFix_Misc7B = 0x417BFF;
void _declspec(naked) HOOK_CrashFix_Misc7()
{
    _asm
    {
        test    ecx, ecx
        je      cont        // Skip much code if ecx is zero (no colmodel)

        mov     esi, dword ptr [ecx+2Ch]
        test    esi, esi
        jmp     RETURN_CrashFix_Misc7
    cont:
        push    7
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc7B
    }
}

////////////////////////////////////////////////////////////////////////
// #5468  1/3
#define HOOKPOS_CrashFix_Misc8                              0x73485D
#define HOOKSIZE_CrashFix_Misc8                             5
DWORD RETURN_CrashFix_Misc8 = 0x734862;
DWORD RETURN_CrashFix_Misc8B = 0x734871;
void _declspec(naked) HOOK_CrashFix_Misc8()
{
    _asm
    {
        test    ecx, ecx
        je      cont        // Skip much code if ecx is zero (no 2d effect plugin)

        mov     ecx, dword ptr [edx+ecx]
        test    ecx, ecx
        jmp     RETURN_CrashFix_Misc8
    cont:
        push    8
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc8B
    }
}

////////////////////////////////////////////////////////////////////////
// #5468  2/3
#define HOOKPOS_CrashFix_Misc9                              0x738B64
#define HOOKSIZE_CrashFix_Misc9                             6
DWORD RETURN_CrashFix_Misc9 = 0x738B6A;
DWORD RETURN_CrashFix_Misc9B = 0x73983A;
void _declspec(naked) HOOK_CrashFix_Misc9()
{
    _asm
    {
        test    esi, esi
        je      cont        // Skip much code if esi is zero (invalid projectile)

        mov     eax, dword ptr [esi+40h]
        test    ah, 1
        jmp     RETURN_CrashFix_Misc9
    cont:
        push    9
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc9B
    }
}

////////////////////////////////////////////////////////////////////////
// #5468  3/3
#define HOOKPOS_CrashFix_Misc10                             0x5334FE
#define HOOKSIZE_CrashFix_Misc10                            6
DWORD RETURN_CrashFix_Misc10 = 0x533504;
DWORD RETURN_CrashFix_Misc10B = 0x533539;
void _declspec(naked) HOOK_CrashFix_Misc10()
{
    _asm
    {
        cmp     ecx, 0x80
        jb      cont  // Skip much code if ecx is small (invalid vector pointer)

        mov     edx, dword ptr [ecx]
        mov     dword ptr [esp], edx
        jmp     RETURN_CrashFix_Misc10
    cont:
        push    10
        call    CrashAverted
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
DWORD RETURN_CrashFix_Misc11 = 0x4D2C67;
DWORD RETURN_CrashFix_Misc11B = 0x4D2E03;
void _declspec(naked) HOOK_CrashFix_Misc11()
{
    _asm
    {
        test    ecx, ecx
        je      cont  // Skip much code if ecx is zero (invalid anim somthing)

        mov     eax, dword ptr [ecx+10h]
        test    eax, eax
        jmp     RETURN_CrashFix_Misc11
    cont:
        push    11
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc11B
    }
}

////////////////////////////////////////////////////////////////////////
// #5530
#define HOOKPOS_CrashFix_Misc12                             0x4D41C5
#define HOOKSIZE_CrashFix_Misc12                            5
DWORD RETURN_CrashFix_Misc12 = 0x4D41CA;
DWORD RETURN_CrashFix_Misc12B = 0x4D4222;
void _declspec(naked) HOOK_CrashFix_Misc12()
{
    _asm
    {
        test    edi, edi
        je      cont  // Skip much code if edi is zero (invalid anim somthing)

        mov     al, byte ptr [edi+0Bh]
        test    al, al
        jmp     RETURN_CrashFix_Misc12
    cont:
        push    12
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc12B
    }
}

////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc13                             0x4D464E
#define HOOKSIZE_CrashFix_Misc13                            6
DWORD RETURN_CrashFix_Misc13 = 0x4D4654;
DWORD RETURN_CrashFix_Misc13B = 0x4D4764;
void _declspec(naked) HOOK_CrashFix_Misc13()
{
    _asm
    {
        cmp     eax, 0x2480
        jb      cont  // Skip much code if eax is less than 0x480 (invalid anim)

        mov     al, byte ptr [eax+0Ah]
        shr     al, 5
        jmp     RETURN_CrashFix_Misc13
    cont:
        push    13
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc13B
    }
}

////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc14                             0x4DD4B5
#define HOOKSIZE_CrashFix_Misc14                            6
DWORD RETURN_CrashFix_Misc14 = 0x4DD4BB;
void _declspec(naked) HOOK_CrashFix_Misc14()
{
    _asm
    {
        mov     eax, dword ptr ds:[0BD00F8h]
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero ( Audio event volumes table not initialized )

        sub     esp, 0D4h
        jmp     RETURN_CrashFix_Misc14
    cont:
        push    14
        call    CrashAverted
        add     esp, 12
        retn    12
    }
}

////////////////////////////////////////////////////////////////////////
void _cdecl DoWait(HANDLE hHandle)
{
    DWORD dwWait = 4000;
    DWORD dwResult = WaitForSingleObject(hHandle, dwWait);
    if (dwResult == WAIT_TIMEOUT)
    {
        AddReportLog(6211, SString("WaitForSingleObject timed out with %08x and %dms", hHandle, dwWait));
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
        dwResult = WaitForSingleObject(hHandle, 1000);
    }
}

// hook info
#define HOOKPOS_FreezeFix_Misc15_US                         0x156CDAE
#define HOOKSIZE_FreezeFix_Misc15_US                        6
#define HOOKPOS_FreezeFix_Misc15_EU                         0x156CDEE
#define HOOKSIZE_FreezeFix_Misc15_EU                        6
DWORD RETURN_FreezeFix_Misc15_US = 0x156CDB4;
DWORD RETURN_FreezeFix_Misc15_EU = 0x156CDF4;
DWORD RETURN_FreezeFix_Misc15_BOTH = 0;
void _declspec(naked) HOOK_FreezeFix_Misc15()
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
DWORD RETURN_CrashFix_Misc16 = 0x5E581B;
void _declspec(naked) HOOK_CrashFix_Misc16()
{
    _asm
    {
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero ( RpAnimBlendClumpGetFirstAssociation returns NULL )

        // continue standard path
        movsx   ecx, word ptr [eax+2Ch]
        xor     edi, edi
        jmp     RETURN_CrashFix_Misc16

    cont:
        push    16
        call    CrashAverted
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
DWORD RETURN_CrashFix_Misc17_US = 0x7F1214;
DWORD RETURN_CrashFix_Misc17_EU = 0x7F1254;
DWORD RETURN_CrashFix_Misc17_BOTH = 0;
DWORD RETURN_CrashFix_Misc17B_US = 0x7F1236;
DWORD RETURN_CrashFix_Misc17B_EU = 0x7F1276;
DWORD RETURN_CrashFix_Misc17B_BOTH = 0;
void _declspec(naked) HOOK_CrashFix_Misc17()
{
    _asm
    {
        cmp     eax, 0
        je      cont  // Skip much code if eax is zero

        // continue standard path
        mov     eax, [eax+90h]
        jmp     RETURN_CrashFix_Misc17_BOTH

    cont:
        push    17
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc17B_BOTH
    }
}

////////////////////////////////////////////////////////////////////////
// Handle GetWheelPosition having wrong data
#define HOOKPOS_CrashFix_Misc18                             0x4C7DAD
#define HOOKSIZE_CrashFix_Misc18                            7
DWORD RETURN_CrashFix_Misc18 = 0x4C7DB4;
void _declspec(naked) HOOK_CrashFix_Misc18()
{
    _asm
    {
        cmp     ebp, 0
        je      cont  // Skip much code if ebp is zero

        // continue standard path
        mov         edx,dword ptr [ebp+40h]
        mov         eax,dword ptr [esp+10h]
        jmp     RETURN_CrashFix_Misc18

    cont:
        push    18
        call    CrashAverted
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
DWORD RETURN_CrashFix_Misc19_US = 0x7F0BFD;
DWORD RETURN_CrashFix_Misc19_EU = 0x7F0C3D;
DWORD RETURN_CrashFix_Misc19_BOTH = 0;
DWORD RETURN_CrashFix_Misc19B_US = 0x7F0C20;
DWORD RETURN_CrashFix_Misc19B_EU = 0x7F0C60;
DWORD RETURN_CrashFix_Misc19B_BOTH = 0;
void _declspec(naked) HOOK_CrashFix_Misc19()
{
    _asm
    {
        cmp     esi, 0
        je      cont  // Skip much code if esi is zero

        // continue standard path
        mov     eax, [esi+98h]
        jmp     RETURN_CrashFix_Misc19_BOTH

    cont:
        push    19
        call    CrashAverted
        mov     edx,dword ptr [ecx+98h]
        test    edx,edx
        jmp     RETURN_CrashFix_Misc19B_BOTH
    }
}

////////////////////////////////////////////////////////////////////////
// Handle CPlaceable::RemoveMatrix having wrong data
#define HOOKPOS_CrashFix_Misc20                             0x54F3B0
#define HOOKSIZE_CrashFix_Misc20                            6
DWORD RETURN_CrashFix_Misc20 = 0x54F3B6;
void _declspec(naked) HOOK_CrashFix_Misc20()
{
    _asm
    {
        cmp     ecx, 0
        je      cont        // Skip much code if ecx is zero

        // continue standard path
        sub     esp, 10h
        mov     eax, [ecx+14h]
        jmp     RETURN_CrashFix_Misc20

    cont:
        push    20
        call    CrashAverted
        retn
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Handle CTaskSimpleCarFallOut::FinishAnimFallOutCB having wrong data
//
//
//////////////////////////////////////////////////////////////////////////////////////////
bool IsTaskSimpleCarFallOutValid(CAnimBlendAssociationSAInterface* pAnimBlendAssociation, CTaskSimpleCarFallOutSAInterface* pTask)
{
    if (pTask->VTBL != (TaskVTBL*)VTBL_CTaskSimpleCarFallOut)
    {
        AddReportLog(8530, SString("IsTaskSimpleCarFallOutValid fail - pTask->VTBL: %08x", pTask->VTBL), 5);
        return false;
    }

    if (pTask->pVehicle)
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pTask->pVehicle);
        CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        if (!pVehicle)
        {
            // Task looks valid, but vehicle is not recognised by MTA
            AddReportLog(8531, SString("IsTaskSimpleCarFallOutValid invalid vehicle ptr - pTask->pVehicle: %08x", pTask->pVehicle), 5);
            pTask->pVehicle = NULL;
            return true;
        }
    }

    return true;
}

#define HOOKPOS_CrashFix_Misc21                             0x648EE0
#define HOOKSIZE_CrashFix_Misc21                            7
DWORD RETURN_CrashFix_Misc21 = 0x648EE7;
void _declspec(naked) HOOK_CrashFix_Misc21()
{
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
        push    21
        call    CrashAverted
        retn
    }
}

////////////////////////////////////////////////////////////////////////
// Handle CAnimBlendAssociation::Init having wrong data
#define HOOKPOS_CrashFix_Misc22                             0x4CEF08
#define HOOKSIZE_CrashFix_Misc22                            6
DWORD RETURN_CrashFix_Misc22 = 0x4CEF25;
void _declspec(naked) HOOK_CrashFix_Misc22()
{
    _asm
    {
        mov         edx,dword ptr [edi+0Ch]

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
        push    22
        call    CrashAverted
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
DWORD RETURN_CrashFix_Misc23 = 0x6E3D17;
void _declspec(naked) HOOK_CrashFix_Misc23()
{
    _asm
    {
        // Ensure door index is reasonable
        mov     edx, [esp+8]
        cmp     edx,16
        jb      ok

        // zero if out of range
        mov     edx,0
        mov     [esp+8], edx
        push    23
        call    CrashAverted

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
DWORD RETURN_CrashFix_Misc24_US = 0x7F0DCE;
DWORD RETURN_CrashFix_Misc24_EU = 0x7F0E0E;
DWORD RETURN_CrashFix_Misc24_BOTH = 0;
void _declspec(naked) HOOK_CrashFix_Misc24()
{
    _asm
    {
        cmp     ebp, 0x480
        jb      cont  // Skip code if ebp is low

        // continue standard path
        mov     eax, [ebp+98h]
        jmp     RETURN_CrashFix_Misc24_BOTH

    cont:
        push    24
        call    CrashAverted
        mov     ebp, 0
        mov     eax, 0
        jmp     RETURN_CrashFix_Misc24_BOTH
    }
}

////////////////////////////////////////////////////////////////////////
// Handle CTaskSimpleCarOpenDoorFromOutside::FinishAnimCarOpenDoorFromOutsideCB having zero pointer to vehicle
#define HOOKPOS_CrashFix_Misc25                             0x646026
#define HOOKSIZE_CrashFix_Misc25                            5
DWORD RETURN_CrashFix_Misc25 = 0x64602B;
void _declspec(naked) HOOK_CrashFix_Misc25()
{
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
        push    25
        call    CrashAverted
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
DWORD RETURN_CrashFix_Misc26 = 0x739FA6;
void _declspec(naked) HOOK_CrashFix_Misc26()
{
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
        push    26
        call    CrashAverted
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
DWORD RETURN_CrashFix_Misc27 = 0x637802;
void _declspec(naked) HOOK_CrashFix_Misc27()
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
        push    27
        call    CrashAverted

cont:
        // Continue standard path
        jmp     RETURN_CrashFix_Misc27
    }
}

////////////////////////////////////////////////////////////////////////
// Handle CObject::ProcessGarageDoorBehaviour object with no dummy
#define HOOKPOS_CrashFix_Misc28                             0x44A4FD
#define HOOKSIZE_CrashFix_Misc28                            6
DWORD RETURN_CrashFix_Misc28 = 0x44A503;
DWORD RETURN_CrashFix_Misc28B = 0x44A650;
void _declspec(naked) HOOK_CrashFix_Misc28()
{
    _asm
    {
        // Execute replaced code
        mov     eax, [esi+170h]

        // Check if dummy pointer is zero
        test    eax, eax
        jne     cont

        push    28
        call    CrashAverted
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
DWORD RETURN_CrashFix_Misc29 = 0x4E0231;
DWORD RETURN_CrashFix_Misc29B = 0x4E0227;
void _declspec(naked) HOOK_CrashFix_Misc29()
{
    _asm
    {
        // Execute replaced code
        movsx   eax,word ptr [esp+8]

        // Check word being -1
        cmp     al, 0xffff
        jz      cont

        // Continue standard path
        jmp     RETURN_CrashFix_Misc29

cont:
        push    29
        call    CrashAverted
        // Skip much code
        jmp     RETURN_CrashFix_Misc29B
    }
}

////////////////////////////////////////////////////////////////////////
// Handle CAnimBlendAssociation::SetFinishCallback with invalid ecx
#define HOOKPOS_CrashFix_Misc30                             0x4CEBE8
#define HOOKSIZE_CrashFix_Misc30                            7
#define HOOKCHECK_CrashFix_Misc30                           0xC7
DWORD RETURN_CrashFix_Misc30 = 0x4CEBEF;
DWORD RETURN_CrashFix_Misc30B = 0x4CEBF5;
void _declspec(naked) HOOK_CrashFix_Misc30()
{
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
        push    30
        call    CrashAverted
        // Skip much code
        jmp     RETURN_CrashFix_Misc30B
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CClumpModelInfo::GetFrameFromId
//
//////////////////////////////////////////////////////////////////////////////////////////
RwFrame* OnMY_CClumpModelInfo_GetFrameFromId_Post(RwFrame* pFrameResult, DWORD _ebx, DWORD _esi, DWORD _edi, DWORD calledFrom, RpClump* pClump, int id)
{
    if (pFrameResult)
        return pFrameResult;

    // Don't check frame if call can legitimately return NULL
    if (calledFrom == 0x6D308F                // CVehicle::SetWindowOpenFlag
        || calledFrom == 0x6D30BF             // CVehicle::ClearWindowOpenFlag
        || calledFrom == 0x4C7DDE             // CVehicleModelInfo::GetOriginalCompPosition
        || calledFrom == 0x4C96BD)            // CVehicleModelInfo::CreateInstance
        return NULL;

    // Ignore external calls
    if (calledFrom < 0x401000 || calledFrom > 0x801000)
        return NULL;

    // Now we have a NULL frame which will cause a crash
    // (Probably due to a custom vehicle with missing frames)

    // See if we can get the model id for reporting
    int   iModelId = 0;
    DWORD pVehicle = NULL;

    if (calledFrom == 0x6D3847)            // CVehicle::AddReplacementUpgrade
        pVehicle = _ebx;
    else if (calledFrom == 0x6DFA61                // CVehicle::AddUpgrade
             || calledFrom == 0x6D3A62)            // CVehicle::GetReplacementUpgrade
        pVehicle = _edi;
    else if (calledFrom == 0x06AC740               // CAutomobile::PreRender (Forklift)
             || calledFrom == 0x6D39F3             // CVehicle::RemoveReplacementUpgrade
             || calledFrom == 0x6D3A32)            // CVehicle::RemoveReplacementUpgrade2
        pVehicle = _esi;

    if (pVehicle > 0x1000)
        iModelId = ((CVehicleSAInterface*)pVehicle)->m_nModelIndex;

    // Need a valid frame to replace the missing one - Find nearest other id
    for (uint i = 2; i < 40; i++)
    {
        RwFrame* pNewFrameResult = NULL;
        uint     uiNewId = id + (i / 2) * ((i & 1) ? -1 : 1);
        DWORD    dwFunc = 0x4C53C0;            // CClumpModelInfo::GetFrameFromId
        _asm
        {
            push    uiNewId
            push    pClump
            call    dwFunc
            add     esp, 8
            mov     pNewFrameResult,eax
        }

        if (pNewFrameResult)
        {
            SString strMsg("No frame for vehicle:%d  frameId:%d  (replaced with:%d  calledfrom:%08x)", iModelId, id, uiNewId, calledFrom);
            AddReportLog(5412, SString("GetFrameFromId - %s", *strMsg));
            LogEvent(5412, "Model frame warning", "GetFrameFromId", strMsg);
            return pNewFrameResult;
        }
    }

    // Couldn't find a replacement frame id
    SString strMsg("No frame for vehicle:%d  frameId:%d  (calledfrom:%08x)", iModelId, id, calledFrom);
    AddReportLog(5413, SString("GetFrameFromId - %s", *strMsg));
    LogEvent(5413, "Model frame error", "GetFrameFromId", strMsg);

    return NULL;
}

// Hook info
#define HOOKPOS_CClumpModelInfo_GetFrameFromId                      0x4C53C0
#define HOOKSIZE_CClumpModelInfo_GetFrameFromId                     7
DWORD RETURN_CClumpModelInfo_GetFrameFromId = 0x4C53C7;
void _declspec(naked) HOOK_CClumpModelInfo_GetFrameFromId()
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

CStreamingInfo* GetStreamingInfoFromModelId(uint id)
{
    CStreamingInfo* pItemInfo = (CStreamingInfo*)(CStreaming__ms_aInfoForModel);
    return pItemInfo + id;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CEntity::GetBoundRect
//
// Check if crash will occur
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CEntity_GetBoundRect(CEntitySAInterface* pEntity)
{
    uint32                     usModelId = pEntity->m_nModelIndex;
    CBaseModelInfoSAInterface* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelId];
    if (!pModelInfo)
    {
        // Crash will occur at offset 00134131
        LogEvent(814, "Model info missing", "CEntity_GetBoundRect", SString("No info for model:%d", usModelId), 5414);
        CArgMap argMap;
        argMap.Set("id", usModelId);
        argMap.Set("reason", "info");
        SetApplicationSetting("diagnostics", "gta-model-fail", argMap.ToString());
    }
    else
    {
        CColModelSAInterface* pColModel = pModelInfo->pColModel;
        if (!pColModel)
        {
            // Crash will occur at offset 00134134
            CStreamingInfo* pStreamingInfo = pGameInterface->GetStreaming()->GetStreamingInfoFromModelId(usModelId);
            SString         strDetails("refs:%d txd:%d RwObj:%08x bOwn:%d bColStr:%d flg:%d off:%d size:%d loadState:%d", pModelInfo->usNumberOfRefs,
                               pModelInfo->usTextureDictionary, pModelInfo->pRwObject, pModelInfo->bDoWeOwnTheColModel,
                               pModelInfo->bCollisionWasStreamedWithModel, pStreamingInfo->flg, pStreamingInfo->offsetInBlocks, pStreamingInfo->sizeInBlocks,
                               pStreamingInfo->loadState);
            LogEvent(815, "Model collision missing", "CEntity_GetBoundRect", SString("No collision for model:%d %s", usModelId, *strDetails), 5415);
            CArgMap argMap;
            argMap.Set("id", usModelId);
            argMap.Set("reason", "collision");
            SetApplicationSetting("diagnostics", "gta-model-fail", argMap.ToString());
        }
    }
}

// Hook info
#define HOOKPOS_CEntity_GetBoundRect                      0x534131
#define HOOKSIZE_CEntity_GetBoundRect                     5
#define HOOKCHECK_CEntity_GetBoundRect                    0x8B
DWORD RETURN_CEntity_GetBoundRect = 0x534136;
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
        mov     eax, [ecx+14h]
        mov     edx, [eax]
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
void OnMY_CVehicle_AddUpgrade_Pre(CVehicleSAInterface* pVehicle, int iUpgradeId, int iFrame)
{
    CArgMap argMap;
    argMap.Set("vehid", pVehicle->m_nModelIndex);
    argMap.Set("upgid", iUpgradeId);
    argMap.Set("frame", iFrame);
    SetApplicationSetting("diagnostics", "gta-upgrade-fail", argMap.ToString());
}

void OnMY_CVehicle_AddUpgrade_Post()
{
    SetApplicationSetting("diagnostics", "gta-upgrade-fail", "");
}

// Hook info
#define HOOKPOS_CVehicle_AddUpgrade                      0x6DFA20
#define HOOKSIZE_CVehicle_AddUpgrade                     6
#define HOOKCHECK_CVehicle_AddUpgrade                    0x51
DWORD RETURN_CVehicle_AddUpgrade = 0x6DFA26;
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
void _declspec(naked) HOOK_TrainCrossingBarrierCrashFix()
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
void _declspec(naked) HOOK_ResetFurnitureObjectCounter()
{
    *(int*)0xBB3A18 = 0;            // InteriorManager_c::ms_objectCounter

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
void OnMY_CVolumetricShadowMgr_Render_Pre()
{
    OnEnterCrashZone(1);
}

void OnMY_CVolumetricShadowMgr_Render_Post()
{
    OnEnterCrashZone(0);
}

// Hook info
#define HOOKPOS_CVolumetricShadowMgr_Render                 0x7113B0
#define HOOKSIZE_CVolumetricShadowMgr_Render                8
#define HOOKCHECK_CVolumetricShadowMgr_Render               0x83
DWORD RETURN_CVolumetricShadowMgr_Render = 0x7113B8;
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
void OnMY_CVolumetricShadowMgr_Update_Pre()
{
    OnEnterCrashZone(2);
}

void OnMY_CVolumetricShadowMgr_Update_Post()
{
    OnEnterCrashZone(0);
}

// Hook info
#define HOOKPOS_CVolumetricShadowMgr_Update                 0x711D90
#define HOOKSIZE_CVolumetricShadowMgr_Update                5
#define HOOKCHECK_CVolumetricShadowMgr_Update               0xB9
DWORD RETURN_CVolumetricShadowMgr_Update = 0x711D95;
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
void OnMY_CAnimManager_CreateAnimAssocGroups(uint uiModelId)
{
    CModelInfo* pModelInfo = pGameInterface->GetModelInfo(uiModelId);
    if (pModelInfo->GetInterface()->pRwObject == NULL)
    {
        // Crash will occur at offset 00349b7b
        LogEvent(816, "Model not loaded", "CAnimManager_CreateAnimAssocGroups", SString("No RwObject for model:%d", uiModelId), 5416);
        CArgMap argMap;
        argMap.Set("id", uiModelId);
        argMap.Set("reason", "createanim");
        SetApplicationSetting("diagnostics", "gta-model-fail", argMap.ToString());
    }
}

// Hook info
#define HOOKPOS_CAnimManager_CreateAnimAssocGroups                 0x4D3D52
#define HOOKSIZE_CAnimManager_CreateAnimAssocGroups                5
#define HOOKCHECK_CAnimManager_CreateAnimAssocGroups               0x8B
DWORD RETURN_CAnimManager_CreateAnimAssocGroups = 0x4D3D59;
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
        push    ecx
        mov     ecx, dword ptr[ARRAY_ModelInfo]
        mov     eax, dword ptr[ecx + eax*4]
        pop     ecx

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
DWORD RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask = 0x6485B2;
DWORD RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask_Invalid = 0x6485E1;
void  OnMY_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask()
{
    LogEvent(817, "CTaskComplexCarSlowBeDraggedOut", "", "CTaskComplexCarSlowBeDraggedOut race condition");
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
void _cdecl OnMY_printf(DWORD dwCalledFrom, const char* szMessage)
{
    SString strMessage = SStringX(szMessage).Replace("\n", "");

    // Ignore unimportant messages
    if (strMessage == "Initialised SoundManager" || strMessage == "%f")
    {
        return;
    }

    SString strContext("GTALOG Called from 0x%08x", dwCalledFrom);
    LogEvent(6311, "printf", strContext, strMessage, 6311);

    // Check for known issues
    if (strMessage == "Error subrastering")
    {
        // Couldn't create render target for CPostEffects
        BrowseToSolution("error-subrastering", EXIT_GAME_FIRST | ASK_GO_ONLINE, _("Problem with graphics driver"));
    }
    else if (strMessage == "Too many objects without modelinfo structures")
    {
        // An img file contains errors
        if (GetApplicationSetting("diagnostics", "img-file-corrupt").empty())
        {
            SetApplicationSetting("diagnostics", "img-file-corrupt", "gta3.img");
        }
    }
}

// hook info
#define HOOKPOS_printf_US                         0x821982
#define HOOKSIZE_printf_US                        7
#define HOOKCHECK_printf_US                       0x6A
#define HOOKPOS_printf_EU                         0x8219C2
#define HOOKSIZE_printf_EU                        7
#define HOOKCHECK_printf_EU                       0x6A
DWORD RETURN_printf_US = 0x821989;
DWORD RETURN_printf_EU = 0x8219C9;
DWORD RETURN_printf_BOTH = 0;
void _declspec(naked) HOOK_printf()
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

////////////////////////////////////////////////////////////////////////
//
// RwMatrixMultiply
//
// Check for invalid matix pointer
//
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_RwMatrixMultiply_US             0x7F18B0
#define HOOKSIZE_RwMatrixMultiply_US            6
#define HOOKCHECK_RwMatrixMultiply_US           0x8B
#define HOOKPOS_RwMatrixMultiply_EU             0x7F18F0
#define HOOKSIZE_RwMatrixMultiply_EU            6
#define HOOKCHECK_RwMatrixMultiply_EU           0x8B
DWORD RETURN_RwMatrixMultiply_US = 0x7F18B6;
DWORD RETURN_RwMatrixMultiply_EU = 0x7F18F6;
DWORD RETURN_RwMatrixMultiply_BOTH = 0;
void _declspec(naked) HOOK_RwMatrixMultiply()
{
    _asm
    {
        mov     eax, [esp+0Ch]
        cmp     eax, 0x480
        jb      cont  // Skip code if eax is low

        mov     ecx, dword ptr ds:[0C979BCh]
        jmp     RETURN_RwMatrixMultiply_BOTH

cont:
        push    31
        call    CrashAverted
        retn
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAnimBlendNode_GetCurrentTranslation
//
// Check for corrupt endKeyFrameIndex
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CAnimBlendNode_GetCurrentTranslation(CAnimBlendNodeSAInterface* pInterface)
{
    // Crash will occur at offset 0xCFCD6
    OnCrashAverted(32);
    CAnimBlendAssociationSAInterface* pAnimAssoc = pInterface->pAnimBlendAssociation;
    CAnimBlendSequenceSAInterface*    pAnimSequence = pInterface->pAnimSequence;
    CAnimBlendHierarchySAInterface*   pAnimHierarchy = pAnimAssoc->pAnimHierarchy;

    bool                           bSequenceExistsInHierarchy = false;
    CAnimBlendSequenceSAInterface* pAnimHierSequence = pAnimHierarchy->pSequences;
    for (int i = 0; i < pAnimHierarchy->usNumSequences; i++)
    {
        if (pAnimHierSequence == pAnimSequence)
        {
            bSequenceExistsInHierarchy = true;
            break;
        }
        pAnimHierSequence++;
    }

    LogEvent(588, "GetCurrentTranslation", "Incorrect endKeyFrameIndex",
             SString("m_endKeyFrameId = %d | pAnimAssoc = %p | GroupID = %d | AnimID = %d | \
                pAnimSeq = %p | BoneID = %d | BoneHash = %u | \
                pAnimHier = %p | HierHash = %u | SequenceExistsInHierarchy: %s",
                     pInterface->m_endKeyFrameId, pAnimAssoc, pAnimAssoc->sAnimGroup, pAnimAssoc->sAnimID, pAnimSequence, pAnimSequence->m_boneId,
                     pAnimSequence->m_hash, pAnimHierarchy, pAnimHierarchy->uiHashKey, bSequenceExistsInHierarchy ? "Yes" : "No"),
             588);
}

// Hook info
#define HOOKPOS_CAnimBlendNode_GetCurrentTranslation                 0x4CFCB5
#define HOOKSIZE_CAnimBlendNode_GetCurrentTranslation                6
DWORD RETURN_CAnimBlendNode_GetCurrentTranslation = 0x4CFCBB;
void _declspec(naked) HOOK_CAnimBlendNode_GetCurrentTranslation()
{
    _asm
    {
        // if end key frame index is greater than 10,000 then return
        cmp     eax, 0x2710
        jg      altcode

        push    ebx
        mov     bl, [edx + 4]
        shr     bl, 1
        jmp     RETURN_CAnimBlendNode_GetCurrentTranslation

        // do alternate code
        altcode :
        pushad
        push    ebp // this
        call    OnMY_CAnimBlendNode_GetCurrentTranslation
        add     esp, 4 * 1
        popad

        pop     edi
        pop     esi
        pop     ebp
        add     esp, 18h
        retn    8
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CStreaming_AreAnimsUsedByRequestedModels
//
// GTA streamer will use this function to decide if IFP blocks should be unloaded or not.
// We will return true to disable unloading.
//
//////////////////////////////////////////////////////////////////////////////////////////
bool __cdecl OnMY_CStreaming_AreAnimsUsedByRequestedModels(int modelID)
{
    // GTA SA possibly cannot have more than 200 IFP blocks since that's the limit
    const int maximumIFPBlocks = 200;
    if (modelID < 0 || modelID > maximumIFPBlocks)
    {
        return false;
    }

    std::unique_ptr<CAnimBlock> pInternalBlock = g_pCore->GetGame()->GetAnimManager()->GetAnimationBlock(modelID);
    if (!pInternalBlock->IsLoaded())
    {
        return false;
    }
    return true;
}

// Hook info
#define HOOKPOS_CStreaming_AreAnimsUsedByRequestedModels                0x407AD5
#define HOOKSIZE_CStreaming_AreAnimsUsedByRequestedModels               7
void _declspec(naked) HOOK_CStreaming_AreAnimsUsedByRequestedModels()
{
    _asm
    {
        push    [esp + 4]
        call    OnMY_CStreaming_AreAnimsUsedByRequestedModels
        add     esp, 0x4
        retn
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTrain::ProcessControl
//
// This hook overwrites the logic to wrap the train's rail distance, because in the
// original game code this could cause an infinite loop
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6F8F83 | 88 9E CA 05 00 00 | mov     [esi + 5CAh], bl
// >>> 0x6F8F89 | D9 86 A8 05 00 00 | fld     dword ptr [esi + 5A8h]
//     0x6F8F8F | D8 1D 50 8B 85 00 | fcomp   ds: __real @00000000
#define HOOKPOS_CTrain__ProcessControl         0x6F8F89
#define HOOKSIZE_CTrain__ProcessControl        6
static DWORD CONTINUE_CTrain__ProcessControl = 0x6F8FE5;

// 0xC37FEC; float RailTrackLength[NUM_TRACKS]
static float* RailTrackLength = reinterpret_cast<float*>(0xC37FEC);

static void _cdecl WrapTrainRailDistance(CTrainSAInterface* train)
{
    // Check if the train is driving on a valid rail track (id < NUM_TRACKS)
    if (train->m_ucRailTrackID >= 4)
    {
        train->m_fTrainRailDistance = 0.0f;
        return;
    }

    // Check if the current rail track has a valid length (>= 1.0f)
    const float railTrackLength = RailTrackLength[train->m_ucRailTrackID];

    if (railTrackLength < 1.0f)
    {
        train->m_fTrainRailDistance = 0.0f;
        return;
    }

    // Check if the current rail distance is in the interval [0, railTrackLength)
    float railDistance = train->m_fTrainRailDistance;

    if (railDistance >= 0.0f && railDistance < railTrackLength)
        return;

    // Wrap the current rail distance
    if (railDistance > 0.0f)
    {
        railDistance = std::fmodf(railDistance, railTrackLength);
    }
    else
    {
        railDistance = railTrackLength - std::fmodf(std::fabsf(railDistance), railTrackLength);
    }

    train->m_fTrainRailDistance = railDistance;
}

static void _declspec(naked) HOOK_CTrain__ProcessControl()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    WrapTrainRailDistance
        add     esp, 4
        popad
        jmp     CONTINUE_CTrain__ProcessControl
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask
//
// This hook adds a null-pointer check for eax, which stores the ped's current vehicle.
// Returning a null-pointer from this function will prevent the animation from being played.
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x648AAC | C2 04 00             | retn   4
// >>> 0x648AAF | 8B 80 84 03 00 00    | mov    eax, [eax + 384h]
//     0x648AB5 | 0F B6 80 DE 00 00 00 | movzx  eax, byte ptr [eax + 0DEh]
#define HOOKPOS_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask         0x648AAF
#define HOOKSIZE_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask        6
static DWORD CONTINUE_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask = 0x648AB5;

static void _cdecl LOG_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask()
{
    LogEvent(819, "CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask", "eax is null", "");
}

static void _declspec(naked) HOOK_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask()
{
    _asm
    {
        test    eax, eax
        jz      returnZeroTaskLocation
        mov     eax, [eax + 384h]
        jmp     CONTINUE_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask

        returnZeroTaskLocation:
        pushad
        call    LOG_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask
        popad
        pop     edi
        pop     esi
        retn    4
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicleModelInfo::LoadVehicleColours
//
// A modified data/carcols.dat can have entries with invalid model names and these cause
// CModelInfo::GetModelInfo to return a null pointer, but the original code doesn't verify
// the return value and tries to use the null pointer. This hook adds a null pointer check
// and then skips the line if in the null case. There are two locations to hook.
//
//////////////////////////////////////////////////////////////////////////////////////////
static void _cdecl LOG_CVehicleModelInfo__LoadVehicleColours(int location, const char* modelName)
{
    LogEvent(820 + location, "CVehicleModelInfo::LoadVehicleColours", "Could not find model by name:", modelName);
}

//     0x5B6B1B | E8 20 EE F0 FF | call  CModelInfo::GetModelInfo
// >>> 0x5B6B20 | 8B F0          | mov   esi, eax
//     0x5B6B22 | 8D 47 FF       | lea   eax, [edi - 1]
//     0x5B6B25 | 99             | cdq
#define HOOKPOS_CVehicleModelInfo__LoadVehicleColours_1         0x5B6B20
#define HOOKSIZE_CVehicleModelInfo__LoadVehicleColours_1        5
static DWORD CONTINUE_CVehicleModelInfo__LoadVehicleColours_1 = 0x5B6B25;
static DWORD SKIP_CVehicleModelInfo__LoadVehicleColours_1 = 0x5B6D04;

static void _declspec(naked) HOOK_CVehicleModelInfo__LoadVehicleColours_1()
{
    _asm
    {
        test    eax, eax
        jnz     continueLoadingColorLineLocation

        pushad
        lea     ecx, [esp + 55Ch - 440h]
        push    ecx
        push    0
        call    LOG_CVehicleModelInfo__LoadVehicleColours
        add     esp, 8
        popad

        add     esp, 54h
        jmp     SKIP_CVehicleModelInfo__LoadVehicleColours_1

        continueLoadingColorLineLocation:
        mov     esi, eax
        lea     eax, [edi - 1]
        jmp     CONTINUE_CVehicleModelInfo__LoadVehicleColours_1
    }
}

//     0x5B6CA5 | E8 96 EC F0 FF | call  CModelInfo::GetModelInfo
// >>> 0x5B6CAA | 8B F0          | mov   esi, eax
//     0x5B6CAC | 8D 47 FF       | lea   eax, [edi - 1]
//     0x5B6CAF | 99             | cdq
#define HOOKPOS_CVehicleModelInfo__LoadVehicleColours_2         0x5B6CAA
#define HOOKSIZE_CVehicleModelInfo__LoadVehicleColours_2        5
static DWORD CONTINUE_CVehicleModelInfo__LoadVehicleColours_2 = 0x5B6CAF;
static DWORD SKIP_CVehicleModelInfo__LoadVehicleColours_2 = 0x5B6D04;

static void _declspec(naked) HOOK_CVehicleModelInfo__LoadVehicleColours_2()
{
    _asm
    {
        test    eax, eax
        jnz     continueLoadingColorLineLocation

        pushad
        lea     ecx, [esp + 59Ch - 440h]
        push    ecx
        push    1
        call    LOG_CVehicleModelInfo__LoadVehicleColours
        add     esp, 8
        popad

        add     esp, 94h
        jmp     SKIP_CVehicleModelInfo__LoadVehicleColours_2

        continueLoadingColorLineLocation:
        mov     esi, eax
        lea     eax, [edi - 1]
        jmp     CONTINUE_CVehicleModelInfo__LoadVehicleColours_2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPlaceName::Process
//
// Prevent the original game code from accessing the ped's vehicle, when it's a null pointer
// and the ped flag bInVehicle is set by setting the ped flag to zero.
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x571F37 | 8B F1             | mov   esi, ecx
// >>> 0x571F39 | 8B 88 6C 04 00 00 | mov   ecx, [eax + 46Ch]
//     0x571F3F | F6 C5 01          | test  ch, 1
#define HOOKPOS_CPlaceName__Process         0x571F39
#define HOOKSIZE_CPlaceName__Process        6
static DWORD CONTINUE_CPlaceName__Process = 0x571F3F;

static void _declspec(naked) HOOK_CPlaceName__Process()
{
    _asm
    {
        pushad
        mov     ecx, [eax + 46Ch]
        test    ch, 1                       // if (ped->pedFlags.bInVehicle
        jz      continueAfterFixLocation
        mov     ebx, [eax + 58Ch]           //     && !ped->m_pVehicle)
        test    ebx, ebx
        jnz     continueAfterFixLocation
        and     ch, 0FEh
        mov     dword ptr [eax + 46Ch], ecx // ped->pedFlags.bInVehicle = 0

        continueAfterFixLocation:
        popad
        mov     ecx, [eax + 46Ch]
        jmp     CONTINUE_CPlaceName__Process
    }
}

static void LOG_CWorld__FindObjectsKindaCollidingSectorList(unsigned int modelId)
{
    CBaseModelInfoSAInterface* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[modelId];
    if (!pModelInfo)
    {
        LogEvent(840, "Model info missing", "CWorld__FindObjectsKindaCollidingSectorList", SString("Corrupt model: %d", modelId), 5601);
        return;
    }

    if (!pModelInfo->pColModel)
    {
        LogEvent(840, "Col model missing", "CWorld__FindObjectsKindaCollidingSectorList", SString("Corrupt col model: %d", modelId), 5601);
    }
}

#define HOOKPOS_CWorld__FindObjectsKindaCollidingSectorList 0x56508C
#define HOOKSIZE_CWorld__FindObjectsKindaCollidingSectorList 0xA
static const unsigned int RETURN_CWorld__FindObjectsKindaCollidingSectorList = 0x565096;
static const unsigned int RETURN_CWorld__FindObjectsKindaCollidingSectorList_SKIP = 0x5650C3;
static void _declspec(naked) HOOK_CWorld__FindObjectsKindaCollidingSectorList()
{
    _asm {
        mov eax, [edx*4+0xA9B0C8]   // CModelInfo::ms_modelInfoPtrs
        test eax, eax
        jz skip

        mov ecx, [eax+0x14]         // m_pColModel
        test ecx, ecx
        jz skip

        jmp RETURN_CWorld__FindObjectsKindaCollidingSectorList

    skip:
        pushad
        push edx
        call LOG_CWorld__FindObjectsKindaCollidingSectorList
        add esp, 4
        popad

        jmp RETURN_CWorld__FindObjectsKindaCollidingSectorList_SKIP
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RpClumpForAllAtomics
//
// Adds a nullptr check for the clump object pointer.
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x749B70 | 8B 44 24 04 | mov  eax, [esp+arg_0]
// >>> 0x749B74 | 53          | push ebx
// >>> 0x749B75 | 55          | push ebp
//     0x749B76 | 56          | push esi
#define HOOKPOS_RpClumpForAllAtomics         0x749B70
#define HOOKSIZE_RpClumpForAllAtomics        6
static DWORD CONTINUE_RpClumpForAllAtomics = 0x749B76;

static void _declspec(naked) HOOK_RpClumpForAllAtomics()
{
    _asm
    {
        mov     eax, [esp+4]    // RpClump* clump
        test    eax, eax
        jnz     continueAfterFixLocation
        retn

        continueAfterFixLocation:
        push    ebx
        push    ebp
        jmp     CONTINUE_RpClumpForAllAtomics
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RpAnimBlendClumpGetFirstAssociation
//
// Adds a nullptr check for the clump object pointer.
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x4D6A70 | 8B 0D 78 F8 B5 00 | mov ecx, ds:_ClumpOffset
//     0x4D6A76 | 8B 44 24 04       | mov eax, [esp+4]
#define HOOKPOS_RpAnimBlendClumpGetFirstAssociation         0x4D6A70
#define HOOKSIZE_RpAnimBlendClumpGetFirstAssociation        6
static DWORD CONTINUE_RpAnimBlendClumpGetFirstAssociation = 0x4D6A76;

static void _declspec(naked) HOOK_RpAnimBlendClumpGetFirstAssociation()
{
    _asm
    {
        mov     eax, [esp+4]            // RpClump* clump
        test    eax, eax
        jnz     continueAfterFixLocation
        retn

        continueAfterFixLocation:
        mov     ecx, ds:[0xB5F878]
        jmp     CONTINUE_RpAnimBlendClumpGetFirstAssociation
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAnimManager::BlendAnimation
//
// Adds a nullptr check for the clump object pointer.
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x4D4610 | 83 EC 14          | sub esp, 14h
// >>> 0x4D4613 | 8B 4C 24 18       | mov ecx, [esp+18h]
//     0x4D4617 | 8B 15 34 EA B4 00 | mov edx, CAnimManager::ms_aAnimAssocGroups
#define HOOKPOS_CAnimManager__BlendAnimation         0x4D4610
#define HOOKSIZE_CAnimManager__BlendAnimation        7
static DWORD CONTINUE_CAnimManager__BlendAnimation = 0x4D4617;

static void _declspec(naked) HOOK_CAnimManager__BlendAnimation()
{
    _asm
    {
        mov     eax, [esp+4]            // RpClump* clump
        test    eax, eax
        jnz     continueAfterFixLocation
        retn

        continueAfterFixLocation:
        sub     esp, 14h
        mov     ecx, [esp+18h]
        jmp     CONTINUE_CAnimManager__BlendAnimation
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// FxSystemBP_c::Load
//
// Remove every FxEmitter without a main texture because the game logic expects AT LEAST
// one texture at index 0 ("main texture").
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x5C0A14 | 5E                   | pop  esi
// >>> 0x5C0A15 | 5D                   | pop  ebp
// >>> 0x5C0A16 | 32 C0                | xor  al, al
// >>> 0x5C0A18 | 5B                   | pop  ebx
// >>> 0x5C0A19 | 64 89 0D 00 00 00 00 | mov  large fs:0, ecx
// >>> 0x5C0A20 | 81 C4 E8 05 00 00    | add  esp, 5E8h
// >>> 0x5C0A26 | C2 0C 00             | retn 0Ch
#define HOOKPOS_FxSystemBP_c__Load  0x5C0A15
#define HOOKSIZE_FxSystemBP_c__Load 19

static void _cdecl POST_PROCESS_FxSystemBP_c__Load(CFxSystemBPSAInterface* blueprint)
{
    if (!blueprint->cNumOfPrims)
        return;

    char count = blueprint->cNumOfPrims;
    char last = count - 1;

    for (char i = last; i >= 0; i--)
    {
        if (blueprint->pPrims[i]->m_apTextures[0])
            continue;

        blueprint->pPrims[i] = nullptr;
        --count;

        if (i != last)
        {
            std::swap(blueprint->pPrims[i], blueprint->pPrims[last]);
            --last;
        }
    }

    if (blueprint->cNumOfPrims != count)
    {
        OnCrashAverted(33);
        blueprint->cNumOfPrims = count;
    }
}

static void _declspec(naked) HOOK_FxSystemBP_c__Load()
{
    _asm
    {
        pushad
        push    ebp
        call    POST_PROCESS_FxSystemBP_c__Load
        add     esp, 4
        popad

        pop     ebp
        xor     al, al
        pop     ebx
    //  mov     large fs:0, ecx
        add     esp, 5E8h
        retn    0Ch
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// FxPrim_c::Enable
//
// Add a null-pointer check for the ecx pointer. This hook is a side-effect of the hook for
// FxSystemBP_c::Load above.
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x4A9F50 | 8A 44 24 04 | mov  al, [esp+4]
// >>> 0x4A9F54 | 88 41 0C    | mov  [ecx+0xC], al
// >>> 0x4A9F57 | C2 04 00    | retn 4
#define HOOKPOS_FxPrim_c__Enable  0x4A9F50
#define HOOKSIZE_FxPrim_c__Enable 10

static void _declspec(naked) HOOK_FxPrim_c__Enable()
{
    _asm
    {
        test    ecx, ecx
        jz      returnFromFunction
        mov     al, [esp+4]
        mov     [ecx+0xC], al

        returnFromFunction:
        retn    4
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for CrashFixHacks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_CrashFixHacks()
{
    EZHookInstall(CrashFix_Misc1);
    EZHookInstall(CrashFix_Misc2);
    // EZHookInstall ( CrashFix_Misc3 );
    EZHookInstall(CrashFix_Misc4);
    EZHookInstall(CrashFix_Misc5);
    EZHookInstall(CrashFix_Misc6);
    EZHookInstall(CrashFix_Misc7);
    EZHookInstall(CrashFix_Misc8);
    EZHookInstall(CrashFix_Misc9);
    EZHookInstall(CrashFix_Misc10);
    EZHookInstall(CrashFix_Misc11);
    // EZHookInstall ( CrashFix_Misc12 );
    EZHookInstall(CrashFix_Misc13);
    EZHookInstall(CrashFix_Misc14);
    EZHookInstall(FreezeFix_Misc15);
    EZHookInstall(CrashFix_Misc16);
    // EZHookInstall ( CrashFix_Misc17 );
    EZHookInstall(CrashFix_Misc18);
    // EZHookInstall ( CrashFix_Misc19 );
    EZHookInstall(CrashFix_Misc20);
    EZHookInstall(CrashFix_Misc21);
    EZHookInstall(CrashFix_Misc22);
    EZHookInstall(CrashFix_Misc23);
    EZHookInstall(CrashFix_Misc24);
    EZHookInstall(CrashFix_Misc25);
    EZHookInstall(CrashFix_Misc26);
    EZHookInstall(CrashFix_Misc27);
    EZHookInstall(CrashFix_Misc28);
    EZHookInstall(CrashFix_Misc29);
    EZHookInstallChecked(CrashFix_Misc30);
    EZHookInstall(CClumpModelInfo_GetFrameFromId);
    EZHookInstallChecked(CEntity_GetBoundRect);
    EZHookInstallChecked(CVehicle_AddUpgrade);
    EZHookInstall(ResetFurnitureObjectCounter);
    EZHookInstallChecked(CVolumetricShadowMgr_Render);
    EZHookInstallChecked(CVolumetricShadowMgr_Update);
    EZHookInstallChecked(CAnimManager_CreateAnimAssocGroups);
    EZHookInstall(CAnimBlendNode_GetCurrentTranslation);
    EZHookInstall(CStreaming_AreAnimsUsedByRequestedModels);
    EZHookInstall(CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask);
    EZHookInstallChecked(printf);
    EZHookInstallChecked(RwMatrixMultiply);
    EZHookInstall(CTrain__ProcessControl);
    EZHookInstall(CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask);
    EZHookInstall(CVehicleModelInfo__LoadVehicleColours_1);
    EZHookInstall(CVehicleModelInfo__LoadVehicleColours_2);
    EZHookInstall(CPlaceName__Process);
    EZHookInstall(CWorld__FindObjectsKindaCollidingSectorList);
    EZHookInstall(RpClumpForAllAtomics);
    EZHookInstall(RpAnimBlendClumpGetFirstAssociation);
    EZHookInstall(CAnimManager__BlendAnimation);
    EZHookInstall(FxSystemBP_c__Load);
    EZHookInstall(FxPrim_c__Enable);

    // Install train crossing crashfix (the temporary variable is required for the template logic)
    void (*temp)() = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_Destructor_TrainCrossing_Check, RETURN_CObject_Destructor_TrainCrossing_Invalid>;
    HookInstall(HOOKPOS_CObject_Destructor_TrainCrossing_Check, (DWORD)temp, 5);
    temp = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_ProcessTrainCrossingBehavior1_Check, RETURN_CObject_ProcessTrainCrossingBehavior_Invalid>;
    HookInstall(HOOKPOS_CObject_ProcessTrainCrossingBehavior1, (DWORD)temp, 5);
    temp = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_ProcessTrainCrossingBehavior2_Check, RETURN_CObject_ProcessTrainCrossingBehavior_Invalid>;
    HookInstall(HOOKPOS_CObject_ProcessTrainCrossingBehavior2, (DWORD)temp, 5);
}
