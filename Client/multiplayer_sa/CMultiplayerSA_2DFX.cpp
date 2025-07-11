/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_2DFX.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

////////////////////////////////////////////////////////////////////////
// CTrafficLights::DisplayActualLight
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTrafficLights__DisplayActualLight 0X49DC18
#define HOOKSIZE_CTrafficLights__DisplayActualLight 5
static constexpr std::uintptr_t RETURN_CTrafficLights__DisplayActualLight = 0x49DC1D;
static constexpr std::uintptr_t SKIP_CTrafficLights__DisplayActualLight = 0x49DE1D;
static void _declspec(naked) HOOK_CTrafficLights__DisplayActualLight()
{
    _asm
    {
        mov ebp, eax
        test ebp, ebp
        jz skip

        mov al, [ebp+0Ch]
        jmp RETURN_CTrafficLights__DisplayActualLight

        skip:
        jmp SKIP_CTrafficLights__DisplayActualLight
    }
}

////////////////////////////////////////////////////////////////////////
// CEntity::HasPreRenderEffects
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CEntity__HasPreRenderEffects 0x532D19
#define HOOKSIZE_CEntity__HasPreRenderEffects 5
static constexpr std::uintptr_t RETURN_CEntity__HasPreRenderEffects = 0x532D1E;
static constexpr std::uintptr_t SKIP_CEntity__HasPreRenderEffects = 0x532D20;
static void _declspec(naked) HOOK_CEntity__HasPreRenderEffects()
{
    _asm
    {
        test eax, eax
        jz skip

        mov cl, [eax+0Ch]
        test cl, cl
        jmp RETURN_CEntity__HasPreRenderEffects

        skip:
        jmp SKIP_CEntity__HasPreRenderEffects
    }
}

////////////////////////////////////////////////////////////////////////
// CEntity::FindTriggerPointCoors
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CEntity__FindTriggerPointCoors 0x5333A8
#define HOOKSIZE_CEntity__FindTriggerPointCoors 6
static constexpr std::uintptr_t RETURN_CEntity__FindTriggerPointCoors = 0x5333AE;
static constexpr std::uintptr_t SKIP_CEntity__FindTriggerPointCoors = 0x5333B9;
static void _declspec(naked) HOOK_CEntity__FindTriggerPointCoors()
{
    _asm
    {
        test eax, eax
        jz skip

        mov esi, eax
        cmp byte ptr [esi+0Ch], 8
        jmp RETURN_CEntity__FindTriggerPointCoors

        skip:
        jmp SKIP_CEntity__FindTriggerPointCoors
    }
}

////////////////////////////////////////////////////////////////////////
// CEntity::GetRandom2dEffect
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CEntity__GetRandom2dEffect 0x533448
#define HOOKSIZE_CEntity__GetRandom2dEffect 6
static constexpr std::uintptr_t RETURN_CEntity__GetRandom2dEffect = 0x53344E;
static constexpr std::uintptr_t SKIP_CEntity__GetRandom2dEffect = 0x533482;
static void _declspec(naked) HOOK_CEntity__GetRandom2dEffect()
{
    _asm
    {
        test eax, eax
        jz skip

        mov esi, eax
        movzx ecx, byte ptr [esi+0Ch]
        jmp RETURN_CEntity__GetRandom2dEffect

        skip:
        jmp SKIP_CEntity__GetRandom2dEffect
    }
}

////////////////////////////////////////////////////////////////////////
// CEntity::CreateEffects
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CEntity__CreateEffects 0x5337D8
#define HOOKSIZE_CEntity__CreateEffects 6
static constexpr std::uintptr_t RETURN_CEntity__CreateEffects = 0x5337DE;
static constexpr std::uintptr_t SKIP_CEntity__CreateEffects = 0x533B9C;
static void _declspec(naked) HOOK_CEntity__CreateEffects()
{
    _asm
    {
        test eax, eax
        jz skip

        mov esi, eax
        movzx eax, byte ptr [esi+0Ch]
        jmp RETURN_CEntity__CreateEffects

        skip:
        jmp SKIP_CEntity__CreateEffects
    }
}

