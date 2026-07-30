/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     GTA SA CULL zone relocation and editable catalog
 *
 *****************************************************************************/

#pragma once

#include <game/CWorld.h>
#include <cstdint>
#include <vector>

class CCullZonesSA
{
public:
    CCullZonesSA();
    ~CCullZonesSA();

    std::size_t   GetCount();
    bool          GetByIndex(std::size_t index, SCullZoneInfo& outInfo);
    std::uint32_t Create(const SCullZoneDefinition& definition, const void* owner);
    bool          Set(std::uint32_t id, const SCullZoneDefinition& definition, const void* owner);
    bool          SetEnabled(std::uint32_t id, bool enabled, const void* owner);
    bool          Remove(std::uint32_t id, const void* owner);
    bool          Restore(std::uint32_t id, const void* owner);
    void          RemoveChangesByOwner(const void* owner);
    void          SetMirrorsEnabled(bool enabled);
    void          UpdateCurrentFlags(const CVector& playerPosition, const CVector& cameraPosition);

private:
    struct SZoneEntry;

    void        EnsureCatalog();
    void        RebuildNativeArrays();
    SZoneEntry* Find(std::uint32_t id);
    bool        CanClaim(SZoneEntry& entry, const void* owner);
    bool        HasCapacityFor(ECullZoneType type, const SZoneEntry* replacedEntry = nullptr) const;

    bool                    m_catalogReady{false};
    bool                    m_mirrorsEnabled{true};
    int                     m_savedMirrorCount{-1};
    std::uint32_t           m_nextId{1};
    std::vector<SZoneEntry> m_entries;
};
