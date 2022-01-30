/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelAtomic.cpp
 *  PURPOSE:     Atomic model class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CModelAtomic::CModelAtomic(uint32_t uiModelID, char strName[22], char strTextureName[22], uint32_t uiDrawDistance, uint32_t uiFlags) : CModelBase(uiModelID)
{
    strcpy(m_modelName, strName);
    strcpy(m_strTextureName, strTextureName);
    SetDrawDistance(uiDrawDistance);
    SetFlags(uiFlags);
}

CModelAtomic::~CModelAtomic()
{

}

void CModelAtomic::Unload()
{
    return;
}

