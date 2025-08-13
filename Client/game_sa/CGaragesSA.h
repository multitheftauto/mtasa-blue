/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGaragesSA.h
 *  PURPOSE:     Header file for garage manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CGarages.h>
#include "CGarageSA.h"

#define CLASS_CGarages 0x96C048

class CGaragesSAInterface
{
public:
    CGarageSAInterface Garages[MAX_GARAGES];
};

class CGaragesSA : public CGarages
{
public:
    CGaragesSA(CGaragesSAInterface* pInterface);
    virtual ~CGaragesSA();

    CGarageSA* GetGarage(DWORD dwID);

    void Initialize();

private:
    CGaragesSAInterface* internalInterface;
    CGarageSA*           Garages[MAX_GARAGES];
};
