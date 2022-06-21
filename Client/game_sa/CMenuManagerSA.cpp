/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMenuManagerSA.cpp
 *  PURPOSE:     Game menu manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

BYTE CMenuManagerSA::GetCurrentScreen()
{
    return this->GetInterface()->bCurrentScreen;
}

BYTE CMenuManagerSA::GetPreviousScreen()
{
    return this->GetInterface()->bLastScreen;
}

void CMenuManagerSA::SwitchToNewScreen(BYTE ScreenID)
{
    // CMenuManager::SwitchToNewScreen
    ((void(__cdecl*)(BYTE))FUNC_SwitchToNewScreen)(ScreenID);
}

void CMenuManagerSA::DisplayHelpText(char* szHelpText)            // DisplayHelperText
{
    // CMenuManager::DisplayHelperText
    ((void(__cdecl*)(const char*))FUNC_DisplayHelperText)(szHelpText);
}
