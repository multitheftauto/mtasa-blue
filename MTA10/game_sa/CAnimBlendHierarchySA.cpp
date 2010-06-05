/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CAnimBlendHierarchySA.cpp
*  PURPOSE:		Animation blend hierarchy
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CAnimBlendHierarchySAInterface::GetIndex ( void )
{
    return ( ( ( DWORD ) this - ARRAY_CAnimManager_Animations ) / 24 );
}