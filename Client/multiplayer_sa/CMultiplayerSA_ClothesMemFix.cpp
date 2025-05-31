/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_ClothesMemFix.cpp
 *  PORPOISE:    Fix memory leak when clothes are changed
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define     FUNC_CPedModelInfo_DeleteRwObject               0x04C6C50
#define     FUNC_CPedModelInfo_SetClump                     0x04C7340

//////////////////////////////////////////////////////////////////////////////////////////
//
// Helper functions
//
//////////////////////////////////////////////////////////////////////////////////////////
void CPedModelInfo_DeleteRwObject(CBaseModelInfoSAInterface* pModelInfo)
{
    DWORD                      dwFunction = FUNC_CPedModelInfo_DeleteRwObject;
    CBaseModelInfoSAInterface* pInterface = pModelInfo;
    _asm
    {
        mov     ecx, pInterface
        call    dwFunction
    }
}

void CPedModelInfo_SetClump(CBaseModelInfoSAInterface* pModelInfo, RwObject* pSavedRwObject)
{
    DWORD                      dwFunction = FUNC_CPedModelInfo_SetClump;
    CBaseModelInfoSAInterface* pInterface = pModelInfo;
    _asm
    {
        push    pSavedRwObject
        mov     ecx, pInterface
        call    dwFunction
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Hook to stop the deletion of the pRwObject from model 0, and save it for later
//
//////////////////////////////////////////////////////////////////////////////////////////
RwObject* pSavedModel0RwObject = NULL;
void      OnMy_CClothesDeleteRwObject()
{
    assert(!pSavedModel0RwObject);
    ushort                     usModelID = 0;
    CBaseModelInfoSAInterface* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID];
    // Save RwObject of model 0
    pSavedModel0RwObject = pModelInfo->pRwObject;
}

// Hook info
#define HOOKPOS_CClothesDeleteRwObject                          0x5A8243
#define HOOKSIZE_CClothesDeleteRwObject                         5
DWORD RETURN_CClothesDeleteRwObject = 0x5A8248;
void _declspec(naked) HOOK_CClothesDeleteRwObject()
{
    _asm
    {
        pushad
        call    OnMy_CClothesDeleteRwObject
        popad

        // Do original code and continue
        mov     ecx, esi
        // Skip deletion
        //call    dword ptr [edx+20h] //; 004C6C50 ; void CPedModelInfo::DeleteRwObject()
        jmp     RETURN_CClothesDeleteRwObject
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Hook to restore pRwObject of model 0
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMy_PostCPedDress()
{
    ushort                     usModelID = 0;
    CBaseModelInfoSAInterface* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID];
    if (pSavedModel0RwObject)
    {
        CPedModelInfo_DeleteRwObject(pModelInfo);
        CPedModelInfo_SetClump(pModelInfo, pSavedModel0RwObject);
        pSavedModel0RwObject = NULL;
    }
}

// Hook info
#define HOOKPOS_PostCPedDress                           0x5A835C
#define HOOKSIZE_PostCPedDress                          5
DWORD RETURN_PostCPedDress = 0x5A8361;
void _declspec(naked) HOOK_PostCPedDress()
{
    _asm
    {
        pushad
        call    OnMy_PostCPedDress
        popad

        // Do original code and continue
        mov     eax, [esi+18h]
        push    edi
        push    eax
        jmp     RETURN_PostCPedDress
    }
}

////////////////////////////////////////////////
//
// CMultiplayerSA::EnableHooks_ClothesMemFix
//
// Enable/disable hooks programmatically
//
////////////////////////////////////////////////
void CMultiplayerSA::EnableHooks_ClothesMemFix(bool bEnable)
{
    // Switch
    if (m_bEnabledClothesMemFix == bEnable)
        return;
    m_bEnabledClothesMemFix = bEnable;

    // Memory saved here
    static CBuffer savedMem;
    SHookInfo      hookInfoList[] = {
        MAKE_HOOK_INFO(CClothesDeleteRwObject),
        MAKE_HOOK_INFO(PostCPedDress),
    };
    SPokeInfo pokeInfoList[] = {
        {0x5A6B84, 0xb3},
        {0x5A6B85, 0x01},
    };

    // Enable or not?
    if (bEnable)
    {
        CBufferWriteStream stream(savedMem);
        for (uint i = 0; i < NUMELMS(hookInfoList); i++)
        {
            const SHookInfo& hookInfo = hookInfoList[i];
            // Save memory before we blat it
            stream.WriteBytes((void*)hookInfo.dwAddress, hookInfo.uiSize);
            // Add hook
            HookInstall(hookInfo.dwAddress, hookInfo.dwHook, hookInfo.uiSize);
        }
        for (uint i = 0; i < NUMELMS(pokeInfoList); i++)
        {
            const SPokeInfo& pokeInfo = pokeInfoList[i];
            // Save memory before we blat it
            stream.WriteBytes((void*)pokeInfo.dwAddress, 1);
            // Add poke
            MemPut<BYTE>(pokeInfo.dwAddress, pokeInfo.ucValue);
        }
    }
    else
    {
        // Restore memory
        CBufferReadStream stream(savedMem);
        for (uint i = 0; i < NUMELMS(hookInfoList); i++)
        {
            const SHookInfo& hookInfo = hookInfoList[i];
            BYTE             temp[10];
            assert(sizeof(temp) >= hookInfo.uiSize);
            stream.ReadBytes(temp, hookInfo.uiSize);
            MemCpy((void*)hookInfo.dwAddress, temp, hookInfo.uiSize);
        }
        for (uint i = 0; i < NUMELMS(pokeInfoList); i++)
        {
            const SPokeInfo& pokeInfo = pokeInfoList[i];
            BYTE             temp[1];
            stream.ReadBytes(temp, 1);
            MemPut<BYTE>(pokeInfo.dwAddress, temp[0]);
        }
    }
}
