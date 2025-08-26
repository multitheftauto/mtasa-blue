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
#include "VehicleUpgrades.hpp"

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

bool CVehicleUpgrades::IsUpgradeCompatible(unsigned short usUpgrade)
{
    eClientVehicleType vehicleType = m_pVehicle->GetVehicleType();

    // No upgrades for trains/boats/bike/bmx/heli/plane
    if (vehicleType == CLIENTVEHICLE_TRAIN || vehicleType == CLIENTVEHICLE_BOAT || vehicleType == CLIENTVEHICLE_BIKE ||
        vehicleType == CLIENTVEHICLE_BMX || vehicleType == CLIENTVEHICLE_HELI || vehicleType == CLIENTVEHICLE_PLANE)
        return false;

    bool bReturn = IsUpgradeCompatibleWithModel(m_pVehicle->GetModel(), usUpgrade);
    
    unsigned char ucSlot = 0;

    // Allow slot 2 to be upgraded regardless of ID and then check it has the required part
    if (GetSlotFromUpgrade(usUpgrade, ucSlot) && (bReturn || ucSlot == 2))
    {
        // Get our model supported upgrades
        SVehicleSupportedUpgrades supportedUpgrades = m_pVehicle->GetModelInfo()->GetVehicleSupportedUpgrades();
        // Initialisation happens when we load the clump which is done when we require a specific model rather than in bulk
        if (supportedUpgrades.m_bInitialised == true)
        {
            // Hood and Vents
            if (ucSlot == 0 || ucSlot == 1)
            {
                if (supportedUpgrades.m_bBonnet == true)
                {
                    if (usUpgrade == 1142 || usUpgrade == 1144)
                    {
                        // We add both left and right for some reason seems to be SA doing it
                        bReturn &= supportedUpgrades.m_bBonnet_Left;
                        bReturn &= supportedUpgrades.m_bBonnet_Left_dam;

                        bReturn &= supportedUpgrades.m_bBonnet_Right;
                        bReturn &= supportedUpgrades.m_bBonnet_Right_dam;
                    }
                    else if (usUpgrade == 1004 || usUpgrade == 1005 || usUpgrade == 1011 || usUpgrade == 1012)
                    {
                        // Just needs m_bBonnet
                        bReturn = true;
                    }
                }
            }
            else if (ucSlot == 2)
            {
                bReturn = supportedUpgrades.m_bSpoiler;
            }
            else if (ucSlot == 3)
            {
                bReturn = supportedUpgrades.m_bSideSkirt_Right;
                bReturn &= supportedUpgrades.m_bSideSkirt_Left;
            }
            else if (ucSlot == 4)
            {
                bReturn = supportedUpgrades.m_bFrontBullbars;
            }
            else if (ucSlot == 5)
            {
                bReturn = supportedUpgrades.m_bRearBullbars;
            }
            else if (ucSlot == 6)
            {
                bReturn = supportedUpgrades.m_bLamps;
                bReturn &= supportedUpgrades.m_bLamps_dam;
            }
            else if (ucSlot == 7)
            {
                bReturn = supportedUpgrades.m_bRoof;
            }
            else if (ucSlot == 13)
            {
                bReturn = supportedUpgrades.m_bExhaust;
            }
            else if (ucSlot == 14)
            {
                bReturn = supportedUpgrades.m_bFrontBumper;
            }
            else if (ucSlot == 15)
            {
                bReturn = supportedUpgrades.m_bRearBumper;
            }
            else if (ucSlot == 16)
            {
                bReturn = supportedUpgrades.m_bMisc;
            }
        }
    }
    return bReturn;
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
