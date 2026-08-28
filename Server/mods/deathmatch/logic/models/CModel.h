/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModel.h
 *  PURPOSE:     Base model class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

class CResource;

enum class eModelType : std::uint8_t
{
    UNKNOWN = 0,
    VEHICLE,
    PED,
    OBJECT
};

class CModel
{
public:
    CModel(std::uint32_t modelId, eModelType modelType, std::uint32_t parentModelId = 0, CResource* resource = nullptr, bool isCustom = false);
    virtual ~CModel() = default;

    std::uint32_t GetModelId() const noexcept { return m_modelId; }
    std::uint32_t GetParentModelId() const noexcept { return m_parentModelId; }
    eModelType    GetModelType() const noexcept { return m_modelType; }
    bool          IsCustom() const noexcept { return m_isCustom; }
    CResource*    GetResource() const noexcept { return m_resource; }

    void               SetParentModelId(std::uint32_t parentModelId) noexcept { m_parentModelId = parentModelId; }
    void               SetResource(CResource* resource) noexcept { m_resource = resource; }
    void               SetCustom(bool isCustom) noexcept { m_isCustom = isCustom; }
    const std::string& GetName() const noexcept { return m_name; }
    void               SetName(std::string name) { m_name = std::move(name); }

protected:
    std::uint32_t m_modelId{0};
    std::uint32_t m_parentModelId{0};
    eModelType    m_modelType{eModelType::UNKNOWN};
    bool          m_isCustom{false};
    CResource*    m_resource{nullptr};
    std::string   m_name;
};
