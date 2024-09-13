/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBmxSA.h
 *  PURPOSE:     Header file for bmx bike vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBmx.h>
#include "CBikeSA.h"

enum class eBmxNodes
{
    NONE = 0,
    CHASSIS,
    FORKS_FRONT,
    FORKS_REAR,
    WHEEL_FRONT,
    WHEEL_REAR,
    HANDLEBARS,
    CHAINSET,
    PEDAL_R,
    PEDAL_L,

    NUM_NODES
};

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
