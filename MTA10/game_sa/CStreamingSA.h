/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.h
*  PURPOSE:     Header file for data streamer class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CStreamingSA_H
#define __CStreamingSA_H

#include <game/CStreaming.h>
#include "Common.h"

#define FUNC_CStreaming_RequestAnimations 0x407120
#define FUNC_CStreaming_RemoveAnimations 0x40c1a0

class CStreamingSA : public CStreaming
{
public:
    void            RequestAnimations   ( int iBlockIndex );
    void            RemoveAnimations    ( int iBlockIndex );
};

#endif