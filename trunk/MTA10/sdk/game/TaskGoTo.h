/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/TaskGoTo.h
*  PURPOSE:     Go to task interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASKGOTO
#define __CGAME_TASKGOTO

#include "Task.h"
#include "CPathFind.h"

enum
{
    WANDER_TYPE_STANDARD=0,
    WANDER_TYPE_COP,
    WANDER_TYPE_MEDIC,
    WANDER_TYPE_CRIMINAL,
    WANDER_TYPE_GANG,
    WANDER_TYPE_SHOP,
    WANDER_TYPE_FLEE,
    WANDER_TYPE_PROSTITUTE
};

#define NO_WANDER_TYPE      9999

class CTaskComplexWander : public virtual CTaskComplex
{
public:
    virtual                     ~CTaskComplexWander ( void ) {};

    virtual CNodeAddress *      GetNextNode()=0;
    virtual CNodeAddress *      GetLastNode()=0;

    virtual int                 GetWanderType()=0;
};

class CTaskComplexWanderStandard : public virtual CTaskComplexWander
{
public:
    virtual                     ~CTaskComplexWanderStandard ( void ) {};
};

#endif
