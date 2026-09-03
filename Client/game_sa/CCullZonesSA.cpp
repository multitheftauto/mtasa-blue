/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCullZonesSA.cpp
 *  PURPOSE:     GTA SA CULL zone relocation and editable catalog
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCullZonesSA.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <numbers>

namespace
{
    constexpr std::size_t ATTRIBUTE_ZONE_CAPACITY = 4096;
    constexpr std::size_t TUNNEL_ZONE_CAPACITY = 256;
    constexpr std::size_t MIRROR_ZONE_CAPACITY = 256;

    constexpr std::uintptr_t ORIGINAL_ATTRIBUTE_ZONES = 0xC81F50;
    constexpr std::uintptr_t ORIGINAL_TUNNEL_ZONES = 0xC81C80;
    constexpr std::uintptr_t ORIGINAL_MIRROR_ZONES = 0xC815C0;
    constexpr std::uintptr_t NUM_ATTRIBUTE_ZONES = 0xC87AC8;
    constexpr std::uintptr_t NUM_TUNNEL_ZONES = 0xC87AC0;
    constexpr std::uintptr_t NUM_MIRROR_ZONES = 0xC87AC4;
    constexpr std::uintptr_t CURRENT_FLAGS_PLAYER = 0xC87AB8;
    constexpr std::uintptr_t CURRENT_FLAGS_CAMERA = 0xC87ABC;
    constexpr std::uintptr_t MIRROR_RENDER_STATE_DIRTY = 0xC7C729;

    struct SNativeZoneDef
    {
        std::int16_t cornerX;
        std::int16_t cornerY;
        std::int16_t vector1X;
        std::int16_t vector1Y;
        std::int16_t vector2X;
        std::int16_t vector2Y;
        std::int16_t minZ;
        std::int16_t maxZ;
    };

    struct SNativeAttributeZone
    {
        SNativeZoneDef def;
        std::uint16_t  flags;
    };

    struct SNativeMirrorZone
    {
        SNativeZoneDef def;
        float          mirrorV;
        std::int8_t    normalX;
        std::int8_t    normalY;
        std::int8_t    normalZ;
        std::uint8_t   flags;
    };

    static_assert(sizeof(SNativeZoneDef) == 0x10, "Invalid GTA CULL zone definition size");
    static_assert(sizeof(SNativeAttributeZone) == 0x12, "Invalid GTA CULL attribute zone size");
    static_assert(sizeof(SNativeMirrorZone) == 0x18, "Invalid GTA CULL mirror zone size");
    static_assert(offsetof(SNativeAttributeZone, flags) == 0x10, "Invalid GTA CULL attribute flags offset");
    static_assert(offsetof(SNativeMirrorZone, mirrorV) == 0x10, "Invalid GTA CULL mirror V offset");
    static_assert(offsetof(SNativeMirrorZone, normalX) == 0x14, "Invalid GTA CULL mirror normal offset");
    static_assert(offsetof(SNativeMirrorZone, flags) == 0x17, "Invalid GTA CULL mirror flags offset");

    std::array<SNativeAttributeZone, ATTRIBUTE_ZONE_CAPACITY> g_attributeZones{};
    std::array<SNativeAttributeZone, TUNNEL_ZONE_CAPACITY>    g_tunnelZones{};
    std::array<SNativeMirrorZone, MIRROR_ZONE_CAPACITY>       g_mirrorZones{};

    template <class T>
    T& NativeGlobal(std::uintptr_t address)
    {
        return *reinterpret_cast<T*>(address);
    }

    void PatchPointer(std::uintptr_t operandAddress, const void* pointer)
    {
        MemPut<DWORD>(operandAddress, reinterpret_cast<DWORD>(pointer));
    }

    const void* Field(const void* object, std::size_t offset)
    {
        return static_cast<const std::uint8_t*>(object) + offset;
    }

