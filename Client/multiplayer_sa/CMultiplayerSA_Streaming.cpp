/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Streaming.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void OnModelLoaded(unsigned int uiModelID)
{
    const int32_t baseTxdId = pGameInterface->GetBaseIDforTXD();
    if (baseTxdId <= 0)
        return;

    if (uiModelID < static_cast<unsigned int>(baseTxdId))
    {
        if (auto* pModelInfo = pGameInterface->GetModelInfo(uiModelID))
            pModelInfo->MakeCustomModel();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CStreaming::ConvertBufferToObject
//
// This hook load custom models
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CStreaming__ConvertBufferToObject  0x40CB88
#define HOOKSIZE_CStreaming__ConvertBufferToObject 9

static void _declspec(naked) HOOK_CStreaming__ConvertBufferToObject()
{
    // clang-format off
    __asm
    {
        push    esi
        call    OnModelLoaded
        pop esi

        pop     edi
        pop     esi
        pop     ebp
        mov     al, 1
        pop     ebx
        add     esp, 20h
        retn
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CStreaming::RetryLoadFile - spin loop timeout
//
// GTA:SA's RetryLoadFile (0x4076B7) has an unbounded while(1) spin loop that
// polls CdStreamGetStatus with no sleep or timeout. If the streaming channel
// stays in an error state (ms_channelError != -1), the loop freezes the game.
//
// This hook intercepts the loop-back decision at 0x40776B (the cmp+jnz that
// checks ms_channelError and jumps back to the loop head) and enforces a
// timeout. On timeout, ms_channelError is forced to -1 so the function exits
// through its normal "error cleared" path.
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CStreaming__RetryLoadFileTimeout  0x40776B
#define HOOKSIZE_CStreaming__RetryLoadFileTimeout 9                        // cmp (7 bytes) + jnz (2 bytes)
static DWORD RETURN_CStreaming__RetryLoadFileTimeout_Exit = 0x407774;      // pop edi; pop esi; jmp CLoadingScreen::Continue
static DWORD RETURN_CStreaming__RetryLoadFileTimeout_LoopBack = 0x4076F4;  // Loop head (mov eax, ms_channel[esi].LoadStatus)

static DWORD s_retryLoopStartTick = 0;
static DWORD s_retryLoopLastCallTick = 0;
static DWORD s_retryTimeoutCount = 0;
static DWORD s_lastTimeoutTick = 0;

static bool ShouldTimeoutRetryLoop()
{
    constexpr DWORD FIRST_TIMEOUT_MS = 5000;
    constexpr DWORD COOLDOWN_MS = 30000;
    DWORD           now = SharedUtil::GetTickCount32();

    // After a quiet period with no timeouts, treat the next error as fresh
    if (s_lastTimeoutTick != 0 && (now - s_lastTimeoutTick) > COOLDOWN_MS)
    {
        s_retryTimeoutCount = 0;
        s_lastTimeoutTick = 0;
    }

    // Detect new invocation: within the spin loop, consecutive calls are
    // microseconds apart. A gap over 100ms means this is a fresh RetryLoadFile
    // call, so reset the timer. This also handles the case where a previous
    // invocation exited via loc_4077A5 without going through our hook.
    if (s_retryLoopStartTick == 0 || (now - s_retryLoopLastCallTick) > 100)
        s_retryLoopStartTick = now;

    s_retryLoopLastCallTick = now;

    // After the first timeout, outer callers (LoadAllRequestedModels and
    // CModelInfoSA::Request) can re-enter RetryLoadFile on the same
    // persistent I/O error. Exit immediately on re-entry to avoid
    // accumulating multi-minute freezes across nested retry loops.
    DWORD timeoutMs = (s_retryTimeoutCount > 0) ? 0 : FIRST_TIMEOUT_MS;

    DWORD elapsed = now - s_retryLoopStartTick;
    if (elapsed >= timeoutMs)
    {
        s_retryLoopStartTick = 0;
        s_retryTimeoutCount++;
        s_lastTimeoutTick = now;
        *(int*)0x8E4B90 = -1;  // CStreaming::ms_channelError = -1 (force clear)
        AddReportLog(8650, SString("RetryLoadFile spin loop timed out after %ums (count: %u)", elapsed, s_retryTimeoutCount));
        return true;
    }

    return false;
}

static void _declspec(naked) HOOK_CStreaming__RetryLoadFileTimeout()
{
    // clang-format off
    __asm
    {
        pushad
        call    ShouldTimeoutRetryLoop
        test    al, al
        jnz     timeout

        popad

        // Original code: cmp ms_channelError, -1; jnz loc_4076F4
        cmp     dword ptr ds:[0x8E4B90], 0FFFFFFFFh
        jnz     loopback

        jmp     RETURN_CStreaming__RetryLoadFileTimeout_Exit

    loopback:
        jmp     RETURN_CStreaming__RetryLoadFileTimeout_LoopBack

    timeout:
        popad
        jmp     RETURN_CStreaming__RetryLoadFileTimeout_Exit
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Streaming
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Streaming()
{
    EZHookInstall(CStreaming__ConvertBufferToObject);
    EZHookInstall(CStreaming__RetryLoadFileTimeout);
}
