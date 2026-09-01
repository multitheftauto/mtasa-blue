/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Audio.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <dsound.h>

#pragma comment(lib, "dsound.lib")

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAEAudioHardware::Initialise
//
// The native audio engine always passes a null device GUID to EAXDirectSoundCreate8, so it
// only ever opens on whatever Windows considers the default device; substituting a chosen
// device's GUID here lets it open on a specific one instead, matching the BASS output
// picker (GH #1146). Confirmed via decompile that the call at 0x4D99F0 is the real
// EAXDirectSoundCreate8 (the earlier one at 0x4D99E1 is CoInitialize); the hook replaces the
// "push 0 ; call [import]" pair immediately before it with "push <guid> ; call [import]".
//
//////////////////////////////////////////////////////////////////////////////////////////
static GUID g_PreferredAudioDeviceGuid;
static bool g_bHasPreferredAudioDeviceGuid = false;

static constexpr std::uintptr_t FUNC_EAXDirectSoundCreate8 = 0x85801C;

#define HOOKPOS_CAEAudioHardware_CreateDirectSound  0x4D99EF
#define HOOKSIZE_CAEAudioHardware_CreateDirectSound 7
static constexpr std::uintptr_t CONTINUE_CAEAudioHardware_CreateDirectSound = 0x4D99F6;
static void __declspec(naked)   HOOK_CAEAudioHardware_CreateDirectSound()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp  g_bHasPreferredAudioDeviceGuid, 0
        jz   useDefault
        push offset g_PreferredAudioDeviceGuid
        jmp  doCall
        useDefault:
        push 0
        doCall:
        call dword ptr [FUNC_EAXDirectSoundCreate8]
        jmp  CONTINUE_CAEAudioHardware_CreateDirectSound
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetPreferredAudioDeviceName
//
// Resolves the given device name to a DirectSound GUID, to be substituted next time
// CAEAudioHardware::Initialise runs (game restart, not immediately - see the hook above)
//
//////////////////////////////////////////////////////////////////////////////////////////
namespace
{
    struct SFindDeviceContext
    {
        const std::string* pstrName;
        bool                bFound;
        GUID                guid;
    };

    BOOL CALLBACK FindDeviceByNameCallback(GUID* pGuid, LPCSTR strDescription, LPCSTR strModule, LPVOID pContext)
    {
        auto* pFindContext = static_cast<SFindDeviceContext*>(pContext);
        if (pGuid && strDescription && *pFindContext->pstrName == strDescription)
        {
            pFindContext->guid = *pGuid;
            pFindContext->bFound = true;
            return FALSE;
        }
        return TRUE;
    }
}            // namespace

void CMultiplayerSA::SetPreferredAudioDeviceName(const std::string& strName)
{
    if (strName.empty())
    {
        g_bHasPreferredAudioDeviceGuid = false;
        return;
    }

    SFindDeviceContext findContext{&strName, false, {}};
    DirectSoundEnumerateA(FindDeviceByNameCallback, &findContext);

    g_bHasPreferredAudioDeviceGuid = findContext.bFound;
    if (findContext.bFound)
        g_PreferredAudioDeviceGuid = findContext.guid;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Audio
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Audio()
{
    EZHookInstall(CAEAudioHardware_CreateDirectSound);
}
