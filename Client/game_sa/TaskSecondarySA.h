/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/TaskSecondarySA.h
 *  PURPOSE:     Secondary game tasks
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector2D.h>
#include <game/TaskSecondary.h>
#include "TaskSA.h"

// typedef int CAnimBlendAssociation;
typedef DWORD IKChain_c;

// ##############################################################################
// ## Name:    CTaskSimpleDuck
// ## Purpose: Make the ped duck
// ##############################################################################

class CTaskSimpleDuckSAInterface : public CTaskSimpleSAInterface
{
public:
    uint   m_nStartTime;
    ushort m_nLengthOfDuck;
    short  m_nShotWhizzingCounter;
    int*   m_pDuckAnim;            // was CAnimBlendAssociation
    int*   m_pMoveAnim;            // was CAnimBlendAssociation

    bool m_bIsFinished;
    bool m_bIsAborting;
    bool m_bNeedToSetDuckFlag;            // incase bIsDucking flag gets cleared elsewhere - so we know to stop duck task
    bool m_bIsInControl;                  // if duck task is being controlled by another task then it requires continuous control

    CVector2D m_vecMoveCommand;
    uchar     m_nDuckControlType;

    uchar m_nCountDownFrames;
};

class CTaskSimpleDuckSA : public virtual CTaskSimpleSA, public virtual CTaskSimpleDuck
{
public:
    CTaskSimpleDuckSA(){};
    CTaskSimpleDuckSA(eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck = 0, short nUseShotsWhizzingEvents = -1);
};