////////////////////////////////////////////////////////////////////////
// CEntity::DestroyEffects
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CEntity__DestroyEffects 0x533C2A
#define HOOKSIZE_CEntity__DestroyEffects 5
static constexpr std::uintptr_t RETURN_CEntity__DestroyEffects = 0x533C2F;
static constexpr std::uintptr_t SKIP_CEntity__DestroyEffects = 0x533D14;
static void _declspec(naked) HOOK_CEntity__DestroyEffects()
{
    _asm
    {
        test eax, eax
        jz skip

        mov esi, eax
        mov al, [esi+0Ch]
        jmp RETURN_CEntity__DestroyEffects

        skip:
        jmp SKIP_CEntity__DestroyEffects
    }
}

////////////////////////////////////////////////////////////////////////
// CEntity::RenderEffects
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CEntity__RenderEffects 0x5342D5
#define HOOKSIZE_CEntity__RenderEffects 5
static constexpr std::uintptr_t RETURN_CEntity__RenderEffects = 0x5342DA;
static constexpr std::uintptr_t SKIP_CEntity__RenderEffects = 0x534301;
static void _declspec(naked) HOOK_CEntity__RenderEffects()
{
    _asm
    {
        mov eax, 0x4C4C70
        call eax

        test eax, eax
        jz skip

        jmp RETURN_CEntity__RenderEffects

        skip:
        jmp SKIP_CEntity__RenderEffects
    }
}

////////////////////////////////////////////////////////////////////////
// CEntity::ProcessLightsForEntity
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CEntity__ProcessLightsForEntity 0x6FC827
#define HOOKSIZE_CEntity__ProcessLightsForEntity 6
static constexpr std::uintptr_t RETURN_CEntity__ProcessLightsForEntity = 0x6FC82D;
static constexpr std::uintptr_t SKIP_CEntity__ProcessLightsForEntity = 0x6FD488;
static void _declspec(naked) HOOK_CEntity__ProcessLightsForEntity()
{
    _asm
    {
        test eax, eax
        jz skip

        mov esi, eax
        xor eax, eax
        mov ecx, edi
        jmp RETURN_CEntity__ProcessLightsForEntity

        skip:
        jmp SKIP_CEntity__ProcessLightsForEntity
    }
}

////////////////////////////////////////////////////////////////////////
// InteriorManager_c::AddSameGroupEffectInfos
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_InteriorManager_c__AddSameGroupEffectInfos 0x598458
#define HOOKSIZE_InteriorManager_c__AddSameGroupEffectInfos 5
static constexpr std::uintptr_t RETURN_InteriorManager_c__AddSameGroupEffectInfos = 0x59845D;
static constexpr std::uintptr_t SKIP_InteriorManager_c__AddSameGroupEffectInfos = 0x598484;
static void _declspec(naked) HOOK_InteriorManager_c__AddSameGroupEffectInfos()
{
    _asm
    {
        mov eax, 0x4C4C70
        call eax

        test eax, eax
        jz skip

        jmp RETURN_InteriorManager_c__AddSameGroupEffectInfos

        skip:
        jmp SKIP_InteriorManager_c__AddSameGroupEffectInfos
    }
}

////////////////////////////////////////////////////////////////////////
// InteriorManager_c::GetVisibleEffects
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_InteriorManager_c__GetVisibleEffects 0x598E5A
#define HOOKSIZE_InteriorManager_c__GetVisibleEffects 6
static constexpr std::uintptr_t RETURN_InteriorManager_c__GetVisibleEffects = 0x598E60;
static constexpr std::uintptr_t SKIP_InteriorManager_c__GetVisibleEffects = 0x598EDD;
static void _declspec(naked) HOOK_InteriorManager_c__GetVisibleEffects()
{
    _asm
    {
        test eax, eax
        jz skip

        mov edi, eax
        cmp byte ptr [edi+0Ch], 5
        jmp RETURN_InteriorManager_c__GetVisibleEffects

        skip:
        jmp SKIP_InteriorManager_c__GetVisibleEffects
    }
}

