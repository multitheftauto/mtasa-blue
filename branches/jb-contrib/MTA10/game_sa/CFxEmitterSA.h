/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        
*  PURPOSE:     
*  DEVELOPERS:  Nobody knows
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CFxEmitterSAInterfaceVTBL
{
public:
    uint32 SCALAR_DELETING_DESTRUCTOR;
    uint32 Init;
    uint32 Update;
    uint32 Reset;
    uint32 AddParticle2;
    uint32 AddParticle;
};

class CFxEmitterSAInterface
{
public:
    CFxEmitterSAInterfaceVTBL* vtbl;
    CFxEmitterBPSAInterface* pFxEmitterBP;
    CFxSystemSAInterface* pFxSystem;
    uint8 bActive;
    uint8 pad1;
    uint8 pad2;
    uint8 pad3;
    uint32 pad4;
};