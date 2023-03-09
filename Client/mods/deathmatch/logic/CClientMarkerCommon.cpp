/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMarkerCommon.cpp
 *  PURPOSE:     Marker entity base class
 *
 *****************************************************************************/

#include <StdInc.h>

CClientMarkerCommon::CClientMarkerCommon()
{
}

CClientMarkerCommon::~CClientMarkerCommon()
{
}

bool CClientMarkerCommon::IsHit(CClientEntity* pEntity) const
{
    // Grab the given entity's position
    CVector vecPosition;
    pEntity->GetPosition(vecPosition);

    // Return whether that point hits or not
    return IsHit(vecPosition);
}

void CClientMarkerCommon::DoPulse()
{
}