////////////////////////////////////////////////////////////////////////
// CAttractorScanner::GetClosestAttractorOfType
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CAttractorScanner__GetClosestAttractorOfType 0x6003B8
#define HOOKSIZE_CAttractorScanner__GetClosestAttractorOfType 9
static constexpr std::uintptr_t RETURN_CAttractorScanner__GetClosestAttractorOfType = 0x6003C1;
static constexpr std::uintptr_t SKIP_CAttractorScanner__GetClosestAttractorOfType = 0x600491;
static void _declspec(naked) HOOK_CAttractorScanner__GetClosestAttractorOfType()
{
    _asm
    {
        test eax, eax
        jz skip

        mov esi, eax
        mov eax, [esp+102Ch+1Ch]
        jmp RETURN_CAttractorScanner__GetClosestAttractorOfType

        skip:
        jmp SKIP_CAttractorScanner__GetClosestAttractorOfType
    }
}

////////////////////////////////////////////////////////////////////////
// CAttractorScanner::ScanForAttractorsInPtrList
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CAttractorScanner__ScanForAttractorsInPtrList 0x603523
#define HOOKSIZE_CAttractorScanner__ScanForAttractorsInPtrList 5
static constexpr std::uintptr_t RETURN_CAttractorScanner__ScanForAttractorsInPtrList = 0x603528;
static constexpr std::uintptr_t SKIP_CAttractorScanner__ScanForAttractorsInPtrList = 0x603548;
static void _declspec(naked) HOOK_CAttractorScanner__ScanForAttractorsInPtrList()
{
    _asm
    {
        mov eax, 0x4C4C70
        call eax

        test eax, eax
        jz skip

        jmp RETURN_CAttractorScanner__ScanForAttractorsInPtrList

        skip:
        jmp SKIP_CAttractorScanner__ScanForAttractorsInPtrList
    }
}

////////////////////////////////////////////////////////////////////////
// CCover::FindCoverPointsForThisBuilding
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CCover__FindCoverPointsForThisBuilding 0x699150
#define HOOKSIZE_CCover__FindCoverPointsForThisBuilding 6
static constexpr std::uintptr_t RETURN_CCover__FindCoverPointsForThisBuilding = 0x699156;
static constexpr std::uintptr_t SKIP_CCover__FindCoverPointsForThisBuilding = 0x699215;
static void _declspec(naked) HOOK_CCover__FindCoverPointsForThisBuilding()
{
    _asm
    {
        test eax, eax
        jz skip

        mov edi, eax
        cmp byte ptr [edi+0Ch], 9
        jmp RETURN_CCover__FindCoverPointsForThisBuilding

        skip:
        jmp SKIP_CCover__FindCoverPointsForThisBuilding
    }
}

////////////////////////////////////////////////////////////////////////
// CEntity::CreateEffects
//
// Call the function after loading the effects to deactivate those removed by the script.
// We have to do it this way because every time an object streams in,
// the effects are read again from the .dff file.
////////////////////////////////////////////////////////////////////////
static void OnEntityEffectsLoaded(CEntitySAInterface* entity)
{
    CModelInfo* modelInfo = pGameInterface->GetModelInfo(entity->m_nModelIndex);
    if (modelInfo)
        modelInfo->On2DFXEffectsLoaded(entity);
}

#define HOOKPOS_CEntity_Post_CreateEffects 0x533BAE
#define HOOKSIZE_CEntity_Post_CreateEffects 10
static void _declspec(naked) HOOK_CEntity_Post_CreateEffects()
{
    _asm
    {
        push ebp
        call OnEntityEffectsLoaded
        add esp, 4

        pop edi
        pop ebp
        pop ebx
        add esp, 0C0h
        retn
    }
}

