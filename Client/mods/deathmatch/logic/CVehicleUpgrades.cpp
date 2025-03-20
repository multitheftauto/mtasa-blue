/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CVehicleUpgrades.cpp
 *  PURPOSE:     Vehicle upgrades class
 *
 *****************************************************************************/

#include <StdInc.h>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cstddef>

#define NUM_MODELS 212
#define NUM_SLOTS 17

char szUpgradeNameEmpty[] = "";

struct SUpgradeName
{
    const char* const szName;
};

static const SFixedArray<SUpgradeName, 17> UpgradeNames = {{{"Hood"},
                                                            {"Vent"},
                                                            {"Spoiler"},
                                                            {"Sideskirt"},
                                                            {"Front Bullbars"},
                                                            {"Rear Bullbars"},
                                                            {"Headlights"},
                                                            {"Roof"},
                                                            {"Nitro"},
                                                            {"Hydraulics"},
                                                            {"Stereo"},
                                                            {"Unknown"},
                                                            {"Wheels"},
                                                            {"Exhaust"},
                                                            {"Front Bumper"},
                                                            {"Rear Bumper"},
                                                            {"Misc"}}};

using Slots = std::array<std::uint32_t, 7>;
static const std::array<Slots, NUM_MODELS> slots = {
    {
        {0x33c2700, 0x0, 0xfffe00, 0x7},
        {0x29be7ff, 0x14060000, 0xfffe00, 0x7, 0x400dcc00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2bbe78f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2bdc70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x3bbe78f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x28fc78f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x0, 0x0, 0x400000},
        {0x2b1c74f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2a9c73f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2bdc70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x23a0780, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x0, 0x0, 0x400000},
        {0x2a9c77f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2fbe7cf, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x283e78f, 0x14060000, 0xfffe00, 0x7, 0x400fcc00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {},
        {0x0, 0x0, 0x400000},
        {0x0, 0x0, 0x400000},
        {},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {},
        {},
        {},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x281c70f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000000, 0x0, 0x7ffe00, 0x7},
        {0x0, 0x0, 0x400000},
        {0x0, 0x0, 0x400000},
        {0x0, 0x0, 0x400000},
        {0x2000000, 0x0, 0x7ffe00, 0x7},
        {0x0, 0x0, 0x400000},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x0, 0x0, 0x400000},
        {0x0, 0x0, 0x400000},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {},
        {},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000000, 0x0, 0x7ffe00, 0x7},
        {0x23e07c0, 0x0, 0xfffe00, 0x7},
        {0x2701730, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x0, 0x0, 0x400000},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x0, 0x0, 0x400000},
        {0x0, 0x0, 0x400000},
        {0x39de77f, 0x14060000, 0xfffe00, 0x7, 0x400c0c00, 0x1c},
        {0x2000700, 0x0, 0xfffe00, 0x7},
        {0x2000700, 0x0, 0xfffe00, 0x7}
    }
};

CVehicleUpgrades::CVehicleUpgrades(CClientVehicle* pVehicle)
{
    m_pVehicle = pVehicle;
    memset(&m_SlotStates[0], 0, sizeof(m_SlotStates));
    m_usLastLocalAddNitroType = 0;
}

bool CVehicleUpgrades::IsUpgrade(unsigned short usModel)
{
    return (usModel >= 1000 && usModel <= 1193);
}

