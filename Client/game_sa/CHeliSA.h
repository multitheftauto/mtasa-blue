/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CHeliSA.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CHeli.h>
#include "CAutomobileSA.h"

class CHeliSAInterface : public CAutomobileSAInterface
{
};

class CHeliSA final : public virtual CHeli, public virtual CAutomobileSA
{
public:
    CHeliSA(CHeliSAInterface* pInterface);
};
