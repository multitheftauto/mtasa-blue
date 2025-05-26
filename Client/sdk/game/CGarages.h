/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CGarages.h
 *  PURPOSE:     Garage manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CGarage;

#define MAX_GARAGES 50

class CGarages
{
public:
    virtual CGarage* GetGarage(DWORD dwID) = 0;
    virtual void     Initialize() = 0;
};
