/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CZoneNames.cpp
*
*****************************************************************************/

#include "StdInc.h"

static const char* szUnknownZone = "Unknown";

static const SFixedArray < const char*, 8 > cityNameList = {
                    "Tierra Robada",
                    "Bone County",
                    "Las Venturas",
                    "San Fierro",
                    "Red County",
                    "Whetstone",
                    "Flint County",
                    "Los Santos",
                };

static const SZone _zoneInfoList [] = {
        { -2353,  2275,     0, -2153,  2475,   200, "Bayside Marina" },
        { -2741,  2175,     0, -2353,  2722,   200, "Bayside" },
        { -2741,  1268,     0, -2533,  1490,   200, "Battery Point" },
        { -2741,   793,     0, -2533,  1268,   200, "Paradiso" },
        { -2741,   458,     0, -2533,   793,   200, "Santa Flora" },
        { -2994,   458,     0, -2741,  1339,   200, "Palisades" },
        { -2867,   277,     0, -2593,   458,   200, "City Hall" },
        { -2994,   277,     0, -2867,   458,   200, "Ocean Flats" },
        { -2994,  -222,     0, -2593,   277,   200, "Ocean Flats" },
        { -2994,  -430,     0, -2831,  -222,   200, "Ocean Flats" },
        { -2270,  -430,     0, -2178,  -324,   200, "Foster Valley" },
        { -2178,  -599,     0, -1794,  -324,   200, "Foster Valley" },
        { -2593,  -222,     0, -2411,    54,   200, "Hashbury" },
        { -2533,   968,     0, -2274,  1358,   200, "Juniper Hollow" },
        { -2533,  1358,     0, -1996,  1501,   200, "Esplanade North" },
        { -1996,  1358,     0, -1524,  1592,   200, "Esplanade North" },
        { -1982,  1274,     0, -1524,  1358,   200, "Esplanade North" },
        { -1871,   744,     0, -1701,  1176,   300, "Financial" },
        { -2274,   744,     0, -1982,  1358,   200, "Calton Heights" },
        { -1982,   744,     0, -1871,  1274,   200, "Downtown" },
        { -1871,  1176,     0, -1620,  1274,   200, "Downtown" },
        { -1700,   744,     0, -1580,  1176,   200, "Downtown" },
        { -1580,   744,     0, -1499,  1025,   200, "Downtown" },
        { -2533,   578,     0, -2274,   968,   200, "Juniper Hill" },
        { -2274,   578,     0, -2078,   744,   200, "Chinatown" },
        { -2078,   578,     0, -1499,   744,   200, "Downtown" },
        { -2329,   458,     0, -1993,   578,   200, "King's" },
        { -2411,   265,     0, -1993,   373,   200, "King's" },
        { -2253,   373,     0, -1993,   458,   200, "King's" },
        { -2411,  -222,     0, -2173,   265,   200, "Garcia" },
        { -2270,  -324,     0, -1794,  -222,   200, "Doherty" },
        { -2173,  -222,     0, -1794,   265,   200, "Doherty" },
        { -1993,   265,     0, -1794,   578,   200, "Downtown" },
        { -1499,   -50,     0, -1242,   249,   200, "Easter Bay Airport" },
        { -1794,   249,     0, -1242,   578,   200, "Easter Basin" },
        { -1794,   -50,     0, -1499,   249,   200, "Easter Basin" },
        { -1620,  1176,     0, -1580,  1274,   200, "Esplanade East" },
        { -1580,  1025,     0, -1499,  1274,   200, "Esplanade East" },
        { -1499,   578,   -79, -1339,  1274,    20, "Esplanade East" },
        { -2324, -2584,     0, -1964, -2212,   200, "Angel Pine" },
        { -1632, -2263,     0, -1601, -2231,   200, "Shady Cabin" },
        { -1166, -2641,     0,  -321, -1856,   200, "Back o Beyond" },
        { -1166, -1856,     0,  -815, -1602,   200, "Leafy Hollow" },
        {  -594, -1648,     0,  -187, -1276,   200, "Flint Range" },
        {  -792,  -698,     0,  -452,  -380,   200, "Fallen Tree" },
        { -1209, -1317,   114,  -908,  -787,   251, "The Farm" },
        { -1645,  2498,     0, -1372,  2777,   200, "El Quebrados" },
        { -1372,  2498,     0, -1277,  2615,   200, "Aldea Malvada" },
        {  -968,  1929,     0,  -481,  2155,   200, "The Sherman Dam" },
        {  -926,  1398,     0,  -719,  1634,   200, "Las Barrancas" },
        {  -376,   826,     0,   123,  1220,   200, "Fort Carson" },
        {   337,   710,  -115,   860,  1031,   203, "Hunter Quarry" },
        {   338,  1228,     0,   664,  1655,   200, "Octane Springs" },
        {   176,  1305,     0,   338,  1520,   200, "Green Palms" },
        {  -405,  1712,     0,  -276,  1892,   200, "Regular Tom" },
        {  -365,  2123,     0,  -208,  2217,   200, "Las Brujas" },
        {    37,  2337,     0,   435,  2677,   200, "Verdant Meadows" },
        {  -354,  2580,     0,  -133,  2816,   200, "Las Payasadas" },
        {  -901,  2221,     0,  -592,  2571,   200, "Arco del Oeste" },
        { -1794,  -730,     0, -1213,   -50,   200, "Easter Bay Airport" },
        {  2576,    62,     0,  2759,   385,   200, "Hankypanky Point" },
        {  2160,  -149,     0,  2576,   228,   200, "Palomino Creek" },
        {  2285,  -768,     0,  2770,  -269,   200, "North Rock" },
        {  1119,   119,     0,  1451,   493,   200, "Montgomery" },
        {  1451,   347,     0,  1582,   420,   200, "Montgomery" },
        {   603,   264,     0,   761,   366,   200, "Hampton Barns" },
        {   508,  -139,     0,  1306,   119,   200, "Fern Ridge" },
        {   580,  -674,     0,   861,  -404,   200, "Dillimore" },
        {   967,  -450,     0,  1176,  -217,   200, "Hilltop Farm" },
        {   104,  -220,     0,   349,   152,   200, "Blueberry" },
        {    19,  -404,     0,   349,  -220,   200, "Blueberry" },
        {  -947,  -304,     0,  -319,   327,   200, "The Panopticon" },
        {  2759,   296,     0,  2774,   594,   200, "Frederick Bridge" },
        {  1664,   401,     0,  1785,   567,   200, "The Mako Span" },
        {  -319,  -220,     0,   104,   293,   200, "Blueberry Acres" },
        {  -222,   293,     0,  -122,   476,   200, "Martin Bridge" },
        {   434,   366,     0,   603,   555,   200, "Fallow Bridge" },
        { -1820, -2643,     0, -1226, -1771,   200, "Shady Creeks" },
        { -2030, -2174,     0, -1820, -1771,   200, "Shady Creeks" },
        { -2533,   458,     0, -2329,   578,   200, "Queens" },
        { -2593,    54,     0, -2411,   458,   200, "Queens" },
        { -2411,   373,     0, -2253,   458,   200, "Queens" },
        {    44, -2892,  -242,  2997,  -768,   900, "Los Santos" },
        {   869,   596,  -242,  2997,  2993,   900, "Las Venturas" },
        {  -480,   596,  -242,   869,  2993,   900, "Bone County" },
        { -2997,  1659,  -242,  -480,  2993,   900, "Tierra Robada" },
        { -2741,  1659,     0, -2616,  2175,   200, "Gant Bridge" },
        { -2741,  1490,     0, -2616,  1659,   200, "Gant Bridge" },
        { -2997, -1115,  -242, -1213,  1659,   900, "San Fierro" },
        { -1213,   596,  -242,  -480,  1659,   900, "Tierra Robada" },
        { -1213,  -768,  -242,  2997,   596,   900, "Red County" },
        { -1213, -2892,  -242,    44,  -768,   900, "Flint County" },
        { -1132,  -768,     0,  -956,  -578,   200, "Easter Bay Chemicals" },
        { -1132,  -787,     0,  -956,  -768,   200, "Easter Bay Chemicals" },
        { -1213,  -730,     0, -1132,   -50,   200, "Easter Bay Airport" },
        { -2178, -1115,     0, -1794,  -599,   200, "Foster Valley" },
        { -2178, -1250,     0, -1794, -1115,   200, "Foster Valley" },
        { -1242,   -50,     0, -1213,   578,   200, "Easter Bay Airport" },
        { -1213,   -50,     0,  -947,   578,   200, "Easter Bay Airport" },
        { -2997, -2892,  -242, -1213, -1115,   900, "Whetstone" },
        {  1249, -2394,   -89,  1852, -2179,   110, "Los Santos International" },
        {  1852, -2394,   -89,  2089, -2179,   110, "Los Santos International" },
        {   930, -2488,   -89,  1249, -2006,   110, "Verdant Bluffs" },
        {  1812, -2179,   -89,  1970, -1852,   110, "El Corona" },
        {  1970, -2179,   -89,  2089, -1852,   110, "Willowfield" },
        {  2089, -2235,   -89,  2201, -1989,   110, "Willowfield" },
        {  2089, -1989,   -89,  2324, -1852,   110, "Willowfield" },
        {  2201, -2095,   -89,  2324, -1989,   110, "Willowfield" },
        {  2373, -2697,   -89,  2809, -2330,   110, "Ocean Docks" },
        {  2201, -2418,   -89,  2324, -2095,   110, "Ocean Docks" },
        {   647, -1804,   -89,   851, -1577,   110, "Marina" },
        {   647, -2173,   -89,   930, -1804,   110, "Verona Beach" },
        {   930, -2006,   -89,  1073, -1804,   110, "Verona Beach" },
        {  1073, -2006,   -89,  1249, -1842,   110, "Verdant Bluffs" },
        {  1249, -2179,   -89,  1692, -1842,   110, "Verdant Bluffs" },
        {  1692, -2179,   -89,  1812, -1842,   110, "El Corona" },
        {   851, -1804,   -89,  1046, -1577,   110, "Verona Beach" },
        {   647, -1577,   -89,   807, -1416,   110, "Marina" },
        {   807, -1577,   -89,   926, -1416,   110, "Marina" },
        {  1161, -1722,   -89,  1323, -1577,   110, "Verona Beach" },
        {  1046, -1722,   -89,  1161, -1577,   110, "Verona Beach" },
        {  1046, -1804,   -89,  1323, -1722,   110, "Conference Center" },
        {  1073, -1842,   -89,  1323, -1804,   110, "Conference Center" },
        {  1323, -1842,   -89,  1701, -1722,   110, "Commerce" },
        {  1323, -1722,   -89,  1440, -1577,   110, "Commerce" },
        {  1370, -1577,   -89,  1463, -1384,   110, "Commerce" },
        {  1463, -1577,   -89,  1667, -1430,   110, "Commerce" },
        {  1440, -1722,   -89,  1583, -1577,   110, "Pershing Square" },
        {  1583, -1722,   -89,  1758, -1577,   110, "Commerce" },
        {  1701, -1842,   -89,  1812, -1722,   110, "Little Mexico" },
        {  1758, -1722,   -89,  1812, -1577,   110, "Little Mexico" },
        {  1667, -1577,   -89,  1812, -1430,   110, "Commerce" },
        {  1812, -1852,   -89,  1971, -1742,   110, "Idlewood" },
        {  1812, -1742,   -89,  1951, -1602,   110, "Idlewood" },
        {  1951, -1742,   -89,  2124, -1602,   110, "Idlewood" },
        {  1812, -1602,   -89,  2124, -1449,   110, "Idlewood" },
        {  2124, -1742,   -89,  2222, -1494,   110, "Idlewood" },
        {  1812, -1449,   -89,  1996, -1350,   110, "Glen Park" },
        {  1812, -1100,   -89,  1994,  -973,   110, "Glen Park" },
        {  1996, -1449,   -89,  2056, -1350,   110, "Jefferson" },
        {  2124, -1494,   -89,  2266, -1449,   110, "Jefferson" },
        {  2056, -1372,   -89,  2281, -1210,   110, "Jefferson" },
        {  2056, -1210,   -89,  2185, -1126,   110, "Jefferson" },
        {  2185, -1210,   -89,  2281, -1154,   110, "Jefferson" },
        {  1994, -1100,   -89,  2056,  -920,   110, "Las Colinas" },
        {  2056, -1126,   -89,  2126,  -920,   110, "Las Colinas" },
        {  2185, -1154,   -89,  2281,  -934,   110, "Las Colinas" },
        {  2126, -1126,   -89,  2185,  -934,   110, "Las Colinas" },
        {  1971, -1852,   -89,  2222, -1742,   110, "Idlewood" },
        {  2222, -1852,   -89,  2632, -1722,   110, "Ganton" },
        {  2222, -1722,   -89,  2632, -1628,   110, "Ganton" },
        {  2541, -1941,   -89,  2703, -1852,   110, "Willowfield" },
        {  2632, -1852,   -89,  2959, -1668,   110, "East Beach" },
        {  2632, -1668,   -89,  2747, -1393,   110, "East Beach" },
        {  2747, -1668,   -89,  2959, -1498,   110, "East Beach" },
        {  2421, -1628,   -89,  2632, -1454,   110, "East Los Santos" },
        {  2222, -1628,   -89,  2421, -1494,   110, "East Los Santos" },
        {  2056, -1449,   -89,  2266, -1372,   110, "Jefferson" },
        {  2266, -1494,   -89,  2381, -1372,   110, "East Los Santos" },
        {  2381, -1494,   -89,  2421, -1454,   110, "East Los Santos" },
        {  2281, -1372,   -89,  2381, -1135,   110, "East Los Santos" },
        {  2381, -1454,   -89,  2462, -1135,   110, "East Los Santos" },
        {  2462, -1454,   -89,  2581, -1135,   110, "East Los Santos" },
        {  2581, -1454,   -89,  2632, -1393,   110, "Los Flores" },
        {  2581, -1393,   -89,  2747, -1135,   110, "Los Flores" },
        {  2747, -1498,   -89,  2959, -1120,   110, "East Beach" },
        {  2747, -1120,   -89,  2959,  -945,   110, "Las Colinas" },
        {  2632, -1135,   -89,  2747,  -945,   110, "Las Colinas" },
        {  2281, -1135,   -89,  2632,  -945,   110, "Las Colinas" },
        {  1463, -1430,   -89,  1724, -1290,   110, "Downtown Los Santos" },
        {  1724, -1430,   -89,  1812, -1250,   110, "Downtown Los Santos" },
        {  1463, -1290,   -89,  1724, -1150,   110, "Downtown Los Santos" },
        {  1370, -1384,   -89,  1463, -1170,   110, "Downtown Los Santos" },
        {  1724, -1250,   -89,  1812, -1150,   110, "Downtown Los Santos" },
        {  1463, -1150,   -89,  1812,  -768,   110, "Mulholland Intersection" },
        {  1414,  -768,   -89,  1667,  -452,   110, "Mulholland" },
        {  1281,  -452,   -89,  1641,  -290,   110, "Mulholland" },
        {  1269,  -768,   -89,  1414,  -452,   110, "Mulholland" },
        {   787, -1416,   -89,  1072, -1310,   110, "Market" },
        {   787, -1310,   -89,   952, -1130,   110, "Vinewood" },
        {   952, -1310,   -89,  1072, -1130,   110, "Market" },
        {  1370, -1170,   -89,  1463, -1130,   110, "Downtown Los Santos" },
        {  1378, -1130,   -89,  1463, -1026,   110, "Downtown Los Santos" },
        {  1391, -1026,   -89,  1463,  -926,   110, "Downtown Los Santos" },
        {  1252, -1130,   -89,  1378, -1026,   110, "Temple" },
        {  1252, -1026,   -89,  1391,  -926,   110, "Temple" },
        {  1252,  -926,   -89,  1357,  -910,   110, "Temple" },
        {  1357,  -926,   -89,  1463,  -768,   110, "Mulholland" },
        {  1318,  -910,   -89,  1357,  -768,   110, "Mulholland" },
        {  1169,  -910,   -89,  1318,  -768,   110, "Mulholland" },
        {   787, -1130,   -89,   952,  -954,   110, "Vinewood" },
        {   952, -1130,   -89,  1096,  -937,   110, "Temple" },
        {  1096, -1130,   -89,  1252, -1026,   110, "Temple" },
        {  1096, -1026,   -89,  1252,  -910,   110, "Temple" },
        {   768,  -954,   -89,   952,  -860,   110, "Mulholland" },
        {   687,  -860,   -89,   911,  -768,   110, "Mulholland" },
        {   737,  -768,   -89,  1142,  -674,   110, "Mulholland" },
        {  1096,  -910,   -89,  1169,  -768,   110, "Mulholland" },
        {   952,  -937,   -89,  1096,  -860,   110, "Mulholland" },
        {   911,  -860,   -89,  1096,  -768,   110, "Mulholland" },
        {   861,  -674,   -89,  1156,  -600,   110, "Mulholland" },
        {   342, -2173,   -89,   647, -1684,   110, "Santa Maria Beach" },
        {    72, -2173,   -89,   342, -1684,   110, "Santa Maria Beach" },
        {    72, -1684,   -89,   225, -1544,   110, "Rodeo" },
        {    72, -1544,   -89,   225, -1404,   110, "Rodeo" },
        {   225, -1684,   -89,   312, -1501,   110, "Rodeo" },
        {   225, -1501,   -89,   334, -1369,   110, "Rodeo" },
        {   334, -1501,   -89,   422, -1406,   110, "Rodeo" },
        {   312, -1684,   -89,   422, -1501,   110, "Rodeo" },
        {   422, -1684,   -89,   558, -1570,   110, "Rodeo" },
        {   558, -1684,   -89,   647, -1384,   110, "Rodeo" },
        {   466, -1570,   -89,   558, -1385,   110, "Rodeo" },
        {   422, -1570,   -89,   466, -1406,   110, "Rodeo" },
        {   647, -1227,   -89,   787, -1118,   110, "Vinewood" },
        {   647, -1118,   -89,   787,  -954,   110, "Richman" },
        {   647,  -954,   -89,   768,  -860,   110, "Richman" },
        {   466, -1385,   -89,   647, -1235,   110, "Rodeo" },
        {   334, -1406,   -89,   466, -1292,   110, "Rodeo" },
        {   225, -1369,   -89,   334, -1292,   110, "Richman" },
        {   225, -1292,   -89,   466, -1235,   110, "Richman" },
        {    72, -1404,   -89,   225, -1235,   110, "Richman" },
        {    72, -1235,   -89,   321, -1008,   110, "Richman" },
        {   321, -1235,   -89,   647, -1044,   110, "Richman" },
        {   321, -1044,   -89,   647,  -860,   110, "Richman" },
        {   321,  -860,   -89,   687,  -768,   110, "Richman" },
        {   321,  -768,   -89,   700,  -674,   110, "Richman" },
        {  2027,   863,   -89,  2087,  1703,   110, "The Strip" },
        {  2106,  1863,   -89,  2162,  2202,   110, "The Strip" },
        {  1817,   863,   -89,  2027,  1083,   110, "The Four Dragons Casino" },
        {  1817,  1083,   -89,  2027,  1283,   110, "The Pink Swan" },
        {  1817,  1283,   -89,  2027,  1469,   110, "The High Roller" },
        {  1817,  1469,   -89,  2027,  1703,   110, "Pirates in Men's Pants" },
        {  1817,  1863,   -89,  2106,  2011,   110, "The Visage" },
        {  1817,  1703,   -89,  2027,  1863,   110, "The Visage" },
        {  1457,   823,   -89,  2377,   863,   110, "Julius Thruway South" },
        {  1197,  1163,   -89,  1236,  2243,   110, "Julius Thruway West" },
        {  2377,   788,   -89,  2537,   897,   110, "Julius Thruway South" },
        {  2537,   676,   -89,  2902,   943,   110, "Rockshore East" },
        {  2087,   943,   -89,  2623,  1203,   110, "Come-A-Lot" },
        {  2087,  1203,   -89,  2640,  1383,   110, "The Camel's Toe" },
        {  2087,  1383,   -89,  2437,  1543,   110, "Royal Casino" },
        {  2087,  1543,   -89,  2437,  1703,   110, "Caligula's Palace" },
        {  2137,  1703,   -89,  2437,  1783,   110, "Caligula's Palace" },
        {  2437,  1383,   -89,  2624,  1783,   110, "Pilgrim" },
        {  2437,  1783,   -89,  2685,  2012,   110, "Starfish Casino" },
        {  2027,  1783,   -89,  2162,  1863,   110, "The Strip" },
        {  2027,  1703,   -89,  2137,  1783,   110, "The Strip" },
        {  2011,  2202,   -89,  2237,  2508,   110, "The Emerald Isle" },
        {  2162,  2012,   -89,  2685,  2202,   110, "Old Venturas Strip" },
        {  2498,  2626,   -89,  2749,  2861,   110, "K.A.C.C. Military Fuels" },
        {  2749,  1937,   -89,  2921,  2669,   110, "Creek" },
        {  2749,  1548,   -89,  2923,  1937,   110, "Sobell Rail Yards" },
        {  2749,  1198,   -89,  2923,  1548,   110, "Linden Station" },
        {  2623,   943,   -89,  2749,  1055,   110, "Julius Thruway East" },
        {  2749,   943,   -89,  2923,  1198,   110, "Linden Side" },
        {  2685,  1055,   -89,  2749,  2626,   110, "Julius Thruway East" },
        {  2498,  2542,   -89,  2685,  2626,   110, "Julius Thruway North" },
        {  2536,  2442,   -89,  2685,  2542,   110, "Julius Thruway East" },
        {  2625,  2202,   -89,  2685,  2442,   110, "Julius Thruway East" },
        {  2237,  2542,   -89,  2498,  2663,   110, "Julius Thruway North" },
        {  2121,  2508,   -89,  2237,  2663,   110, "Julius Thruway North" },
        {  1938,  2508,   -89,  2121,  2624,   110, "Julius Thruway North" },
        {  1534,  2433,   -89,  1848,  2583,   110, "Julius Thruway North" },
        {  1236,  2142,   -89,  1297,  2243,   110, "Julius Thruway West" },
        {  1848,  2478,   -89,  1938,  2553,   110, "Julius Thruway North" },
        {  1777,   863,   -89,  1817,  2342,   110, "Harry Gold Parkway" },
        {  1817,  2011,   -89,  2106,  2202,   110, "Redsands East" },
        {  1817,  2202,   -89,  2011,  2342,   110, "Redsands East" },
        {  1848,  2342,   -89,  2011,  2478,   110, "Redsands East" },
        {  1704,  2342,   -89,  1848,  2433,   110, "Julius Thruway North" },
        {  1236,  1883,   -89,  1777,  2142,   110, "Redsands West" },
        {  1297,  2142,   -89,  1777,  2243,   110, "Redsands West" },
        {  1377,  2243,   -89,  1704,  2433,   110, "Redsands West" },
        {  1704,  2243,   -89,  1777,  2342,   110, "Redsands West" },
        {  1236,  1203,   -89,  1457,  1883,   110, "Las Venturas Airport" },
        {  1457,  1203,   -89,  1777,  1883,   110, "Las Venturas Airport" },
        {  1457,  1143,   -89,  1777,  1203,   110, "Las Venturas Airport" },
        {  1457,   863,   -89,  1777,  1143,   110, "LVA Freight Depot" },
        {  1197,  1044,   -89,  1277,  1163,   110, "Blackfield Intersection" },
        {  1166,   795,   -89,  1375,  1044,   110, "Blackfield Intersection" },
        {  1277,  1044,   -89,  1315,  1087,   110, "Blackfield Intersection" },
        {  1375,   823,   -89,  1457,   919,   110, "Blackfield Intersection" },
        {  1375,   919,   -89,  1457,  1203,   110, "LVA Freight Depot" },
        {  1277,  1087,   -89,  1375,  1203,   110, "LVA Freight Depot" },
        {  1315,  1044,   -89,  1375,  1087,   110, "LVA Freight Depot" },
        {  1236,  1163,   -89,  1277,  1203,   110, "LVA Freight Depot" },
        {   964,  1044,   -89,  1197,  1203,   110, "Greenglass College" },
        {   964,   930,   -89,  1166,  1044,   110, "Greenglass College" },
        {   964,  1203,   -89,  1197,  1403,   110, "Blackfield" },
        {   964,  1403,   -89,  1197,  1726,   110, "Blackfield" },
        {  2237,  2202,   -89,  2536,  2542,   110, "Roca Escalante" },
        {  2536,  2202,   -89,  2625,  2442,   110, "Roca Escalante" },
        {  1823,   596,   -89,  1997,   823,   110, "Last Dime Motel" },
        {  1997,   596,   -89,  2377,   823,   110, "Rockshore West" },
        {  2377,   596,   -89,  2537,   788,   110, "Rockshore West" },
        {  1558,   596,   -89,  1823,   823,   110, "Randolph Industrial Estate" },
        {  1375,   596,   -89,  1558,   823,   110, "Blackfield Chapel" },
        {  1325,   596,   -89,  1375,   795,   110, "Blackfield Chapel" },
        {  1377,  2433,   -89,  1534,  2507,   110, "Julius Thruway North" },
        {  1098,  2243,   -89,  1377,  2507,   110, "Pilson Intersection" },
        {   883,  1726,   -89,  1098,  2507,   110, "Whitewood Estates" },
        {  1534,  2583,   -89,  1848,  2863,   110, "Prickle Pine" },
        {  1117,  2507,   -89,  1534,  2723,   110, "Prickle Pine" },
        {  1848,  2553,   -89,  1938,  2863,   110, "Prickle Pine" },
        {  2121,  2663,   -89,  2498,  2861,   110, "Spinybed" },
        {  1938,  2624,   -89,  2121,  2861,   110, "Prickle Pine" },
        {  2624,  1383,   -89,  2685,  1783,   110, "Pilgrim" },
        {  2450,   385,  -100,  2759,   562,   200, "San Andreas Sound" },
        {  1916,  -233,  -100,  2131,    13,   200, "Fisher's Lagoon" },
        { -1339,   828,   -89, -1213,  1057,   110, "Garver Bridge" },
        { -1213,   950,   -89, -1087,  1178,   110, "Garver Bridge" },
        { -1499,   696,  -179, -1339,   925,    20, "Garver Bridge" },
        { -1339,   599,   -89, -1213,   828,   110, "Kincaid Bridge" },
        { -1213,   721,   -89, -1087,   950,   110, "Kincaid Bridge" },
        { -1087,   855,   -89,  -961,   986,   110, "Kincaid Bridge" },
        {  -321, -2224,   -89,    44, -1724,   110, "Los Santos Inlet" },
        {  -789,  1659,   -89,  -599,  1929,   110, "Sherman Reservoir" },
        {  -314,  -753,   -89,  -106,  -463,   110, "Flint Water" },
        { -1709,  -833,     0, -1446,  -730,   200, "Easter Tunnel" },
        { -2290,  2548,   -89, -1950,  2723,   110, "Bayside Tunnel" },
        {  -410,  1403,     0,  -137,  1681,   200, "'The Big Ear'" },
        {   -90,  1286,     0,   153,  1554,   200, "Lil' Probe Inn" },
        {  -936,  2611,     0,  -715,  2847,   200, "Valle Ocultado" },
        {  1812, -1350,   -89,  2056, -1100,   110, "Glen Park" },
        {  2324, -2302,   -89,  2703, -2145,   110, "Ocean Docks" },
        {  2811,  1229,   -39,  2861,  1407,    60, "Linden Station" },
        {  1692, -1971,   -20,  1812, -1932,    79, "Unity Station" },
        {   647, -1416,   -89,   787, -1227,   110, "Vinewood" },
        {   787, -1410,   -34,   866, -1310,    65, "Market Station" },
        { -2007,    56,     0, -1922,   224,   100, "Cranberry Station" },
        {  1377,  2600,   -21,  1492,  2687,    78, "Yellow Bell Station" },
        { -2616,  1501,     0, -1996,  1659,   200, "San Fierro Bay" },
        { -2616,  1659,     0, -1996,  2175,   200, "San Fierro Bay" },
        {  -464,  2217,     0,  -208,  2580,   200, "El Castillo del Diablo" },
        {  -208,  2123,     0,   114,  2337,   200, "El Castillo del Diablo" },
        {  -208,  2337,     0,     8,  2487,   200, "El Castillo del Diablo" },
        {   -91,  1655,   -50,   421,  2123,   250, "Restricted Area" },
        {  1546,   208,     0,  1745,   347,   200, "Montgomery Intersection" },
        {  1582,   347,     0,  1664,   401,   200, "Montgomery Intersection" },
        { -1119,  1178,   -89,  -862,  1351,   110, "Robada Intersection" },
        {  -187, -1596,   -89,    17, -1276,   110, "Flint Intersection" },
        { -1315,  -405,    15, -1264,  -209,    25, "Easter Bay Airport" },
        { -1354,  -287,    15, -1315,  -209,    25, "Easter Bay Airport" },
        { -1490,  -209,    15, -1264,  -148,    25, "Easter Bay Airport" },
        {  1072, -1416,   -89,  1370, -1130,   110, "Market" },
        {   926, -1577,   -89,  1370, -1416,   110, "Market" },
        { -2646,  -355,     0, -2270,  -222,   200, "Avispa Country Club" },
        { -2831,  -430,     0, -2646,  -222,   200, "Avispa Country Club" },
        { -2994,  -811,     0, -2178,  -430,   200, "Missionary Hill" },
        { -2178, -1771,   -47, -1936, -1250,   576, "Mount Chiliad" },
        { -2997, -1115,   -47, -2178,  -971,   576, "Mount Chiliad" },
        { -2994, -2189,   -47, -2178, -1115,   576, "Mount Chiliad" },
        { -2178, -2189,   -47, -2030, -1771,   576, "Mount Chiliad" },
        {  1117,  2723,   -89,  1457,  2863,   110, "Yellow Bell Golf Course" },
        {  1457,  2723,   -89,  1534,  2863,   110, "Yellow Bell Golf Course" },
        {  1515,  1586,   -12,  1729,  1714,    87, "Las Venturas Airport" },
        {  2089, -2394,   -89,  2201, -2235,   110, "Ocean Docks" },
        {  1382, -2730,   -89,  2201, -2394,   110, "Los Santos International" },
        {  2201, -2730,   -89,  2324, -2418,   110, "Ocean Docks" },
        {  1974, -2394,   -39,  2089, -2256,    60, "Los Santos International" },
        {  1400, -2669,   -39,  2189, -2597,    60, "Los Santos International" },
        {  2051, -2597,   -39,  2152, -2394,    60, "Los Santos International" },
        {  2437,  1858,   -39,  2495,  1970,    60, "Starfish Casino" },
        {  -399, -1075,    -1,  -319,  -977,   198, "Beacon Hill" },
        { -2361,  -417,     0, -2270,  -355,   200, "Avispa Country Club" },
        { -2667,  -302,   -28, -2646,  -262,    71, "Avispa Country Club" },
        { -2395,  -222,     0, -2354,  -204,   200, "Garcia" },
        { -2470,  -355,     0, -2270,  -318,    46, "Avispa Country Club" },
        { -2550,  -355,     0, -2470,  -318,    39, "Avispa Country Club" },
        {  2703, -2126,   -89,  2959, -1852,   110, "Playa del Seville" },
        {  2703, -2302,   -89,  2959, -2126,   110, "Ocean Docks" },
        {  2162,  1883,   -89,  2437,  2012,   110, "Starfish Casino" },
        {  2162,  1783,   -89,  2437,  1883,   110, "The Clown's Pocket" },
        {  2324, -2145,   -89,  2703, -2059,   110, "Ocean Docks" },
        {  2324, -2059,   -89,  2541, -1852,   110, "Willowfield" },
        {  2541, -2059,   -89,  2703, -1941,   110, "Willowfield" },
        {  1098,  1726,   -89,  1197,  2243,   110, "Whitewood Estates" },
    };
