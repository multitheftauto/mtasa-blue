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
#include <cstdint>
#include <unordered_map>

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

const std::unordered_map<std::uint16_t, std::unordered_set<std::uint16_t>> slots = {
    {400, {1020, 1021, 1019, 1018, 1013, 1024, 1008, 1009, 1010}},
    {401, {1005, 1004, 1142, 1143, 1144, 114, 1020, 1019, 1013, 1008, 1009, 1010, 1006, 1001, 1003, 1017, 1007}},
    {402, {1009, 1010}},
    {404, {1020, 1021, 1019, 1013, 1008, 1009, 1010, 1002, 1016, 1000, 1017, 1007}},
    {405, {1020, 1021, 1019, 1018, 1008, 1009, 1010, 1001, 1014, 1023, 1000}},
    {409, {1009}},
    {410, {1019, 1021, 1020, 1013, 1024, 1008, 1009, 1010, 1001, 1023, 1003, 1017, 1007}},
    {411, {1008, 1009, 1010}},
    {412, {1008, 1009, 1010}},
    {415, {1019, 1018, 1008, 1009, 1010, 1001, 1023, 1003, 1017, 1007}},
    {418, {1020, 1021, 1008, 1009, 1010, 1006, 1002, 1016}},
    {419, {1008, 1009, 1010}},
    {420, {1005, 1004, 1021, 1019, 1008, 1009, 1010, 1001, 1003}},
    {421, {1020, 1021, 1019, 1018, 1008, 1009, 1010, 1014, 1023, 1016, 1000}},
    {422, {1020, 1021, 1019, 1013, 1008, 1009, 1010, 1017, 1007}},
    {426, {1005, 1004, 1021, 1019, 1008, 1009, 1010, 1006, 1001, 1003}},
    {429, {1008, 1009, 1010}},
    {436, {1020, 1021, 1022, 1019, 1013, 1008, 1009, 1010, 1006, 1001, 1003, 1017, 1007}},
    {438, {1008, 1009, 1010}},
    {439, {1003, 1023, 1001, 1008, 1009, 1010, 1017, 1007, 1142, 1143, 1144, 1145, 1013}},
    {442, {1008, 1009, 1010}},
    {445, {1008, 1009, 1010}},
    {451, {1008, 1009, 1010}},
    {458, {1008, 1009, 1010}},
    {466, {1008, 1009, 1010}},
    {467, {1008, 1009, 1010}},
    {474, {1008, 1009, 1010}},
    {475, {1008, 1009, 1010}},
    {477, {1020, 1021, 1019, 1018, 1008, 1009, 1010, 1006, 1017, 1007}},
    {478, {1005, 1004, 1012, 1020, 1021, 1022, 1013, 1024, 1008, 1009, 1010}},
    {479, {1008, 1009, 1010}},
    {480, {1008, 1009, 1010}},
    {489, {1005, 1004, 1020, 1019, 1018, 1013, 1024, 1008, 1009, 1010, 1006, 1002, 1016, 1000}},
    {491, {1142, 1143, 1144, 1145, 1020, 1021, 1019, 1018, 1008, 1009, 1010, 1014, 1023, 1003, 1017, 1007}},
    {492, {1005, 1004, 1008, 1009, 1010, 1006, 1016, 1000}},
    {496, {1006, 1017, 1007, 1011, 1019, 1023, 1001, 1008, 1009, 1010, 1003, 1002, 1142, 1143, 1020}},
    {500, {1020, 1021, 1019, 1013, 1024, 1008, 1009, 1010}},
    {506, {1009}},
    {507, {1008, 1009, 1010}},
    {516, {1004, 1020, 1021, 1019, 1018, 1008, 1009, 1010, 1002, 1015, 1016, 1000, 1017, 1007}},
    {517, {1142, 1143, 1144, 1145, 1020, 1019, 1018, 1008, 1009, 1010, 1002, 1023, 1016, 1003, 1017, 1007}},
    {518, {1005, 1142, 1143, 1144, 1145, 1020, 1018, 1013, 1008, 1009, 1010, 1006, 1001, 1023, 1003, 1017, 1007}},
    {526, {1008, 1009, 1010}},
    {527, {1021, 1020, 1018, 1008, 1009, 1010, 1001, 1014, 1015, 1017, 1007}},
    {529, {1012, 1011, 1020, 1019, 1018, 1008, 1009, 1010, 1006, 1001, 1023, 1003, 1017, 1007}},
    {533, {1008, 1009, 1010}},
    {534, {1126, 1127, 1179, 1185, 1100, 1123, 1125, 1008, 1009, 1010, 1180, 1178, 1101, 1122, 1124, 1106}},
    {535, {1109, 1110, 1113, 1114, 1115, 1116, 1117, 1008, 1009, 1010, 1120, 1118, 1121, 1119}},
    {536, {1104, 1105, 1182, 1181, 1008, 1009, 1010, 1184, 1183, 1128, 1103, 1107, 1108}},
    {540, {1004, 1142, 1143, 1144, 1145, 1020, 1019, 1018, 1024, 1008, 1009, 1010, 1006, 1001, 1023, 1017, 1007}},
    {541, {1008, 1009, 1010}},
    {542, {1144, 1145, 1020, 1021, 1019, 1018, 1008, 1009, 1010, 1014, 1015}},
    {545, {1008, 1009, 1010}},
    {546, {1004, 1142, 1143, 1144, 1145, 1019, 1018, 1024, 1008, 1009, 1010, 1006, 1002, 1001, 1023, 1017, 1007}},
    {547, {1142, 1143, 1020, 1021, 1019, 1018, 1008, 1009, 1010, 1016, 1003, 1000}},
    {549, {1012, 1011, 1142, 1143, 1144, 1145, 1020, 1019, 1018, 1008, 1009, 1010, 1001, 1023, 1003, 1017, 1007}},
    {550, {1005, 1004, 1142, 1143, 1144, 1145, 1020, 1019, 1018, 1008, 1009, 1010, 1006, 1001, 1023, 1003}},
    {551, {1005, 1020, 1021, 1019, 1018, 1008, 1009, 1010, 1006, 1002, 1023, 1016, 1003}},
    {555, {1008, 1009, 1010}},
    {558, {1092, 1089, 1166, 1165, 1008, 1009, 1010, 1168, 1167, 1088, 1091, 1164, 1163, 1094, 1090, 1095, 1093}},
    {559, {1065, 1066, 1160, 1173, 1008, 1009, 1010, 1159, 1161, 1162, 1158, 1067, 1068, 1071, 1069, 1072, 1070}},
    {560, {1028, 1029, 1169, 1170, 1008, 1009, 1010, 1141, 1140, 1032, 1033, 1138, 1139, 1027, 1026, 1030, 1031}},
    {561, {1064, 1059, 1155, 1157, 1008, 1009, 1010, 1154, 1156, 1055, 1061, 1058, 1060, 1062, 1056, 1063, 1057}},
    {562, {1034, 1037, 1171, 1172, 1008, 1009, 1010, 1149, 1148, 1038, 1035, 1147, 1146, 1040, 1036, 1041, 1039}},
    {565, {1046, 1045, 1153, 1152, 1008, 1009, 1010, 1150, 1151, 1054, 1053, 1049, 1050, 1051, 1047, 1052, 1048}},
    {566, {1008, 1009, 1010}},
    {567, {1129, 1132, 1189, 1188, 1008, 1009, 1010, 1187, 1186, 1130, 1131, 1102, 1133}},
    {575, {1044, 1043, 1174, 1175, 1008, 1009, 1010, 1176, 1177, 1099, 1042}},
    {576, {1136, 1135, 1191, 1190, 1008, 1009, 1010, 1192, 1193, 1137, 1134}},
    {579, {1008, 1009, 1010}},
    {580, {1020, 1018, 1008, 1009, 1010, 1006, 1001, 1023, 1017, 1007}},
    {585, {1142, 1143, 1144, 1145, 1020, 1019, 1018, 1013, 1008, 1009, 1010, 1006, 1001, 1023, 1003, 1017, 1007}},
    {587, {1008, 1009, 1010}},
    {589, {1005, 1004, 1144, 1145, 1020, 1018, 1008, 1009, 1010, 1024, 1013, 1006, 1016, 1000, 1017, 1007}},
    {600, {1005, 1004, 1020, 1022, 1018, 1013, 1008, 1009, 1010, 1006, 1017, 1007}},
    {602, {1008, 1009, 1010}},
    {603, {1144, 1145, 1142, 1143, 1020, 1019, 1018, 1024, 1008, 1009, 1010, 1006, 1001, 1023, 1017, 1007}},
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

bool CVehicleUpgrades::IsUpgradeCompatible(const std::uint16_t upgrade) noexcept
{
    const auto type = m_pVehicle->GetVehicleType();

    if (type == CLIENTVEHICLE_TRAIN || type == CLIENTVEHICLE_BOAT)
    {
        return false;
    }

    auto model = m_pVehicle->GetModel();
    auto* info = g_pGame->GetModelInfo(model);

    if (info && info->GetParentID() != 0)
    {
        model = static_cast<std::uint16_t>(info->GetParentID());
    }

    const auto it = slots.find(model);

    if (it == slots.end())
    {
        return false;
    }

    return it->second.count(upgrade) > 0;
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
