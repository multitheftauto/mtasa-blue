/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPlane.h
*  PURPOSE:     Plane vehicle entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PLANE
#define __CGAME_PLANE

#include "CVehicle.h"

class CPlane : public virtual CVehicle
{
public:
    virtual             ~CPlane ( void ) {};
};

#endif
