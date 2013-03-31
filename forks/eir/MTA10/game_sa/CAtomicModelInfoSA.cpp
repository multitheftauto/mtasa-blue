/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAtomicModelInfoSA.cpp
*  PURPOSE:     Atomic model instance
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

void CAtomicModelInfoSA::SetAtomic( RpAtomic *atomic )
{
    // TODO
}

void CDamageAtomicModelInfoSA::SetupPipeline( RpAtomic *link )
{
    m_atomicLink = link;

    if ( link->IsNight() )
        RpAtomicSetupObjectPipeline( link );
    else if ( link->pipeline == RW_ATOMIC_RENDER_VEHICLE )
        RpAtomicSetupVehiclePipeline( link );
}