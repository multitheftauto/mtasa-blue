/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_SingleLinkNode.cpp
 *  PORPOISE:    Unlimites the SingleLinkNode pool
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

constexpr std::size_t NODE_POOL_CAPACITY = 90000;
constexpr std::size_t NODE_POOL_SIZE_BYTES = NODE_POOL_CAPACITY * 8;

constexpr std::uint32_t      HOOKPOS_SingleLinkNodeConstructor = 0x5522A0;
constexpr std::size_t        HOOKSIZE_SingleLinkNodeConstructor = 5;
static void _declspec(naked) HOOK_SingleLinkNodeConstructor()
{
    _asm {
        push 0x8
        call malloc
        add esp, 4
        pop edi
        pop esi
        ret
    }
}

constexpr std::uint32_t    HOOKPOS_SingleLinkNodeDestructor = 0x552396;
constexpr std::size_t      HOOKSIZE_SingleLinkNodeDestructor = 5;
static const std::uint32_t CONTINUE_SingleLinkNodeDestructor = 0x55239B;
static void _declspec(naked) HOOK_SingleLinkNodeDestructor()
{
    _asm {
        mov eax, [esp+0x4] ; Node to delete
        mov edx, [ecx] ; Pool objects array
        cmp edx, eax
        ja out_of_pool
        add edx, NODE_POOL_SIZE_BYTES ; Pool end
        cmp edx, eax
        jb out_of_pool
        push ebx ; original
        jmp CONTINUE_SingleLinkNodeDestructor
     out_of_pool:
        push eax
        call free
        pop eax
        retn
    }
}

void CMultiplayerSA::InitHooks_SingleLinkNode()
{
    EZHookInstall(SingleLinkNodeConstructor);
    EZHookInstall(SingleLinkNodeDestructor);
}
