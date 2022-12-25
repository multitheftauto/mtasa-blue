/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CParticleSA.h
 *  PURPOSE:     Header file for particle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CParticle.h>

class CParticleSAInterface
{
};

class CParticleSA : public CParticle
{
private:
    CParticleSAInterface* internalInterface;

public:
};
