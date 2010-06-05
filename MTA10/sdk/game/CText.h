/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CText.h
*  PURPOSE:		Game text interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TEXT
#define __CGAME_TEXT

class CText
{
public:
	virtual char	* GetText(char * szKey)=0;
	virtual void	SetText(char * szKey, char * szValue)=0;
};

#endif