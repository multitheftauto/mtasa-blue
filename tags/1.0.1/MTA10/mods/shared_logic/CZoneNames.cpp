/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CZoneNames.cpp
*  PURPOSE:     Zone names class
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

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


void CZoneNames::AddZone ( const char* szName, CVector& vecBottomLeft, CVector& vecTopRight )
{
    SZone* pZone = new SZone;
    strncpy ( pZone->szName, szName, MAX_ZONE_NAME );
    pZone->szName [ MAX_ZONE_NAME - 1 ] = 0;
    pZone->vecBottomLeft = vecBottomLeft;
    pZone->vecTopRight = vecTopRight;
    m_Zones.push_back ( pZone );
}


SZone* CZoneNames::GetZone ( CVector& vecPosition )
{
    SZone* pSmallestZone = NULL;
    float fSmallestZoneSize = 0.0f;

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


const char* CZoneNames::GetZoneName ( CVector& vecPosition )
{
    SZone* pZone = GetZone ( vecPosition );
    if ( pZone )
    {
        return pZone->szName;
    }
    return szUnknownZone;
}


void CZoneNames::AddCity ( const char* szName, CVector& vecBottomLeft, CVector& vecTopRight )
{
    SZone* pZone = new SZone;
    strncpy ( pZone->szName, szName, MAX_ZONE_NAME );
    pZone->szName [ MAX_ZONE_NAME - 1 ] = 0;
    pZone->vecBottomLeft = vecBottomLeft;
    pZone->vecTopRight = vecTopRight;
    m_Cities.push_back ( pZone );
}


SZone* CZoneNames::GetCity ( CVector& vecPosition )
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


const char* CZoneNames::GetCityName ( CVector& vecPosition )
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
    AddZone ( "Bayside Marina", CVector ( -2353.17f, 2275.79f, 0.0f ), CVector ( -2153.17f, 2475.79f, 200.0f ) );
    AddZone ( "Bayside", CVector ( -2741.07f, 2175.15f, 0.0f ), CVector ( -2353.17f, 2722.79f, 200.0f ) );
    AddZone ( "Battery Point", CVector ( -2741.07f, 1268.41f, -4.57764e-005f ), CVector ( -2533.04f, 1490.47f, 200.0f ) );
    AddZone ( "Paradiso", CVector ( -2741.07f, 793.411f, -6.10352e-005f ), CVector ( -2533.04f, 1268.41f, 200.0f ) );
    AddZone ( "Santa Flora", CVector ( -2741.07f, 458.411f, -7.62939e-005f ), CVector ( -2533.04f, 793.411f, 200.0f ) );
    AddZone ( "Palisades", CVector ( -2994.49f, 458.411f, -6.10352e-005f ), CVector ( -2741.07f, 1339.61f, 200.0f ) );
    AddZone ( "City Hall", CVector ( -2867.85f, 277.411f, -9.15527e-005f ), CVector ( -2593.44f, 458.411f, 200.0f ) );
    AddZone ( "Ocean Flats", CVector ( -2994.49f, 277.411f, -9.15527e-005f ), CVector ( -2867.85f, 458.411f, 200.0f ) );
    AddZone ( "Ocean Flats", CVector ( -2994.49f, -222.589f, -0.000106812f ), CVector ( -2593.44f, 277.411f, 200.0f ) );
    AddZone ( "Ocean Flats", CVector ( -2994.49f, -430.276f, -0.00012207f ), CVector ( -2831.89f, -222.589f, 200.0f ) );
    AddZone ( "Foster Valley", CVector ( -2270.04f, -430.276f, -0.00012207f ), CVector ( -2178.69f, -324.114f, 200.0f ) );
    AddZone ( "Foster Valley", CVector ( -2178.69f, -599.884f, -0.00012207f ), CVector ( -1794.92f, -324.114f, 200.0f ) );
    AddZone ( "Hashbury", CVector ( -2593.44f, -222.589f, -0.000106812f ), CVector ( -2411.22f, 54.722f, 200.0f ) );
    AddZone ( "Juniper Hollow", CVector ( -2533.04f, 968.369f, -6.10352e-005f ), CVector ( -2274.17f, 1358.9f, 200.0f ) );
    AddZone ( "Esplanade North", CVector ( -2533.04f, 1358.9f, -4.57764e-005f ), CVector ( -1996.66f, 1501.21f, 200.0f ) );
    AddZone ( "Esplanade North", CVector ( -1996.66f, 1358.9f, -4.57764e-005f ), CVector ( -1524.24f, 1592.51f, 200.0f ) );
    AddZone ( "Esplanade North", CVector ( -1982.32f, 1274.26f, -4.57764e-005f ), CVector ( -1524.24f, 1358.9f, 200.0f ) );
    AddZone ( "Financial", CVector ( -1871.72f, 744.17f, -6.10352e-005f ), CVector ( -1701.3f, 1176.42f, 300.0f ) );
    AddZone ( "Calton Heights", CVector ( -2274.17f, 744.17f, -6.10352e-005f ), CVector ( -1982.32f, 1358.9f, 200.0f ) );
    AddZone ( "Downtown", CVector ( -1982.32f, 744.17f, -6.10352e-005f ), CVector ( -1871.72f, 1274.26f, 200.0f ) );
    AddZone ( "Downtown", CVector ( -1871.72f, 1176.42f, -4.57764e-005f ), CVector ( -1620.3f, 1274.26f, 200.0f ) );
    AddZone ( "Downtown", CVector ( -1700.01f, 744.267f, -6.10352e-005f ), CVector ( -1580.01f, 1176.52f, 200.0f ) );
    AddZone ( "Downtown", CVector ( -1580.01f, 744.267f, -6.10352e-005f ), CVector ( -1499.89f, 1025.98f, 200.0f ) );
    AddZone ( "Juniper Hill", CVector ( -2533.04f, 578.396f, -7.62939e-005f ), CVector ( -2274.17f, 968.369f, 200.0f ) );
    AddZone ( "Chinatown", CVector ( -2274.17f, 578.396f, -7.62939e-005f ), CVector ( -2078.67f, 744.17f, 200.0f ) );
    AddZone ( "Downtown", CVector ( -2078.67f, 578.396f, -7.62939e-005f ), CVector ( -1499.89f, 744.267f, 200.0f ) );
    AddZone ( "King's", CVector ( -2329.31f, 458.411f, -7.62939e-005f ), CVector ( -1993.28f, 578.396f, 200.0f ) );
    AddZone ( "King's", CVector ( -2411.22f, 265.243f, -9.15527e-005f ), CVector ( -1993.28f, 373.539f, 200.0f ) );
    AddZone ( "King's", CVector ( -2253.54f, 373.539f, -9.15527e-005f ), CVector ( -1993.28f, 458.411f, 200.0f ) );
    AddZone ( "Garcia", CVector ( -2411.22f, -222.589f, -0.000114441f ), CVector ( -2173.04f, 265.243f, 200.0f ) );
    AddZone ( "Doherty", CVector ( -2270.04f, -324.114f, -0.00012207f ), CVector ( -1794.92f, -222.589f, 200.0f ) );
    AddZone ( "Doherty", CVector ( -2173.04f, -222.589f, -0.000106812f ), CVector ( -1794.92f, 265.243f, 200.0f ) );
    AddZone ( "Downtown", CVector ( -1993.28f, 265.243f, -9.15527e-005f ), CVector ( -1794.92f, 578.396f, 200.0f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1499.89f, -50.0963f, -0.000106812f ), CVector ( -1242.98f, 249.904f, 200.0f ) );
    AddZone ( "Easter Basin", CVector ( -1794.92f, 249.904f, -9.15527e-005f ), CVector ( -1242.98f, 578.396f, 200.0f ) );
    AddZone ( "Easter Basin", CVector ( -1794.92f, -50.0963f, -0.000106812f ), CVector ( -1499.89f, 249.904f, 200.0f ) );
    AddZone ( "Esplanade East", CVector ( -1620.3f, 1176.52f, -4.57764e-005f ), CVector ( -1580.01f, 1274.26f, 200.0f ) );
    AddZone ( "Esplanade East", CVector ( -1580.01f, 1025.98f, -6.10352e-005f ), CVector ( -1499.89f, 1274.26f, 200.0f ) );
    AddZone ( "Esplanade East", CVector ( -1499.89f, 578.396f, -79.6152f ), CVector ( -1339.89f, 1274.26f, 20.3848f ) );
    AddZone ( "Angel Pine", CVector ( -2324.94f, -2584.29f, -6.10352e-005f ), CVector ( -1964.22f, -2212.11f, 200.0f ) );
    AddZone ( "Shady Cabin", CVector ( -1632.83f, -2263.44f, -3.05176e-005f ), CVector ( -1601.33f, -2231.79f, 200.0f ) );
    AddZone ( "Back o Beyond", CVector ( -1166.97f, -2641.19f, 0.0f ), CVector ( -321.744f, -1856.03f, 200.0f ) );
    AddZone ( "Leafy Hollow", CVector ( -1166.97f, -1856.03f, 0.0f ), CVector ( -815.624f, -1602.07f, 200.0f ) );
    AddZone ( "Flint Range", CVector ( -594.191f, -1648.55f, 0.0f ), CVector ( -187.7f, -1276.6f, 200.0f ) );
    AddZone ( "Fallen Tree", CVector ( -792.254f, -698.555f, -5.34058e-005f ), CVector ( -452.404f, -380.043f, 200.0f ) );
    AddZone ( "The Farm", CVector ( -1209.67f, -1317.1f, 114.981f ), CVector ( -908.161f, -787.391f, 251.981f ) );
    AddZone ( "El Quebrados", CVector ( -1645.23f, 2498.52f, 0.0f ), CVector ( -1372.14f, 2777.85f, 200.0f ) );
    AddZone ( "Aldea Malvada", CVector ( -1372.14f, 2498.52f, 0.0f ), CVector ( -1277.59f, 2615.35f, 200.0f ) );
    AddZone ( "The Sherman Dam", CVector ( -968.772f, 1929.41f, -3.05176e-005f ), CVector ( -481.126f, 2155.26f, 200.0f ) );
    AddZone ( "Las Barrancas", CVector ( -926.13f, 1398.73f, -3.05176e-005f ), CVector ( -719.234f, 1634.69f, 200.0f ) );
    AddZone ( "Fort Carson", CVector ( -376.233f, 826.326f, -3.05176e-005f ), CVector ( 123.717f, 1220.44f, 200.0f ) );
    AddZone ( "Hunter Quarry", CVector ( 337.244f, 710.84f, -115.239f ), CVector ( 860.554f, 1031.71f, 203.761f ) );
    AddZone ( "Octane Springs", CVector ( 338.658f, 1228.51f, 0.0f ), CVector ( 664.308f, 1655.05f, 200.0f ) );
    AddZone ( "Green Palms", CVector ( 176.581f, 1305.45f, -3.05176e-005f ), CVector ( 338.658f, 1520.72f, 200.0f ) );
    AddZone ( "Regular Tom", CVector ( -405.77f, 1712.86f, -3.05176e-005f ), CVector ( -276.719f, 1892.75f, 200.0f ) );
    AddZone ( "Las Brujas", CVector ( -365.167f, 2123.01f, -3.05176e-005f ), CVector ( -208.57f, 2217.68f, 200.0f ) );
    AddZone ( "Verdant Meadows", CVector ( 37.0325f, 2337.18f, -3.05176e-005f ), CVector ( 435.988f, 2677.9f, 200.0f ) );
    AddZone ( "Las Payasadas", CVector ( -354.332f, 2580.36f, 2.09808e-005f ), CVector ( -133.625f, 2816.82f, 200.0f ) );
    AddZone ( "Arco del Oeste", CVector ( -901.129f, 2221.86f, 0.0f ), CVector ( -592.09f, 2571.97f, 200.0f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1794.92f, -730.118f, -3.05176e-005f ), CVector ( -1213.91f, -50.0963f, 200.0f ) );
    AddZone ( "Hankypanky Point", CVector ( 2576.92f, 62.1579f, 0.0f ), CVector ( 2759.25f, 385.503f, 200.0f ) );
    AddZone ( "Palomino Creek", CVector ( 2160.22f, -149.004f, 0.0f ), CVector ( 2576.92f, 228.322f, 200.0f ) );
    AddZone ( "North Rock", CVector ( 2285.37f, -768.027f, 0.0f ), CVector ( 2770.59f, -269.74f, 200.0f ) );
    AddZone ( "Montgomery", CVector ( 1119.51f, 119.526f, -3.05176e-005f ), CVector ( 1451.4f, 493.323f, 200.0f ) );
    AddZone ( "Montgomery", CVector ( 1451.4f, 347.457f, -6.10352e-005f ), CVector ( 1582.44f, 420.802f, 200.0f ) );
    AddZone ( "Hampton Barns", CVector ( 603.035f, 264.312f, 0.0f ), CVector ( 761.994f, 366.572f, 200.0f ) );
    AddZone ( "Fern Ridge", CVector ( 508.189f, -139.259f, 0.0f ), CVector ( 1306.66f, 119.526f, 200.0f ) );
    AddZone ( "Dillimore", CVector ( 580.794f, -674.885f, -9.53674e-006f ), CVector ( 861.085f, -404.79f, 200.0f ) );
    AddZone ( "Hilltop Farm", CVector ( 967.383f, -450.39f, -3.05176e-005f ), CVector ( 1176.78f, -217.9f, 200.0f ) );
    AddZone ( "Blueberry", CVector ( 104.534f, -220.137f, 2.38419e-007f ), CVector ( 349.607f, 152.236f, 200.0f ) );
    AddZone ( "Blueberry", CVector ( 19.6074f, -404.136f, 3.8147e-006f ), CVector ( 349.607f, -220.137f, 200.0f ) );
    AddZone ( "The Panopticon", CVector ( -947.98f, -304.32f, -1.14441e-005f ), CVector ( -319.676f, 327.071f, 200.0f ) );
    AddZone ( "Frederick Bridge", CVector ( 2759.25f, 296.501f, 0.0f ), CVector ( 2774.25f, 594.757f, 200.0f ) );
    AddZone ( "The Mako Span", CVector ( 1664.62f, 401.75f, 0.0f ), CVector ( 1785.14f, 567.203f, 200.0f ) );
    AddZone ( "Blueberry Acres", CVector ( -319.676f, -220.137f, 0.0f ), CVector ( 104.534f, 293.324f, 200.0f ) );
    AddZone ( "Martin Bridge", CVector ( -222.179f, 293.324f, 0.0f ), CVector ( -122.126f, 476.465f, 200.0f ) );
    AddZone ( "Fallow Bridge", CVector ( 434.341f, 366.572f, 0.0f ), CVector ( 603.035f, 555.68f, 200.0f ) );
    AddZone ( "Shady Creeks", CVector ( -1820.64f, -2643.68f, -8.01086e-005f ), CVector ( -1226.78f, -1771.66f, 200.0f ) );
    AddZone ( "Shady Creeks", CVector ( -2030.12f, -2174.89f, -6.10352e-005f ), CVector ( -1820.64f, -1771.66f, 200.0f ) );
    AddZone ( "Queens", CVector ( -2533.04f, 458.411f, 0.0f ), CVector ( -2329.31f, 578.396f, 200.0f ) );
    AddZone ( "Queens", CVector ( -2593.44f, 54.722f, 0.0f ), CVector ( -2411.22f, 458.411f, 200.0f ) );
    AddZone ( "Queens", CVector ( -2411.22f, 373.539f, 0.0f ), CVector ( -2253.54f, 458.411f, 200.0f ) );
    AddZone ( "Los Santos", CVector ( 44.6147f, -2892.97f, -242.99f ), CVector ( 2997.06f, -768.027f, 900.0f ) );
    AddZone ( "Las Venturas", CVector ( 869.461f, 596.349f, -242.99f ), CVector ( 2997.06f, 2993.87f, 900.0f ) );
    AddZone ( "Bone County", CVector ( -480.539f, 596.349f, -242.99f ), CVector ( 869.461f, 2993.87f, 900.0f ) );
    AddZone ( "Tierra Robada", CVector ( -2997.47f, 1659.68f, -242.99f ), CVector ( -480.539f, 2993.87f, 900.0f ) );
    AddZone ( "Gant Bridge", CVector ( -2741.45f, 1659.68f, -6.10352e-005f ), CVector ( -2616.4f, 2175.15f, 200.0f ) );
    AddZone ( "Gant Bridge", CVector ( -2741.07f, 1490.47f, -6.10352e-005f ), CVector ( -2616.4f, 1659.68f, 200.0f ) );
    AddZone ( "San Fierro", CVector ( -2997.47f, -1115.58f, -242.99f ), CVector ( -1213.91f, 1659.68f, 900.0f ) );
    AddZone ( "Tierra Robada", CVector ( -1213.91f, 596.349f, -242.99f ), CVector ( -480.539f, 1659.68f, 900.0f ) );
    AddZone ( "Red County", CVector ( -1213.91f, -768.027f, -242.99f ), CVector ( 2997.06f, 596.349f, 900.0f ) );
    AddZone ( "Flint County", CVector ( -1213.91f, -2892.97f, -242.99f ), CVector ( 44.6147f, -768.027f, 900.0f ) );
    AddZone ( "Easter Bay Chemicals", CVector ( -1132.82f, -768.027f, 0.0f ), CVector ( -956.476f, -578.118f, 200.0f ) );
    AddZone ( "Easter Bay Chemicals", CVector ( -1132.82f, -787.391f, 0.0f ), CVector ( -956.476f, -768.027f, 200.0f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1213.91f, -730.118f, 0.0f ), CVector ( -1132.82f, -50.0963f, 200.0f ) );
    AddZone ( "Foster Valley", CVector ( -2178.69f, -1115.58f, 0.0f ), CVector ( -1794.92f, -599.884f, 200.0f ) );
    AddZone ( "Foster Valley", CVector ( -2178.69f, -1250.97f, 0.0f ), CVector ( -1794.92f, -1115.58f, 200.0f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1242.98f, -50.0963f, 0.0f ), CVector ( -1213.91f, 578.396f, 200.0f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1213.91f, -50.096f, -4.57764e-005f ), CVector ( -947.98f, 578.396f, 200.0f ) );
    AddZone ( "Whetstone", CVector ( -2997.47f, -2892.97f, -242.99f ), CVector ( -1213.91f, -1115.58f, 900.0f ) );
    AddZone ( "Los Santos International", CVector ( 1249.62f, -2394.33f, -89.0839f ), CVector ( 1852.0f, -2179.25f, 110.916f ) );
    AddZone ( "Los Santos International", CVector ( 1852.0f, -2394.33f, -89.0839f ), CVector ( 2089.0f, -2179.25f, 110.916f ) );
    AddZone ( "Verdant Bluffs", CVector ( 930.221f, -2488.42f, -89.0839f ), CVector ( 1249.62f, -2006.78f, 110.916f ) );
    AddZone ( "El Corona", CVector ( 1812.62f, -2179.25f, -89.0839f ), CVector ( 1970.62f, -1852.87f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 1970.62f, -2179.25f, -89.0839f ), CVector ( 2089.0f, -1852.87f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2089.0f, -2235.84f, -89.0839f ), CVector ( 2201.82f, -1989.9f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2089.0f, -1989.9f, -89.0839f ), CVector ( 2324.0f, -1852.87f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2201.82f, -2095.0f, -89.0839f ), CVector ( 2324.0f, -1989.9f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2373.77f, -2697.09f, -89.0837f ), CVector ( 2809.22f, -2330.46f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2201.82f, -2418.33f, -89.0837f ), CVector ( 2324.0f, -2095.0f, 110.916f ) );
    AddZone ( "Marina", CVector ( 647.712f, -1804.21f, -89.0839f ), CVector ( 851.449f, -1577.59f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 647.712f, -2173.29f, -89.0839f ), CVector ( 930.221f, -1804.21f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 930.221f, -2006.78f, -89.0839f ), CVector ( 1073.22f, -1804.21f, 110.916f ) );
    AddZone ( "Verdant Bluffs", CVector ( 1073.22f, -2006.78f, -89.0839f ), CVector ( 1249.62f, -1842.27f, 110.916f ) );
    AddZone ( "Verdant Bluffs", CVector ( 1249.62f, -2179.25f, -89.0839f ), CVector ( 1692.62f, -1842.27f, 110.916f ) );
    AddZone ( "El Corona", CVector ( 1692.62f, -2179.25f, -89.0839f ), CVector ( 1812.62f, -1842.27f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 851.449f, -1804.21f, -89.0839f ), CVector ( 1046.15f, -1577.59f, 110.916f ) );
    AddZone ( "Marina", CVector ( 647.712f, -1577.59f, -89.0838f ), CVector ( 807.922f, -1416.25f, 110.916f ) );
    AddZone ( "Marina", CVector ( 807.922f, -1577.59f, -89.0839f ), CVector ( 926.922f, -1416.25f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 1161.52f, -1722.26f, -89.0839f ), CVector ( 1323.9f, -1577.59f, 110.916f ) );
    AddZone ( "Verona Beach", CVector ( 1046.15f, -1722.26f, -89.0839f ), CVector ( 1161.52f, -1577.59f, 110.916f ) );
    AddZone ( "Conference Center", CVector ( 1046.15f, -1804.21f, -89.0839f ), CVector ( 1323.9f, -1722.26f, 110.916f ) );
    AddZone ( "Conference Center", CVector ( 1073.22f, -1842.27f, -89.0839f ), CVector ( 1323.9f, -1804.21f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1323.9f, -1842.27f, -89.0839f ), CVector ( 1701.9f, -1722.26f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1323.9f, -1722.26f, -89.0839f ), CVector ( 1440.9f, -1577.59f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1370.85f, -1577.59f, -89.084f ), CVector ( 1463.9f, -1384.95f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1463.9f, -1577.59f, -89.0839f ), CVector ( 1667.96f, -1430.87f, 110.916f ) );
    AddZone ( "Pershing Square", CVector ( 1440.9f, -1722.26f, -89.0839f ), CVector ( 1583.5f, -1577.59f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1583.5f, -1722.26f, -89.0839f ), CVector ( 1758.9f, -1577.59f, 110.916f ) );
    AddZone ( "Little Mexico", CVector ( 1701.9f, -1842.27f, -89.0839f ), CVector ( 1812.62f, -1722.26f, 110.916f ) );
    AddZone ( "Little Mexico", CVector ( 1758.9f, -1722.26f, -89.0839f ), CVector ( 1812.62f, -1577.59f, 110.916f ) );
    AddZone ( "Commerce", CVector ( 1667.96f, -1577.59f, -89.0839f ), CVector ( 1812.62f, -1430.87f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1812.62f, -1852.87f, -89.0839f ), CVector ( 1971.66f, -1742.31f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1812.62f, -1742.31f, -89.0839f ), CVector ( 1951.66f, -1602.31f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1951.66f, -1742.31f, -89.0839f ), CVector ( 2124.66f, -1602.31f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1812.62f, -1602.31f, -89.0839f ), CVector ( 2124.66f, -1449.67f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 2124.66f, -1742.31f, -89.0839f ), CVector ( 2222.56f, -1494.03f, 110.916f ) );
    AddZone ( "Glen Park", CVector ( 1812.62f, -1449.67f, -89.0839f ), CVector ( 1996.91f, -1350.72f, 110.916f ) );
    AddZone ( "Glen Park", CVector ( 1812.62f, -1100.82f, -89.0839f ), CVector ( 1994.33f, -973.38f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 1996.91f, -1449.67f, -89.0839f ), CVector ( 2056.86f, -1350.72f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2124.66f, -1494.03f, -89.0839f ), CVector ( 2266.21f, -1449.67f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2056.86f, -1372.04f, -89.0839f ), CVector ( 2281.45f, -1210.74f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2056.86f, -1210.74f, -89.0839f ), CVector ( 2185.33f, -1126.32f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2185.33f, -1210.74f, -89.0839f ), CVector ( 2281.45f, -1154.59f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 1994.33f, -1100.82f, -89.0839f ), CVector ( 2056.86f, -920.815f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2056.86f, -1126.32f, -89.0839f ), CVector ( 2126.86f, -920.815f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2185.33f, -1154.59f, -89.0839f ), CVector ( 2281.45f, -934.489f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2126.86f, -1126.32f, -89.0839f ), CVector ( 2185.33f, -934.489f, 110.916f ) );
    AddZone ( "Idlewood", CVector ( 1971.66f, -1852.87f, -89.0839f ), CVector ( 2222.56f, -1742.31f, 110.916f ) );
    AddZone ( "Ganton", CVector ( 2222.56f, -1852.87f, -89.0839f ), CVector ( 2632.83f, -1722.33f, 110.916f ) );
    AddZone ( "Ganton", CVector ( 2222.56f, -1722.33f, -89.0839f ), CVector ( 2632.83f, -1628.53f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2541.7f, -1941.4f, -89.0839f ), CVector ( 2703.58f, -1852.87f, 110.916f ) );
    AddZone ( "East Beach", CVector ( 2632.83f, -1852.87f, -89.0839f ), CVector ( 2959.35f, -1668.13f, 110.916f ) );
    AddZone ( "East Beach", CVector ( 2632.83f, -1668.13f, -89.0839f ), CVector ( 2747.74f, -1393.42f, 110.916f ) );
    AddZone ( "East Beach", CVector ( 2747.74f, -1668.13f, -89.0839f ), CVector ( 2959.35f, -1498.62f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2421.03f, -1628.53f, -89.0839f ), CVector ( 2632.83f, -1454.35f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2222.56f, -1628.53f, -89.0839f ), CVector ( 2421.03f, -1494.03f, 110.916f ) );
    AddZone ( "Jefferson", CVector ( 2056.86f, -1449.67f, -89.0839f ), CVector ( 2266.21f, -1372.04f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2266.26f, -1494.03f, -89.0839f ), CVector ( 2381.68f, -1372.04f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2381.68f, -1494.03f, -89.0839f ), CVector ( 2421.03f, -1454.35f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2281.45f, -1372.04f, -89.084f ), CVector ( 2381.68f, -1135.04f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2381.68f, -1454.35f, -89.0839f ), CVector ( 2462.13f, -1135.04f, 110.916f ) );
    AddZone ( "East Los Santos", CVector ( 2462.13f, -1454.35f, -89.0839f ), CVector ( 2581.73f, -1135.04f, 110.916f ) );
    AddZone ( "Los Flores", CVector ( 2581.73f, -1454.35f, -89.0839f ), CVector ( 2632.83f, -1393.42f, 110.916f ) );
    AddZone ( "Los Flores", CVector ( 2581.73f, -1393.42f, -89.0839f ), CVector ( 2747.74f, -1135.04f, 110.916f ) );
    AddZone ( "East Beach", CVector ( 2747.74f, -1498.62f, -89.0839f ), CVector ( 2959.35f, -1120.04f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2747.74f, -1120.04f, -89.0839f ), CVector ( 2959.35f, -945.035f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2632.74f, -1135.04f, -89.0839f ), CVector ( 2747.74f, -945.035f, 110.916f ) );
    AddZone ( "Las Colinas", CVector ( 2281.45f, -1135.04f, -89.0839f ), CVector ( 2632.74f, -945.035f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1463.9f, -1430.87f, -89.084f ), CVector ( 1724.76f, -1290.87f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1724.76f, -1430.87f, -89.0839f ), CVector ( 1812.62f, -1250.9f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1463.9f, -1290.87f, -89.084f ), CVector ( 1724.76f, -1150.87f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1370.85f, -1384.95f, -89.0839f ), CVector ( 1463.9f, -1170.87f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1724.76f, -1250.9f, -89.0839f ), CVector ( 1812.62f, -1150.87f, 110.916f ) );
    AddZone ( "Mulholland Intersection", CVector ( 1463.9f, -1150.87f, -89.0839f ), CVector ( 1812.62f, -768.027f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1414.07f, -768.027f, -89.0839f ), CVector ( 1667.61f, -452.425f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1281.13f, -452.425f, -89.0839f ), CVector ( 1641.13f, -290.913f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1269.13f, -768.027f, -89.0839f ), CVector ( 1414.07f, -452.425f, 110.916f ) );
    AddZone ( "Market", CVector ( 787.461f, -1416.25f, -89.0838f ), CVector ( 1072.66f, -1310.21f, 110.916f ) );
    AddZone ( "Vinewood", CVector ( 787.461f, -1310.21f, -89.0838f ), CVector ( 952.663f, -1130.84f, 110.916f ) );
    AddZone ( "Market", CVector ( 952.663f, -1310.21f, -89.0839f ), CVector ( 1072.66f, -1130.85f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1370.85f, -1170.87f, -89.0839f ), CVector ( 1463.9f, -1130.85f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1378.33f, -1130.85f, -89.0838f ), CVector ( 1463.9f, -1026.33f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1391.05f, -1026.33f, -89.0839f ), CVector ( 1463.9f, -926.999f, 110.916f ) );
    AddZone ( "Temple", CVector ( 1252.33f, -1130.85f, -89.0839f ), CVector ( 1378.33f, -1026.33f, 110.916f ) );
    AddZone ( "Temple", CVector ( 1252.33f, -1026.33f, -89.0839f ), CVector ( 1391.05f, -926.999f, 110.916f ) );
    AddZone ( "Temple", CVector ( 1252.33f, -926.999f, -89.0839f ), CVector ( 1357.0f, -910.17f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1357.0f, -926.999f, -89.0838f ), CVector ( 1463.9f, -768.027f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1318.13f, -910.17f, -89.0839f ), CVector ( 1357.0f, -768.027f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1169.13f, -910.17f, -89.0838f ), CVector ( 1318.13f, -768.027f, 110.916f ) );
    AddZone ( "Vinewood", CVector ( 787.461f, -1130.84f, -89.0839f ), CVector ( 952.604f, -954.662f, 110.916f ) );
    AddZone ( "Temple", CVector ( 952.663f, -1130.84f, -89.084f ), CVector ( 1096.47f, -937.184f, 110.916f ) );
    AddZone ( "Temple", CVector ( 1096.47f, -1130.84f, -89.0838f ), CVector ( 1252.33f, -1026.33f, 110.916f ) );
    AddZone ( "Temple", CVector ( 1096.47f, -1026.33f, -89.0839f ), CVector ( 1252.33f, -910.17f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 768.694f, -954.662f, -89.0838f ), CVector ( 952.604f, -860.619f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 687.802f, -860.619f, -89.0839f ), CVector ( 911.802f, -768.027f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 737.573f, -768.027f, -89.0838f ), CVector ( 1142.29f, -674.885f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 1096.47f, -910.17f, -89.0838f ), CVector ( 1169.13f, -768.027f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 952.604f, -937.184f, -89.0839f ), CVector ( 1096.47f, -860.619f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 911.802f, -860.619f, -89.0838f ), CVector ( 1096.47f, -768.027f, 110.916f ) );
    AddZone ( "Mulholland", CVector ( 861.085f, -674.885f, -89.0839f ), CVector ( 1156.55f, -600.896f, 110.916f ) );
    AddZone ( "Santa Maria Beach", CVector ( 342.648f, -2173.29f, -89.0838f ), CVector ( 647.712f, -1684.65f, 110.916f ) );
    AddZone ( "Santa Maria Beach", CVector ( 72.6481f, -2173.29f, -89.0839f ), CVector ( 342.648f, -1684.65f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 72.6481f, -1684.65f, -89.084f ), CVector ( 225.165f, -1544.17f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 72.6481f, -1544.17f, -89.0839f ), CVector ( 225.165f, -1404.97f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 225.165f, -1684.65f, -89.0839f ), CVector ( 312.803f, -1501.95f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 225.165f, -1501.95f, -89.0839f ), CVector ( 334.503f, -1369.62f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 334.503f, -1501.95f, -89.0839f ), CVector ( 422.68f, -1406.05f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 312.803f, -1684.65f, -89.0839f ), CVector ( 422.68f, -1501.95f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 422.68f, -1684.65f, -89.0839f ), CVector ( 558.099f, -1570.2f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 558.099f, -1684.65f, -89.0839f ), CVector ( 647.522f, -1384.93f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 466.223f, -1570.2f, -89.0839f ), CVector ( 558.099f, -1385.07f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 422.68f, -1570.2f, -89.0839f ), CVector ( 466.223f, -1406.05f, 110.916f ) );
    AddZone ( "Vinewood", CVector ( 647.557f, -1227.28f, -89.0839f ), CVector ( 787.461f, -1118.28f, 110.916f ) );
    AddZone ( "Richman", CVector ( 647.557f, -1118.28f, -89.0839f ), CVector ( 787.461f, -954.662f, 110.916f ) );
    AddZone ( "Richman", CVector ( 647.557f, -954.662f, -89.0839f ), CVector ( 768.694f, -860.619f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 466.223f, -1385.07f, -89.0839f ), CVector ( 647.522f, -1235.07f, 110.916f ) );
    AddZone ( "Rodeo", CVector ( 334.503f, -1406.05f, -89.0839f ), CVector ( 466.223f, -1292.07f, 110.916f ) );
    AddZone ( "Richman", CVector ( 225.165f, -1369.62f, -89.0839f ), CVector ( 334.503f, -1292.07f, 110.916f ) );
    AddZone ( "Richman", CVector ( 225.165f, -1292.07f, -89.084f ), CVector ( 466.223f, -1235.07f, 110.916f ) );
    AddZone ( "Richman", CVector ( 72.6481f, -1404.97f, -89.0839f ), CVector ( 225.165f, -1235.07f, 110.916f ) );
    AddZone ( "Richman", CVector ( 72.6481f, -1235.07f, -89.0839f ), CVector ( 321.356f, -1008.15f, 110.916f ) );
    AddZone ( "Richman", CVector ( 321.356f, -1235.07f, -89.0839f ), CVector ( 647.522f, -1044.07f, 110.916f ) );
    AddZone ( "Richman", CVector ( 321.356f, -1044.07f, -89.0839f ), CVector ( 647.557f, -860.619f, 110.916f ) );
    AddZone ( "Richman", CVector ( 321.356f, -860.619f, -89.0839f ), CVector ( 687.802f, -768.027f, 110.916f ) );
    AddZone ( "Richman", CVector ( 321.356f, -768.027f, -89.0839f ), CVector ( 700.794f, -674.885f, 110.916f ) );
    AddZone ( "The Strip", CVector ( 2027.4f, 863.229f, -89.0839f ), CVector ( 2087.39f, 1703.23f, 110.916f ) );
    AddZone ( "The Strip", CVector ( 2106.7f, 1863.23f, -89.0839f ), CVector ( 2162.39f, 2202.76f, 110.916f ) );
    AddZone ( "The Four Dragons Casino", CVector ( 1817.39f, 863.232f, -89.084f ), CVector ( 2027.39f, 1083.23f, 110.916f ) );
    AddZone ( "The Pink Swan", CVector ( 1817.39f, 1083.23f, -89.0839f ), CVector ( 2027.39f, 1283.23f, 110.916f ) );
    AddZone ( "The High Roller", CVector ( 1817.39f, 1283.23f, -89.0839f ), CVector ( 2027.39f, 1469.23f, 110.916f ) );
    AddZone ( "Pirates in Men's Pants", CVector ( 1817.39f, 1469.23f, -89.084f ), CVector ( 2027.4f, 1703.23f, 110.916f ) );
    AddZone ( "The Visage", CVector ( 1817.39f, 1863.23f, -89.0839f ), CVector ( 2106.7f, 2011.83f, 110.916f ) );
    AddZone ( "The Visage", CVector ( 1817.39f, 1703.23f, -89.0839f ), CVector ( 2027.4f, 1863.23f, 110.916f ) );
    AddZone ( "Julius Thruway South", CVector ( 1457.39f, 823.228f, -89.0839f ), CVector ( 2377.39f, 863.229f, 110.916f ) );
    AddZone ( "Julius Thruway West", CVector ( 1197.39f, 1163.39f, -89.0839f ), CVector ( 1236.63f, 2243.23f, 110.916f ) );
    AddZone ( "Julius Thruway South", CVector ( 2377.39f, 788.894f, -89.0839f ), CVector ( 2537.39f, 897.901f, 110.916f ) );
    AddZone ( "Rockshore East", CVector ( 2537.39f, 676.549f, -89.0839f ), CVector ( 2902.35f, 943.235f, 110.916f ) );
    AddZone ( "Come-A-Lot", CVector ( 2087.39f, 943.235f, -89.0839f ), CVector ( 2623.18f, 1203.23f, 110.916f ) );
    AddZone ( "The Camel's Toe", CVector ( 2087.39f, 1203.23f, -89.0839f ), CVector ( 2640.4f, 1383.23f, 110.916f ) );
    AddZone ( "Royal Casino", CVector ( 2087.39f, 1383.23f, -89.0839f ), CVector ( 2437.39f, 1543.23f, 110.916f ) );
    AddZone ( "Caligula's Palace", CVector ( 2087.39f, 1543.23f, -89.0839f ), CVector ( 2437.39f, 1703.23f, 110.916f ) );
    AddZone ( "Caligula's Palace", CVector ( 2137.4f, 1703.23f, -89.0839f ), CVector ( 2437.39f, 1783.23f, 110.916f ) );
    AddZone ( "Pilgrim", CVector ( 2437.39f, 1383.23f, -89.0839f ), CVector ( 2624.4f, 1783.23f, 110.916f ) );
    AddZone ( "Starfish Casino", CVector ( 2437.39f, 1783.23f, -89.0839f ), CVector ( 2685.16f, 2012.18f, 110.916f ) );
    AddZone ( "The Strip", CVector ( 2027.4f, 1783.23f, -89.084f ), CVector ( 2162.39f, 1863.23f, 110.916f ) );
    AddZone ( "The Strip", CVector ( 2027.4f, 1703.23f, -89.0839f ), CVector ( 2137.4f, 1783.23f, 110.916f ) );
    AddZone ( "The Emerald Isle", CVector ( 2011.94f, 2202.76f, -89.0839f ), CVector ( 2237.4f, 2508.23f, 110.916f ) );
    AddZone ( "Old Venturas Strip", CVector ( 2162.39f, 2012.18f, -89.0839f ), CVector ( 2685.16f, 2202.76f, 110.916f ) );
    AddZone ( "K.A.C.C. Military Fuels", CVector ( 2498.21f, 2626.55f, -89.0839f ), CVector ( 2749.9f, 2861.55f, 110.916f ) );
    AddZone ( "Creek", CVector ( 2749.9f, 1937.25f, -89.0839f ), CVector ( 2921.62f, 2669.79f, 110.916f ) );
    AddZone ( "Sobell Rail Yards", CVector ( 2749.9f, 1548.99f, -89.0839f ), CVector ( 2923.39f, 1937.25f, 110.916f ) );
    AddZone ( "Linden Station", CVector ( 2749.9f, 1198.99f, -89.0839f ), CVector ( 2923.39f, 1548.99f, 110.916f ) );
    AddZone ( "Julius Thruway East", CVector ( 2623.18f, 943.235f, -89.0839f ), CVector ( 2749.9f, 1055.96f, 110.916f ) );
    AddZone ( "Linden Side", CVector ( 2749.9f, 943.235f, -89.0839f ), CVector ( 2923.39f, 1198.99f, 110.916f ) );
    AddZone ( "Julius Thruway East", CVector ( 2685.16f, 1055.96f, -89.0839f ), CVector ( 2749.9f, 2626.55f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 2498.21f, 2542.55f, -89.0839f ), CVector ( 2685.16f, 2626.55f, 110.916f ) );
    AddZone ( "Julius Thruway East", CVector ( 2536.43f, 2442.55f, -89.0839f ), CVector ( 2685.16f, 2542.55f, 110.916f ) );
    AddZone ( "Julius Thruway East", CVector ( 2625.16f, 2202.76f, -89.0839f ), CVector ( 2685.16f, 2442.55f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 2237.4f, 2542.55f, -89.0839f ), CVector ( 2498.21f, 2663.17f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 2121.4f, 2508.23f, -89.0839f ), CVector ( 2237.4f, 2663.17f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1938.8f, 2508.23f, -89.0839f ), CVector ( 2121.4f, 2624.23f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1534.56f, 2433.23f, -89.0839f ), CVector ( 1848.4f, 2583.23f, 110.916f ) );
    AddZone ( "Julius Thruway West", CVector ( 1236.63f, 2142.86f, -89.084f ), CVector ( 1297.47f, 2243.23f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1848.4f, 2478.49f, -89.0839f ), CVector ( 1938.8f, 2553.49f, 110.916f ) );
    AddZone ( "Harry Gold Parkway", CVector ( 1777.39f, 863.232f, -89.0839f ), CVector ( 1817.39f, 2342.83f, 110.916f ) );
    AddZone ( "Redsands East", CVector ( 1817.39f, 2011.83f, -89.0839f ), CVector ( 2106.7f, 2202.76f, 110.916f ) );
    AddZone ( "Redsands East", CVector ( 1817.39f, 2202.76f, -89.0839f ), CVector ( 2011.94f, 2342.83f, 110.916f ) );
    AddZone ( "Redsands East", CVector ( 1848.4f, 2342.83f, -89.084f ), CVector ( 2011.94f, 2478.49f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1704.59f, 2342.83f, -89.0839f ), CVector ( 1848.4f, 2433.23f, 110.916f ) );
    AddZone ( "Redsands West", CVector ( 1236.63f, 1883.11f, -89.0839f ), CVector ( 1777.39f, 2142.86f, 110.916f ) );
    AddZone ( "Redsands West", CVector ( 1297.47f, 2142.86f, -89.084f ), CVector ( 1777.39f, 2243.23f, 110.916f ) );
    AddZone ( "Redsands West", CVector ( 1377.39f, 2243.23f, -89.0839f ), CVector ( 1704.59f, 2433.23f, 110.916f ) );
    AddZone ( "Redsands West", CVector ( 1704.59f, 2243.23f, -89.0839f ), CVector ( 1777.39f, 2342.83f, 110.916f ) );
    AddZone ( "Las Venturas Airport", CVector ( 1236.63f, 1203.28f, -89.0839f ), CVector ( 1457.37f, 1883.11f, 110.916f ) );
    AddZone ( "Las Venturas Airport", CVector ( 1457.37f, 1203.28f, -89.0839f ), CVector ( 1777.39f, 1883.11f, 110.916f ) );
    AddZone ( "Las Venturas Airport", CVector ( 1457.37f, 1143.21f, -89.0839f ), CVector ( 1777.4f, 1203.28f, 110.916f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1457.39f, 863.229f, -89.0839f ), CVector ( 1777.4f, 1143.21f, 110.916f ) );
    AddZone ( "Blackfield Intersection", CVector ( 1197.39f, 1044.69f, -89.0839f ), CVector ( 1277.05f, 1163.39f, 110.916f ) );
    AddZone ( "Blackfield Intersection", CVector ( 1166.53f, 795.01f, -89.0839f ), CVector ( 1375.6f, 1044.69f, 110.916f ) );
    AddZone ( "Blackfield Intersection", CVector ( 1277.05f, 1044.69f, -89.0839f ), CVector ( 1315.35f, 1087.63f, 110.916f ) );
    AddZone ( "Blackfield Intersection", CVector ( 1375.6f, 823.228f, -89.084f ), CVector ( 1457.39f, 919.447f, 110.916f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1375.6f, 919.447f, -89.0839f ), CVector ( 1457.37f, 1203.28f, 110.916f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1277.05f, 1087.63f, -89.0839f ), CVector ( 1375.6f, 1203.28f, 110.916f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1315.35f, 1044.69f, -89.0839f ), CVector ( 1375.6f, 1087.63f, 110.916f ) );
    AddZone ( "LVA Freight Depot", CVector ( 1236.63f, 1163.41f, -89.0839f ), CVector ( 1277.05f, 1203.28f, 110.916f ) );
    AddZone ( "Greenglass College", CVector ( 964.391f, 1044.69f, -89.0839f ), CVector ( 1197.39f, 1203.22f, 110.916f ) );
    AddZone ( "Greenglass College", CVector ( 964.391f, 930.89f, -89.0839f ), CVector ( 1166.53f, 1044.69f, 110.916f ) );
    AddZone ( "Blackfield", CVector ( 964.391f, 1203.22f, -89.084f ), CVector ( 1197.39f, 1403.22f, 110.916f ) );
    AddZone ( "Blackfield", CVector ( 964.391f, 1403.22f, -89.084f ), CVector ( 1197.39f, 1726.22f, 110.916f ) );
    AddZone ( "Roca Escalante", CVector ( 2237.4f, 2202.76f, -89.0839f ), CVector ( 2536.43f, 2542.55f, 110.916f ) );
    AddZone ( "Roca Escalante", CVector ( 2536.43f, 2202.76f, -89.0839f ), CVector ( 2625.16f, 2442.55f, 110.916f ) );
    AddZone ( "Last Dime Motel", CVector ( 1823.08f, 596.349f, -89.0839f ), CVector ( 1997.22f, 823.228f, 110.916f ) );
    AddZone ( "Rockshore West", CVector ( 1997.22f, 596.349f, -89.0839f ), CVector ( 2377.39f, 823.228f, 110.916f ) );
    AddZone ( "Rockshore West", CVector ( 2377.39f, 596.349f, -89.084f ), CVector ( 2537.39f, 788.894f, 110.916f ) );
    AddZone ( "Randolph Industrial Estate", CVector ( 1558.09f, 596.349f, -89.084f ), CVector ( 1823.08f, 823.235f, 110.916f ) );
    AddZone ( "Blackfield Chapel", CVector ( 1375.6f, 596.349f, -89.084f ), CVector ( 1558.09f, 823.228f, 110.916f ) );
    AddZone ( "Blackfield Chapel", CVector ( 1325.6f, 596.349f, -89.084f ), CVector ( 1375.6f, 795.01f, 110.916f ) );
    AddZone ( "Julius Thruway North", CVector ( 1377.39f, 2433.23f, -89.0839f ), CVector ( 1534.56f, 2507.23f, 110.916f ) );
    AddZone ( "Pilson Intersection", CVector ( 1098.39f, 2243.23f, -89.0839f ), CVector ( 1377.39f, 2507.23f, 110.916f ) );
    AddZone ( "Whitewood Estates", CVector ( 883.308f, 1726.22f, -89.0839f ), CVector ( 1098.31f, 2507.23f, 110.916f ) );
    AddZone ( "Prickle Pine", CVector ( 1534.56f, 2583.23f, -89.0839f ), CVector ( 1848.4f, 2863.23f, 110.916f ) );
    AddZone ( "Prickle Pine", CVector ( 1117.4f, 2507.23f, -89.0839f ), CVector ( 1534.56f, 2723.23f, 110.916f ) );
    AddZone ( "Prickle Pine", CVector ( 1848.4f, 2553.49f, -89.0839f ), CVector ( 1938.8f, 2863.23f, 110.916f ) );
    AddZone ( "Spinybed", CVector ( 2121.4f, 2663.17f, -89.0839f ), CVector ( 2498.21f, 2861.55f, 110.916f ) );
    AddZone ( "Prickle Pine", CVector ( 1938.8f, 2624.23f, -89.0839f ), CVector ( 2121.4f, 2861.55f, 110.916f ) );
    AddZone ( "Pilgrim", CVector ( 2624.4f, 1383.23f, -89.084f ), CVector ( 2685.16f, 1783.23f, 110.916f ) );
    AddZone ( "San Andreas Sound", CVector ( 2450.39f, 385.503f, -100.0f ), CVector ( 2759.25f, 562.349f, 200.0f ) );
    AddZone ( "Fisher's Lagoon", CVector ( 1916.99f, -233.323f, -100.0f ), CVector ( 2131.72f, 13.8002f, 200.0f ) );
    AddZone ( "Garver Bridge", CVector ( -1339.89f, 828.129f, -89.0839f ), CVector ( -1213.91f, 1057.04f, 110.916f ) );
    AddZone ( "Garver Bridge", CVector ( -1213.91f, 950.022f, -89.0839f ), CVector ( -1087.93f, 1178.93f, 110.916f ) );
    AddZone ( "Garver Bridge", CVector ( -1499.89f, 696.442f, -179.615f ), CVector ( -1339.89f, 925.353f, 20.3848f ) );
    AddZone ( "Kincaid Bridge", CVector ( -1339.89f, 599.218f, -89.0839f ), CVector ( -1213.91f, 828.129f, 110.916f ) );
    AddZone ( "Kincaid Bridge", CVector ( -1213.91f, 721.111f, -89.0839f ), CVector ( -1087.93f, 950.022f, 110.916f ) );
    AddZone ( "Kincaid Bridge", CVector ( -1087.93f, 855.37f, -89.0839f ), CVector ( -961.95f, 986.281f, 110.916f ) );
    AddZone ( "Los Santos Inlet", CVector ( -321.744f, -2224.43f, -89.0839f ), CVector ( 44.6147f, -1724.43f, 110.916f ) );
    AddZone ( "Sherman Reservoir", CVector ( -789.737f, 1659.68f, -89.084f ), CVector ( -599.505f, 1929.41f, 110.916f ) );
    AddZone ( "Flint Water", CVector ( -314.426f, -753.874f, -89.0839f ), CVector ( -106.339f, -463.073f, 110.916f ) );
    AddZone ( "Easter Tunnel", CVector ( -1709.71f, -833.034f, -1.52588e-005f ), CVector ( -1446.01f, -730.118f, 200.0f ) );
    AddZone ( "Bayside Tunnel", CVector ( -2290.19f, 2548.29f, -89.084f ), CVector ( -1950.19f, 2723.29f, 110.916f ) );
    AddZone ( "'The Big Ear'", CVector ( -410.02f, 1403.34f, -3.05176e-005f ), CVector ( -137.969f, 1681.23f, 200.0f ) );
    AddZone ( "Lil' Probe Inn", CVector ( -90.2183f, 1286.85f, -3.05176e-005f ), CVector ( 153.859f, 1554.12f, 200.0f ) );
    AddZone ( "Valle Ocultado", CVector ( -936.668f, 2611.44f, 2.09808e-005f ), CVector ( -715.961f, 2847.9f, 200.0f ) );
    AddZone ( "Glen Park", CVector ( 1812.62f, -1350.72f, -89.0839f ), CVector ( 2056.86f, -1100.82f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2324.0f, -2302.33f, -89.0839f ), CVector ( 2703.58f, -2145.1f, 110.916f ) );
    AddZone ( "Linden Station", CVector ( 2811.25f, 1229.59f, -39.594f ), CVector ( 2861.25f, 1407.59f, 60.406f ) );
    AddZone ( "Unity Station", CVector ( 1692.62f, -1971.8f, -20.4921f ), CVector ( 1812.62f, -1932.8f, 79.5079f ) );
    AddZone ( "Vinewood", CVector ( 647.712f, -1416.25f, -89.0839f ), CVector ( 787.461f, -1227.28f, 110.916f ) );
    AddZone ( "Market Station", CVector ( 787.461f, -1410.93f, -34.1263f ), CVector ( 866.009f, -1310.21f, 65.8737f ) );
    AddZone ( "Cranberry Station", CVector ( -2007.83f, 56.3063f, 0.0f ), CVector ( -1922.0f, 224.782f, 100.0f ) );
    AddZone ( "Yellow Bell Station", CVector ( 1377.48f, 2600.43f, -21.9263f ), CVector ( 1492.45f, 2687.36f, 78.0737f ) );
    AddZone ( "San Fierro Bay", CVector ( -2616.4f, 1501.21f, -3.05176e-005f ), CVector ( -1996.66f, 1659.68f, 200.0f ) );
    AddZone ( "San Fierro Bay", CVector ( -2616.4f, 1659.68f, -3.05176e-005f ), CVector ( -1996.66f, 2175.15f, 200.0f ) );
    AddZone ( "El Castillo del Diablo", CVector ( -464.515f, 2217.68f, 0.0f ), CVector ( -208.57f, 2580.36f, 200.0f ) );
    AddZone ( "El Castillo del Diablo", CVector ( -208.57f, 2123.01f, -7.62939e-006f ), CVector ( 114.033f, 2337.18f, 200.0f ) );
    AddZone ( "El Castillo del Diablo", CVector ( -208.57f, 2337.18f, 0.0f ), CVector ( 8.42999f, 2487.18f, 200.0f ) );
    AddZone ( "Restricted Area", CVector ( -91.586f, 1655.05f, -50.0f ), CVector ( 421.234f, 2123.01f, 250.0f ) );
    AddZone ( "Montgomery Intersection", CVector ( 1546.65f, 208.164f, 0.0f ), CVector ( 1745.83f, 347.457f, 200.0f ) );
    AddZone ( "Montgomery Intersection", CVector ( 1582.44f, 347.457f, 0.0f ), CVector ( 1664.62f, 401.75f, 200.0f ) );
    AddZone ( "Robada Intersection", CVector ( -1119.01f, 1178.93f, -89.084f ), CVector ( -862.025f, 1351.45f, 110.916f ) );
    AddZone ( "Flint Intersection", CVector ( -187.7f, -1596.76f, -89.0839f ), CVector ( 17.0632f, -1276.6f, 110.916f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1315.42f, -405.388f, 15.4061f ), CVector ( -1264.4f, -209.543f, 25.4061f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1354.39f, -287.398f, 15.4061f ), CVector ( -1315.42f, -209.543f, 25.4061f ) );
    AddZone ( "Easter Bay Airport", CVector ( -1490.33f, -209.543f, 15.4061f ), CVector ( -1264.4f, -148.388f, 25.4061f ) );
    AddZone ( "Market", CVector ( 1072.66f, -1416.25f, -89.084f ), CVector ( 1370.85f, -1130.85f, 110.916f ) );
    AddZone ( "Market", CVector ( 926.922f, -1577.59f, -89.0839f ), CVector ( 1370.85f, -1416.25f, 110.916f ) );
    AddZone ( "Avispa Country Club", CVector ( -2646.4f, -355.493f, 0.0f ), CVector ( -2270.04f, -222.589f, 200.0f ) );
    AddZone ( "Avispa Country Club", CVector ( -2831.89f, -430.276f, -6.10352e-005f ), CVector ( -2646.4f, -222.589f, 200.0f ) );
    AddZone ( "Missionary Hill", CVector ( -2994.49f, -811.276f, 0.0f ), CVector ( -2178.69f, -430.276f, 200.0f ) );
    AddZone ( "Mount Chiliad", CVector ( -2178.69f, -1771.66f, -47.9166f ), CVector ( -1936.12f, -1250.97f, 576.083f ) );
    AddZone ( "Mount Chiliad", CVector ( -2997.47f, -1115.58f, -47.9166f ), CVector ( -2178.69f, -971.913f, 576.083f ) );
    AddZone ( "Mount Chiliad", CVector ( -2994.49f, -2189.91f, -47.9166f ), CVector ( -2178.69f, -1115.58f, 576.083f ) );
    AddZone ( "Mount Chiliad", CVector ( -2178.69f, -2189.91f, -47.9166f ), CVector ( -2030.12f, -1771.66f, 576.083f ) );
    AddZone ( "Yellow Bell Golf Course", CVector ( 1117.4f, 2723.23f, -89.0839f ), CVector ( 1457.46f, 2863.23f, 110.916f ) );
    AddZone ( "Yellow Bell Golf Course", CVector ( 1457.46f, 2723.23f, -89.0839f ), CVector ( 1534.56f, 2863.23f, 110.916f ) );
    AddZone ( "Las Venturas Airport", CVector ( 1515.81f, 1586.4f, -12.5f ), CVector ( 1729.95f, 1714.56f, 87.5f ) );
    AddZone ( "Ocean Docks", CVector ( 2089.0f, -2394.33f, -89.0839f ), CVector ( 2201.82f, -2235.84f, 110.916f ) );
    AddZone ( "Los Santos International", CVector ( 1382.73f, -2730.88f, -89.0839f ), CVector ( 2201.82f, -2394.33f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2201.82f, -2730.88f, -89.0839f ), CVector ( 2324.0f, -2418.33f, 110.916f ) );
    AddZone ( "Los Santos International", CVector ( 1974.63f, -2394.33f, -39.0839f ), CVector ( 2089.0f, -2256.59f, 60.9161f ) );
    AddZone ( "Los Santos International", CVector ( 1400.97f, -2669.26f, -39.0839f ), CVector ( 2189.82f, -2597.26f, 60.9161f ) );
    AddZone ( "Los Santos International", CVector ( 2051.63f, -2597.26f, -39.0839f ), CVector ( 2152.45f, -2394.33f, 60.9161f ) );
    AddZone ( "Starfish Casino", CVector ( 2437.39f, 1858.1f, -39.0839f ), CVector ( 2495.09f, 1970.85f, 60.9161f ) );
    AddZone ( "Beacon Hill", CVector ( -399.633f, -1075.52f, -1.48904f ), CVector ( -319.033f, -977.516f, 198.511f ) );
    AddZone ( "Avispa Country Club", CVector ( -2361.51f, -417.199f, 0.0f ), CVector ( -2270.04f, -355.493f, 200.0f ) );
    AddZone ( "Avispa Country Club", CVector ( -2667.81f, -302.135f, -28.8305f ), CVector ( -2646.4f, -262.32f, 71.1695f ) );
    AddZone ( "Garcia", CVector ( -2395.14f, -222.589f, -5.34058e-005f ), CVector ( -2354.09f, -204.792f, 200.0f ) );
    AddZone ( "Avispa Country Club", CVector ( -2470.04f, -355.493f, 0.0f ), CVector ( -2270.04f, -318.493f, 46.1f ) );
    AddZone ( "Avispa Country Club", CVector ( -2550.04f, -355.493f, 0.0f ), CVector ( -2470.04f, -318.493f, 39.7f ) );
    AddZone ( "Playa del Seville", CVector ( 2703.58f, -2126.9f, -89.0839f ), CVector ( 2959.35f, -1852.87f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2703.58f, -2302.33f, -89.0839f ), CVector ( 2959.35f, -2126.9f, 110.916f ) );
    AddZone ( "Starfish Casino", CVector ( 2162.39f, 1883.23f, -89.0839f ), CVector ( 2437.39f, 2012.18f, 110.916f ) );
    AddZone ( "The Clown's Pocket", CVector ( 2162.39f, 1783.23f, -89.0839f ), CVector ( 2437.39f, 1883.23f, 110.916f ) );
    AddZone ( "Ocean Docks", CVector ( 2324.0f, -2145.1f, -89.084f ), CVector ( 2703.58f, -2059.23f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2324.0f, -2059.23f, -89.0839f ), CVector ( 2541.7f, -1852.87f, 110.916f ) );
    AddZone ( "Willowfield", CVector ( 2541.7f, -2059.23f, -89.0839f ), CVector ( 2703.58f, -1941.4f, 110.916f ) );
    AddZone ( "Whitewood Estates", CVector ( 1098.31f, 1726.22f, -89.0839f ), CVector ( 1197.39f, 2243.23f, 110.916f ) );
    AddZone ( "Downtown Los Santos", CVector ( 1507.51f, -1385.21f, 110.916f ), CVector ( 1582.55f, -1325.31f, 335.916f ) );
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