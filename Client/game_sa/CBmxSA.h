/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CBmxSA.h
 *  PURPOSE:     Header file for bmx bike vehicle entity class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBmx.h>
#include "CBikeSA.h"

class CBmxSAInterface : public CBikeSAInterface
{
    // fill this
};

class CBmxSA final : public virtual CBmx, public virtual CBikeSA
{
public:
    CBmxSA(CBmxSAInterface* pInterface);

    CBmxSAInterface* GetBmxInterface() { return reinterpret_cast<CBmxSAInterface*>(GetInterface()); }
};
