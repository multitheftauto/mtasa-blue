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
    if (uiModelID < pGameInterface->GetBaseIDforTXD())
        pGameInterface->GetModelInfo(uiModelID)->MakeCustomModel();
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
    _asm {
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
}