static const IMPLEMENT_FIXED_ARRAY( SZone, zoneInfoList );


CZoneNames::CZoneNames ( void )
{
    // Initialize zone tree
    for ( uint i = 0 ; i < NUMELMS( zoneInfoList ) ; i++ )
    {
        const SZone* pZone = &zoneInfoList[i];
        CVector vecBottomLeft ( pZone->x1, pZone->y1, pZone->z1 );
        CVector vecTopRight ( pZone->x2, pZone->y2, pZone->z2 );
        m_ZoneTree.Insert ( &vecBottomLeft.fX, &vecTopRight.fX, pZone );
    }

    // Initialize city name map
    for ( uint i = 0 ; i < NUMELMS( cityNameList ) ; i++ )
        MapInsert ( m_CityNameMap, cityNameList[i] );
}


//
// Get zone name at position
//
const char* CZoneNames::GetZoneName ( const CVector& vecPosition )
{
    // Check inside map bounds
    if ( vecPosition.fX < -3000 ||
         vecPosition.fY < -3000 ||
         vecPosition.fZ < -3000 ||
         vecPosition.fX > 3000 ||
         vecPosition.fY > 3000 ||
         vecPosition.fZ > 3000 )
    {
        return szUnknownZone;
    }

    // Use a gradually increasing sphere to get the nearest zone
    float fRadius = 0;
    for ( uint i = 0 ; i < 10 ; i++ )
    {
        const SZone* pZone = GetSmallestZoneInSphere ( vecPosition, fRadius );
        if ( pZone )
            return pZone->szName;

        fRadius = pow ( 2.f, (float)i );
    }

    return szUnknownZone;
}


