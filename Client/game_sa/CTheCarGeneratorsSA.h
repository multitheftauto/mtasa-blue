/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTheCarGeneratorsSA.h
 *  PURPOSE:     Header file for car entity generator class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CTheCarGenerators.h>
#include "Common.h"

#include "CCarGeneratorSA.h"

#define MAX_CAR_GENERATORS      500

#define ARRAY_CarGenerators     0xC27AD0

class CTheCarGeneratorsSA : public CTheCarGenerators
{
private:
    CCarGeneratorSA* CarGenerators[MAX_CAR_GENERATORS];

public:
    CTheCarGeneratorsSA();
    ~CTheCarGeneratorsSA();

    CCarGenerator* GetCarGenerator(DWORD dwIndex);
};
