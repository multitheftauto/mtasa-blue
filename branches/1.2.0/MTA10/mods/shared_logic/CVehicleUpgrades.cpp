/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CVehicleUpgrades.cpp
*  PURPOSE:     Vehicle upgrades class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

char szUpgradeNameEmpty [] = "";

struct SUpgradeName
{
    char szName [32];
};

SUpgradeName UpgradeNames [17] =
{ {"Hood"}, {"Vent"}, {"Spoiler"}, {"Sideskirt"}, {"Front Bullbars"},
{"Rear Bullbars"}, {"Headlights"}, {"Roof"}, {"Nitro"}, {"Hydraulics"}, 
{"Stereo"}, {"Unknown"}, {"Wheels"}, {"Exhaust"}, {"Front Bumper"},
{"Rear Bumper"}, {"Misc"} };


CVehicleUpgrades::CVehicleUpgrades ( CClientVehicle* pVehicle )
{
    m_pVehicle = pVehicle;
    memset ( m_SlotStates, 0, sizeof ( m_SlotStates ) );
}


bool CVehicleUpgrades::IsUpgrade ( unsigned short usModel )
{
    return ( usModel >= 1000 && usModel <= 1193 );
}


bool CVehicleUpgrades::IsUpgradeCompatible ( unsigned short usUpgrade )
{
    unsigned short usModel = m_pVehicle->GetModel ();

    // No upgrades for trains
    if ( usModel == 537 ||
         usModel == 538 ||
         usModel == 449 ||
         usModel == 569 ||
         usModel == 570 ||
         usModel == 590  )
    {
        return false;
    }

    unsigned short us = usUpgrade;

    eClientVehicleType vehicleType = m_pVehicle->GetVehicleType ();
    if ( vehicleType == CLIENTVEHICLE_BOAT || vehicleType == CLIENTVEHICLE_PLANE ||
         vehicleType == CLIENTVEHICLE_BIKE || vehicleType == CLIENTVEHICLE_BMX )
         return false;

    if ( us == VEHICLEUPGRADE_NITRO_5X || us == VEHICLEUPGRADE_NITRO_2X ||
         us == VEHICLEUPGRADE_NITRO_10X || us == VEHICLEUPGRADE_HYDRAULICS )
         return true;

    // Wheels should be compatible with any car
    if ( us == 1025 || us == 1073 || us == 1074 || us == 1075 || us == 1076 ||
         us == 1077 || us == 1078 || us == 1079 || us == 1080 || us == 1081 ||
         us == 1082 || us == 1083 || us == 1084 || us == 1085 || us == 1096 ||
         us == 1097 || us == 1098 )
         return true;

    switch ( usModel )
    {
        case 400: return ( us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010 );
        case 401: return ( us == 1005 || us == 1004 || us == 1142 || us == 1143 || us == 1144 || us == 114 || us == 1020 || us == 1019 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1003 || us == 1017 || us == 1007 );
        case 402: return ( us == 1009 || us == 1009 || us == 1010 );
        case 404: return ( us == 1020 || us == 1021 || us == 1019 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1002 || us == 1016 || us == 1000 || us == 1017 || us == 1007 );
        case 405: return ( us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1014 || us == 1023 || us == 1000 );
        case 409: return ( us == 1009 );
        case 410: return ( us == 1019 || us == 1021 || us == 1020 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007 );
        case 411: return ( us == 1008 || us == 1009 || us == 1010 );
        case 412: return ( us == 1008 || us == 1009 || us == 1010 );
        case 415: return ( us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007 );
        case 418: return ( us == 1020 || us == 1021 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1002 || us == 1016 );
        case 419: return ( us == 1008 || us == 1009 || us == 1010 );
        case 420: return ( us == 1005 || us == 1004 || us == 1021 || us == 1019 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1003 );
        case 421: return ( us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1014 || us == 1023 || us == 1016 || us == 1000 );
        case 422: return ( us == 1020 || us == 1021 || us == 1019 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1017 || us == 1007 );
        case 426: return ( us == 1005 || us == 1004 || us == 1021 || us == 1019 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1003 );
        case 429: return ( us == 1008 || us == 1009 || us == 1010 );
        case 436: return ( us == 1020 || us == 1021 || us == 1022 || us == 1019 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1003 || us == 1017 || us == 1007 );
        case 438: return ( us == 1008 || us == 1009 || us == 1010 );
        case 439: return ( us == 1003 || us == 1023 || us == 1001 || us == 1008 || us == 1009 || us == 1010 || us == 1017 || us == 1007 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1013 );
        case 442: return ( us == 1008 || us == 1009 || us == 1010 );
        case 445: return ( us == 1008 || us == 1009 || us == 1010 );
        case 451: return ( us == 1008 || us == 1009 || us == 1010 );
        case 458: return ( us == 1008 || us == 1009 || us == 1010 );
        case 466: return ( us == 1008 || us == 1009 || us == 1010 );
        case 467: return ( us == 1008 || us == 1009 || us == 1010 );
        case 474: return ( us == 1008 || us == 1009 || us == 1010 );
        case 475: return ( us == 1008 || us == 1009 || us == 1010 );
        case 477: return ( us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1017 || us == 1007 );
        case 478: return ( us == 1005 || us == 1004 || us == 1012 || us == 1020 || us == 1021 || us == 1022 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010 );
        case 479: return ( us == 1008 || us == 1009 || us == 1010 );
        case 480: return ( us == 1008 || us == 1009 || us == 1010 );
        case 489: return ( us == 1005 || us == 1004 || us == 1020 || us == 1019 || us == 1018 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1002 || us == 1016 || us == 1000 );
        case 491: return ( us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1014 || us == 1023 || us == 1003 || us == 1017 || us == 1007 );
        case 492: return ( us == 1005 || us == 1004 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1016 || us == 1000 );
        case 496: return ( us == 1006 || us == 1017 || us == 1007 || us == 1011 || us == 1019 || us == 1023 || us == 1001 || us == 1008 || us == 1009 || us == 1010 || us == 1003 || us == 1002 || us == 1142 || us == 1143 || us == 1020 );
        case 500: return ( us == 1020 || us == 1021 || us == 1019 || us == 1013 || us == 1024 || us == 1008 || us == 1009 || us == 1010 );
        case 506: return ( us == 1009 );
        case 507: return ( us == 1008 || us == 1009 || us == 1010 );
        case 516: return ( us == 1004 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1002 || us == 1015 || us == 1016 || us == 1000 || us == 1017 || us == 1007 );
        case 517: return ( us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1002 || us == 1023 || us == 1016 || us == 1003 || us == 1017 || us == 1007 );
        case 518: return ( us == 1005 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1018 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007 );
        case 526: return ( us == 1008 || us == 1009 || us == 1010 );
        case 527: return ( us == 1021 || us == 1020 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1014 || us == 1015 || us == 1017 || us == 1007 );
        case 529: return ( us == 1012 || us == 1011 || us == 1020 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007 );
        case 533: return ( us == 1008 || us == 1009 || us == 1010 );
        case 534: return ( us == 1126 || us == 1127 || us == 1179 || us == 1185 || us == 1100 || us == 1123 || us == 1125 || us == 1008 || us == 1009 || us == 1010 || us == 1180 || us == 1178 || us == 1101 || us == 1122 || us == 1124 || us == 1106 );
        case 535: return ( us == 1109 || us == 1110 || us == 1113 || us == 1114 || us == 1115 || us == 1116 || us == 1117 || us == 1008 || us == 1009 || us == 1010 || us == 1120 || us == 1118 || us == 1121 || us == 1119 );
        case 536: return ( us == 1104 || us == 1105 || us == 1182 || us == 1181 || us == 1008 || us == 1009 || us == 1010 || us == 1184 || us == 1183 || us == 1128 || us == 1103 || us == 1107 || us == 1108 );
        case 540: return ( us == 1004 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1024 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1017 || us == 1007 );
        case 541: return ( us == 1008 || us == 1009 || us == 1010 );
        case 542: return ( us == 1144 || us == 1145 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1014 || us == 1015 );
        case 545: return ( us == 1008 || us == 1009 || us == 1010 );
        case 546: return ( us == 1004 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1019 || us == 1018 || us == 1024 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1002 || us == 1001 || us == 1023 || us == 1017 || us == 1007 );
        case 547: return ( us == 1142 || us == 1143 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1016 || us == 1003 || us == 1000 );
        case 549: return ( us == 1012 || us == 1011 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007 );
        case 550: return ( us == 1005 || us == 1004 || us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1003 );
        case 551: return ( us == 1005 || us == 1020 || us == 1021 || us == 1019 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1002 || us == 1023 || us == 1016 || us == 1003 );
        case 555: return ( us == 1008 || us == 1009 || us == 1010 );
        case 558: return ( us == 1092 || us == 1089 || us == 1166 || us == 1165 || us == 1008 || us == 1009 || us == 1010 || us == 1168 || us == 1167 || us == 1088 || us == 1091 || us == 1164 || us == 1163 || us == 1094 || us == 1090 || us == 1095 || us == 1093 );
        case 559: return ( us == 1065 || us == 1066 || us == 1160 || us == 1173 || us == 1008 || us == 1009 || us == 1010 || us == 1159 || us == 1161 || us == 1162 || us == 1158 || us == 1067 || us == 1068 || us == 1071 || us == 1069 || us == 1072 || us == 1070 || us == 1009 );
        case 560: return ( us == 1028 || us == 1029 || us == 1169 || us == 1170 || us == 1008 || us == 1009 || us == 1010 || us == 1141 || us == 1140 || us == 1032 || us == 1033 || us == 1138 || us == 1139 || us == 1027 || us == 1026 || us == 1030 || us == 1031 );
        case 561: return ( us == 1064 || us == 1059 || us == 1155 || us == 1157 || us == 1008 || us == 1009 || us == 1010 || us == 1154 || us == 1156 || us == 1055 || us == 1061 || us == 1058 || us == 1060 || us == 1062 || us == 1056 || us == 1063 || us == 1057 );
        case 562: return ( us == 1034 || us == 1037 || us == 1171 || us == 1172 || us == 1008 || us == 1009 || us == 1010 || us == 1149 || us == 1148 || us == 1038 || us == 1035 || us == 1147 || us == 1146 || us == 1040 || us == 1036 || us == 1041 || us == 1039 );
        case 565: return ( us == 1046 || us == 1045 || us == 1153 || us == 1152 || us == 1008 || us == 1009 || us == 1010 || us == 1150 || us == 1151 || us == 1054 || us == 1053 || us == 1049 || us == 1050 || us == 1051 || us == 1047 || us == 1052 || us == 1048 );
        case 566: return ( us == 1008 || us == 1009 || us == 1010 );
        case 567: return ( us == 1129 || us == 1132 || us == 1189 || us == 1188 || us == 1008 || us == 1009 || us == 1010 || us == 1187 || us == 1186 || us == 1130 || us == 1131 || us == 1102 || us == 1133 );
        case 575: return ( us == 1044 || us == 1043 || us == 1174 || us == 1175 || us == 1008 || us == 1009 || us == 1010 || us == 1176 || us == 1177 || us == 1099 || us == 1042 );
        case 576: return ( us == 1136 || us == 1135 || us == 1191 || us == 1190 || us == 1008 || us == 1009 || us == 1010 || us == 1192 || us == 1193 || us == 1137 || us == 1134 );
        case 579: return ( us == 1008 || us == 1009 || us == 1010 );
        case 580: return ( us == 1020 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1017 || us == 1007 );
        case 585: return ( us == 1142 || us == 1143 || us == 1144 || us == 1145 || us == 1020 || us == 1019 || us == 1018 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1003 || us == 1017 || us == 1007 );
        case 587: return ( us == 1008 || us == 1009 || us == 1010 );
        case 589: return ( us == 1005 || us == 1004 || us == 1144 || us == 1145 || us == 1020 || us == 1018 || us == 1008 || us == 1009 || us == 1010 || us == 1024 || us == 1013 || us == 1006 || us == 1016 || us == 1000 || us == 1017 || us == 1007 );
        case 600: return ( us == 1005 || us == 1004 || us == 1020 || us == 1022 || us == 1018 || us == 1013 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1017 || us == 1007 );
        case 602: return ( us == 1008 || us == 1009 || us == 1010 );
        case 603: return ( us == 1144 || us == 1145 || us == 1142 || us == 1143 || us == 1020 || us == 1019 || us == 1018 || us == 1024 || us == 1008 || us == 1009 || us == 1010 || us == 1006 || us == 1001 || us == 1023 || us == 1017 || us == 1007 );
    }
    return false;
}