    void InstallRelocation()
    {
        static bool installed = false;
        if (installed)
            return;

        // Preserve data if another startup path happened to load CULL entries before
        // the MTA game wrapper was constructed. Normally all three counts are zero here.
        const int attributeCount = std::clamp(NativeGlobal<int>(NUM_ATTRIBUTE_ZONES), 0, static_cast<int>(ATTRIBUTE_ZONE_CAPACITY));
        const int tunnelCount = std::clamp(NativeGlobal<int>(NUM_TUNNEL_ZONES), 0, static_cast<int>(TUNNEL_ZONE_CAPACITY));
        const int mirrorCount = std::clamp(NativeGlobal<int>(NUM_MIRROR_ZONES), 0, static_cast<int>(MIRROR_ZONE_CAPACITY));
        std::memcpy(g_attributeZones.data(), reinterpret_cast<const void*>(ORIGINAL_ATTRIBUTE_ZONES), attributeCount * sizeof(SNativeAttributeZone));
        std::memcpy(g_tunnelZones.data(), reinterpret_cast<const void*>(ORIGINAL_TUNNEL_ZONES), tunnelCount * sizeof(SNativeAttributeZone));
        std::memcpy(g_mirrorZones.data(), reinterpret_cast<const void*>(ORIGINAL_MIRROR_ZONES), mirrorCount * sizeof(SNativeMirrorZone));

        // GTA SA 1.0 US operands verified against the corresponding lookup and
        // loader functions. Every field operand must move with its native array.
        PatchPointer(0x72DA85, g_mirrorZones.data());               // CCullZones::FindMirrorAttributesForCoors: array base
        PatchPointer(0x72DAC8, Field(g_mirrorZones.data(), 0x00));  // CCullZones::FindMirrorAttributesForCoors: corner X
        PatchPointer(0x72DC35, Field(g_mirrorZones.data(), 0x00));  // CCullZones::AddMirrorAttributeZone: corner X
        PatchPointer(0x72DC52, Field(g_mirrorZones.data(), 0x02));  // CCullZones::AddMirrorAttributeZone: corner Y
        PatchPointer(0x72DC64, Field(g_mirrorZones.data(), 0x04));  // CCullZones::AddMirrorAttributeZone: first vector X
        PatchPointer(0x72DC74, Field(g_mirrorZones.data(), 0x06));  // CCullZones::AddMirrorAttributeZone: first vector Y
        PatchPointer(0x72DC86, Field(g_mirrorZones.data(), 0x0C));  // CCullZones::AddMirrorAttributeZone: minimum Z
        PatchPointer(0x72DC98, Field(g_mirrorZones.data(), 0x08));  // CCullZones::AddMirrorAttributeZone: second vector X
        PatchPointer(0x72DCA8, Field(g_mirrorZones.data(), 0x0A));  // CCullZones::AddMirrorAttributeZone: second vector Y
        PatchPointer(0x72DCC2, Field(g_mirrorZones.data(), 0x0E));  // CCullZones::AddMirrorAttributeZone: maximum Z
        PatchPointer(0x72DCCC, Field(g_mirrorZones.data(), 0x17));  // CCullZones::AddMirrorAttributeZone: flags
        PatchPointer(0x72DCD2, Field(g_mirrorZones.data(), 0x10));  // CCullZones::AddMirrorAttributeZone: mirror V
        PatchPointer(0x72DCE7, Field(g_mirrorZones.data(), 0x14));  // CCullZones::AddMirrorAttributeZone: normal X
        PatchPointer(0x72DCFC, Field(g_mirrorZones.data(), 0x15));  // CCullZones::AddMirrorAttributeZone: normal Y
        PatchPointer(0x72DD0F, Field(g_mirrorZones.data(), 0x16));  // CCullZones::AddMirrorAttributeZone: normal Z

        PatchPointer(0x72DA13, Field(g_tunnelZones.data(), 0x10));  // CCullZones::FindTunnelAttributesForCoors: flags
        PatchPointer(0x72DB74, Field(g_tunnelZones.data(), 0x00));  // CCullZones::AddTunnelAttributeZone: corner X
        PatchPointer(0x72DB91, Field(g_tunnelZones.data(), 0x02));  // CCullZones::AddTunnelAttributeZone: corner Y
        PatchPointer(0x72DBA3, Field(g_tunnelZones.data(), 0x04));  // CCullZones::AddTunnelAttributeZone: first vector X
        PatchPointer(0x72DBB3, Field(g_tunnelZones.data(), 0x06));  // CCullZones::AddTunnelAttributeZone: first vector Y
        PatchPointer(0x72DBC5, Field(g_tunnelZones.data(), 0x0C));  // CCullZones::AddTunnelAttributeZone: minimum Z
        PatchPointer(0x72DBD7, Field(g_tunnelZones.data(), 0x08));  // CCullZones::AddTunnelAttributeZone: second vector X
        PatchPointer(0x72DBE7, Field(g_tunnelZones.data(), 0x0A));  // CCullZones::AddTunnelAttributeZone: second vector Y
        PatchPointer(0x72DBF3, Field(g_tunnelZones.data(), 0x0E));  // CCullZones::AddTunnelAttributeZone: maximum Z
        PatchPointer(0x72DC07, Field(g_tunnelZones.data(), 0x10));  // CCullZones::AddTunnelAttributeZone: flags

        PatchPointer(0x72D993, Field(g_attributeZones.data(), 0x10));  // CCullZones lookup at 0x72D970: flags
        PatchPointer(0x72DAFA, g_attributeZones.data());               // CCullZones lookup at 0x72DAD0: array base
        PatchPointer(0x72DB42, Field(g_attributeZones.data(), 0x00));  // CCullZones lookup at 0x72DAD0: corner X
        PatchPointer(0x72DFDA, Field(g_attributeZones.data(), 0x00));  // CCullZones loader at 0x72DF70: corner X
        PatchPointer(0x72DFF7, Field(g_attributeZones.data(), 0x02));  // CCullZones loader at 0x72DF70: corner Y
        PatchPointer(0x72E009, Field(g_attributeZones.data(), 0x04));  // CCullZones loader at 0x72DF70: first vector X
        PatchPointer(0x72E019, Field(g_attributeZones.data(), 0x06));  // CCullZones loader at 0x72DF70: first vector Y
        PatchPointer(0x72E02B, Field(g_attributeZones.data(), 0x0C));  // CCullZones loader at 0x72DF70: minimum Z
        PatchPointer(0x72E03D, Field(g_attributeZones.data(), 0x08));  // CCullZones loader at 0x72DF70: second vector X
        PatchPointer(0x72E04D, Field(g_attributeZones.data(), 0x0A));  // CCullZones loader at 0x72DF70: second vector Y
        PatchPointer(0x72E05A, Field(g_attributeZones.data(), 0x0E));  // CCullZones loader at 0x72DF70: maximum Z
        PatchPointer(0x72E068, Field(g_attributeZones.data(), 0x10));  // CCullZones loader at 0x72DF70: flags

        installed = true;
    }

