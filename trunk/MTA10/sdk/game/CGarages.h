/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CGarages.h
*  PURPOSE:     Garage manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_GARAGES
#define __CGAME_GARAGES

#include "CGarage.h"
#include "Common.h"

#define MAX_GARAGES 50

class CGarages
{
public:
    virtual CGarage*            GetGarage   ( DWORD dwID ) = 0;
    virtual void                Initialize  ( ) = 0;
};

#endif
