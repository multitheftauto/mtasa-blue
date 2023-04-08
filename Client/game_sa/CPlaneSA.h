/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlaneSA.h
 *  PURPOSE:     Header file for plane vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPlane.h>
#include "CAutomobileSA.h"

class CPlaneSAInterface : public CAutomobileSAInterface
{
    // + 2508 = undercarrige possition (float - 1.0 = up, 0.0 = down)
    // fill this
};

class CPlaneSA final : public virtual CPlane, public virtual CAutomobileSA
{
public:
    CPlaneSA(CPlaneSAInterface* pInterface);

    CPlaneSAInterface* GetPlaneInterface() { return reinterpret_cast<CPlaneSAInterface*>(GetInterface()); }
};
