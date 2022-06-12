/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelPed.h
 *  PURPOSE:     Ped model class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CModelPed;
#pragma once

#include "CModelBase.h"

class CModelPed : public CModelBase
{
public:
    CModelPed(uint32_t uiModelID) : CModelBase(eModelInfoType::PED, uiModelID){};
    ~CModelPed(){};

    virtual CModelPed* Clone(uint32_t uiModelID);
    virtual void       Unload();

private:
    eModelInfoType m_eType = eModelInfoType::PED;
};
