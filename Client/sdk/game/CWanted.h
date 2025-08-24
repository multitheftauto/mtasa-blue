/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWanted.h
 *  PURPOSE:     Wanted level interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CWanted
{
public:
    virtual void SetWantedLevel(DWORD dwWantedLevel) = 0;
    virtual char GetWantedLevel() = 0;
    virtual void SetWantedLevelNoFlash(DWORD dwWantedLevel) = 0;
};