////////////////////////////////////////////////////////////////////////
// CCustomRoadsignMgr::RoadsignGetLineAndRow
//
// Some characters and symbols in roadsignfont.png are unavailable,
// and we want them to be accessible
////////////////////////////////////////////////////////////////////////
static void RoadsignGetLineAndRow(int* line, int* row)
{
    char letter = 0;
    _asm
    {
        mov letter, al
    }

    switch (letter)
    {
        case '!':
            *line = 0;
            *row = 0;
            break;
        case '#':
            *line = 3;
            *row = 21;
            break;
        case '$':
            *line = 0;
            *row = 22;
            break;
        case '%':
            *line = 2;
            *row = 21;
            break;
        case '&':
            *line = 2;
            *row = 0;
            break;
        case '(':
            *line = 0;
            *row = 1;
            break;
        case ')':
            *line = 1;
            *row = 1;
            break;
        case '*':
            *line = 1;
            *row = 22;
            break;
        case '+':
            *line = 2;
            *row = 1;
            break;
        case '-':
            *line = 0;
            *row = 2;
            break;
        case '.':
            *line = 1;
            *row = 2;
            break;
        case '0':
            *line = 3;
            *row = 2;
            break;
        case '1':
            *line = 0;
            *row = 3;
            break;
        case '2':
            *line = 1;
            *row = 3;
            break;
        case '3':
            *line = 2;
            *row = 3;
            break;
        case '4':
            *line = 3;
            *row = 3;
            break;
        case '5':
            *line = 0;
            *row = 4;
            break;
        case '6':
            *line = 1;
            *row = 4;
            break;
        case '7':
            *line = 2;
            *row = 4;
            break;
        case '8':
            *line = 3;
            *row = 4;
            break;
        case '9':
            *line = 0;
            *row = 5;
            break;
        case ':':
            *line = 2;
            *row = 5;
            break;
        case ';':
            *line = 1;
            *row = 5;
            break;
        case '<':
            *line = 2;
            *row = 20;
            break;
        case '>':
            *line = 3;
            *row = 20;
            break;
        case '?':
            *line = 3;
            *row = 5;
            break;
        case '@':
            *line = 2;
            *row = 22;
            break;
        case 'A':
            *line = 0;
            *row = 6;
            break;
        case 'B':
            *line = 1;
            *row = 6;
            break;
        case 'C':
            *line = 2;
            *row = 6;
            break;
        case 'D':
            *line = 3;
            *row = 6;
            break;
        case 'E':
            *line = 0;
            *row = 7;
            break;
        case 'F':
            *line = 1;
            *row = 7;
            break;
        case 'G':
            *line = 2;
            *row = 7;
            break;
        case 'H':
            *line = 3;
            *row = 7;
            break;
        case 'I':
            *line = 0;
            *row = 8;
            break;
        case 'J':
            *line = 1;
            *row = 8;
            break;
        case 'K':
            *line = 2;
            *row = 8;
            break;
        case 'L':
            *line = 3;
            *row = 8;
            break;
        case 'M':
            *line = 0;
            *row = 9;
            break;
        case 'N':
            *line = 1;
            *row = 9;
            break;
        case 'O':
            *line = 2;
            *row = 9;
            break;
        case 'P':
            *line = 3;
            *row = 9;
            break;
        case 'Q':
            *line = 0;
            *row = 10;
            break;
        case 'R':
            *line = 1;
            *row = 10;
            break;
        case 'S':
            *line = 2;
            *row = 10;
            break;
        case 'T':
            *line = 3;
            *row = 10;
            break;
        case 'U':
            *line = 0;
            *row = 11;
            break;
        case 'V':
            *line = 1;
            *row = 11;
            break;
        case 'W':
            *line = 2;
            *row = 11;
            break;
        case 'X':
            *line = 3;
            *row = 11;
            break;
        case 'Y':
            *line = 0;
            *row = 12;
            break;
        case 'Z':
            *line = 1;
            *row = 12;
            break;
        case '[':
            *line = 2;
            *row = 12;
            break;
        case ']':
            *line = 0;
            *row = 13;
            break;
        case '^':
            *line = 0;
            *row = 21;
            break;
        case 'a':
            *line = 1;
            *row = 13;
            break;
        case 'b':
            *line = 2;
            *row = 13;
            break;
        case 'c':
            *line = 3;
            *row = 13;
            break;
        case 'd':
            *line = 0;
            *row = 14;
            break;
        case 'e':
            *line = 1;
            *row = 14;
            break;
        case 'f':
            *line = 2;
            *row = 14;
            break;
        case 'g':
            *line = 3;
            *row = 14;
            break;
        case 'h':
            *line = 0;
            *row = 15;
            break;
        case 'i':
            *line = 1;
            *row = 15;
            break;
        case 'j':
            *line = 2;
            *row = 15;
            break;
        case 'k':
            *line = 3;
            *row = 15;
            break;
        case 'l':
            *line = 0;
            *row = 16;
            break;
        case 'm':
            *line = 1;
            *row = 16;
            break;
        case 'n':
            *line = 2;
            *row = 16;
            break;
        case 'o':
            *line = 3;
            *row = 16;
            break;
        case 'p':
            *line = 0;
            *row = 17;
            break;
        case 'q':
            *line = 1;
            *row = 17;
            break;
        case 'r':
            *line = 2;
            *row = 17;
            break;
        case 's':
            *line = 3;
            *row = 17;
            break;
        case 't':
            *line = 0;
            *row = 18;
            break;
        case 'u':
            *line = 1;
            *row = 18;
            break;
        case 'v':
            *line = 2;
            *row = 18;
            break;
        case 'w':
            *line = 3;
            *row = 18;
            break;
        case 'x':
            *line = 0;
            *row = 19;
            break;
        case 'y':
            *line = 1;
            *row = 19;
            break;
        case 'z':
            *line = 2;
            *row = 19;
            break;
        case '{':
            *line = 0;
            *row = 23;
            break;
        case '|':
            *line = 3;
            *row = 22;
            break;
        case '}':
            *line = 2;
            *row = 23;
            break;
        case '~':
            *line = 1;
            *row = 21;
            break;
        // missing characters & symbols
        case '/':
            *line = 2;
            *row = 2;
            break;
        case ',':
            *line = 3;
            *row = 1;
            break;
        case '\\': // | character
            *line = 0;
            *row = 20;
            break;
        case '\x04': // scull ♦
            *line = 3;
            *row = 23;
            break;
        case '\xB9': // invalid ╣
            *line = 0;
            *row = 24;
            break;
        case '\xB6': // parking icon icon ¶
            *line = 1;
            *row = 24;
            break;
        case '\xA7': // winding road or something §
            *line = 2;
            *row = 24;
            break;
        case '\x07': // info icon •
            *line = 3;
            *row = 24;
            break;
        case '\x01': // alien icon ☺
            *line = 0;
            *row = 25;
            break;
        case '\x02': // human icon ☻
            *line = 1;
            *row = 25;
            break;
        case '\xDB': // train icon █
            *line = 2;
            *row = 25;
            break;
        case '\x1E': // hill icon or something ▲
            *line = 3;
            *row = 25;
            break;
        /////////////////////////////
        default:
            *line = 0;
            *row = 26;
            break;
    }
}

