/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSystemManagerSA.h
*  PURPOSE:     Header file for particle system manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

// TODO: Add VTBLs and all Activators

// <Particle Activators Base Class>
class CParticleActivatorSAInterface
{
public:
    uint32* vtbl;
    uint8 bLooped;
    uint8 ucNumKeys;
    uint8 pad1;
    uint8 pad2;
    uint32* uiTimeArray;
    uint32* uiValueArray;
};

