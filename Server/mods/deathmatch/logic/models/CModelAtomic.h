/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelAtomic.h
 *  PURPOSE:     Atomic model class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

class CModelAtomic;
#pragma once

#include "CModelBase.h"

class CModelAtomic : public CModelBase
{
public:
    CModelAtomic(uint32_t uiModelID) : CModelBase(eModelInfoType::ATOMIC, uiModelID){};
    ~CModelAtomic(){};

    CModelAtomic* Clone(uint32_t uiModelID);
    void          Unload();
};