void CMultiplayerSA::InitHooks_2DFX()
{
    EZHookInstall(CTrafficLights__DisplayActualLight);
    EZHookInstall(CEntity__HasPreRenderEffects);
    EZHookInstall(CEntity__FindTriggerPointCoors);
    EZHookInstall(CEntity__GetRandom2dEffect);
    EZHookInstall(CEntity__CreateEffects);
    EZHookInstall(CEntity__DestroyEffects);
    EZHookInstall(CEntity__RenderEffects);
    EZHookInstall(CEntity__ProcessLightsForEntity);
    EZHookInstall(InteriorManager_c__AddSameGroupEffectInfos);
    EZHookInstall(InteriorManager_c__GetVisibleEffects);
    EZHookInstall(CAttractorScanner__GetClosestAttractorOfType);
    EZHookInstall(CAttractorScanner__ScanForAttractorsInPtrList);
    EZHookInstall(CCover__FindCoverPointsForThisBuilding);

    EZHookInstall(CEntity_Post_CreateEffects);

    // CCustomRoadsignMgr::RoadsignGetLineAndRow
    HookInstallCall(0x6FEBF1, (DWORD)&RoadsignGetLineAndRow);

    // Allow set roadsign text alpha
    MemSet((void*)0x6FF3FF, 0x90, 5);
}
