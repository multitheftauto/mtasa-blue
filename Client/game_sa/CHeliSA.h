/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CHeliSA.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
