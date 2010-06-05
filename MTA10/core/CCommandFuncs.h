/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCommandFuncs.h
*  PURPOSE:		Header file for command functionality class
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*				Derek Abdine <>
*				Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOMMANDFUNCS_H
#define __CCOMMANDFUNCS_H

#include <string>

#define DECLARE_COMMAND_HANDLER(a) static void a ( const char * szParameters );

class CCommandFuncs
{
public:
    DECLARE_COMMAND_HANDLER ( Help );
    DECLARE_COMMAND_HANDLER ( Exit );
    DECLARE_COMMAND_HANDLER ( Ver );
	DECLARE_COMMAND_HANDLER ( Time );
    DECLARE_COMMAND_HANDLER ( Clear );
    DECLARE_COMMAND_HANDLER ( Load );
    DECLARE_COMMAND_HANDLER ( Unload );

    DECLARE_COMMAND_HANDLER ( Vid );
    DECLARE_COMMAND_HANDLER ( Window );

    DECLARE_COMMAND_HANDLER ( Connect );
    DECLARE_COMMAND_HANDLER ( Reconnect );
    DECLARE_COMMAND_HANDLER ( Bind );
    DECLARE_COMMAND_HANDLER ( Unbind );
    DECLARE_COMMAND_HANDLER ( Binds );
    DECLARE_COMMAND_HANDLER ( CopyGTAControls );
    DECLARE_COMMAND_HANDLER ( ClearDebug )
	DECLARE_COMMAND_HANDLER ( HUD );
	DECLARE_COMMAND_HANDLER ( ScreenShot );
	DECLARE_COMMAND_HANDLER ( ConnectionType );
    DECLARE_COMMAND_HANDLER ( SaveConfig );
};

#endif