    bool ToInt16(float value, std::int16_t& output)
    {
        if (!std::isfinite(value) || value < -32768.0f || value > 32767.0f)
            return false;
        output = static_cast<std::int16_t>(value);
        return true;
    }

    bool BuildNativeDef(const SCullZoneDefinition& input, SNativeZoneDef& output)
    {
        if (!std::isfinite(input.centerX) || !std::isfinite(input.centerY) || !std::isfinite(input.centerZ) || !std::isfinite(input.width) ||
            !std::isfinite(input.depth) || !std::isfinite(input.height) || !std::isfinite(input.rotationDegrees) || input.width <= 0.0f ||
            input.depth <= 0.0f || input.height <= 0.0f)
            return false;

        const float radians = input.rotationDegrees * std::numbers::pi_v<float> / 180.0f;
        const float halfWidth = input.width * 0.5f;
        const float halfDepth = input.depth * 0.5f;
        const float vector1X = std::cos(radians) * halfWidth;
        const float vector1Y = std::sin(radians) * halfWidth;
        const float vector2X = -std::sin(radians) * halfDepth;
        const float vector2Y = std::cos(radians) * halfDepth;

        return ToInt16(input.centerX - vector1X - vector2X, output.cornerX) && ToInt16(input.centerY - vector1Y - vector2Y, output.cornerY) &&
               ToInt16(vector1X * 2.0f, output.vector1X) && ToInt16(vector1Y * 2.0f, output.vector1Y) && ToInt16(vector2X * 2.0f, output.vector2X) &&
               ToInt16(vector2Y * 2.0f, output.vector2Y) && ToInt16(input.centerZ - input.height * 0.5f, output.minZ) &&
               ToInt16(input.centerZ + input.height * 0.5f, output.maxZ) && output.minZ < output.maxZ && (output.vector1X != 0 || output.vector1Y != 0) &&
               (output.vector2X != 0 || output.vector2Y != 0);
    }

