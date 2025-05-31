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
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
    m_bDestroyInterface = true;
}

CColModelSA::CColModelSA(CColModelSAInterface* pInterface)
{
    m_pInterface = pInterface;
    m_bDestroyInterface = false;
}

CColModelSA::~CColModelSA()
{
    if (m_bDestroyInterface)
    {
        DWORD dwThis = (DWORD)m_pInterface;
        DWORD dwFunc = FUNC_CColModel_Destructor;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
        delete m_pInterface;
    }
}
