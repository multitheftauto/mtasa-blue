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
#include <game/CAEAudioHardware.h>

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
// Resolves the given device name to a DirectSound GUID, to be substituted by the hook above,
// then restarts the audio hardware right away so it takes effect immediately
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

extern CGame* pGameInterface;

void CMultiplayerSA::RestartAudioHardware()
{
    // Same teardown/rebuild the game already does once at normal startup, just run again on
    // demand; causes the same brief hitch, but never needs a full game restart to take effect
    CAEAudioHardware* pAEAudioHardware = pGameInterface->GetAEAudioHardware();
    if (!pAEAudioHardware)
        return;

    pAEAudioHardware->Terminate();
    if (!pAEAudioHardware->Initialise())
        OutputDebugLine("[Audio] Native audio hardware failed to (re)initialise; no SFX/radio/vehicle sounds until it recovers");
}

void CMultiplayerSA::SetPreferredAudioDeviceName(const std::string& strName)
{
    if (strName.empty())
        g_bHasPreferredAudioDeviceGuid = false;
    else
    {
        SFindDeviceContext findContext{&strName, false, {}};
        DirectSoundEnumerateA(FindDeviceByNameCallback, &findContext);

        g_bHasPreferredAudioDeviceGuid = findContext.bFound;
        if (findContext.bFound)
            g_PreferredAudioDeviceGuid = findContext.guid;
    }

    RestartAudioHardware();
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
