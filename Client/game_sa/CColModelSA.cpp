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

CColModelSA::CColModelSA()
{
    m_pInterface = new CColModelSAInterface;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CColModel_Constructor;

    try
    {
        // clang-format off
        __asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
        // clang-format on
    }
    catch (...)
    {
        // Clean up on constructor failure
        delete m_pInterface;
        m_pInterface = nullptr;
        throw;
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
        DWORD dwThis = (DWORD)m_pInterface;
        DWORD dwFunc = FUNC_CColModel_Destructor;

        try
        {
            // clang-format off
            __asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }
            // clang-format on
        }
        catch (...)
        {
            // Ensure cleanup completes on exception
        }

        delete m_pInterface;
        m_pInterface = nullptr;
    }
}

void CColModelSA::Destroy()
{
    delete this;
}
