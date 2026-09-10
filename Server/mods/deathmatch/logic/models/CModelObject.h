/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModelObject.h
 *  PURPOSE:     Object model class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CModel.h"

class CModelObject : public CModel
{
public:
    CModelObject(std::uint32_t modelId, std::uint32_t parentModelId = 0, CResource* resource = nullptr, bool isCustom = false);
};
