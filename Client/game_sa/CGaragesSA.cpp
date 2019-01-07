/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CGaragesSA.cpp
 *  PURPOSE:     Garage manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// Disables pay and sprays (making sounds when closing) / https://wiki.multitheftauto.com/wiki/Garage
static const std::array<bool, MAX_GARAGES> aDisabledGarages =
{
    false, true, false, false, false, false, false, false, true, false, false, true, true, false, false, false, false, false, false, true, false,
    false, false, true, false, false, true, true, false, true, false, false, true, false, false, false, true, false, false, false, true, true,
    false, false, false, false, false, true, false, false,
};

CGaragesSA::CGaragesSA(CGaragesSAInterface* pInterface) : m_pInterface(pInterface)
{
    for (size_t i = 0; i < m_Garages.size(); ++i)
    {
        auto pGarageInterface = reinterpret_cast<CGarageSAInterface*>(CLASS_CGarages + 216 * i);
        m_Garages[i] = new CGarageSA(pGarageInterface);
    }
}

CGaragesSA::~CGaragesSA()
{
    for (CGarageSA* pGarage : m_Garages)
        delete pGarage;
}

void CGaragesSA::Initialize()
{
    for (size_t i = 0; i < aDisabledGarages.size(); ++i)
    {
        if (aDisabledGarages[i])
            m_Garages[i]->SetType(1);
    }
}

CGarageSA* CGaragesSA::GetGarage(size_t uiIndex)
{
    if (uiIndex >= m_Garages.size())
        return nullptr;

    // Check if garage is disabled
    if (aDisabledGarages[uiIndex])
        return nullptr;

    return m_Garages[uiIndex];
}
