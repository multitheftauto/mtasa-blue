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
// "push ebx ; call [import]" pair, where ebx holds the null default GUID, with a push of the
// chosen GUID before the same import call.
//
//////////////////////////////////////////////////////////////////////////////////////////
static GUID g_PreferredAudioDeviceGuid;
static bool g_bHasPreferredAudioDeviceGuid = false;

// Must stay a #define: a constexpr symbol here would make the asm below call the variable's own
// storage as code instead of going through the import slot
#define FUNC_EAXDirectSoundCreate8 0x85801C

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
        call dword ptr ds:[FUNC_EAXDirectSoundCreate8]
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
        bool               bFound;
        GUID               guid;
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
}  // namespace

extern CGame*          pGameInterface;
extern CCoreInterface* g_pCore;

// Terminate() already stops and joins both threads (0x4D97A0 calls both Stop and both
// WaitForExit), so there is no race to fix here. It just never closes their handles, since the
// game normally only runs this once at process exit; closing them ourselves stops a repeated
// switch from leaking a handle pair every time
static constexpr std::uintptr_t VAR_AudioStreamThreadHandle = 0xB606CC;
static constexpr std::uintptr_t VAR_AudioSmoothFadeThreadHandle = 0xB608D0;

static void CloseLeakedAudioThreadHandle(std::uintptr_t slotAddress)
{
    HANDLE& handle = *reinterpret_cast<HANDLE*>(slotAddress);
    if (handle)
    {
        CloseHandle(handle);
        handle = nullptr;
    }
}

// Terminate() deletes the CAEMP3BankLoader (m_pMP3BankLoader, this+0xD98) without closing the
// CdStream handles it opened for every AUDIO\SFX pak; Initialise() then reopens them all in a
// fresh loader. That never mattered for a one-shot Terminate at process exit, but on repeated
// switches it fills the shared 32-slot gStreamFileHandles table, which PAKFILES.DAT is already
// close to using up on its own; once full, sound banks fail to load silently, with no crash
// (vehicle radio is unaffected, it reads through plain fopen()). Each pak's CdStreamHandle is
// (index << 24) into that same table (m_StreamHandles at +0x20, m_PakLkupCount at +0x10), so this
// closes exactly those handles
static constexpr std::uintptr_t OBJ_AEAudioHardware = 0xB5F8B8;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_MP3BankLoader = 0xD98;
static constexpr std::uintptr_t OFFSET_CAEBankLoader_PakLkupCount = 0x10;
static constexpr std::uintptr_t OFFSET_CAEBankLoader_StreamHandles = 0x20;
static constexpr std::uintptr_t VAR_GStreamFileHandles = 0x8E4010;
static constexpr std::uint32_t  CD_STREAM_HANDLE_BITS = 24;
static constexpr std::uint32_t  MAX_CD_STREAM_HANDLES = 32;

static void ReleaseLeakedSfxPakStreamHandles()
{
    const std::uintptr_t pBankLoader = *reinterpret_cast<std::uintptr_t*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_MP3BankLoader);
    if (!pBankLoader)
        return;

    const std::uint16_t usPakCount = *reinterpret_cast<std::uint16_t*>(pBankLoader + OFFSET_CAEBankLoader_PakLkupCount);
    std::int32_t* const pStreamHandles = *reinterpret_cast<std::int32_t**>(pBankLoader + OFFSET_CAEBankLoader_StreamHandles);
    if (!pStreamHandles)
        return;

    HANDLE* const pGlobalStreamHandles = reinterpret_cast<HANDLE*>(VAR_GStreamFileHandles);
    for (std::uint16_t i = 0; i < usPakCount; i++)
    {
        // CdStreamOpen returns 0 both on a failed open and on a legitimate slot-0 open; slot 0 in
        // practice always belongs to the game's own boot-time streaming archives, opened before
        // any SFX pak, so skipping 0 here is the safe reading either way
        if (pStreamHandles[i] == 0)
            continue;

        const std::uint32_t uiIndex = static_cast<std::uint32_t>(pStreamHandles[i]) >> CD_STREAM_HANDLE_BITS;
        if (uiIndex >= MAX_CD_STREAM_HANDLES)
            continue;

        HANDLE& hFile = pGlobalStreamHandles[uiIndex];
        if (hFile && hFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hFile);
            hFile = nullptr;
        }
    }
}

