/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBuildingRemoval.cpp
*  PURPOSE:     Header file for custom weapon stats.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#include "StdInc.h"

CBuildingRemoval::CBuildingRemoval ( unsigned short usModel, float fRadius, CVector vecPos )
{
    m_usModel = usModel;
    m_fRadius = fRadius;
    m_vecPos = vecPos;
}

CBuildingRemoval::~CBuildingRemoval ( )
{

}