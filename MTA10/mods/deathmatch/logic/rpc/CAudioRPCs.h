/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CAudioRPCs.h
*  PURPOSE:     Header for audio RPC class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAudioRPCs_H
#define __CAudioRPCs_H

#include "CRPCFunctions.h"

class CAudioRPCs : public CRPCFunctions
{
public:
    static void                 LoadFunctions           ( void );

    DECLARE_RPC ( PlaySound );

private:
    enum
	{
		AUDIO_FRONTEND,
		AUDIO_MISSION_PRELOAD,
		AUDIO_MISSION_PLAY
	};
};

#endif