// Logs the hardware's real post-restart state; useful evidence if a future regression brings the
// silent-SFX symptom back
static constexpr std::uintptr_t OFFSET_AEAudioHardware_Initialised = 0x0;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_HardwareMixAvailable = 0x4;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_NumAvailableChannels = 0x8C;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_NumChannels = 0x8E;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_DSCapsFreeHw3DAllBuffers = 0xDE4;

static void LogAudioHardwareState()
{
    const bool          bInitialised = *reinterpret_cast<bool*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_Initialised);
    const bool          bHwMixAvailable = *reinterpret_cast<bool*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_HardwareMixAvailable);
    const std::uint16_t usNumAvailableChannels = *reinterpret_cast<std::uint16_t*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_NumAvailableChannels);
    const std::uint16_t usNumChannels = *reinterpret_cast<std::uint16_t*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_NumChannels);
    const std::uint32_t uiFreeHw3DAllBuffers = *reinterpret_cast<std::uint32_t*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_DSCapsFreeHw3DAllBuffers);

    OutputDebugLine(SString("[Audio] post-Initialise state: initialised=%d hwMix=%d numChannels=%d numAvailable=%d freeHw3DBuffers=%u", bInitialised,
                            bHwMixAvailable, usNumChannels, usNumAvailableChannels, uiFreeHw3DAllBuffers));
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Device-only restart
//
// Three investigation passes never pinned down what accumulates across repeated
// Terminate()/Initialise() cycles to silence SFX after a couple of switches (the hardware's own
// post-Initialise state came back identical every time, ruling out everything inside it). Rather
// than keep chasing that, this sidesteps the problem: switching output device never needs to
// touch the SFX pak/bank loader, the MP3 track loader, or the radio track manager, none of which
// read or write the output device, even though Terminate()/Initialise() rebuild all of it anyway.
// This rebuilds only what is genuinely device-dependent instead: the DirectSound device, the 3D
// listener, every channel, and the two threads that service them, calling the same native
// functions Initialise() itself calls for that part, verified address for address against
// 0x4D9930/0x4D97A0. Everything left untouched keeps whatever valid state it already had.
//
//////////////////////////////////////////////////////////////////////////////////////////
static constexpr std::uintptr_t OFFSET_AEAudioHardware_DSDevice = 0xDA0;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_DSCaps = 0xDAC;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_SpeakerConfig = 0xDA4;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_DirectSound3DListener = 0xE0C;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_StreamingChannel = 0xE10;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_StreamThread = 0xE14;
static constexpr std::uintptr_t OFFSET_AEAudioHardware_Channels = 0xE64;     // CAEAudioChannel*[64]
static constexpr std::uintptr_t OFFSET_AEAudioHardware_ChannelFlags0 = 0x6;  // int16, only slot 0 (the streaming channel) is set here
static constexpr std::uint32_t  MAX_NUM_AUDIO_CHANNELS = 64;

// Same double dereference the native SetCooperativeLevel call itself uses (PSGLOBAL(window) in
// the reversed source); verified against the raw disassembly rather than an MTA-level window accessor
static constexpr std::uintptr_t VAR_GameWindowHandlePtr = 0xC17054;

