/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTrailerSA.h
 *  PURPOSE:     Header file for trailer vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CTrailer.h>
#include "CAutomobileSA.h"

class CTrailerSAInterface : public CAutomobileSAInterface
{
    // fill this
};

class CTrailerSA : public virtual CTrailer, public virtual CAutomobileSA
{
private:
public:
    CTrailerSA(CTrailerSAInterface* trailer);
    CTrailerSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2);

    virtual ~CTrailerSA() {}
};
