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
class CParticleActivatorSAInterfaceVTBL
{
public:
    uint32 SCALAR_DELETING_DESTRUCTOR;
    uint32 ReadData;
};

// <Particle Activator Base Class>
class CParticleActivatorSAInterface
{
public:
    uint32* vtbl;
    uint8 bLooped;
    uint8 ucNumKeys;
    uint8 ucValuesCount;
    uint8 pad2;
    uint32* uiTimeArray; // uint32 uiTimeArray[ucNumKeys];
    uint32** uiValue2DArray; // uint32 uiValue2DArray[ucValuesCount][ucNumKeys];
};

