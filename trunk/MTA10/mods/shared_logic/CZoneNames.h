/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CZoneNames.h
*  PURPOSE:     Zone names class header
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#ifndef __CZoneNames_H
#define __CZoneNames_H

#include <list>
#include "CClientCommon.h"
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

    void                            AddZone                 ( const char* szName, CVector& vecBottomLeft, CVector& vecTopRight );
    SZone*                          GetZone                 ( CVector& vecPosition );
    const char*                     GetZoneName             ( CVector& vecPosition );

    void                            AddCity                 ( const char* szName, CVector& vecBottomLeft, CVector& vecTopRight );
    SZone*                          GetCity                 ( CVector& vecPosition );
    const char*                     GetCityName             ( CVector& vecPosition );

    void                            LoadZones               ( void );
    void                            ClearZones              ( void );

    void                            LoadCities              ( void );
    void                            ClearCities             ( void );

protected:
    list < SZone* >                 m_Zones;
    list < SZone* >                 m_Cities;
};

#endif