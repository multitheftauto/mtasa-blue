/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModelObject.cpp
 *  PURPOSE:     Object model class implementation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelObject.h"

CModelObject::CModelObject(std::uint32_t modelId, std::uint32_t parentModelId, CResource* resource, bool isCustom)
    : CModel(modelId, eModelType::OBJECT, parentModelId, resource, isCustom)
{
}
