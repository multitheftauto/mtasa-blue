/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/TaskPhysicalResponse.h
*  PURPOSE:     Physical response task interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASKPHYSICALRESPONSE
#define __CGAME_TASKPHYSICALRESPONSE

#include "Task.h"

class CTaskSimpleChoking : public virtual CTaskSimple
{
public:
    virtual                     ~CTaskSimpleChoking ( void ) {};

    virtual CPed*               GetAttacker         ( void ) = 0;
    virtual unsigned int        GetTimeRemaining    ( void ) = 0;
    virtual unsigned int        GetTimeStarted      ( void ) = 0;
    virtual bool                IsTeargas           ( void ) = 0;
    virtual bool                IsFinished          ( void ) = 0;

    virtual void                UpdateChoke         ( CPed* pPed, CPed* pAttacker, bool bIsTearGas ) = 0;
};


#endif
