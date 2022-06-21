/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColModelSA.cpp
 *  PURPOSE:     Collision model entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CColModelSA::CColModelSA()
{
    // CColModel::CColModel
    m_pInterface = ((CColModelSAInterface*(__thiscall*)(CColModelSAInterface*))(FUNC_CColModel_Constructor))(new CColModelSAInterface);
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
        // CColModel::~CColModel
        ((void(__thiscall*)(CColModelSAInterface*))(FUNC_CColModel_Destructor))(m_pInterface);
        delete m_pInterface;
    }
}
