/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/TaskPhysicalResponseSA.h
 *  PURPOSE:     Physical response game tasks
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/TaskPhysicalResponse.h>
#include "TaskSA.h"

#define FUNC_CTaskSimpleChoking__Constructor                0x6202C0
#define FUNC_CTaskSimpleChoking__UpdateChoke                0x620660

class CTaskSimpleChokingSAInterface : public CTaskSimpleSAInterface
{
public:
    class CPedSAInterface* m_pAttacker;
    DWORD*                 m_pAnim;
    std::uint32_t           m_nTimeRemaining;
    std::uint32_t           m_nTimeStarted;
    std::uint8_t          m_bIsTeargas;
    bool                   m_bIsFinished;
};

class CTaskSimpleChokingSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleChoking
{
public:
    CTaskSimpleChokingSA(){};
    CTaskSimpleChokingSA(CPed* pAttacker, bool bIsTearGas);

    CPed*        GetAttacker();
    std::uint32_t GetTimeRemaining();
    std::uint32_t GetTimeStarted();
    bool         IsTeargas();
    bool         IsFinished();

    void UpdateChoke(CPed* pPed, CPed* pAttacker, bool bIsTearGas);
};
