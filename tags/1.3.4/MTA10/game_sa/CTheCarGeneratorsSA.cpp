/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTheCarGeneratorsSA.cpp
*  PURPOSE:     Car entity generator
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTheCarGeneratorsSA::CTheCarGeneratorsSA()
{
    DEBUG_TRACE("CTheCarGeneratorsSA::CTheCarGeneratorsSA()");
    for(int i = 0; i<MAX_CAR_GENERATORS;i++)
        CarGenerators[i] = new CCarGeneratorSA((CCarGeneratorSAInterface *)(ARRAY_CarGenerators + i * sizeof(CCarGeneratorSAInterface)));
}

CTheCarGeneratorsSA::~CTheCarGeneratorsSA ( void )
{
    for ( int i = 0; i < MAX_CAR_GENERATORS; i++ )
    {
        delete CarGenerators [i];
    }
}

CCarGenerator * CTheCarGeneratorsSA::GetCarGenerator(DWORD dwIndex)
{
    DEBUG_TRACE("CCarGenerator * CTheCarGeneratorsSA::GetCarGenerator(DWORD dwIndex)");
    if(dwIndex < MAX_CAR_GENERATORS)
        return CarGenerators[dwIndex];
    else
        return NULL;
}
