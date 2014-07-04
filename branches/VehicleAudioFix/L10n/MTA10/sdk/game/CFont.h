/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CFont.h
*  PURPOSE:     Game font interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_FONT
#define __CGAME_FONT

class CFont
{
public:
    virtual void        PrintString ( float fX, float fY, char * szString )=0;
};

#endif
