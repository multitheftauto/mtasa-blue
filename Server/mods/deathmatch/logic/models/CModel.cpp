/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModel.cpp
 *  PURPOSE:     Base model class implementation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModel.h"

CModel::CModel(std::uint32_t modelId, eModelType modelType, std::uint32_t parentModelId, CResource* resource, bool isCustom)
    : m_modelId(modelId), m_parentModelId(parentModelId), m_modelType(modelType), m_isCustom(isCustom), m_resource(resource)
{
}