static constexpr std::uintptr_t FUNC_OperatorNew = 0x82119A;
static constexpr std::uintptr_t FUNC_InitDirectSoundListener = 0x4D9640;
static constexpr std::uintptr_t FUNC_CAEStreamingChannel_Constructor = 0x4F1800;
static constexpr std::uintptr_t FUNC_CAEStreamingChannel_Initialise = 0x4F22F0;
static constexpr std::uintptr_t FUNC_CAEStaticChannel_Constructor = 0x4F0B10;
static constexpr std::uintptr_t FUNC_CAEAudioChannel_SetVolume = 0x4D7C60;
static constexpr std::uintptr_t FUNC_CAEStreamThread_Stop = 0x4F1590;
static constexpr std::uintptr_t FUNC_CAEStreamThread_WaitForExit = 0x4F1220;
static constexpr std::uintptr_t FUNC_CAEStreamThread_Initialise = 0x4F1680;
static constexpr std::uintptr_t FUNC_CAEStreamThread_Start = 0x4F11F0;
static constexpr std::uintptr_t FUNC_CAESmoothFadeThread_Stop = 0x4EEA30;
static constexpr std::uintptr_t FUNC_CAESmoothFadeThread_WaitForExit = 0x4EEA40;
static constexpr std::uintptr_t FUNC_CAESmoothFadeThread_Initialise = 0x4EEEC0;
static constexpr std::uintptr_t FUNC_CAESmoothFadeThread_Start = 0x4EEA10;

static void PartialTerminateAudioDevice()
{
    reinterpret_cast<void(__thiscall*)(void*)>(FUNC_CAEStreamThread_Stop)(reinterpret_cast<void*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_StreamThread));
    reinterpret_cast<void(__thiscall*)(void*)>(FUNC_CAESmoothFadeThread_Stop)(reinterpret_cast<void*>(VAR_AudioSmoothFadeThreadHandle));
    reinterpret_cast<void(__thiscall*)(void*)>(FUNC_CAEStreamThread_WaitForExit)(
        reinterpret_cast<void*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_StreamThread));
    reinterpret_cast<void(__thiscall*)(void*)>(FUNC_CAESmoothFadeThread_WaitForExit)(reinterpret_cast<void*>(VAR_AudioSmoothFadeThreadHandle));

    // Each channel deleted through its own vtable's scalar deleting destructor (arg 1 also frees
    // the memory), the same call the real Terminate() makes for this array
    void** const ppChannels = reinterpret_cast<void**>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_Channels);
    for (std::uint32_t i = 0; i < MAX_NUM_AUDIO_CHANNELS; i++)
    {
        if (void* pChannel = ppChannels[i])
        {
            void** const pVtable = *reinterpret_cast<void***>(pChannel);
            reinterpret_cast<void(__thiscall*)(void*, int)>(pVtable[0])(pChannel, 1);
            ppChannels[i] = nullptr;
        }
    }
    *reinterpret_cast<void**>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_StreamingChannel) = nullptr;

    IDirectSound3DListener*& pListener = *reinterpret_cast<IDirectSound3DListener**>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_DirectSound3DListener);
    if (pListener)
    {
        pListener->Release();
        pListener = nullptr;
    }

    IDirectSound8*& pDevice = *reinterpret_cast<IDirectSound8**>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_DSDevice);
    if (pDevice)
    {
        pDevice->Release();
        pDevice = nullptr;
    }

    *reinterpret_cast<bool*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_Initialised) = false;
}

