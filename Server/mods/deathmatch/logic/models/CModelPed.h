/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelPed.h
 *  PURPOSE:     Ped model class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
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
