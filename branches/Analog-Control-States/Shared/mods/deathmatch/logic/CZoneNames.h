/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CZoneNames.h
*
*****************************************************************************/

#include "RTree.h"

struct SZone
{
    short x1, y1, z1;
    short x2, y2, z2;
    const char* szName;
};

typedef RTree < const SZone*, float, 3 > CZoneTree;


class CZoneNames
{
public:
                                    CZoneNames              ( void );
    const char*                     GetZoneName             ( const CVector& vecPosition );
    const char*                     GetCityName             ( const CVector& vecPosition );

protected:
    const SZone*                    GetSmallestZoneInSphere ( const CVector& vecPosition, float fRadius );
    const SZone*                    GetCityZoneInSphere     ( const CVector& vecPosition, float fRadius );

    CZoneTree                       m_ZoneTree;
    std::set < SString >            m_CityNameMap;
};