static bool PartialInitialiseAudioDevice()
{
    IDirectSound8*& pDevice = *reinterpret_cast<IDirectSound8**>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_DSDevice);

    CoInitialize(nullptr);

    typedef HRESULT(WINAPI * EAXDirectSoundCreate8Func)(const GUID*, IDirectSound8**, IUnknown*);
    const auto  pfnCreate = *reinterpret_cast<EAXDirectSoundCreate8Func*>(FUNC_EAXDirectSoundCreate8);
    const GUID* pGuid = g_bHasPreferredAudioDeviceGuid ? &g_PreferredAudioDeviceGuid : nullptr;
    if (FAILED(pfnCreate(pGuid, &pDevice, nullptr)))
        return false;

    DSCAPS& dsCaps = *reinterpret_cast<DSCAPS*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_DSCaps);
    dsCaps.dwSize = sizeof(DSCAPS);
    pDevice->GetCaps(&dsCaps);

    const HWND hWindow = *reinterpret_cast<HWND*>(*reinterpret_cast<std::uintptr_t*>(VAR_GameWindowHandlePtr));
    if (FAILED(pDevice->SetCooperativeLevel(hWindow, DSSCL_PRIORITY)))
        return false;

    const bool bListenerOk = reinterpret_cast<bool(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t)>(FUNC_InitDirectSoundListener)(
        reinterpret_cast<void*>(OBJ_AEAudioHardware), 2, 48000, 16);
    if (!bListenerOk)
        return false;

    DWORD& dwSpeakerConfig = *reinterpret_cast<DWORD*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_SpeakerConfig);
    pDevice->GetSpeakerConfig(&dwSpeakerConfig);

    void*&      pStreamingChannelSlot = *reinterpret_cast<void**>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_StreamingChannel);
    void* const pStreamingChannel = reinterpret_cast<void*(__cdecl*)(std::size_t)>(FUNC_OperatorNew)(0x60098);
    if (!pStreamingChannel)
        return false;
    reinterpret_cast<void(__thiscall*)(void*, IDirectSound8*, std::uint16_t)>(FUNC_CAEStreamingChannel_Constructor)(pStreamingChannel, pDevice, 0);
    pStreamingChannelSlot = pStreamingChannel;
    reinterpret_cast<void(__thiscall*)(void*)>(FUNC_CAEStreamingChannel_Initialise)(pStreamingChannel);

    const std::uint32_t uiFreeHw3DAllBuffers = *reinterpret_cast<std::uint32_t*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_DSCapsFreeHw3DAllBuffers);
    std::uint16_t&      usNumChannels = *reinterpret_cast<std::uint16_t*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_NumChannels);
    bool&               bHwMixAvailable = *reinterpret_cast<bool*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_HardwareMixAvailable);
    if (uiFreeHw3DAllBuffers < 0x18)
    {
        usNumChannels = 0x30;
        bHwMixAvailable = false;
    }
    else
    {
        usNumChannels = static_cast<std::uint16_t>(std::min<std::uint32_t>(uiFreeHw3DAllBuffers, 0x40) - 7);
        bHwMixAvailable = true;
    }

    void** const ppChannels = reinterpret_cast<void**>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_Channels);
    for (std::uint16_t i = 1; i < usNumChannels; i++)
    {
        void* const pChannel = reinterpret_cast<void*(__cdecl*)(std::size_t)>(FUNC_OperatorNew)(0x90);
        if (pChannel)
            reinterpret_cast<void(__thiscall*)(void*, IDirectSound8*, std::uint16_t, bool, std::uint32_t, std::uint16_t)>(FUNC_CAEStaticChannel_Constructor)(
                pChannel, pDevice, i, bHwMixAvailable, 44100, 16);
        ppChannels[i] = pChannel;
    }
    ppChannels[0] = pStreamingChannel;

    reinterpret_cast<void(__thiscall*)(void*, float)>(FUNC_CAEAudioChannel_SetVolume)(pStreamingChannel, -100.0f);
    *reinterpret_cast<std::int16_t*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_ChannelFlags0) = 0x37;

    reinterpret_cast<void(__thiscall*)(void*)>(FUNC_CAESmoothFadeThread_Initialise)(reinterpret_cast<void*>(VAR_AudioSmoothFadeThreadHandle));
    reinterpret_cast<void(__thiscall*)(void*)>(FUNC_CAESmoothFadeThread_Start)(reinterpret_cast<void*>(VAR_AudioSmoothFadeThreadHandle));

    void* const pStreamThread = reinterpret_cast<void*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_StreamThread);
    reinterpret_cast<void(__thiscall*)(void*, void*)>(FUNC_CAEStreamThread_Initialise)(pStreamThread, pStreamingChannel);
    reinterpret_cast<void(__thiscall*)(void*)>(FUNC_CAEStreamThread_Start)(pStreamThread);

    *reinterpret_cast<std::uint16_t*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_NumAvailableChannels) = usNumChannels;
    *reinterpret_cast<bool*>(OBJ_AEAudioHardware + OFFSET_AEAudioHardware_Initialised) = true;
    return true;
}

