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
    CModelPed(uint32_t uiModelID) : CModelBase(uiModelID){};
    ~CModelPed(){};

    eModelInfoType GetType() { return eModelInfoType::PED; };
    CModelPed*     Clone(uint32_t uiModelID);
    void           Unload();
};
