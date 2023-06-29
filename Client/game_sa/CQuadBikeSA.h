/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CQuadBikeSA.h
 *  PURPOSE:     Header file for quad bike vehicle entity class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CQuadBike.h>
#include "CAutomobileSA.h"

class CQuadBikeSAInterface : public CAutomobileSAInterface
{
    // fill this
};

class CQuadBikeSA final : public virtual CQuadBike, public virtual CAutomobileSA
{
public:
    CQuadBikeSA(CQuadBikeSAInterface* pInterface);

    CQuadBikeSAInterface* GetQuadBikeInterface() { return reinterpret_cast<CQuadBikeSAInterface*>(GetInterface()); }
};