bool CVehicleUpgrades::IsUpgradeCompatible(const std::uint16_t upgrade)
{
    eClientVehicleType type = m_pVehicle->GetVehicleType();

    if (type == CLIENTVEHICLE_TRAIN || type == CLIENTVEHICLE_BOAT)
    {
        return false;
    }

    if (upgrade == 1086)
    {
        return true;
    }

    if (type == CLIENTVEHICLE_BIKE || type == CLIENTVEHICLE_BMX || type == CLIENTVEHICLE_HELI)
    {
        return false;
    }

    std::uint16_t model = m_pVehicle->GetModel();
    auto* info = g_pGame->GetModelInfo(model);

    if (info && info->GetParentID() != 0)
    {
        model = static_cast<std::uint16_t>(info->GetParentID());
    }

    try
    {
        const std::size_t id = model - 400;
        const Slots& array = slots.at(id);

        const std::size_t selected = upgrade - 1000;
        const std::size_t index = selected / 32;

        const std::uint32_t slot = array.at(index);
        const std::size_t bit = selected - (index * 32);

        return slot & (1 << bit);
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

bool CVehicleUpgrades::GetSlotFromUpgrade(unsigned short us, unsigned char& ucSlot)
{
    if (us == 1011 || us == 1012 || us == 1111 || us == 1112 || us == 1142 || /* bonet */
        us == 1143 || us == 1144 || us == 1145)
    {
        ucSlot = 0;
        return true;
    }
    if (us == 1004 || us == 1005 || us == 1142 || us == 1143 || us == 1144 || us == 1145) /* vent */
    {
        ucSlot = 1;
        return true;
    }
    if (us == 1000 || us == 1001 || us == 1002 || us == 1003 || us == 1014 || /* spoiler */
        us == 1015 || us == 1016 || us == 1023 || us == 1049 || us == 1050 || us == 1058 || us == 1060 || us == 1138 || us == 1139 || us == 1146 ||
        us == 1147 || us == 1158 || us == 1162 || us == 1163 || us == 1164)
    {
        ucSlot = 2;
        return true;
    }
    if (us == 1007 || us == 1017 || us == 1026 || us == 1027 || us == 1030 || /* sideskirt */
        us == 1031 || us == 1036 || us == 1039 || us == 1040 || us == 1041 || us == 1042 || us == 1047 || us == 1048 || us == 1051 || us == 1052 ||
        us == 1056 || us == 1057 || us == 1062 || us == 1063 || us == 1069 || us == 1070 || us == 1071 || us == 1072 || us == 1090 || us == 1093 ||
        us == 1094 || us == 1095 || us == 1099 || us == 1101 || us == 1102 || us == 1106 || us == 1107 || us == 1108 || us == 1118 || us == 1119 ||
        us == 1120 || us == 1121 || us == 1122 || us == 1124 || us == 1133 || us == 1134 || us == 1137)
    {
        ucSlot = 3;
        return true;
    }
    if (us == 1115 || us == 1116)            // front bullbars
    {
        ucSlot = 4;
        return true;
    }
    if (us == 1109 || us == 1110)            // rear bullbars
    {
        ucSlot = 5;
        return true;
    }
    if (us == 1013 || us == 1024)            // lamps
    {
        ucSlot = 6;
        return true;
    }
    if (us == 1006 || us == 1032 || us == 1033 || us == 1035 || us == 1038 || /* roof */
        us == 1053 || us == 1054 || us == 1055 || us == 1061 || us == 1067 || us == 1068 || us == 1088 || us == 1091 || us == 1103 || us == 1128 ||
        us == 1130 || us == 1131)
    {
        ucSlot = 7;
        return true;
    }
    if (us == 1008 || us == 1009 || us == 1010)            // nitro
    {
        ucSlot = 8;
        return true;
    }
    if (us == 1087)            // hydraulics
    {
        ucSlot = 9;
        return true;
    }
    if (us == 1086)            // stereo
    {
        ucSlot = 10;
        return true;
    }
    if (us == 1025 || us == 1073 || us == 1074 || us == 1075 || us == 1076 || /* wheels */
        us == 1077 || us == 1078 || us == 1079 || us == 1080 || us == 1081 || us == 1082 || us == 1083 || us == 1084 || us == 1085 || us == 1096 ||
        us == 1097 || us == 1098)
    {
        ucSlot = 12;
        return true;
    }
    if (us == 1018 || us == 1019 || us == 1020 || us == 1021 || us == 1022 || /* exhaust */
        us == 1028 || us == 1029 || us == 1034 || us == 1037 || us == 1043 || us == 1044 || us == 1045 || us == 1046 || us == 1059 || us == 1064 ||
        us == 1065 || us == 1066 || us == 1089 || us == 1092 || us == 1104 || us == 1105 || us == 1113 || us == 1114 || us == 1126 || us == 1127 ||
        us == 1129 || us == 1132 || us == 1135 || us == 1136)
    {
        ucSlot = 13;
        return true;
    }
    if (us == 1117 || us == 1152 || us == 1153 || us == 1155 || us == 1157 || /* front bumper */
        us == 1160 || us == 1165 || us == 1166 || us == 1169 || us == 1170 || us == 1171 || us == 1172 || us == 1173 || us == 1174 || us == 1175 ||
        us == 1179 || us == 1181 || us == 1182 || us == 1185 || us == 1188 || us == 1189 || us == 1190 || us == 1191)
    {
        ucSlot = 14;
        return true;
    }
    if (us == 1140 || us == 1141 || us == 1148 || us == 1149 || us == 1150 || /* rear bumper */
        us == 1151 || us == 1154 || us == 1156 || us == 1159 || us == 1161 || us == 1167 || us == 1168 || us == 1176 || us == 1177 || us == 1178 ||
        us == 1180 || us == 1183 || us == 1184 || us == 1186 || us == 1187 || us == 1192 || us == 1193)
    {
        ucSlot = 15;
        return true;
    }

    if (us == 1100 || us == 1123 || us == 1125) /* misc */
    {
        ucSlot = 16;
        return true;
    }

    return false;
}

bool CVehicleUpgrades::AddUpgrade(unsigned short usUpgrade, bool bAddedLocally)
{
    if (m_pVehicle)
    {
        // If its a compatible upgrade
        if (IsUpgradeCompatible(usUpgrade))
        {
            // If upgrade is nitro, and the same one was added locally less than a 2 seconds ago, don't add
            if (usUpgrade == VEHICLEUPGRADE_NITRO_5X || usUpgrade == VEHICLEUPGRADE_NITRO_2X || usUpgrade == VEHICLEUPGRADE_NITRO_10X)
            {
                if (bAddedLocally)
                {
                    m_usLastLocalAddNitroType = usUpgrade;
                    m_lastLocalAddNitroTimer.Reset();
                }
                else
                {
                    if (HasUpgrade(usUpgrade) && usUpgrade == m_usLastLocalAddNitroType)
                    {
                        if (m_lastLocalAddNitroTimer.Get() < 2000)
                            return true;
                    }
                }
            }

            ForceAddUpgrade(usUpgrade);

            return true;
        }
    }
    return false;
}

void CVehicleUpgrades::AddAllUpgrades()
{
    if (m_pVehicle)
    {
        unsigned short usUpgrade = 1000;
        for (; usUpgrade <= 1193; usUpgrade++)
        {
            if (IsUpgradeCompatible(usUpgrade))
            {
                ForceAddUpgrade(usUpgrade);
            }
        }
    }
}

void CVehicleUpgrades::ForceAddUpgrade(unsigned short usUpgrade)
{
    unsigned char ucSlot;
    if (GetSlotFromUpgrade(usUpgrade, ucSlot))
    {
        CVehicle* pVehicle = m_pVehicle->GetGameVehicle();
        if (pVehicle)
        {
            // Grab the upgrade model
            CModelInfo* pModelInfo = g_pGame->GetModelInfo(usUpgrade);
            if (pModelInfo)
            {
                if (!g_pGame->IsASyncLoadingEnabled() || !pModelInfo->IsLoaded())
                {
                    // Request and load now
                    pModelInfo->Request(BLOCKING, "CVehicleUpgrades::ForceAddUpgrade");
                }
                // Add the upgrade
                pVehicle->AddVehicleUpgrade(usUpgrade);
            }
        }

        // Add it to the slot
        m_SlotStates[ucSlot] = usUpgrade;

        // Reset wheel scale if it is a wheel upgrade
        if (ucSlot == 12)
            m_pVehicle->ResetWheelScale();
    }
}

// Not really a restream.
// Call after changing from/to a custom model to see the result immediately
void CVehicleUpgrades::RestreamVehicleUpgrades(unsigned short usUpgrade)
{
    unsigned char ucSlot;
    if (GetSlotFromUpgrade(usUpgrade, ucSlot))
    {
        if (m_SlotStates[ucSlot] == usUpgrade)
            ForceAddUpgrade(usUpgrade);
    }
}

bool CVehicleUpgrades::HasUpgrade(unsigned short usUpgrade)
{
    unsigned char ucSlot = 0;
    for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
    {
        if (m_SlotStates[ucSlot] == usUpgrade)
            return true;
    }

    return false;
}

bool CVehicleUpgrades::RemoveUpgrade(unsigned short usUpgrade)
{
    if (HasUpgrade(usUpgrade))
    {
        if (m_pVehicle)
        {
            CVehicle* pVehicle = m_pVehicle->GetGameVehicle();
            if (pVehicle)
            {
                pVehicle->RemoveVehicleUpgrade(usUpgrade);
            }

            unsigned char ucSlot;
            if (GetSlotFromUpgrade(usUpgrade, ucSlot))
            {
                m_SlotStates[ucSlot] = 0;
            }

            // Reset wheel scale if it is a wheel upgrade
            if (ucSlot == 12)
                m_pVehicle->ResetWheelScale();

            return true;
        }
    }

    return false;
}

unsigned short CVehicleUpgrades::GetSlotState(unsigned char ucSlot)
{
    if (ucSlot < VEHICLE_UPGRADE_SLOTS)
        return m_SlotStates[ucSlot];

    return 0;
}

void CVehicleUpgrades::ReAddAll()
{
    unsigned char ucSlot = 0;
    for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
    {
        if (m_SlotStates[ucSlot])
        {
            if (IsUpgradeCompatible(m_SlotStates[ucSlot]))
            {
                ForceAddUpgrade(m_SlotStates[ucSlot]);
            }
            else
            {
                // Not compatible with the mod they are using so just ignore it
                m_SlotStates[ucSlot] = 0;
            }
        }
    }
}

void CVehicleUpgrades::RemoveAll(bool bRipFromVehicle)
{
    unsigned char ucSlot = 0;
    for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
    {
        if (m_SlotStates[ucSlot])
        {
            if (bRipFromVehicle)
            {
                if (m_pVehicle)
                {
                    CVehicle* pVehicle = m_pVehicle->GetGameVehicle();
                    if (pVehicle)
                    {
                        pVehicle->RemoveVehicleUpgrade(m_SlotStates[ucSlot]);
                    }
                }
            }
            m_SlotStates[ucSlot] = 0;

            // Reset wheel scale for wheel upgrades
            if (ucSlot == 12 && m_pVehicle)
                m_pVehicle->ResetWheelScale();
        }
    }
}

const char* CVehicleUpgrades::GetSlotName(unsigned char ucSlot)
{
    if (ucSlot < VEHICLE_UPGRADE_SLOTS)
        return UpgradeNames[ucSlot].szName;

    return szUpgradeNameEmpty;
}
