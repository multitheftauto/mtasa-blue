/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CBuilding.h
*  PURPOSE:     Building entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_BUILDING
#define __CGAME_BUILDING

#include "CEntity.h"

class CBuilding : public virtual CEntity
{
public:
    virtual                         ~CBuilding ( void ) {};
};

#endif