bool CVehicleUpgrades::GetSlotFromUpgrade ( unsigned short us, unsigned char& ucSlot )
{
    // TODO: 1013, 1024, 1109, 1110
    if ( us == 1000 || us == 1001 || us == 1002 || us == 1003 || us == 1014 || /* spoiler */
         us == 1015 || us == 1016 || us == 1023 || us == 1049 || us == 1050 ||
         us == 1058 || us == 1060 || us == 1138 || us == 1139 || us == 1146 ||
         us == 1147 || us == 1158 || us == 1162 || us == 1163 || us == 1164 )
    {
        ucSlot = 2;
        return true;
    }
    if ( us == 1004 || us == 1005 ) // vent
    {
        ucSlot = 1;
        return true;
    }
    if ( us == 1006 || us == 1032 || us == 1033 || us == 1035 || us == 1038 || /* roof */
         us == 1053 || us == 1054 || us == 1055 || us == 1061 || us == 1067 ||
         us == 1068 || us == 1088 || us == 1091 || us == 1103 || us == 1128 ||
         us == 1130 || us == 1131 )
    {
        ucSlot = 7;
        return true;
    }
    if ( us == 1007 || us == 1017 || us == 1026 || us == 1027 || us == 1030 || /* sideskirt */
         us == 1031 || us == 1036 || us == 1039 || us == 1040 || us == 1041 ||
         us == 1042 || us == 1047 || us == 1048 || us == 1051 || us == 1052 ||
         us == 1056 || us == 1057 || us == 1062 || us == 1063 || us == 1069 ||
         us == 1070 || us == 1071 || us == 1072 || us == 1090 || us == 1093 ||
         us == 1094 || us == 1095 || us == 1099 || us == 1101 || us == 1102 ||
         us == 1106 || us == 1107 || us == 1108 || us == 1118 || us == 1119 ||
         us == 1120 || us == 1121 || us == 1122 || us == 1124 || us == 1133 ||
         us == 1134 || us == 1137 )
    {
        ucSlot = 3;
        return true;
    }
    if ( us == 1008 || us == 1009 || us == 1010 ) // nitro
    {
        ucSlot = 8;
        return true;
    }
    if ( us == 1011 || us == 1012 || us == 1111 || us == 1112 || us == 1142 || /* bonet */
         us == 1143 || us == 1144 || us == 1145 )
    {
        ucSlot = 0;
        return true;
    }
    if ( us == 1018 || us == 1019 || us == 1020 || us == 1021 || us == 1022 || /* exhaust */
         us == 1028 || us == 1029 || us == 1034 || us == 1037 || us == 1043 ||
         us == 1044 || us == 1045 || us == 1046 || us == 1059 || us == 1064 ||
         us == 1065 || us == 1066 || us == 1089 || us == 1092 || us == 1104 ||
         us == 1105 || us == 1113 || us == 1114 || us == 1126 || us == 1127 ||
         us == 1129 || us == 1132 || us == 1135 || us == 1136 )
    {
        ucSlot = 13;
        return true;
    }
    if ( us == 1025 || us == 1073 || us == 1074 || us == 1075 || us == 1076 || /* wheels */
         us == 1077 || us == 1078 || us == 1079 || us == 1080 || us == 1081 ||
         us == 1082 || us == 1083 || us == 1084 || us == 1085 || us == 1096 ||
         us == 1097 || us == 1098 )
    {
        ucSlot = 12;
        return true;
    }
    if ( us == 1086 ) // stereo
    {
        ucSlot = 10;
        return true;
    }
    if ( us == 1087 ) // hydraulics
    {
        ucSlot = 9;
        return true;
    }
    if ( us == 1100 || us == 1123 || us == 1125 ) // misc
    {
        ucSlot = 16;
        return true;
    }
    if ( us == 1115 || us == 1116 ) // front bullbars
    {
        ucSlot = 4;
        return true;
    }
    if ( us == 1117 || us == 1152 || us == 1153 || us == 1155 || us == 1157 || /* front bumper */
         us == 1160 || us == 1165 || us == 1166 || us == 1169 || us == 1170 ||
         us == 1171 || us == 1172 || us == 1173 || us == 1174 || us == 1175 ||
         us == 1179 || us == 1181 || us == 1182 || us == 1185 || us == 1188 ||
         us == 1189 || us == 1190 || us == 1191 )
    {
        ucSlot = 14;
        return true;
    }
    if ( us == 1140 || us == 1141 || us == 1148 || us == 1149 || us == 1150 || /* rear bumper */
         us == 1151 || us == 1154 || us == 1156 || us == 1159 || us == 1161 ||
         us == 1167 || us == 1168 || us == 1176 || us == 1177 || us == 1178 ||
         us == 1180 || us == 1183 || us == 1184 || us == 1186 || us == 1187 ||
         us == 1192 || us == 1193 )
    {
        ucSlot = 15;
        return true;
    }

    return false;
}


