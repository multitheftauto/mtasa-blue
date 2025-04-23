/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/TaskIK.h
 *  PURPOSE:     Inverse kinematics task interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Task.h"

#define NUM_IK_CHAIN_SLOTS 5 //(28 - sizeof(CTaskSimple)) / 4

class CTaskSimpleIKChain : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleIKChain(){};
};

class CTaskSimpleIKLookAt : public virtual CTaskSimpleIKChain
{
public:
    virtual ~CTaskSimpleIKLookAt(){};
};

class CTaskSimpleIKManager : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleIKManager(){};

    virtual int                 AddIKChainTask(CTaskSimpleIKChain* pIKChainTask, int slotID = -1) = 0;
    virtual void                RemoveIKChainTask(int slotID) = 0;
    virtual void                BlendOut(int slotID, int blendOutTime) = 0;
    virtual unsigned char       IsSlotEmpty(int slotID) = 0;
    virtual CTaskSimpleIKChain* GetTaskAtSlot(int slotID) = 0;
};

class CTaskSimpleTriggerLookAt : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleTriggerLookAt(){};
};
