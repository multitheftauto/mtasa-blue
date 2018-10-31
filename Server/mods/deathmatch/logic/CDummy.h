/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDummy.h
 *  PURPOSE:     Dummy entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"

class CDummy : public CElement
{
public:
    CDummy(class CGroups* pGroups, CElement* pParent);
    ~CDummy(void);
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    bool IsEntity(void) { return true; }

    void Unlink(void);

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    class CGroups* m_pGroups;
};
