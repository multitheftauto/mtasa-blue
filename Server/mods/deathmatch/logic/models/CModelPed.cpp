/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModelPed.cpp
 *  PURPOSE:     Ped model class implementation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelPed.h"

CModelPed::CModelPed(std::uint32_t modelId, std::uint32_t parentModelId, CResource* resource, bool isCustom)
    : CModel(modelId, eModelType::PED, parentModelId, resource, isCustom)
{
}