//
// Get best match zone within 2D distance of position
//
const SZone* CZoneNames::GetSmallestZoneInSphere ( const CVector& vecPosition, float fRadius )
{
    CBox box ( vecPosition, fRadius );
    box.vecMin.fZ = vecPosition.fZ;
    box.vecMax.fZ = vecPosition.fZ;

    std::vector < const SZone* > results;
    m_ZoneTree.Search( &box.vecMin.fX, &box.vecMax.fX, results );

    float fSmallestSize = 0;
    const SZone* pSmallestZone = NULL;
    for ( uint i = 0 ; i < results.size () ; i++ )
    {
        const SZone* pZone = results[i];
        float sizeX = static_cast < float > ( pZone->x2 - pZone->x1 );
        float sizeY = static_cast < float > ( pZone->y2 - pZone->y1 );
        float sizeZ = static_cast < float > ( pZone->z2 - pZone->z1 );
        float fZoneSize = sizeX * sizeY * sizeZ;

        if ( !pSmallestZone || fZoneSize < fSmallestSize )
        {
            fSmallestSize = fZoneSize;
            pSmallestZone = pZone;
        }
    }
    return pSmallestZone;
}


//
// Get city name at position
//
const char* CZoneNames::GetCityName ( const CVector& vecPosition )
{
    // Check inside map bounds
    if ( vecPosition.fX < -3000 ||
         vecPosition.fY < -3000 ||
         vecPosition.fZ < -3000 ||
         vecPosition.fX > 3000 ||
         vecPosition.fY > 3000 ||
         vecPosition.fZ > 3000 )
    {
        return szUnknownZone;
    }

    // Use a gradually increasing sphere to get the nearest city zone
    float fRadius = 0;
    for ( uint i = 0 ; i < 10 ; i++ )
    {
        const SZone* pZone = GetCityZoneInSphere ( vecPosition, fRadius );
        if ( pZone )
            return pZone->szName;

        fRadius = pow ( 2.f, (float)i );
    }

    return szUnknownZone;
}


//
// Get city zone within 2D distance of position
//
const SZone* CZoneNames::GetCityZoneInSphere ( const CVector& vecPosition, float fRadius )
{
    CBox box ( vecPosition, fRadius );
    box.vecMin.fZ = vecPosition.fZ;
    box.vecMax.fZ = vecPosition.fZ;

    std::vector < const SZone* > results;
    m_ZoneTree.Search( &box.vecMin.fX, &box.vecMax.fX, results );

    for ( uint i = 0 ; i < results.size () ; i++ )
    {
        const SZone* pZone = results[i];
        if ( MapContains ( m_CityNameMap, pZone->szName ) )
            return pZone;
    }

    return NULL;
}