    bool BuildNativeMirrorZone(const SCullZoneDefinition& input, SNativeMirrorZone& output)
    {
        if (input.flags > 0xFF || !BuildNativeDef(input, output.def) || !std::isfinite(input.mirrorV) || !std::isfinite(input.mirrorNormalX) ||
            !std::isfinite(input.mirrorNormalY) || !std::isfinite(input.mirrorNormalZ))
            return false;

        // GTA constructs a reflection matrix directly from this normal. A
        // non-unit vector can make that matrix singular instead of reflecting.
        const float normalLength = std::hypot(input.mirrorNormalX, input.mirrorNormalY, input.mirrorNormalZ);
        if (std::abs(normalLength - 1.0f) > 0.01f)
            return false;

        output.flags = static_cast<std::uint8_t>(input.flags);
        output.mirrorV = input.mirrorV;
        // Native IPL normals are signed percentages. Unit diagonals therefore
        // quantize to roughly 0.99 length, matching GTA's original precision.
        output.normalX = static_cast<std::int8_t>(input.mirrorNormalX * 100.0f);
        output.normalY = static_cast<std::int8_t>(input.mirrorNormalY * 100.0f);
        output.normalZ = static_cast<std::int8_t>(input.mirrorNormalZ * 100.0f);
        return true;
    }

    SCullZoneDefinition ToDefinition(ECullZoneType type, const SNativeZoneDef& native, std::uint16_t flags)
    {
        SCullZoneDefinition output;
        output.type = type;
        output.centerX = native.cornerX + (native.vector1X + native.vector2X) * 0.5f;
        output.centerY = native.cornerY + (native.vector1Y + native.vector2Y) * 0.5f;
        output.centerZ = (native.minZ + native.maxZ) * 0.5f;
        output.width = std::hypot(static_cast<float>(native.vector1X), static_cast<float>(native.vector1Y));
        output.depth = std::hypot(static_cast<float>(native.vector2X), static_cast<float>(native.vector2Y));
        output.height = static_cast<float>(native.maxZ - native.minZ);
        output.rotationDegrees = std::atan2(static_cast<float>(native.vector1Y), static_cast<float>(native.vector1X)) * 180.0f / std::numbers::pi_v<float>;
        output.flags = flags;
        return output;
    }

    bool IsPointWithin(const SNativeZoneDef& zone, const CVector& point)
    {
        if (zone.minZ >= point.fZ || zone.maxZ <= point.fZ)
            return false;

        const float deltaX = point.fX - zone.cornerX;
        const float deltaY = point.fY - zone.cornerY;
        const float vector1X = zone.vector1X;
        const float vector1Y = zone.vector1Y;
        const float vector2X = zone.vector2X;
        const float vector2Y = zone.vector2Y;
        const float projection1 = vector1X * deltaX + vector1Y * deltaY;
        const float projection2 = vector2X * deltaX + vector2Y * deltaY;
        const float length1Squared = vector1X * vector1X + vector1Y * vector1Y;
        const float length2Squared = vector2X * vector2X + vector2Y * vector2Y;
        return projection1 >= 0.0f && projection1 <= length1Squared && projection2 >= 0.0f && projection2 <= length2Squared;
    }

    std::uint16_t FindAttributeFlags(const CVector& point)
    {
        const int     count = std::clamp(NativeGlobal<int>(NUM_ATTRIBUTE_ZONES), 0, static_cast<int>(ATTRIBUTE_ZONE_CAPACITY));
        std::uint16_t flags = 0;
        for (int i = 0; i < count; ++i)
        {
            if (IsPointWithin(g_attributeZones[i].def, point))
                flags |= g_attributeZones[i].flags;
        }
        return flags;
    }
}  // namespace

