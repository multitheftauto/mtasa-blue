/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDummy.h
 *  PURPOSE:     Dummy entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"

class CDummy final : public CElement
{
public:
    CDummy(class CGroups* pGroups, CElement* pParent);
    ~CDummy();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    bool IsEntity() { return true; }

    void Unlink();

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    class CGroups* m_pGroups;
};
