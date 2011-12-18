/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CZoneNames.cpp
*  PURPOSE:     GTA world zone name definitions class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

const char szUnknownZone [ 8 ] = "Unknown";

CZoneNames::CZoneNames ( void )
{
    LoadZones ();
    LoadCities ();
}


CZoneNames::~CZoneNames ( void )
{
    ClearZones ();
    ClearCities ();
}


void CZoneNames::AddZone ( const char* szName, const CVector vecBottomLeft, const CVector vecTopRight )
{
    SZone* pZone = new SZone;
    strncpy ( pZone->szName, szName, MAX_ZONE_NAME );
    pZone->szName [ MAX_ZONE_NAME - 1 ] = 0;
    pZone->vecBottomLeft = vecBottomLeft;
    pZone->vecTopRight = vecTopRight;
    m_Zones.push_back ( pZone );
}


SZone* CZoneNames::GetZone ( const CVector vecPosition )
{
    list < SZone* > ::iterator iter = m_Zones.begin ();
    for ( ; iter != m_Zones.end (); iter++ )
    {
        SZone* pZone = *iter;
        CVector* vecBottomLeft = &pZone->vecBottomLeft;
        CVector* vecTopRight = &pZone->vecTopRight;
        
        if ( vecPosition.fX >= vecBottomLeft->fX &&
             vecPosition.fY >= vecBottomLeft->fY &&
             vecPosition.fZ >= vecBottomLeft->fZ &&
             vecPosition.fX <= vecTopRight->fX &&
             vecPosition.fY <= vecTopRight->fY &&
             vecPosition.fZ <= vecTopRight->fZ )
        {
            return pZone;
        }
    }
    return NULL;
}


const char* CZoneNames::GetZoneName ( const CVector vecPosition )
{
    SZone* pZone = GetZone ( vecPosition );
    if ( pZone )
    {
        return pZone->szName;
    }
    return szUnknownZone;
}


void CZoneNames::AddCity ( const char* szName, const CVector vecBottomLeft, const CVector vecTopRight )
{
    SZone* pZone = new SZone;
    strncpy ( pZone->szName, szName, MAX_ZONE_NAME );
    pZone->szName [ MAX_ZONE_NAME - 1 ] = 0;
    pZone->vecBottomLeft = vecBottomLeft;
    pZone->vecTopRight = vecTopRight;
    m_Cities.push_back ( pZone );
}


SZone* CZoneNames::GetCity ( CVector vecPosition )
{
    SZone* pSmallestZone = NULL;
    float fSmallestZoneSize = 0.0f;

    list < SZone* > ::iterator iter = m_Cities.begin ();
    for ( ; iter != m_Cities.end (); iter++ )
    {
        SZone* pZone = *iter;
        CVector* vecBottomLeft = &pZone->vecBottomLeft;
        CVector* vecTopRight = &pZone->vecTopRight;
        if ( vecPosition.fX >= vecBottomLeft->fX &&
            vecPosition.fY >= vecBottomLeft->fY &&
            vecPosition.fZ >= vecBottomLeft->fZ &&
            vecPosition.fX <= vecTopRight->fX &&
            vecPosition.fY <= vecTopRight->fY &&
            vecPosition.fZ <= vecTopRight->fZ )
        {
            float fWidth = vecTopRight->fX - vecBottomLeft->fX;
            float fDepth = vecTopRight->fY - vecBottomLeft->fY;
            float fHeight = vecTopRight->fZ - vecBottomLeft->fZ;
            float fZoneSize = ( fWidth * fDepth * fHeight );

            if ( pSmallestZone == NULL || fZoneSize < fSmallestZoneSize )
            {
                pSmallestZone = pZone;
                fSmallestZoneSize = fZoneSize;
            }
        }
    }

    return pSmallestZone;
}


const char* CZoneNames::GetCityName ( const CVector vecPosition )
{
    SZone* pZone = GetCity ( vecPosition );
    if ( pZone )
    {
        return pZone->szName;
    }
    return szUnknownZone;
}


