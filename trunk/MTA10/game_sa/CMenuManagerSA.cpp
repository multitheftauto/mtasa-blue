/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CMenuManagerSA.cpp
*  PURPOSE:     Game menu manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
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
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwFunc = FUNC_SwitchToNewScreen;
    DWORD dwScreenID = ScreenID;
    
    _asm
    {
        mov     eax, dwThis
        push    dwScreenID
        call    dwFunc
    }
}

void CMenuManagerSA::DisplayHelpText ( char * szHelpText ) // DisplayHelperText
{
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwFunc = FUNC_DisplayHelperText;

    _asm
    {
        mov     eax, dwThis
        push    szHelpText
        call    dwFunc
    }
}