bool CVehicleUpgrades::AddUpgrade ( unsigned short usUpgrade )
{
    if ( m_pVehicle )
    {
        // If its a compatible upgrade
        if ( IsUpgradeCompatible ( usUpgrade ) )
        {
            ForceAddUpgrade ( usUpgrade );

            return true;
        }
    }           
    return false;
}


void CVehicleUpgrades::AddAllUpgrades ( void )
{
    if ( m_pVehicle )
    {
        unsigned short usUpgrade = 1000;
        for ( ; usUpgrade <= 1193 ; usUpgrade++ )
        {
            if ( IsUpgradeCompatible ( usUpgrade ) )
            {
                ForceAddUpgrade ( usUpgrade );
            }
        }
    }  
}


void CVehicleUpgrades::ForceAddUpgrade ( unsigned short usUpgrade )
{
    unsigned char ucSlot;
    if ( GetSlotFromUpgrade ( usUpgrade, ucSlot ) )
    {
        CVehicle* pVehicle = m_pVehicle->GetGameVehicle ();
        if ( pVehicle )
        {
            // Grab the upgrade model
            CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usUpgrade );
            if ( pModelInfo )
            {
                if ( !g_pGame->IsASyncLoadingEnabled () || !pModelInfo->IsLoaded () )
                {
                    // Request and load now
                    pModelInfo->RequestVehicleUpgrade ();

                    g_pGame->GetStreaming()->LoadAllRequestedModels ();
                }
                // Add the upgrade
                pVehicle->AddVehicleUpgrade ( usUpgrade );
            }
        }

        // Add it to the slot
        m_SlotStates [ ucSlot ] = usUpgrade;
    }
}