void CZoneNames::LoadZones ( void )
{
    AddZone ( "Easter Bay Airport", CVector ( -1354.390f, -287.398f, 15.406f ), CVector ( -1315.420f, -209.543f, 25.406f ) );
    AddZone ( "Avispa Country Club", CVector ( -2667.810f, -302.135f, -28.831f ), CVector ( -2646.400f, -262.320f, 71.169f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1315.420f, -405.388f, 15.406f ), CVector ( -1264.400f, -209.543f, 25.406f ) );
    AddZone ( "Avispa Country Club", CVector ( -2550.040f, -355.493f, 0.000f ), CVector ( -2470.040f, -318.493f, 39.700f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1490.330f, -209.543f, 15.406f ), CVector ( -1264.400f, -148.388f, 25.406f ) );
    AddZone ( "Garcia", CVector ( -2395.140f, -222.589f, -5.340580e-5f ), CVector ( -2354.090f, -204.792f, 200.000f ) );
    AddZone ( "Shady Cabin", CVector ( -1632.830f, -2263.440f, -3.051760e-5f ), CVector ( -1601.330f, -2231.790f, 200.000f ) );
    AddZone ( "East Los Santos", CVector ( 2381.680f, -1494.030f, -89.084f ), CVector ( 2421.030f, -1454.350f, 110.916f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1236.630f, 1163.410f, -89.084f ), CVector ( 1277.050f, 1203.280f, 110.916f ) );
    AddZone ( "Blackfield Intersection", CVector ( 1277.050f, 1044.690f, -89.084f ), CVector ( 1315.350f, 1087.630f, 110.916f ) );
    AddZone ( "Avispa Country Club", CVector ( -2470.040f, -355.493f, 0.000f ), CVector ( -2270.040f, -318.493f, 46.100f ) );
    AddZone ( "Temple", CVector ( 1252.330f, -926.999f, -89.084f ), CVector ( 1357.000f, -910.170f, 110.916f ) );
    AddZone ( "Unity Station", CVector ( 1692.620f, -1971.800f, -20.492f ), CVector ( 1812.620f, -1932.800f, 79.508f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1315.350f, 1044.690f, -89.084f ), CVector ( 1375.600f, 1087.630f, 110.916f ) );
    AddZone ( "Los Flores", CVector ( 2581.730f, -1454.350f, -89.084f ), CVector ( 2632.830f, -1393.420f, 110.916f ) );
    AddZone ( "Starfish Casino", CVector ( 2437.390f, 1858.100f, -39.084f ), CVector ( 2495.090f, 1970.850f, 60.916f ) );
    AddZone ( "Easter Bay Chemicals", CVector ( -1132.820f, -787.391f, 0.000f ), CVector ( -956.476f, -768.027f, 200.000f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1370.850f, -1170.870f, -89.084f ), CVector ( 1463.900f, -1130.850f, 110.916f ) );
    AddZone ( "Esplanade East", CVector ( -1620.300f, 1176.520f, -4.577640e-5f ), CVector ( -1580.010f, 1274.260f, 200.000f ) );
    AddZone ( "Market Station", CVector ( 787.461f, -1410.930f, -34.126f ), CVector ( 866.009f, -1310.210f, 65.874f ) );
    AddZone ( "Linden Station", CVector ( 2811.250f, 1229.590f, -39.594f ), CVector ( 2861.250f, 1407.590f, 60.406f ) );
    AddZone ( "Montgomery Intersection", CVector ( 1582.440f, 347.457f, 0.000f ), CVector ( 1664.620f, 401.750f, 200.000f ) );
    AddZone ( "Frederick Bridge", CVector ( 2759.250f, 296.501f, 0.000f ), CVector ( 2774.250f, 594.757f, 200.000f ) );
    AddZone ( "Yellow Bell Station", CVector ( 1377.480f, 2600.430f, -21.926f ), CVector ( 1492.450f, 2687.360f, 78.074f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1507.510f, -1385.210f, 110.916f ), CVector ( 1582.550f, -1325.310f, 335.916f ) );
    AddZone ( "Jefferson", CVector ( 2185.330f, -1210.740f, -89.084f ), CVector ( 2281.450f, -1154.590f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1318.130f, -910.170f, -89.084f ), CVector ( 1357.000f, -768.027f, 110.916f ) );
    AddZone ( "Avispa Country Club", CVector ( -2361.510f, -417.199f, 0.000f ), CVector ( -2270.040f, -355.493f, 200.000f ) );
    AddZone ( "Jefferson", CVector ( 1996.910f, -1449.670f, -89.084f ), CVector ( 2056.860f, -1350.720f, 110.916f ) );
    AddZone ( "Julius Thruway West", CVector ( 1236.630f, 2142.860f, -89.084f ), CVector ( 1297.470f, 2243.230f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2124.660f, -1494.030f, -89.084f ), CVector ( 2266.210f, -1449.670f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1848.400f, 2478.490f, -89.084f ), CVector ( 1938.800f, 2553.490f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 422.680f, -1570.200f, -89.084f ), CVector ( 466.223f, -1406.050f, 110.916f ) );
    AddZone ( "Cranberry Station", CVector ( -2007.830f, 56.306f, 0.000f ), CVector ( -1922.000f, 224.782f, 100.000f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1391.050f, -1026.330f, -89.084f ), CVector ( 1463.900f, -926.999f, 110.916f ) );
    AddZone ( "Redsands West", CVector ( 1704.590f, 2243.230f, -89.084f ), CVector ( 1777.390f, 2342.830f, 110.916f ) );
    AddZone ( "Little Mexico", CVector ( 1758.900f, -1722.260f, -89.084f ), CVector ( 1812.620f, -1577.590f, 110.916f ) );
    AddZone ( "Blackfield Intersection", CVector ( 1375.600f, 823.228f, -89.084f ), CVector ( 1457.390f, 919.447f, 110.916f ) );
    AddZone ( "Los Santos International", CVector ( 1974.630f, -2394.330f, -39.084f ), CVector ( 2089.000f, -2256.590f, 60.916f ) );
    AddZone ( "Beacon Hill", CVector ( -399.633f, -1075.520f, -1.489f ), CVector ( -319.033f, -977.516f, 198.511f ) );
    AddZone ( "Rodeo", CVector ( 334.503f, -1501.950f, -89.084f ), CVector ( 422.680f, -1406.050f, 110.916f ) );
    AddZone ( "Richman", CVector ( 225.165f, -1369.620f, -89.084f ), CVector ( 334.503f, -1292.070f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1724.760f, -1250.900f, -89.084f ), CVector ( 1812.620f, -1150.870f, 110.916f ) );
    AddZone ( "The Strip", CVector ( 2027.400f, 1703.230f, -89.084f ), CVector ( 2137.400f, 1783.230f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1378.330f, -1130.850f, -89.084f ), CVector ( 1463.900f, -1026.330f, 110.916f ) );
    AddZone ( "Blackfield Intersection", CVector ( 1197.390f, 1044.690f, -89.084f ), CVector ( 1277.050f, 1163.390f, 110.916f ) );
    AddZone ( "Conference Center", CVector ( 1073.220f, -1842.270f, -89.084f ), CVector ( 1323.900f, -1804.210f, 110.916f ) );
    AddZone ( "Montgomery", CVector ( 1451.400f, 347.457f, -6.103520e-5f ), CVector ( 1582.440f, 420.802f, 200.000f ) );
    AddZone ( "Foster Valley", CVector ( -2270.040f, -430.276f, -1.220700e-4f ), CVector ( -2178.690f, -324.114f, 200.000f ) );
    AddZone ( "Blackfield Chapel", CVector ( 1325.600f, 596.349f, -89.084f ), CVector ( 1375.600f, 795.010f, 110.916f ) );
    AddZone ( "Los Santos International", CVector ( 2051.630f, -2597.260f, -39.084f ), CVector ( 2152.450f, -2394.330f, 60.916f ) );
    AddZone ( "Mulholland", CVector ( 1096.470f, -910.170f, -89.084f ), CVector ( 1169.130f, -768.027f, 110.916f ) );
    AddZone ( "Yellow Bell Golf Course", CVector ( 1457.460f, 2723.230f, -89.084f ), CVector ( 1534.560f, 2863.230f, 110.916f ) );
    AddZone ( "The Strip", CVector ( 2027.400f, 1783.230f, -89.084f ), CVector ( 2162.390f, 1863.230f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2056.860f, -1210.740f, -89.084f ), CVector ( 2185.330f, -1126.320f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 952.604f, -937.184f, -89.084f ), CVector ( 1096.470f, -860.619f, 110.916f ) );
    AddZone ( "Aldea Malvada", CVector ( -1372.140f, 2498.520f, 0.000f ), CVector ( -1277.590f, 2615.350f, 200.000f ) );
    AddZone ( "Las Colinas", CVector ( 2126.860f, -1126.320f, -89.084f ), CVector ( 2185.330f, -934.489f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 1994.330f, -1100.820f, -89.084f ), CVector ( 2056.860f, -920.815f, 110.916f ) );
    AddZone ( "Richman", CVector ( 647.557f, -954.662f, -89.084f ), CVector ( 768.694f, -860.619f, 110.916f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1277.050f, 1087.630f, -89.084f ), CVector ( 1375.600f, 1203.280f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1377.390f, 2433.230f, -89.084f ), CVector ( 1534.560f, 2507.230f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2201.820f, -2095.000f, -89.084f ), CVector ( 2324.000f, -1989.900f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1704.590f, 2342.830f, -89.084f ), CVector ( 1848.400f, 2433.230f, 110.916f ) );
    AddZone ( "Temple", CVector ( 1252.330f, -1130.850f, -89.084f ), CVector ( 1378.330f, -1026.330f, 110.916f ) );
    AddZone ( "Little Mexico", CVector ( 1701.900f, -1842.270f, -89.084f ), CVector ( 1812.620f, -1722.260f, 110.916f ) );
    AddZone ( "Queens", CVector ( -2411.220f, 373.539f, 0.000f ), CVector ( -2253.540f, 458.411f, 200.000f ) );
    AddZone ( "Las Venturas Airport", CVector ( 1515.810f, 1586.400f, -12.500f ), CVector ( 1729.950f, 1714.560f, 87.500f ) );
    AddZone ( "Richman", CVector ( 225.165f, -1292.070f, -89.084f ), CVector ( 466.223f, -1235.070f, 110.916f ) );
    AddZone ( "Temple", CVector ( 1252.330f, -1026.330f, -89.084f ), CVector ( 1391.050f, -926.999f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2266.260f, -1494.030f, -89.084f ), CVector ( 2381.680f, -1372.040f, 110.916f ) );
    AddZone ( "Julius Thruway East", CVector ( 2623.180f, 943.235f, -89.084f ), CVector ( 2749.900f, 1055.960f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2541.700f, -1941.400f, -89.084f ), CVector ( 2703.580f, -1852.870f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2056.860f, -1126.320f, -89.084f ), CVector ( 2126.860f, -920.815f, 110.916f ) );
    AddZone ( "Julius Thruway East", CVector ( 2625.160f, 2202.760f, -89.084f ), CVector ( 2685.160f, 2442.550f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 225.165f, -1501.950f, -89.084f ), CVector ( 334.503f, -1369.620f, 110.916f ) );
    AddZone ( "Las Brujas", CVector ( -365.167f, 2123.010f, -3.051760e-5f ), CVector ( -208.570f, 2217.680f, 200.000f ) );
    AddZone ( "Julius Thruway East", CVector ( 2536.430f, 2442.550f, -89.084f ), CVector ( 2685.160f, 2542.550f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 334.503f, -1406.050f, -89.084f ), CVector ( 466.223f, -1292.070f, 110.916f ) );
    AddZone ( "Vinewood", CVector ( 647.557f, -1227.280f, -89.084f ), CVector ( 787.461f, -1118.280f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 422.680f, -1684.650f, -89.084f ), CVector ( 558.099f, -1570.200f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 2498.210f, 2542.550f, -89.084f ), CVector ( 2685.160f, 2626.550f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1724.760f, -1430.870f, -89.084f ), CVector ( 1812.620f, -1250.900f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 225.165f, -1684.650f, -89.084f ), CVector ( 312.803f, -1501.950f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2056.860f, -1449.670f, -89.084f ), CVector ( 2266.210f, -1372.040f, 110.916f ) );
    AddZone ( "Hampton Barns", CVector ( 603.035f, 264.312f, 0.000f ), CVector ( 761.994f, 366.572f, 200.000f ) );
    AddZone ( "Temple", CVector ( 1096.470f, -1130.840f, -89.084f ), CVector ( 1252.330f, -1026.330f, 110.916f ) );
    AddZone ( "Kincaid Bridge", CVector ( -1087.930f, 855.370f, -89.084f ), CVector ( -961.950f, 986.281f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 1046.150f, -1722.260f, -89.084f ), CVector ( 1161.520f, -1577.590f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1323.900f, -1722.260f, -89.084f ), CVector ( 1440.900f, -1577.590f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1357.000f, -926.999f, -89.084f ), CVector ( 1463.900f, -768.027f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 466.223f, -1570.200f, -89.084f ), CVector ( 558.099f, -1385.070f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 911.802f, -860.619f, -89.084f ), CVector ( 1096.470f, -768.027f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 768.694f, -954.662f, -89.084f ), CVector ( 952.604f, -860.619f, 110.916f ) );
    AddZone ( "Julius Thruway South", CVector ( 2377.390f, 788.894f, -89.084f ), CVector ( 2537.390f, 897.901f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1812.620f, -1852.870f, -89.084f ), CVector ( 1971.660f, -1742.310f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2089.000f, -2394.330f, -89.084f ), CVector ( 2201.820f, -2235.840f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1370.850f, -1577.590f, -89.084f ), CVector ( 1463.900f, -1384.950f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 2121.400f, 2508.230f, -89.084f ), CVector ( 2237.400f, 2663.170f, 110.916f ) );
    AddZone ( "Temple", CVector ( 1096.470f, -1026.330f, -89.084f ), CVector ( 1252.330f, -910.170f, 110.916f ) );
    AddZone ( "Glen Park", CVector ( 1812.620f, -1449.670f, -89.084f ), CVector ( 1996.910f, -1350.720f, 110.916f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1242.980f, -50.096f, 0.000f ), CVector ( -1213.910f, 578.396f, 200.000f ) );
    AddZone ( "Martin Bridge", CVector ( -222.179f, 293.324f, 0.000f ), CVector ( -122.126f, 476.465f, 200.000f ) );
    AddZone ( "The Strip", CVector ( 2106.700f, 1863.230f, -89.084f ), CVector ( 2162.390f, 2202.760f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2541.700f, -2059.230f, -89.084f ), CVector ( 2703.580f, -1941.400f, 110.916f ) );
    AddZone ( "Marina", CVector ( 807.922f, -1577.590f, -89.084f ), CVector ( 926.922f, -1416.250f, 110.916f ) );
    AddZone ( "Las Venturas Airport", CVector ( 1457.370f, 1143.210f, -89.084f ), CVector ( 1777.400f, 1203.280f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1812.620f, -1742.310f, -89.084f ), CVector ( 1951.660f, -1602.310f, 110.916f ) );
    AddZone ( "Esplanade East", CVector ( -1580.010f, 1025.980f, -6.103520e-5f ), CVector ( -1499.890f, 1274.260f, 200.000f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1370.850f, -1384.950f, -89.084f ), CVector ( 1463.900f, -1170.870f, 110.916f ) );
    AddZone ( "The Mako Span", CVector ( 1664.620f, 401.750f, 0.000f ), CVector ( 1785.140f, 567.203f, 200.000f ) );
    AddZone ( "Rodeo", CVector ( 312.803f, -1684.650f, -89.084f ), CVector ( 422.680f, -1501.950f, 110.916f ) );
    AddZone ( "Pershing Square", CVector ( 1440.900f, -1722.260f, -89.084f ), CVector ( 1583.500f, -1577.590f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 687.802f, -860.619f, -89.084f ), CVector ( 911.802f, -768.027f, 110.916f ) );
    AddZone ( "Gant Bridge", CVector ( -2741.070f, 1490.470f, -6.103520e-5f ), CVector ( -2616.400f, 1659.680f, 200.000f ) );
    AddZone ( "Las Colinas", CVector ( 2185.330f, -1154.590f, -89.084f ), CVector ( 2281.450f, -934.489f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1169.130f, -910.170f, -89.084f ), CVector ( 1318.130f, -768.027f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1938.800f, 2508.230f, -89.084f ), CVector ( 2121.400f, 2624.230f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1667.960f, -1577.590f, -89.084f ), CVector ( 1812.620f, -1430.870f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 72.648f, -1544.170f, -89.084f ), CVector ( 225.165f, -1404.970f, 110.916f ) );
    AddZone ( "Roca Escalante", CVector ( 2536.430f, 2202.760f, -89.084f ), CVector ( 2625.160f, 2442.550f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 72.648f, -1684.650f, -89.084f ), CVector ( 225.165f, -1544.170f, 110.916f ) );
    AddZone ( "Market", CVector ( 952.663f, -1310.210f, -89.084f ), CVector ( 1072.660f, -1130.850f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2632.740f, -1135.040f, -89.084f ), CVector ( 2747.740f, -945.035f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 861.085f, -674.885f, -89.084f ), CVector ( 1156.550f, -600.896f, 110.916f ) );
    AddZone ( "King's", CVector ( -2253.540f, 373.539f, -9.155270e-5f ), CVector ( -1993.280f, 458.411f, 200.000f ) );
    AddZone ( "Redsands East", CVector ( 1848.400f, 2342.830f, -89.084f ), CVector ( 2011.940f, 2478.490f, 110.916f ) );
    AddZone ( "Downtown", CVector ( -1580.010f, 744.267f, -6.103520e-5f ), CVector ( -1499.890f, 1025.980f, 200.000f ) );
    AddZone ( "Conference Center", CVector ( 1046.150f, -1804.210f, -89.084f ), CVector ( 1323.900f, -1722.260f, 110.916f ) );
    AddZone ( "Richman", CVector ( 647.557f, -1118.280f, -89.084f ), CVector ( 787.461f, -954.662f, 110.916f ) );
    AddZone ( "Ocean Flats", CVector ( -2994.490f, 277.411f, -9.155270e-5f ), CVector ( -2867.850f, 458.411f, 200.000f ) );
    AddZone ( "Greenglass College", CVector ( 964.391f, 930.890f, -89.084f ), CVector ( 1166.530f, 1044.690f, 110.916f ) );
    AddZone ( "Glen Park", CVector ( 1812.620f, -1100.820f, -89.084f ), CVector ( 1994.330f, -973.380f, 110.916f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1375.600f, 919.447f, -89.084f ), CVector ( 1457.370f, 1203.280f, 110.916f ) );
    AddZone ( "Regular Tom", CVector ( -405.770f, 1712.860f, -3.051760e-5f ), CVector ( -276.719f, 1892.750f, 200.000f ) );
    AddZone ( "Verona Beach", CVector ( 1161.520f, -1722.260f, -89.084f ), CVector ( 1323.900f, -1577.590f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2281.450f, -1372.040f, -89.084f ), CVector ( 2381.680f, -1135.040f, 110.916f ) );
    AddZone ( "Caligula's Palace", CVector ( 2137.400f, 1703.230f, -89.084f ), CVector ( 2437.390f, 1783.230f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1951.660f, -1742.310f, -89.084f ), CVector ( 2124.660f, -1602.310f, 110.916f ) );
    AddZone ( "Pilgrim", CVector ( 2624.400f, 1383.230f, -89.084f ), CVector ( 2685.160f, 1783.230f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 2124.660f, -1742.310f, -89.084f ), CVector ( 2222.560f, -1494.030f, 110.916f ) );
    AddZone ( "Queens", CVector ( -2533.040f, 458.411f, 0.000f ), CVector ( -2329.310f, 578.396f, 200.000f ) );
    AddZone ( "Downtown", CVector ( -1871.720f, 1176.420f, -4.577640e-5f ), CVector ( -1620.300f, 1274.260f, 200.000f ) );
    AddZone ( "Commerce", CVector ( 1583.500f, -1722.260f, -89.084f ), CVector ( 1758.900f, -1577.590f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2381.680f, -1454.350f, -89.084f ), CVector ( 2462.130f, -1135.040f, 110.916f ) );
    AddZone ( "Marina", CVector ( 647.712f, -1577.590f, -89.084f ), CVector ( 807.922f, -1416.250f, 110.916f ) );
    AddZone ( "Richman", CVector ( 72.648f, -1404.970f, -89.084f ), CVector ( 225.165f, -1235.070f, 110.916f ) );
    AddZone ( "Vinewood", CVector ( 647.712f, -1416.250f, -89.084f ), CVector ( 787.461f, -1227.280f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2222.560f, -1628.530f, -89.084f ), CVector ( 2421.030f, -1494.030f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 558.099f, -1684.650f, -89.084f ), CVector ( 647.522f, -1384.930f, 110.916f ) );
    AddZone ( "Easter Tunnel", CVector ( -1709.710f, -833.034f, -1.525880e-5f ), CVector ( -1446.010f, -730.118f, 200.000f ) );
    AddZone ( "Rodeo", CVector ( 466.223f, -1385.070f, -89.084f ), CVector ( 647.522f, -1235.070f, 110.916f ) );
    AddZone ( "Redsands East", CVector ( 1817.390f, 2202.760f, -89.084f ), CVector ( 2011.940f, 2342.830f, 110.916f ) );
    AddZone ( "The Clown's Pocket", CVector ( 2162.390f, 1783.230f, -89.084f ), CVector ( 2437.390f, 1883.230f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1971.660f, -1852.870f, -89.084f ), CVector ( 2222.560f, -1742.310f, 110.916f ) );
    AddZone ( "Montgomery Intersection", CVector ( 1546.650f, 208.164f, 0.000f ), CVector ( 1745.830f, 347.457f, 200.000f ) );
    AddZone ( "Willowfield", CVector ( 2089.000f, -2235.840f, -89.084f ), CVector ( 2201.820f, -1989.900f, 110.916f ) );
    AddZone ( "Temple", CVector ( 952.663f, -1130.840f, -89.084f ), CVector ( 1096.470f, -937.184f, 110.916f ) );
    AddZone ( "Prickle Pine", CVector ( 1848.400f, 2553.490f, -89.084f ), CVector ( 1938.800f, 2863.230f, 110.916f ) );
    AddZone ( "Los Santos International", CVector ( 1400.970f, -2669.260f, -39.084f ), CVector ( 2189.820f, -2597.260f, 60.916f ) );
    AddZone ( "Garver Bridge", CVector ( -1213.910f, 950.022f, -89.084f ), CVector ( -1087.930f, 1178.930f, 110.916f ) );
    AddZone ( "Garver Bridge", CVector ( -1339.890f, 828.129f, -89.084f ), CVector ( -1213.910f, 1057.040f, 110.916f ) );
    AddZone ( "Kincaid Bridge", CVector ( -1339.890f, 599.218f, -89.084f ), CVector ( -1213.910f, 828.129f, 110.916f ) );
    AddZone ( "Kincaid Bridge", CVector ( -1213.910f, 721.111f, -89.084f ), CVector ( -1087.930f, 950.022f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 930.221f, -2006.780f, -89.084f ), CVector ( 1073.220f, -1804.210f, 110.916f ) );
    AddZone ( "Verdant Bluffs", CVector ( 1073.220f, -2006.780f, -89.084f ), CVector ( 1249.620f, -1842.270f, 110.916f ) );
    AddZone ( "Vinewood", CVector ( 787.461f, -1130.840f, -89.084f ), CVector ( 952.604f, -954.662f, 110.916f ) );
    AddZone ( "Vinewood", CVector ( 787.461f, -1310.210f, -89.084f ), CVector ( 952.663f, -1130.840f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1463.900f, -1577.590f, -89.084f ), CVector ( 1667.960f, -1430.870f, 110.916f ) );
    AddZone ( "Market", CVector ( 787.461f, -1416.250f, -89.084f ), CVector ( 1072.660f, -1310.210f, 110.916f ) );
    AddZone ( "Rockshore West", CVector ( 2377.390f, 596.349f, -89.084f ), CVector ( 2537.390f, 788.894f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 2237.400f, 2542.550f, -89.084f ), CVector ( 2498.210f, 2663.170f, 110.916f ) );
    AddZone ( "East Beach", CVector ( 2632.830f, -1668.130f, -89.084f ), CVector ( 2747.740f, -1393.420f, 110.916f ) );
    AddZone ( "Fallow Bridge", CVector ( 434.341f, 366.572f, 0.000f ), CVector ( 603.035f, 555.680f, 200.000f ) );
    AddZone ( "Willowfield", CVector ( 2089.000f, -1989.900f, -89.084f ), CVector ( 2324.000f, -1852.870f, 110.916f ) );
    AddZone ( "Chinatown", CVector ( -2274.170f, 578.396f, -7.629390e-5f ), CVector ( -2078.670f, 744.170f, 200.000f ) );
    AddZone ( "El Castillo del Diablo", CVector ( -208.570f, 2337.180f, 0.000f ), CVector ( 8.430f, 2487.180f, 200.000f ) );
    AddZone ( "Ocean Docks", CVector ( 2324.000f, -2145.100f, -89.084f ), CVector ( 2703.580f, -2059.230f, 110.916f ) );
    AddZone ( "Easter Bay Chemicals", CVector ( -1132.820f, -768.027f, 0.000f ), CVector ( -956.476f, -578.118f, 200.000f ) );
    AddZone ( "The Visage", CVector ( 1817.390f, 1703.230f, -89.084f ), CVector ( 2027.400f, 1863.230f, 110.916f ) );
    AddZone ( "Ocean Flats", CVector ( -2994.490f, -430.276f, -1.220700e-4f ), CVector ( -2831.890f, -222.589f, 200.000f ) );
    AddZone ( "Richman", CVector ( 321.356f, -860.619f, -89.084f ), CVector ( 687.802f, -768.027f, 110.916f ) );
    AddZone ( "Green Palms", CVector ( 176.581f, 1305.450f, -3.051760e-5f ), CVector ( 338.658f, 1520.720f, 200.000f ) );
    AddZone ( "Richman", CVector ( 321.356f, -768.027f, -89.084f ), CVector ( 700.794f, -674.885f, 110.916f ) );
    AddZone ( "Starfish Casino", CVector ( 2162.390f, 1883.230f, -89.084f ), CVector ( 2437.390f, 2012.180f, 110.916f ) );
    AddZone ( "East Beach", CVector ( 2747.740f, -1668.130f, -89.084f ), CVector ( 2959.350f, -1498.620f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2056.860f, -1372.040f, -89.084f ), CVector ( 2281.450f, -1210.740f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1463.900f, -1290.870f, -89.084f ), CVector ( 1724.760f, -1150.870f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1463.900f, -1430.870f, -89.084f ), CVector ( 1724.760f, -1290.870f, 110.916f ) );
    AddZone ( "Garver Bridge", CVector ( -1499.890f, 696.442f, -179.615f ), CVector ( -1339.890f, 925.353f, 20.385f ) );
    AddZone ( "Julius Thruway South", CVector ( 1457.390f, 823.228f, -89.084f ), CVector ( 2377.390f, 863.229f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2421.030f, -1628.530f, -89.084f ), CVector ( 2632.830f, -1454.350f, 110.916f ) );
    AddZone ( "Greenglass College", CVector ( 964.391f, 1044.690f, -89.084f ), CVector ( 1197.390f, 1203.220f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2747.740f, -1120.040f, -89.084f ), CVector ( 2959.350f, -945.035f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 737.573f, -768.027f, -89.084f ), CVector ( 1142.290f, -674.885f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2201.820f, -2730.880f, -89.084f ), CVector ( 2324.000f, -2418.330f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2462.130f, -1454.350f, -89.084f ), CVector ( 2581.730f, -1135.040f, 110.916f ) );
    AddZone ( "Ganton", CVector ( 2222.560f, -1722.330f, -89.084f ), CVector ( 2632.830f, -1628.530f, 110.916f ) );
    AddZone ( "Avispa Country Club", CVector ( -2831.890f, -430.276f, -6.103520e-5f ), CVector ( -2646.400f, -222.589f, 200.000f ) );
    AddZone ( "Willowfield", CVector ( 1970.620f, -2179.250f, -89.084f ), CVector ( 2089.000f, -1852.870f, 110.916f ) );
    AddZone ( "Esplanade North", CVector ( -1982.320f, 1274.260f, -4.577640e-5f ), CVector ( -1524.240f, 1358.900f, 200.000f ) );
    AddZone ( "The High Roller", CVector ( 1817.390f, 1283.230f, -89.084f ), CVector ( 2027.390f, 1469.230f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2201.820f, -2418.330f, -89.084f ), CVector ( 2324.000f, -2095.000f, 110.916f ) );
    AddZone ( "Last Dime Motel", CVector ( 1823.080f, 596.349f, -89.084f ), CVector ( 1997.220f, 823.228f, 110.916f ) );
    AddZone ( "Bayside Marina", CVector ( -2353.170f, 2275.790f, 0.000f ), CVector ( -2153.170f, 2475.790f, 200.000f ) );
    AddZone ( "King's", CVector ( -2329.310f, 458.411f, -7.629390e-5f ), CVector ( -1993.280f, 578.396f, 200.000f ) );
    AddZone ( "El Corona", CVector ( 1692.620f, -2179.250f, -89.084f ), CVector ( 1812.620f, -1842.270f, 110.916f ) );
    AddZone ( "Blackfield Chapel", CVector ( 1375.600f, 596.349f, -89.084f ), CVector ( 1558.090f, 823.228f, 110.916f ) );
    AddZone ( "The Pink Swan", CVector ( 1817.390f, 1083.230f, -89.084f ), CVector ( 2027.390f, 1283.230f, 110.916f ) );
    AddZone ( "Julius Thruway West", CVector ( 1197.390f, 1163.390f, -89.084f ), CVector ( 1236.630f, 2243.230f, 110.916f ) );
    AddZone ( "Los Flores", CVector ( 2581.730f, -1393.420f, -89.084f ), CVector ( 2747.740f, -1135.040f, 110.916f ) );
    AddZone ( "The Visage", CVector ( 1817.390f, 1863.230f, -89.084f ), CVector ( 2106.700f, 2011.830f, 110.916f ) );
    AddZone ( "Prickle Pine", CVector ( 1938.800f, 2624.230f, -89.084f ), CVector ( 2121.400f, 2861.550f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 851.449f, -1804.210f, -89.084f ), CVector ( 1046.150f, -1577.590f, 110.916f ) );
    AddZone ( "Robada Intersection", CVector ( -1119.010f, 1178.930f, -89.084f ), CVector ( -862.025f, 1351.450f, 110.916f ) );
    AddZone ( "Linden Side", CVector ( 2749.900f, 943.235f, -89.084f ), CVector ( 2923.390f, 1198.990f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2703.580f, -2302.330f, -89.084f ), CVector ( 2959.350f, -2126.900f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2324.000f, -2059.230f, -89.084f ), CVector ( 2541.700f, -1852.870f, 110.916f ) );
    AddZone ( "King's", CVector ( -2411.220f, 265.243f, -9.155270e-5f ), CVector ( -1993.280f, 373.539f, 200.000f ) );
    AddZone ( "Commerce", CVector ( 1323.900f, -1842.270f, -89.084f ), CVector ( 1701.900f, -1722.260f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1269.130f, -768.027f, -89.084f ), CVector ( 1414.070f, -452.425f, 110.916f ) );
    AddZone ( "Marina", CVector ( 647.712f, -1804.210f, -89.084f ), CVector ( 851.449f, -1577.590f, 110.916f ) );
    AddZone ( "Battery Point", CVector ( -2741.070f, 1268.410f, -4.577640e-5f ), CVector ( -2533.040f, 1490.470f, 200.000f ) );
    AddZone ( "The Four Dragons Casino", CVector ( 1817.390f, 863.232f, -89.084f ), CVector ( 2027.390f, 1083.230f, 110.916f ) );
    AddZone ( "Blackfield", CVector ( 964.391f, 1203.220f, -89.084f ), CVector ( 1197.390f, 1403.220f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1534.560f, 2433.230f, -89.084f ), CVector ( 1848.400f, 2583.230f, 110.916f ) );
    AddZone ( "Yellow Bell Golf Course", CVector ( 1117.400f, 2723.230f, -89.084f ), CVector ( 1457.460f, 2863.230f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1812.620f, -1602.310f, -89.084f ), CVector ( 2124.660f, -1449.670f, 110.916f ) );
    AddZone ( "Redsands West", CVector ( 1297.470f, 2142.860f, -89.084f ), CVector ( 1777.390f, 2243.230f, 110.916f ) );
    AddZone ( "Doherty", CVector ( -2270.040f, -324.114f, -1.220700e-4f ), CVector ( -1794.920f, -222.589f, 200.000f ) );
    AddZone ( "Hilltop Farm", CVector ( 967.383f, -450.390f, -3.051760e-5f ), CVector ( 1176.780f, -217.900f, 200.000f ) );
    AddZone ( "Las Barrancas", CVector ( -926.130f, 1398.730f, -3.051760e-5f ), CVector ( -719.234f, 1634.690f, 200.000f ) );
    AddZone ( "Pirates in Men's Pants", CVector ( 1817.390f, 1469.230f, -89.084f ), CVector ( 2027.400f, 1703.230f, 110.916f ) );
    AddZone ( "City Hall", CVector ( -2867.850f, 277.411f, -9.155270e-5f ), CVector ( -2593.440f, 458.411f, 200.000f ) );
    AddZone ( "Avispa Country Club", CVector ( -2646.400f, -355.493f, 0.000f ), CVector ( -2270.040f, -222.589f, 200.000f ) );
    AddZone ( "The Strip", CVector ( 2027.400f, 863.229f, -89.084f ), CVector ( 2087.390f, 1703.230f, 110.916f ) );
    AddZone ( "Hashbury", CVector ( -2593.440f, -222.589f, -1.068120e-4f ), CVector ( -2411.220f, 54.722f, 200.000f ) );
    AddZone ( "Los Santos International", CVector ( 1852.000f, -2394.330f, -89.084f ), CVector ( 2089.000f, -2179.250f, 110.916f ) );
    AddZone ( "Whitewood Estates", CVector ( 1098.310f, 1726.220f, -89.084f ), CVector ( 1197.390f, 2243.230f, 110.916f ) );
    AddZone ( "Sherman Reservoir", CVector ( -789.737f, 1659.680f, -89.084f ), CVector ( -599.505f, 1929.410f, 110.916f ) );
    AddZone ( "El Corona", CVector ( 1812.620f, -2179.250f, -89.084f ), CVector ( 1970.620f, -1852.870f, 110.916f ) );
    AddZone ( "Downtown", CVector ( -1700.010f, 744.267f, -6.103520e-5f ), CVector ( -1580.010f, 1176.520f, 200.000f ) );
    AddZone ( "Foster Valley", CVector ( -2178.690f, -1250.970f, 0.000f ), CVector ( -1794.920f, -1115.580f, 200.000f ) );
    AddZone ( "Las Payasadas", CVector ( -354.332f, 2580.360f, 2.098080e-5f ), CVector ( -133.625f, 2816.820f, 200.000f ) );
    AddZone ( "Valle Ocultado", CVector ( -936.668f, 2611.440f, 2.098080e-5f ), CVector ( -715.961f, 2847.900f, 200.000f ) );
    AddZone ( "Blackfield Intersection", CVector ( 1166.530f, 795.010f, -89.084f ), CVector ( 1375.600f, 1044.690f, 110.916f ) );
    AddZone ( "Ganton", CVector ( 2222.560f, -1852.870f, -89.084f ), CVector ( 2632.830f, -1722.330f, 110.916f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1213.910f, -730.118f, 0.000f ), CVector ( -1132.820f, -50.096f, 200.000f ) );
    AddZone ( "Redsands East", CVector ( 1817.390f, 2011.830f, -89.084f ), CVector ( 2106.700f, 2202.760f, 110.916f ) );
    AddZone ( "Esplanade East", CVector ( -1499.890f, 578.396f, -79.615f ), CVector ( -1339.890f, 1274.260f, 20.385f ) );
    AddZone ( "Caligula's Palace", CVector ( 2087.390f, 1543.230f, -89.084f ), CVector ( 2437.390f, 1703.230f, 110.916f ) );
    AddZone ( "Royal Casino", CVector ( 2087.390f, 1383.230f, -89.084f ), CVector ( 2437.390f, 1543.230f, 110.916f ) );
    AddZone ( "Richman", CVector ( 72.648f, -1235.070f, -89.084f ), CVector ( 321.356f, -1008.150f, 110.916f ) );
    AddZone ( "Starfish Casino", CVector ( 2437.390f, 1783.230f, -89.084f ), CVector ( 2685.160f, 2012.180f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1281.130f, -452.425f, -89.084f ), CVector ( 1641.130f, -290.913f, 110.916f ) );
    AddZone ( "Downtown", CVector ( -1982.320f, 744.170f, -6.103520e-5f ), CVector ( -1871.720f, 1274.260f, 200.000f ) );
    AddZone ( "Hankypanky Point", CVector ( 2576.920f, 62.158f, 0.000f ), CVector ( 2759.250f, 385.503f, 200.000f ) );
    AddZone ( "K.A.C.C. Military Fuels", CVector ( 2498.210f, 2626.550f, -89.084f ), CVector ( 2749.900f, 2861.550f, 110.916f ) );
    AddZone ( "Harry Gold Parkway", CVector ( 1777.390f, 863.232f, -89.084f ), CVector ( 1817.390f, 2342.830f, 110.916f ) );
    AddZone ( "Bayside Tunnel", CVector ( -2290.190f, 2548.290f, -89.084f ), CVector ( -1950.190f, 2723.290f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2324.000f, -2302.330f, -89.084f ), CVector ( 2703.580f, -2145.100f, 110.916f ) );
    AddZone ( "Richman", CVector ( 321.356f, -1044.070f, -89.084f ), CVector ( 647.557f, -860.619f, 110.916f ) );
    AddZone ( "Randolph Industrial Estate", CVector ( 1558.090f, 596.349f, -89.084f ), CVector ( 1823.080f, 823.235f, 110.916f ) );
    AddZone ( "East Beach", CVector ( 2632.830f, -1852.870f, -89.084f ), CVector ( 2959.350f, -1668.130f, 110.916f ) );
    AddZone ( "Flint Water", CVector ( -314.426f, -753.874f, -89.084f ), CVector ( -106.339f, -463.073f, 110.916f ) );
    AddZone ( "Blueberry", CVector ( 19.607f, -404.136f, 3.814700e-6f ), CVector ( 349.607f, -220.137f, 200.000f ) );
    AddZone ( "Linden Station", CVector ( 2749.900f, 1198.990f, -89.084f ), CVector ( 2923.390f, 1548.990f, 110.916f ) );
    AddZone ( "Glen Park", CVector ( 1812.620f, -1350.720f, -89.084f ), CVector ( 2056.860f, -1100.820f, 110.916f ) );
    AddZone ( "Downtown", CVector ( -1993.280f, 265.243f, -9.155270e-5f ), CVector ( -1794.920f, 578.396f, 200.000f ) );
    AddZone ( "Redsands West", CVector ( 1377.390f, 2243.230f, -89.084f ), CVector ( 1704.590f, 2433.230f, 110.916f ) );
    AddZone ( "Richman", CVector ( 321.356f, -1235.070f, -89.084f ), CVector ( 647.522f, -1044.070f, 110.916f ) );
    AddZone ( "Gant Bridge", CVector ( -2741.450f, 1659.680f, -6.103520e-5f ), CVector ( -2616.400f, 2175.150f, 200.000f ) );
    AddZone ( "Lil' Probe Inn", CVector ( -90.218f, 1286.850f, -3.051760e-5f ), CVector ( 153.859f, 1554.120f, 200.000f ) );
    AddZone ( "Flint Intersection", CVector ( -187.700f, -1596.760f, -89.084f ), CVector ( 17.063f, -1276.600f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2281.450f, -1135.040f, -89.084f ), CVector ( 2632.740f, -945.035f, 110.916f ) );
    AddZone ( "Sobell Rail Yards", CVector ( 2749.900f, 1548.990f, -89.084f ), CVector ( 2923.390f, 1937.250f, 110.916f ) );
    AddZone ( "The Emerald Isle", CVector ( 2011.940f, 2202.760f, -89.084f ), CVector ( 2237.400f, 2508.230f, 110.916f ) );
    AddZone ( "El Castillo del Diablo", CVector ( -208.570f, 2123.010f, -7.629390e-6f ), CVector ( 114.033f, 2337.180f, 200.000f ) );
    AddZone ( "Santa Flora", CVector ( -2741.070f, 458.411f, -7.629390e-5f ), CVector ( -2533.040f, 793.411f, 200.000f ) );
    AddZone ( "Playa del Seville", CVector ( 2703.580f, -2126.900f, -89.084f ), CVector ( 2959.350f, -1852.870f, 110.916f ) );
    AddZone ( "Market", CVector ( 926.922f, -1577.590f, -89.084f ), CVector ( 1370.850f, -1416.250f, 110.916f ) );
    AddZone ( "Queens", CVector ( -2593.440f, 54.722f, 0.000f ), CVector ( -2411.220f, 458.411f, 200.000f ) );
    AddZone ( "Pilson Intersection", CVector ( 1098.390f, 2243.230f, -89.084f ), CVector ( 1377.390f, 2507.230f, 110.916f ) );
    AddZone ( "Spinybed", CVector ( 2121.400f, 2663.170f, -89.084f ), CVector ( 2498.210f, 2861.550f, 110.916f ) );
    AddZone ( "Pilgrim", CVector ( 2437.390f, 1383.230f, -89.084f ), CVector ( 2624.400f, 1783.230f, 110.916f ) );
    AddZone ( "Blackfield", CVector ( 964.391f, 1403.220f, -89.084f ), CVector ( 1197.390f, 1726.220f, 110.916f ) );
    AddZone ( "'The Big Ear'", CVector ( -410.020f, 1403.340f, -3.051760e-5f ), CVector ( -137.969f, 1681.230f, 200.000f ) );
    AddZone ( "Dillimore", CVector ( 580.794f, -674.885f, -9.536740e-6f ), CVector ( 861.085f, -404.790f, 200.000f ) );
    AddZone ( "El Quebrados", CVector ( -1645.230f, 2498.520f, 0.000f ), CVector ( -1372.140f, 2777.850f, 200.000f ) );
    AddZone ( "Esplanade North", CVector ( -2533.040f, 1358.900f, -4.577640e-5f ), CVector ( -1996.660f, 1501.210f, 200.000f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1499.890f, -50.096f, -1.068120e-4f ), CVector ( -1242.980f, 249.904f, 200.000f ) );
    AddZone ( "Fisher's Lagoon", CVector ( 1916.990f, -233.323f, -100.000f ), CVector ( 2131.720f, 13.800f, 200.000f ) );
    AddZone ( "Mulholland", CVector ( 1414.070f, -768.027f, -89.084f ), CVector ( 1667.610f, -452.425f, 110.916f ) );
    AddZone ( "East Beach", CVector ( 2747.740f, -1498.620f, -89.084f ), CVector ( 2959.350f, -1120.040f, 110.916f ) );
    AddZone ( "San Andreas Sound", CVector ( 2450.390f, 385.503f, -100.000f ), CVector ( 2759.250f, 562.349f, 200.000f ) );
    AddZone ( "Shady Creeks", CVector ( -2030.120f, -2174.890f, -6.103520e-5f ), CVector ( -1820.640f, -1771.660f, 200.000f ) );
    AddZone ( "Market", CVector ( 1072.660f, -1416.250f, -89.084f ), CVector ( 1370.850f, -1130.850f, 110.916f ) );
    AddZone ( "Rockshore West", CVector ( 1997.220f, 596.349f, -89.084f ), CVector ( 2377.390f, 823.228f, 110.916f ) );
    AddZone ( "Prickle Pine", CVector ( 1534.560f, 2583.230f, -89.084f ), CVector ( 1848.400f, 2863.230f, 110.916f ) );
    AddZone ( "Easter Basin", CVector ( -1794.920f, -50.096f, -1.068120e-4f ), CVector ( -1499.890f, 249.904f, 200.000f ) );
    AddZone ( "Leafy Hollow", CVector ( -1166.970f, -1856.030f, 0.000f ), CVector ( -815.624f, -1602.070f, 200.000f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1457.390f, 863.229f, -89.084f ), CVector ( 1777.400f, 1143.210f, 110.916f ) );
    AddZone ( "Prickle Pine", CVector ( 1117.400f, 2507.230f, -89.084f ), CVector ( 1534.560f, 2723.230f, 110.916f ) );
    AddZone ( "Blueberry", CVector ( 104.534f, -220.137f, 2.384190e-7f ), CVector ( 349.607f, 152.236f, 200.000f ) );
    AddZone ( "El Castillo del Diablo", CVector ( -464.515f, 2217.680f, 0.000f ), CVector ( -208.570f, 2580.360f, 200.000f ) );
    AddZone ( "Downtown", CVector ( -2078.670f, 578.396f, -7.629390e-5f ), CVector ( -1499.890f, 744.267f, 200.000f ) );
    AddZone ( "Rockshore East", CVector ( 2537.390f, 676.549f, -89.084f ), CVector ( 2902.350f, 943.235f, 110.916f ) );
    AddZone ( "San Fierro Bay", CVector ( -2616.400f, 1501.210f, -3.051760e-5f ), CVector ( -1996.660f, 1659.680f, 200.000f ) );
    AddZone ( "Paradiso", CVector ( -2741.070f, 793.411f, -6.103520e-5f ), CVector ( -2533.040f, 1268.410f, 200.000f ) );
    AddZone ( "The Camel's Toe", CVector ( 2087.390f, 1203.230f, -89.084f ), CVector ( 2640.400f, 1383.230f, 110.916f ) );
    AddZone ( "Old Venturas Strip", CVector ( 2162.390f, 2012.180f, -89.084f ), CVector ( 2685.160f, 2202.760f, 110.916f ) );
    AddZone ( "Juniper Hill", CVector ( -2533.040f, 578.396f, -7.629390e-5f ), CVector ( -2274.170f, 968.369f, 200.000f ) );
    AddZone ( "Juniper Hollow", CVector ( -2533.040f, 968.369f, -6.103520e-5f ), CVector ( -2274.170f, 1358.900f, 200.000f ) );
    AddZone ( "Roca Escalante", CVector ( 2237.400f, 2202.760f, -89.084f ), CVector ( 2536.430f, 2542.550f, 110.916f ) );
    AddZone ( "Julius Thruway East", CVector ( 2685.160f, 1055.960f, -89.084f ), CVector ( 2749.900f, 2626.550f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 647.712f, -2173.290f, -89.084f ), CVector ( 930.221f, -1804.210f, 110.916f ) );
    AddZone ( "Foster Valley", CVector ( -2178.690f, -599.884f, -1.220700e-4f ), CVector ( -1794.920f, -324.114f, 200.000f ) );
    AddZone ( "Arco del Oeste", CVector ( -901.129f, 2221.860f, 0.000f ), CVector ( -592.090f, 2571.970f, 200.000f ) );
    AddZone ( "Fallen Tree", CVector ( -792.254f, -698.555f, -5.340580e-5f ), CVector ( -452.404f, -380.043f, 200.000f ) );
    AddZone ( "The Farm", CVector ( -1209.670f, -1317.100f, 114.981f ), CVector ( -908.161f, -787.391f, 251.981f ) );
    AddZone ( "The Sherman Dam", CVector ( -968.772f, 1929.410f, -3.051760e-5f ), CVector ( -481.126f, 2155.260f, 200.000f ) );
    AddZone ( "Esplanade North", CVector ( -1996.660f, 1358.900f, -4.577640e-5f ), CVector ( -1524.240f, 1592.510f, 200.000f ) );
    AddZone ( "Financial", CVector ( -1871.720f, 744.170f, -6.103520e-5f ), CVector ( -1701.300f, 1176.420f, 300.000f ) );
    AddZone ( "Garcia", CVector ( -2411.220f, -222.589f, -1.144410e-4f ), CVector ( -2173.040f, 265.243f, 200.000f ) );
    AddZone ( "Montgomery", CVector ( 1119.510f, 119.526f, -3.051760e-5f ), CVector ( 1451.400f, 493.323f, 200.000f ) );
    AddZone ( "Creek", CVector ( 2749.900f, 1937.250f, -89.084f ), CVector ( 2921.620f, 2669.790f, 110.916f ) );
    AddZone ( "Los Santos International", CVector ( 1249.620f, -2394.330f, -89.084f ), CVector ( 1852.000f, -2179.250f, 110.916f ) );
    AddZone ( "Santa Maria Beach", CVector ( 72.648f, -2173.290f, -89.084f ), CVector ( 342.648f, -1684.650f, 110.916f ) );
    AddZone ( "Mulholland Intersection", CVector ( 1463.900f, -1150.870f, -89.084f ), CVector ( 1812.620f, -768.027f, 110.916f ) );
    AddZone ( "Angel Pine", CVector ( -2324.940f, -2584.290f, -6.103520e-5f ), CVector ( -1964.220f, -2212.110f, 200.000f ) );
    AddZone ( "Verdant Meadows", CVector ( 37.032f, 2337.180f, -3.051760e-5f ), CVector ( 435.988f, 2677.900f, 200.000f ) );
    AddZone ( "Octane Springs", CVector ( 338.658f, 1228.510f, 0.000f ), CVector ( 664.308f, 1655.050f, 200.000f ) );
    AddZone ( "Come-A-Lot", CVector ( 2087.390f, 943.235f, -89.084f ), CVector ( 2623.180f, 1203.230f, 110.916f ) );
    AddZone ( "Redsands West", CVector ( 1236.630f, 1883.110f, -89.084f ), CVector ( 1777.390f, 2142.860f, 110.916f ) );
    AddZone ( "Santa Maria Beach", CVector ( 342.648f, -2173.290f, -89.084f ), CVector ( 647.712f, -1684.650f, 110.916f ) );
    AddZone ( "Verdant Bluffs", CVector ( 1249.620f, -2179.250f, -89.084f ), CVector ( 1692.620f, -1842.270f, 110.916f ) );
    AddZone ( "Las Venturas Airport", CVector ( 1236.630f, 1203.280f, -89.084f ), CVector ( 1457.370f, 1883.110f, 110.916f ) );
    AddZone ( "Flint Range", CVector ( -594.191f, -1648.550f, 0.000f ), CVector ( -187.700f, -1276.600f, 200.000f ) );
    AddZone ( "Verdant Bluffs", CVector ( 930.221f, -2488.420f, -89.084f ), CVector ( 1249.620f, -2006.780f, 110.916f ) );
    AddZone ( "Palomino Creek", CVector ( 2160.220f, -149.004f, 0.000f ), CVector ( 2576.920f, 228.322f, 200.000f ) );
    AddZone ( "Ocean Docks", CVector ( 2373.770f, -2697.090f, -89.084f ), CVector ( 2809.220f, -2330.460f, 110.916f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1213.910f, -50.096f, -4.577640e-5f ), CVector ( -947.980f, 578.396f, 200.000f ) );
    AddZone ( "Whitewood Estates", CVector ( 883.308f, 1726.220f, -89.084f ), CVector ( 1098.310f, 2507.230f, 110.916f ) );
    AddZone ( "Calton Heights", CVector ( -2274.170f, 744.170f, -6.103520e-5f ), CVector ( -1982.320f, 1358.900f, 200.000f ) );
    AddZone ( "Easter Basin", CVector ( -1794.920f, 249.904f, -9.155270e-5f ), CVector ( -1242.980f, 578.396f, 200.000f ) );
    AddZone ( "Los Santos Inlet", CVector ( -321.744f, -2224.430f, -89.084f ), CVector ( 44.615f, -1724.430f, 110.916f ) );
    AddZone ( "Doherty", CVector ( -2173.040f, -222.589f, -1.068120e-4f ), CVector ( -1794.920f, 265.243f, 200.000f ) );
    AddZone ( "Mount Chiliad", CVector ( -2178.690f, -2189.910f, -47.917f ), CVector ( -2030.120f, -1771.660f, 576.083f ) );
    AddZone ( "Fort Carson", CVector ( -376.233f, 826.326f, -3.051760e-5f ), CVector ( 123.717f, 1220.440f, 200.000f ) );
    AddZone ( "Foster Valley", CVector ( -2178.690f, -1115.580f, 0.000f ), CVector ( -1794.920f, -599.884f, 200.000f ) );
    AddZone ( "Ocean Flats", CVector ( -2994.490f, -222.589f, -1.068120e-4f ), CVector ( -2593.440f, 277.411f, 200.000f ) );
    AddZone ( "Fern Ridge", CVector ( 508.189f, -139.259f, 0.000f ), CVector ( 1306.660f, 119.526f, 200.000f ) );
    AddZone ( "Bayside", CVector ( -2741.070f, 2175.150f, 0.000f ), CVector ( -2353.170f, 2722.790f, 200.000f ) );
    AddZone ( "Las Venturas Airport", CVector ( 1457.370f, 1203.280f, -89.084f ), CVector ( 1777.390f, 1883.110f, 110.916f ) );
    AddZone ( "Blueberry Acres", CVector ( -319.676f, -220.137f, 0.000f ), CVector ( 104.534f, 293.324f, 200.000f ) );
    AddZone ( "Palisades", CVector ( -2994.490f, 458.411f, -6.103520e-5f ), CVector ( -2741.070f, 1339.610f, 200.000f ) );
    AddZone ( "North Rock", CVector ( 2285.370f, -768.027f, 0.000f ), CVector ( 2770.590f, -269.740f, 200.000f ) );
    AddZone ( "Hunter Quarry", CVector ( 337.244f, 710.840f, -115.239f ), CVector ( 860.554f, 1031.710f, 203.761f ) );
    AddZone ( "Los Santos International", CVector ( 1382.730f, -2730.880f, -89.084f ), CVector ( 2201.820f, -2394.330f, 110.916f ) );
    AddZone ( "Missionary Hill", CVector ( -2994.490f, -811.276f, 0.000f ), CVector ( -2178.690f, -430.276f, 200.000f ) );
    AddZone ( "San Fierro Bay", CVector ( -2616.400f, 1659.680f, -3.051760e-5f ), CVector ( -1996.660f, 2175.150f, 200.000f ) );
    AddZone ( "Restricted Area", CVector ( -91.586f, 1655.050f, -50.000f ), CVector ( 421.234f, 2123.010f, 250.000f ) );
    AddZone ( "Mount Chiliad", CVector ( -2997.470f, -1115.580f, -47.917f ), CVector ( -2178.690f, -971.913f, 576.083f ) );
    AddZone ( "Mount Chiliad", CVector ( -2178.690f, -1771.660f, -47.917f ), CVector ( -1936.120f, -1250.970f, 576.083f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1794.920f, -730.118f, -3.051760e-5f ), CVector ( -1213.910f, -50.096f, 200.000f ) );
    AddZone ( "The Panopticon", CVector ( -947.980f, -304.320f, -1.144410e-5f ), CVector ( -319.676f, 327.071f, 200.000f ) );
    AddZone ( "Shady Creeks", CVector ( -1820.640f, -2643.680f, -8.010860e-5f ), CVector ( -1226.780f, -1771.660f, 200.000f ) );
    AddZone ( "Back o Beyond", CVector ( -1166.970f, -2641.190f, 0.000f ), CVector ( -321.744f, -1856.030f, 200.000f ) );
    AddZone ( "Mount Chiliad", CVector ( -2994.490f, -2189.910f, -47.917f ), CVector ( -2178.690f, -1115.580f, 576.083f ) );
    AddZone ( "Tierra Robada", CVector ( -1213.910f, 596.349f, -242.990f ), CVector ( -480.539f, 1659.680f, 900.000f ) );
    AddZone ( "Flint County", CVector ( -1213.910f, -2892.970f, -242.990f ), CVector ( 44.615f, -768.027f, 900.000f ) );
    AddZone ( "Whetstone", CVector ( -2997.470f, -2892.970f, -242.990f ), CVector ( -1213.910f, -1115.580f, 900.000f ) );
    AddZone ( "Bone County", CVector ( -480.539f, 596.349f, -242.990f ), CVector ( 869.461f, 2993.870f, 900.000f ) );
    AddZone ( "Tierra Robada", CVector ( -2997.470f, 1659.680f, -242.990f ), CVector ( -480.539f, 2993.870f, 900.000f ) );
    AddZone ( "San Fierro", CVector ( -2997.470f, -1115.580f, -242.990f ), CVector ( -1213.910f, 1659.680f, 900.000f ) );
    AddZone ( "Las Venturas", CVector ( 869.461f, 596.349f, -242.990f ), CVector ( 2997.060f, 2993.870f, 900.000f ) );
    AddZone ( "Red County", CVector ( -1213.910f, -768.027f, -242.990f ), CVector ( 2997.060f, 596.349f, 900.000f ) );
    AddZone ( "Los Santos", CVector ( 44.615f, -2892.970f, -242.990f ), CVector ( 2997.060f, -768.027f, 900.000f ) );
}