struct CCullZonesSA::SZoneEntry
{
    std::uint32_t        id{};
    ECullZoneType        type{ECullZoneType::ATTRIBUTE};
    ECullZoneType        originalType{ECullZoneType::ATTRIBUTE};
    SNativeAttributeZone attribute{};
    SNativeMirrorZone    mirror{};
    SNativeAttributeZone originalAttribute{};
    SNativeMirrorZone    originalMirror{};
    const void*          owner{};
    bool                 enabled{true};
    bool                 original{false};
};

CCullZonesSA::CCullZonesSA()
{
    InstallRelocation();
}

CCullZonesSA::~CCullZonesSA() = default;

void CCullZonesSA::EnsureCatalog()
{
    if (m_catalogReady)
        return;

    const int attributeCount = std::clamp(NativeGlobal<int>(NUM_ATTRIBUTE_ZONES), 0, static_cast<int>(ATTRIBUTE_ZONE_CAPACITY));
    const int tunnelCount = std::clamp(NativeGlobal<int>(NUM_TUNNEL_ZONES), 0, static_cast<int>(TUNNEL_ZONE_CAPACITY));
    const int mirrorCount = std::clamp(std::max(NativeGlobal<int>(NUM_MIRROR_ZONES), m_savedMirrorCount), 0, static_cast<int>(MIRROR_ZONE_CAPACITY));
    OutputReleaseLine(SString("[CULL] adopted native zones: attribute=%d tunnel=%d mirror=%d", attributeCount, tunnelCount, mirrorCount));
    m_entries.reserve(attributeCount + tunnelCount + mirrorCount + 64);

    const auto addAttributeEntries = [this](ECullZoneType type, const SNativeAttributeZone* zones, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            SZoneEntry entry;
            entry.id = m_nextId++;
            entry.type = type;
            entry.originalType = type;
            entry.attribute = zones[i];
            entry.originalAttribute = zones[i];
            entry.original = true;
            m_entries.push_back(entry);
        }
    };

    addAttributeEntries(ECullZoneType::ATTRIBUTE, g_attributeZones.data(), attributeCount);
    addAttributeEntries(ECullZoneType::TUNNEL, g_tunnelZones.data(), tunnelCount);
    for (int i = 0; i < mirrorCount; ++i)
    {
        SZoneEntry entry;
        entry.id = m_nextId++;
        entry.type = ECullZoneType::MIRROR;
        entry.originalType = ECullZoneType::MIRROR;
        entry.mirror = g_mirrorZones[i];
        entry.originalMirror = g_mirrorZones[i];
        entry.original = true;
        m_entries.push_back(entry);
    }

    m_catalogReady = true;
    m_savedMirrorCount = -1;
    if (!m_mirrorsEnabled)
        RebuildNativeArrays();
}

std::size_t CCullZonesSA::GetCount()
{
    EnsureCatalog();
    return m_entries.size();
}

bool CCullZonesSA::GetByIndex(std::size_t index, SCullZoneInfo& outInfo)
{
    EnsureCatalog();
    if (index >= m_entries.size())
        return false;

    const SZoneEntry&   entry = m_entries[index];
    SCullZoneDefinition definition;
    if (entry.type == ECullZoneType::MIRROR)
    {
        definition = ToDefinition(entry.type, entry.mirror.def, entry.mirror.flags);
        definition.mirrorV = entry.mirror.mirrorV;
        definition.mirrorNormalX = entry.mirror.normalX / 100.0f;
        definition.mirrorNormalY = entry.mirror.normalY / 100.0f;
        definition.mirrorNormalZ = entry.mirror.normalZ / 100.0f;
    }
    else
        definition = ToDefinition(entry.type, entry.attribute.def, entry.attribute.flags);

    static_cast<SCullZoneDefinition&>(outInfo) = definition;
    outInfo.id = entry.id;
    outInfo.enabled = entry.enabled;
    outInfo.original = entry.original;
    return true;
}

CCullZonesSA::SZoneEntry* CCullZonesSA::Find(std::uint32_t id)
{
    auto it = std::find_if(m_entries.begin(), m_entries.end(), [id](const SZoneEntry& entry) { return entry.id == id; });
    return it == m_entries.end() ? nullptr : &*it;
}

