/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColCallback.h
*  PURPOSE:     Collision entity callback interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOLCALLBACK_H
#define __CCOLCALLBACK_H

#include "CColShape.h"

class CColCallback  
{
public:
    virtual void        Callback_OnCollision            ( CColShape& Shape, CElement& Element ) = 0;
    virtual void        Callback_OnLeave                ( CColShape& Shape, CElement& Element ) = 0;
    virtual void        Callback_OnCollisionDestroy     ( CColShape* pShape ) = 0;
};

#endif