void CZoneNames::ClearZones ( void )
{
    list < SZone* > ::iterator iter = m_Zones.begin ();
    for ( ; iter != m_Zones.end (); iter++ )
    {
        delete *iter;
    }
    m_Zones.clear ();
}


void CZoneNames::LoadCities ( void )
{
    AddCity ( "Las Venturas", CVector ( 685.0f, 476.093f, -500.0f ), CVector ( 3000.0f, 3000.0f, 500.0f ) );
    AddCity ( "San Fierro", CVector ( -3000.0f, -742.306f, -500.0f ), CVector ( -1270.53f, 1530.24f, 500.0f ) );
    AddCity ( "San Fierro", CVector ( -1270.53f, -402.481f, -500.0f ), CVector ( -1038.45f, 832.495f, 500.0f ) );
    AddCity ( "San Fierro", CVector ( -1038.45f, -145.539f, -500.0f ), CVector ( -897.546f, 376.632f, 500.0f ) );
    AddCity ( "Los Santos", CVector ( 480.0f, -3000.0f, -500.0f ), CVector ( 3000.0f, -850.0f, 500.0f ) );
    AddCity ( "Los Santos", CVector ( 80.0f, -2101.61f, -500.0f ), CVector ( 1075.0f, -1239.61f, 500.0f ) );
    AddCity ( "Tierra Robada", CVector ( -1213.91f, 596.349f, -242.99f ), CVector ( -480.539f, 1659.68f, 900.0f ) );
    AddCity ( "Red County", CVector ( -1213.91f, -768.027f, -242.99f ), CVector ( 2997.06f, 596.349f, 900.0f ) );
    AddCity ( "Flint County", CVector ( -1213.91f, -2892.97f, -242.99f ), CVector ( 44.6147f, -768.027f, 900.0f ) );
    AddCity ( "Whetstone", CVector ( -2997.47f, -2892.97f, -242.99f ), CVector ( -1213.91f, -1115.58f, 900.0f ) );
}


void CZoneNames::ClearCities ( void )
{
    list < SZone* > ::iterator iter = m_Cities.begin ();
    for ( ; iter != m_Cities.end (); iter++ )
    {
        delete *iter;
    }
    m_Cities.clear ();
}

