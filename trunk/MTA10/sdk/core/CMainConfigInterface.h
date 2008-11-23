/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/core/CMainConfigInterface.h
*  PURPOSE:		Core configuration interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMAINCONFIGINTERFACE_H
#define __CMAINCONFIGINTERFACE_H

#include <string>

class CMainConfigInterface
{
public:
    virtual float                   GetTextScale            ( void ) = 0;
    virtual void                    SetTextScale            ( float fScale ) = 0;

	virtual unsigned short			GetFPSLimit             ( void ) = 0;
	virtual void					SetFPSLimit             ( unsigned short usLimit ) = 0;
};

#endif
