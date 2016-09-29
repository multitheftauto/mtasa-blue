/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CMenuManager.h
*  PURPOSE:     Menu manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_MENUMANAGER
#define __CGAME_MENUMANAGER

#include <windows.h>

class CMenuManager
{
public:
    virtual BYTE    GetCurrentScreen()=0;
    virtual BYTE    GetPreviousScreen()=0;
    virtual void    SwitchToNewScreen(BYTE ScreenID)=0;
    virtual void    DisplayHelpText ( char * szHelpText )=0;
};

#endif
