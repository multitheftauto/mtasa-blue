/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColModelSA.cpp
 *  PURPOSE:     Collision model entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColModelSA.h"

#include <stdexcept>

namespace
{
    bool CallNativeColModelConstructor(CColModelSAInterface* pInterface) noexcept
    {
        if (!pInterface)
            return false;

        uintptr_t dwThis = reinterpret_cast<uintptr_t>(pInterface);
        DWORD dwFunc = FUNC_CColModel_Constructor;

        __try
        {
            // clang-format off
            __asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }
            // clang-format on
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }

        return true;
    }

    void CallNativeColModelDestructor(CColModelSAInterface* pInterface) noexcept
    {
        if (!pInterface)
            return;

        uintptr_t dwThis = reinterpret_cast<uintptr_t>(pInterface);
        DWORD dwFunc = FUNC_CColModel_Destructor;

        __try
        {
            // clang-format off
            __asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }
            // clang-format on
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }
}

CColModelSA::CColModelSA()
    : m_pInterface(new CColModelSAInterface), m_bDestroyInterface(false), m_bValid(false)
{
    if (!CallNativeColModelConstructor(m_pInterface))
    {
        delete m_pInterface;
        m_pInterface = nullptr;
        throw std::runtime_error("CColModelSA native constructor failed");
    }

    m_bDestroyInterface = true;
    m_bValid = true;
}

CColModelSA::CColModelSA(CColModelSAInterface* pInterface)
{
    m_pInterface = pInterface;
    m_bDestroyInterface = false;
    m_bValid = true;
}

CColModelSA::~CColModelSA()
{
    m_bValid = false;

    if (m_bDestroyInterface && m_pInterface)
    {
        CallNativeColModelDestructor(m_pInterface);
        delete m_pInterface;
        m_pInterface = nullptr;
    }
}

void CColModelSA::Destroy()
{
    delete this;
}
