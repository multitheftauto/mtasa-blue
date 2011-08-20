/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

enum
{
    ZONE0,  // Near sync
    ZONE1,  // Mid sync + out of fov
    ZONE2,  // Mid sync + way out of fov
    ZONE3,  // Far sync
    ZONE_MAX
};

struct CBandwidthSettings
{
public:
    CBandwidthSettings ( void )
    {
        SetNone ();
    }

    void SetNone ( void )
    {
        fZone0Radius = 0;
        fZone1Angle = 0;
        fZone2Angle = 0;
        ZoneUpdateIntervals [ ZONE0 ] = 0;
        ZoneUpdateIntervals [ ZONE1 ] = 0;
        ZoneUpdateIntervals [ ZONE2 ] = 0;
        ZoneUpdateIntervals [ ZONE3 ] = 1000;
        iMaxZoneIfOtherCanSee = 0;
        UpdateCachedValues ();
    }

    void SetMedium ( void )
    {
        fZone0Radius = 50;
        fZone1Angle = 135;
        fZone2Angle = 225;
        ZoneUpdateIntervals [ ZONE0 ] = 0;
        ZoneUpdateIntervals [ ZONE1 ] = 200;
        ZoneUpdateIntervals [ ZONE2 ] = 400;
        ZoneUpdateIntervals [ ZONE3 ] = 1500;
        iMaxZoneIfOtherCanSee = 1;
        UpdateCachedValues ();
    }

    void SetMaximum ( void )
    {
        fZone0Radius = 20;
        fZone1Angle = 115;
        fZone2Angle = 185;
        ZoneUpdateIntervals [ ZONE0 ] = 0;
        ZoneUpdateIntervals [ ZONE1 ] = 500;
        ZoneUpdateIntervals [ ZONE2 ] = 1000;
        ZoneUpdateIntervals [ ZONE3 ] = 2000;
        iMaxZoneIfOtherCanSee = 2;
        UpdateCachedValues ();
    }

    void UpdateCachedValues ( void )
    {
        fZone1Dot = cos ( fZone1Angle / 180.f * PI * 0.5f );
        fZone2Dot = cos ( fZone2Angle / 180.f * PI * 0.5f );
        fZone0RadiusSq = fZone0Radius * fZone0Radius;
    }

    float fZone0Radius;                     // 10 - 200     Use zone1&2 only if other player is at least this distance away
    float fZone1Angle;                      // 90 - 350     Use zone1 if angle to other player is greater than this
    float fZone2Angle;                      // 100 - 360    Use zone2 if angle to other player is greater than this

    float fZone1Dot;
    float fZone2Dot;
    float fZone0RadiusSq;
    int ZoneUpdateIntervals [ ZONE_MAX ];   // First is always 0, next is Zone1 interval then Zone2 and Zone3
    int iMaxZoneIfOtherCanSee;
};

extern CBandwidthSettings* g_pBandwidthSettings;
