/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/TaskAttack.h
 *  PURPOSE:     Attack task interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Task.h"

class CEntity;
class CVector;

class CTaskSimpleGangDriveBy : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleGangDriveBy(){};
};

enum eGunCommand
{
    GCOMMAND_NONE = 0,
    GCOMMAND_AIM,
    GCOMMAND_FIRE,
    GCOMMAND_FIREBURST,
    GCOMMAND_RELOAD,
    GCOMMAND_PISTOLWHIP,
    GCOMMAND_END_LEISURE,
    GCOMMAND_END_NOW
};

class CVector2D;

class CTaskSimpleUseGun : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleUseGun(){};

    virtual bool ControlGun(CPed* pPed, CEntity* pTargetEntity, char nCommand) = 0;
    virtual bool ControlGunMove(CVector2D* pMoveVec) = 0;
    virtual void Reset(CPed* pPed, CEntity* pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength = 1) = 0;

    virtual bool GetSkipAim() = 0;
};

class CTaskSimpleFight : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleFight(){};
};
