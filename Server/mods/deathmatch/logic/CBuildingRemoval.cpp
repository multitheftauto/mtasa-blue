/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBuildingRemoval.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBuildingRemoval.h"

CBuildingRemoval::CBuildingRemoval(unsigned short usModel, float fRadius, const CVector& vecPos, char cInterior)
{
    m_usModel = usModel;
    m_fRadius = fRadius;
    m_vecPos = vecPos;
    m_cInterior = cInterior;
}

CBuildingRemoval::~CBuildingRemoval()
{
}