bool CCullZonesSA::CanClaim(SZoneEntry& entry, const void* owner)
{
    if (!owner || (!entry.original && entry.owner != owner) || (entry.original && entry.owner && entry.owner != owner))
        return false;
    entry.owner = owner;
    return true;
}

bool CCullZonesSA::HasCapacityFor(ECullZoneType type, const SZoneEntry* replacedEntry) const
{
    std::size_t count = 0;
    for (const SZoneEntry& entry : m_entries)
    {
        if (&entry == replacedEntry)
            continue;

        // A modified original must retain capacity in its vanilla array. This
        // guarantees that resource cleanup can always restore it, even when a
        // different resource creates zones while the original is disabled.
        if (entry.enabled && entry.type == type)
            ++count;
        else if (entry.original && entry.originalType == type)
            ++count;
    }

    const std::size_t capacity = type == ECullZoneType::ATTRIBUTE ? ATTRIBUTE_ZONE_CAPACITY
                                 : type == ECullZoneType::TUNNEL  ? TUNNEL_ZONE_CAPACITY
                                                                  : MIRROR_ZONE_CAPACITY;
    return count < capacity;
}

std::uint32_t CCullZonesSA::Create(const SCullZoneDefinition& definition, const void* owner)
{
    EnsureCatalog();
    if (!owner || !HasCapacityFor(definition.type))
        return 0;

    SZoneEntry entry;
    entry.id = m_nextId++;
    entry.type = definition.type;
    entry.owner = owner;
    if (definition.type == ECullZoneType::MIRROR)
    {
        if (!BuildNativeMirrorZone(definition, entry.mirror))
            return 0;
    }
    else
    {
        if (!BuildNativeDef(definition, entry.attribute.def))
            return 0;
        entry.attribute.flags = definition.flags;
    }

    m_entries.push_back(entry);
    RebuildNativeArrays();
    return entry.id;
}

bool CCullZonesSA::Set(std::uint32_t id, const SCullZoneDefinition& definition, const void* owner)
{
    EnsureCatalog();
    SZoneEntry* entry = Find(id);
    if (!entry || !owner || (!entry->original && entry->owner != owner) || (entry->original && entry->owner && entry->owner != owner) ||
        (!entry->enabled && !HasCapacityFor(definition.type, entry)) ||
        (entry->enabled && entry->type != definition.type && !HasCapacityFor(definition.type, entry)))
        return false;

    SNativeAttributeZone attribute{};
    SNativeMirrorZone    mirror{};
    if (definition.type == ECullZoneType::MIRROR)
    {
        if (!BuildNativeMirrorZone(definition, mirror))
            return false;
    }
    else
    {
        if (!BuildNativeDef(definition, attribute.def))
            return false;
        attribute.flags = definition.flags;
    }

    entry->owner = owner;
    entry->type = definition.type;
    entry->attribute = attribute;
    entry->mirror = mirror;
    entry->enabled = true;
    RebuildNativeArrays();
    return true;
}

bool CCullZonesSA::SetEnabled(std::uint32_t id, bool enabled, const void* owner)
{
    EnsureCatalog();
    SZoneEntry* entry = Find(id);
    if (!entry || !owner || (!entry->original && entry->owner != owner) || (entry->original && entry->owner && entry->owner != owner) ||
        (enabled && !entry->enabled && !HasCapacityFor(entry->type, entry)))
        return false;
    entry->owner = owner;
    entry->enabled = enabled;
    RebuildNativeArrays();
    return true;
}

bool CCullZonesSA::Remove(std::uint32_t id, const void* owner)
{
    EnsureCatalog();
    auto it = std::find_if(m_entries.begin(), m_entries.end(), [id](const SZoneEntry& entry) { return entry.id == id; });
    if (it == m_entries.end() || !CanClaim(*it, owner))
        return false;

    if (it->original)
        it->enabled = false;
    else
        m_entries.erase(it);
    RebuildNativeArrays();
    return true;
}

bool CCullZonesSA::Restore(std::uint32_t id, const void* owner)
{
    EnsureCatalog();
    SZoneEntry* entry = Find(id);
    if (!entry || !entry->original || entry->owner != owner || !HasCapacityFor(entry->originalType, entry))
        return false;

    entry->type = entry->originalType;
    entry->attribute = entry->originalAttribute;
    entry->mirror = entry->originalMirror;
    entry->enabled = true;
    entry->owner = nullptr;
    RebuildNativeArrays();
    return true;
}

