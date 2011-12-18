/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTextSA.cpp
*  PURPOSE:     Text rendering
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

VOID HOOK_CText_Get();
DWORD STORE_CText_Get = 0;
char * szText;
char * szReplacementText;

TextEntry TextEntries[MAX_TEXT_ENTRIES];

CTextSA::CTextSA()
{
    //HookInstall(FUNC_CText_Get, (DWORD)HOOK_CText_Get, &STORE_CText_Get, 9);
}

char * CTextSA::GetText(char * szKey)
{
    return NULL;
}

void CTextSA::SetText(char * szKey, char * szValue)
{
    for(int i = 0; i < MAX_TEXT_ENTRIES; i++)
    {
        if ( TextEntries[i].szKey[0] == 0 ) 
        {
            strcpy(TextEntries[i].szKey, szKey);
            strcpy(TextEntries[i].szValue, szValue);
            return;
        }
    }
}

char * GetReplacementText(char * szKey)
{
    // NOTE: This is really slow (0.8% of program time spent here) so needs to be
    // rewriten before it can be readded. As far as I'm aware we do not use this code
    // at the moment anyway (eAi) 2006-04-17

    /*for(int i = 0; i < MAX_TEXT_ENTRIES; i++)
    {
        if ( strcmp(TextEntries[i].szKey, szKey) == 0 ) 
        {
            if ( TextEntries[i].szValue[0] != 0 )
            {
                return TextEntries[i].szValue;
            }
        }
    }*/
    return NULL;
}

VOID _declspec(naked) HOOK_CText_Get()
{
    _asm
    {
        sub     esp, 0x20
        push    esi
        push    edi
        mov     edi, [esp+0x2C]
        mov     szText, edi
        pushad
    }

    szReplacementText = GetReplacementText(szText);
    if ( szReplacementText != NULL )
    {
        //OutputDebugString("Replaced Text!");
        _asm
        {
            popad
            pop     edi
            pop     esi
            add     esp, 0x20
            mov     eax, szReplacementText
            retn    4
        }
    }
    else
    {
        //OutputDebugString(szText);
        _asm
        {
            popad
            mov     eax, FUNC_CText_Get
            add     eax, 9
            jmp     eax
        }
    }
}
