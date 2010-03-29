/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTextSA.h
*  PURPOSE:     Header file for text rendering class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TEXT
#define __CGAMESA_TEXT

#include <game/CText.h>

#define FUNC_CText_Get          0x6A0050

#define MAX_TEXT_ENTRIES        512

struct TextEntry
{
    char szKey[16];
    char szValue[255];
};

class CTextSA : public CText
{
public:
    CTextSA();
    char    * GetText(char * szKey);
    void    SetText(char * szKey, char * szValue);
    //char  * GetReplacementText ( char * szKey );
};

#endif