void CCullZonesSA::RemoveChangesByOwner(const void* owner)
{
    if (!owner || !m_catalogReady)
        return;

    bool changed = false;
    for (SZoneEntry& entry : m_entries)
    {
        if (entry.original && entry.owner == owner)
        {
            entry.type = entry.originalType;
            entry.attribute = entry.originalAttribute;
            entry.mirror = entry.originalMirror;
            entry.enabled = true;
            entry.owner = nullptr;
            changed = true;
        }
    }

    const auto oldSize = m_entries.size();
    m_entries.erase(std::remove_if(m_entries.begin(), m_entries.end(), [owner](const SZoneEntry& entry) { return !entry.original && entry.owner == owner; }),
                    m_entries.end());
    if (changed || oldSize != m_entries.size())
        RebuildNativeArrays();
}

void CCullZonesSA::SetMirrorsEnabled(bool enabled)
{
    m_mirrorsEnabled = enabled;
    if (!m_catalogReady)
    {
        int& nativeCount = NativeGlobal<int>(NUM_MIRROR_ZONES);
        if (!enabled)
        {
            // Core can toggle mirrors before GTA loads CULL IPL sections. Preserve
            // any count already loaded without freezing an empty Lua catalog.
            m_savedMirrorCount = std::max(m_savedMirrorCount, nativeCount);
            nativeCount = 0;
        }
        else if (m_savedMirrorCount >= 0)
        {
            nativeCount = std::max(nativeCount, m_savedMirrorCount);
            m_savedMirrorCount = -1;
            NativeGlobal<std::uint8_t>(MIRROR_RENDER_STATE_DIRTY) = true;
        }
        return;
    }

    RebuildNativeArrays();
}

void CCullZonesSA::UpdateCurrentFlags(const CVector& playerPosition, const CVector& cameraPosition)
{
    NativeGlobal<int>(CURRENT_FLAGS_PLAYER) = FindAttributeFlags(playerPosition);
    NativeGlobal<int>(CURRENT_FLAGS_CAMERA) = FindAttributeFlags(cameraPosition);
}

void CCullZonesSA::RebuildNativeArrays()
{
    std::size_t attributeCount = 0;
    std::size_t tunnelCount = 0;
    std::size_t mirrorCount = 0;
    for (const SZoneEntry& entry : m_entries)
    {
        if (!entry.enabled)
            continue;

        switch (entry.type)
        {
            case ECullZoneType::ATTRIBUTE:
                if (attributeCount < g_attributeZones.size())
                    g_attributeZones[attributeCount++] = entry.attribute;
                else
                    assert(false && "CULL attribute capacity invariant violated");
                break;
            case ECullZoneType::TUNNEL:
                if (tunnelCount < g_tunnelZones.size())
                    g_tunnelZones[tunnelCount++] = entry.attribute;
                else
                    assert(false && "CULL tunnel capacity invariant violated");
                break;
            case ECullZoneType::MIRROR:
                if (mirrorCount < g_mirrorZones.size())
                    g_mirrorZones[mirrorCount++] = entry.mirror;
                else
                    assert(false && "CULL mirror capacity invariant violated");
                break;
        }
    }

    NativeGlobal<int>(NUM_ATTRIBUTE_ZONES) = static_cast<int>(attributeCount);
    NativeGlobal<int>(NUM_TUNNEL_ZONES) = static_cast<int>(tunnelCount);
    NativeGlobal<int>(NUM_MIRROR_ZONES) = m_mirrorsEnabled ? static_cast<int>(mirrorCount) : 0;

    // Prevent removed attributes from lingering until the next attribute-mask
    // refresh, and ask the mirror renderer to recreate its buffers.
    NativeGlobal<int>(CURRENT_FLAGS_PLAYER) = 0;
    NativeGlobal<int>(CURRENT_FLAGS_CAMERA) = 0;
    NativeGlobal<std::uint8_t>(MIRROR_RENDER_STATE_DIRTY) = true;
}
