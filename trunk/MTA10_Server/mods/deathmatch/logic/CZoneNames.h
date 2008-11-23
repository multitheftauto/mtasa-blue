/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CZoneNames.h
*  PURPOSE:     GTA world zone name definitions class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CZoneNames_H
#define __CZoneNames_H

#include "CCommon.h"
#include <list>
using namespace std;

#define MAX_ZONE_NAME 32

struct SZone
{
    char szName [ MAX_ZONE_NAME ];
    CVector vecBottomLeft;
    CVector vecTopRight;
};

class CZoneNames
{
public:
                                    CZoneNames              ( void );
                                    ~CZoneNames             ( void );

    void                            AddZone                 ( const char* szName, CVector vecBottomLeft, CVector vecTopRight );
    SZone*                          GetZone                 ( CVector vecPosition );
    const char*                     GetZoneName             ( CVector vecPosition );

    void                            AddCity                 ( const char* szName, CVector vecBottomLeft, CVector vecTopRight );
    SZone*                          GetCity                 ( CVector vecPosition );
    const char*                     GetCityName             ( CVector vecPosition );

    void                            LoadZones               ( void );
    void                            ClearZones              ( void );

    void                            LoadCities              ( void );
    void                            ClearCities             ( void );

protected:
    list < SZone* >                 m_Zones;
    list < SZone* >                 m_Cities;
};

#endif