bool CVehicleUpgrades::HasUpgrade ( unsigned short usUpgrade )
{
    unsigned char ucSlot = 0;
    for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS ; ucSlot++ )
    {
        if ( m_SlotStates [ ucSlot ] == usUpgrade )
            return true;
    }

    return false;
}


bool CVehicleUpgrades::RemoveUpgrade ( unsigned short usUpgrade )
{
    if ( HasUpgrade ( usUpgrade ) )
    {
        if ( m_pVehicle )
        {
            CVehicle* pVehicle = m_pVehicle->GetGameVehicle ();
            if ( pVehicle )
            {
                pVehicle->RemoveVehicleUpgrade ( usUpgrade );
            }

            unsigned char ucSlot;
            if ( GetSlotFromUpgrade ( usUpgrade, ucSlot ) )
            {
                m_SlotStates [ ucSlot ] = 0;
            }

            return true;
        }
    }

    return false;
}

unsigned short CVehicleUpgrades::GetSlotState ( unsigned char ucSlot )
{
    if ( ucSlot < VEHICLE_UPGRADE_SLOTS )
        return m_SlotStates [ ucSlot ];

    return 0;
}


void CVehicleUpgrades::ReAddAll ( void )
{
    unsigned char ucSlot = 0;
    for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS ; ucSlot++ )
    {
        if ( m_SlotStates [ ucSlot ] )
        {
            #ifdef MTA_DEBUG
                assert ( IsUpgradeCompatible ( m_SlotStates [ ucSlot ] ) );
            #endif
            ForceAddUpgrade ( m_SlotStates [ ucSlot ] );
        }
    }
}


void CVehicleUpgrades::RemoveAll ( bool bRipFromVehicle )
{
    unsigned char ucSlot = 0;
    for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS ; ucSlot++ )
    {
        if ( m_SlotStates [ ucSlot ] )
        {
            if ( bRipFromVehicle )
            {
                if ( m_pVehicle )
                {
                    CVehicle* pVehicle = m_pVehicle->GetGameVehicle ();
                    if ( pVehicle )
                    {
                        pVehicle->RemoveVehicleUpgrade ( m_SlotStates [ ucSlot ] );
                    }
                }
            }
            m_SlotStates [ ucSlot ] = 0;
        }
    }
}


const char* CVehicleUpgrades::GetSlotName ( unsigned char ucSlot )
{
    if ( ucSlot < VEHICLE_UPGRADE_SLOTS )
        return UpgradeNames [ ucSlot ].szName;

    return szUpgradeNameEmpty;
}