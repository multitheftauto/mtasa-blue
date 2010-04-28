/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CWanted.h
*  PURPOSE:     Wanted level interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_WANTED
#define __CGAME_WANTED

#include <windows.h>

class CWanted
{
public:
    virtual void            SetMaximumWantedLevel ( DWORD dwWantedLevel )=0;
    virtual void            SetWantedLevel ( DWORD dwWantedLevel )=0;
    virtual char            GetWantedLevel ( void )=0;
    virtual void            SetWantedLevelNoDrop ( DWORD dwWantedLevel )=0;
};

#endif
