/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CVisibilityPlugins.h
*  PURPOSE:     Visibility plugins interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVisibilityPlugins_H
#define __CVisibilityPlugins_H

struct RpClump;

class CVisibilityPlugins
{
public:
    virtual void                SetClumpAlpha       ( RpClump * pClump, int iAlpha ) = 0;
};

#endif
