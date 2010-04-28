/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPlayerPed.h
*  PURPOSE:     Played ped entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


#ifndef __CGAME_PLAYERPED
#define __CGAME_PLAYERPED

#include "CPed.h"
#include "CWanted.h"

class CPlayerPed : public virtual CPed
{
public:
    virtual                 ~CPlayerPed ( void ) {};

    virtual CWanted*        GetWanted               ( void ) = 0;

    virtual float           GetSprintEnergy         ( void ) = 0;
    virtual void            SetSprintEnergy         ( float fSprintEnergy ) = 0;

    virtual void            SetInitialState         ( void ) = 0;

    virtual eMoveAnim       GetMoveAnim             ( void ) = 0;
    virtual void            SetMoveAnim             ( eMoveAnim iAnimGroup ) = 0;
};

#endif
