/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColCallback.h
*  PURPOSE:     Collision entity events class
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTCOLCALLBACK_H
#define __CCLIENTCOLCALLBACK_H

#include "CClientColShape.h"

class CClientColCallback  
{
public:
    virtual void        Callback_OnCollision            ( CClientColShape& Shape, CClientEntity& Entity ) = 0;
    virtual void        Callback_OnLeave                ( CClientColShape& Shape, CClientEntity& Entity ) = 0;
};

#endif