void CMultiplayerSA::RestartAudioHardware()
{
    CAEAudioHardware* pAEAudioHardware = pGameInterface->GetAEAudioHardware();
    if (!pAEAudioHardware)
        return;

    // Try the device-only path first: lighter, faster, and it never touches the SFX pak/bank
    // loader that three investigation passes never found the fault in. Two attempts, since the
    // driver can briefly report the old device as still allocated while it finishes releasing it
    for (int iAttempt = 0; iAttempt < 2; iAttempt++)
    {
        PartialTerminateAudioDevice();
        CloseLeakedAudioThreadHandle(VAR_AudioStreamThreadHandle);
        CloseLeakedAudioThreadHandle(VAR_AudioSmoothFadeThreadHandle);

        if (PartialInitialiseAudioDevice())
        {
            LogAudioHardwareState();
            return;
        }

        Sleep(50);
    }

    OutputDebugLine("[Audio] Device-only restart failed twice; falling back to a full native audio reinitialise");

    // Same teardown/rebuild the game already does once at normal startup, just run again on
    // demand; causes the same brief hitch, but never needs a full game restart to take effect
    for (int iAttempt = 0; iAttempt < 4; iAttempt++)
    {
        // Must run before Terminate deletes the bank loader. Safe to call again on a retry too:
        // Initialise() unconditionally allocates and repopulates a fresh loader, reopening every
        // pak's CD stream, before any of its own failure paths can return
        ReleaseLeakedSfxPakStreamHandles();

        pAEAudioHardware->Terminate();
        CloseLeakedAudioThreadHandle(VAR_AudioStreamThreadHandle);
        CloseLeakedAudioThreadHandle(VAR_AudioSmoothFadeThreadHandle);

        if (pAEAudioHardware->Initialise())
        {
            LogAudioHardwareState();
            return;
        }

        Sleep(50);
    }

    // The last attempt's Initialise() still reopened every pak's CD stream before failing; the
    // loop exits here instead of looping back, so nothing above catches that one
    ReleaseLeakedSfxPakStreamHandles();

    OutputDebugLine("[Audio] Native audio hardware failed to reinitialise; no SFX/radio/vehicle sounds until the next device switch");
}

void CMultiplayerSA::SetPreferredAudioDeviceName(const std::string& strName)
{
    const bool bHadGuid = g_bHasPreferredAudioDeviceGuid;
    const GUID oldGuid = g_PreferredAudioDeviceGuid;

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

    // Already open on this device; restarting again is wasteful at best and can deadlock if this
    // lands mid join while the game is streaming audio banks
    if (bHadGuid == g_bHasPreferredAudioDeviceGuid && (!bHadGuid || IsEqualGUID(oldGuid, g_PreferredAudioDeviceGuid)))
        return;

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

    // Resolve the device remembered in the settings before the game opens its audio for the
    // first time, so even the menu starts on it directly with no restart
    std::string strPreferredDevice;
    g_pCore->GetCVars()->Get("audio_output_device", strPreferredDevice);
    if (!strPreferredDevice.empty())
    {
        SFindDeviceContext findContext{&strPreferredDevice, false, {}};
        DirectSoundEnumerateA(FindDeviceByNameCallback, &findContext);
        if (findContext.bFound)
        {
            g_PreferredAudioDeviceGuid = findContext.guid;
            g_bHasPreferredAudioDeviceGuid = true;
        }
    }
}
