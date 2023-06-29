/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CTrailerSA.h
 *  PURPOSE:     Header file for trailer vehicle entity class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CTrailer.h>
#include "CAutomobileSA.h"

class CTrailerSAInterface : public CAutomobileSAInterface {};

class CTrailerSA final : public virtual CTrailer, public virtual CAutomobileSA
{
public:
    CTrailerSA(CTrailerSAInterface* pInterface);

    CTrailerSAInterface* GetTrailerInterface() { return reinterpret_cast<CTrailerSAInterface*>(GetInterface()); }
};
