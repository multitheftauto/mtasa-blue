/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CMenuManagerSA.h
*  PURPOSE:     Header file for game menu manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_MENUMANAGER
#define __CGAMESA_MENUMANAGER

#include <game/CMenuManager.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define CLASS_CMenuManager          0xBA6748

#define FUNC_SwitchToNewScreen      0x573680
#define FUNC_DisplayHelperText      0x57E240

class CMenuManagerSAInterface
{
public:
    BYTE bPadding[92];
    bool bIsActive;
    BYTE bPadding2[256];
    BYTE bCurrentScreen;
    BYTE bLastScreen;
};

class CMenuManagerSA : public CMenuManager
{
private:
    CMenuManagerSAInterface * internalInterface;
public:
    CMenuManagerSA() { internalInterface = (CMenuManagerSAInterface *)CLASS_CMenuManager; }

    CMenuManagerSAInterface * GetInterface() { return internalInterface; }

    BYTE    GetCurrentScreen();
    BYTE    GetPreviousScreen();
    void    SwitchToNewScreen(BYTE ScreenID);
    void    DisplayHelpText ( char * szHelpText );
};

#endif
