/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/TaskPhysicalResponseSA.h
 *  PURPOSE:     Physical response game tasks
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/TaskPhysicalResponse.h>
#include "TaskSA.h"

class CTaskSimpleChokingSAInterface : public CTaskSimpleSAInterface
{
public:
    class CPedSAInterface* m_pAttacker;
    DWORD*                 m_pAnim;
    uint                   m_nTimeRemaining;
    uint                   m_nTimeStarted;
    uchar                  m_bIsTeargas;
    bool                   m_bIsFinished;
};

class CTaskSimpleChokingSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleChoking
{
public:
    CTaskSimpleChokingSA(){};
    CTaskSimpleChokingSA(CPed* pAttacker, bool bIsTearGas);

    CPed* GetAttacker();
    uint  GetTimeRemaining();
    uint  GetTimeStarted();
    bool  IsTeargas();
    bool  IsFinished();

    void UpdateChoke(CPed* pPed, CPed* pAttacker, bool bIsTearGas);
};
