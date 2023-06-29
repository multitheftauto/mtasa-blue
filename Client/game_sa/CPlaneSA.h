/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CPlaneSA.h
 *  PURPOSE:     Header file for plane vehicle entity class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPlane.h>
#include "CAutomobileSA.h"

class CPlaneSAInterface : public CAutomobileSAInterface {};

class CPlaneSA final : public virtual CPlane, public virtual CAutomobileSA
{
public:
    CPlaneSA(CPlaneSAInterface* pInterface);

    CPlaneSAInterface* GetPlaneInterface() { return reinterpret_cast<CPlaneSAInterface*>(GetInterface()); }
};
