/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGaragesSA.h
 *  PURPOSE:     Header file for garage manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CGarages.h>
#include "CGarageSA.h"
#include <array>

class CGaragesSAInterface
{
public:
    CGarageSAInterface Garages[MAX_GARAGES];
};

class CGaragesSA : public CGarages
{
public:
    CGaragesSA(CGaragesSAInterface* pInterface);
    ~CGaragesSA() override;

    CGarageSA* GetGarage(size_t uiIndex) override;

    void Initialize() override;

private:
    CGaragesSAInterface*                m_pInterface;
    std::array<CGarageSA*, MAX_GARAGES> m_Garages;
};
