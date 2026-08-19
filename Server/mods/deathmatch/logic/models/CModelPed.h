/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModelPed.h
 *  PURPOSE:     Ped model class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CModel.h"

class CModelPed : public CModel
{
public:
    CModelPed(std::uint32_t modelId, std::uint32_t parentModelId = 0, CResource* resource = nullptr, bool isCustom = false);
};
