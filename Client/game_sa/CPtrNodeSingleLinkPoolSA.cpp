/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPtrNodeSingleLinkPoolSA.cpp
 *  PURPOSE:     Custom implementation for the CPtrNodeSingleLinkPool pool
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPtrNodeSingleLinkPoolSA.h"

CPtrNodeSingleLinkPoolSA::pool_t* CPtrNodeSingleLinkPoolSA::m_customPool = nullptr;

CPtrNodeSingleLinkPoolSA::CPtrNodeSingleLinkPoolSA()
{
    if (!m_customPool)
        m_customPool = new CPtrNodeSingleLinkPoolSA::pool_t();
}

constexpr std::uint32_t HOOKPOS_SingleLinkNodeConstructor = 0x552380;
constexpr std::size_t   HOOKSIZE_SingleLinkNodeConstructor = 6;
static CPtrNodeSingleLinkPoolSA::pool_item_t* __cdecl HOOK_SingleLinkNodeConstructor()
{
    return CPtrNodeSingleLinkPoolSA::GetPoolInstance()->AllocateItem();
}

constexpr std::uint32_t HOOKPOS_SingleLinkNodeDestructor = 0x552390;
constexpr std::size_t   HOOKSIZE_SingleLinkNodeDestructor = 6;
static CPtrNodeSingleLinkPoolSA::pool_item_t* __cdecl HOOK_SingleLinkNodeDestructor(CPtrNodeSingleLinkPoolSA::pool_item_t* item)
{
    CPtrNodeSingleLinkPoolSA::GetPoolInstance()->RemoveItem(item);
    // The game doesen't use the return value
    return item;
}

// Replace pool->RemoveItem here
constexpr std::uint32_t      HOOKPOS_CPtrListSingleLink_Flush = 0x55243B;
constexpr std::size_t        HOOKSIZE_CPtrListSingleLink_Flush = 6;
constexpr std::uint32_t      CONTINUE_CPtrListSingleLink_Flush = 0x55245B;
static void _declspec(naked) HOOK_CPtrListSingleLink_Flush()
{
    __asm {
        mov edi, ecx ; save register

        ; CPtrNodeSingleLinkPoolSA::m_customPool->RemoveItem(eax)

        mov ecx, CPtrNodeSingleLinkPoolSA::m_customPool
        push eax
        call CPtrNodeSingleLinkPoolSA::pool_t::RemoveItem

        mov ecx, edi ; restore
        jmp CONTINUE_CPtrListSingleLink_Flush
    }
}

void CPtrNodeSingleLinkPoolSA::StaticSetHooks()
{
    EZHookInstall(SingleLinkNodeConstructor);
    EZHookInstall(SingleLinkNodeDestructor);
    EZHookInstall(CPtrListSingleLink_Flush);

    // Skip the original pool initialization
    MemCpy((void*)0x550F26, "\xEB\x2D", 2);
}

