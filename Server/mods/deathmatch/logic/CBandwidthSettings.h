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
        bBulletSyncEnabled = false;
        SetNone ();
    }

    void SetNone ( void )
    {
        EnableLightSync ( false );
        fZone0Radius = 50;
        fZone1Angle = 135;
        fZone2Angle = 225;
        ZoneUpdateIntervals [ ZONE0 ] = 0;
        ZoneUpdateIntervals [ ZONE1 ] = 0;
        ZoneUpdateIntervals [ ZONE2 ] = 0;
        ZoneUpdateIntervals [ ZONE3 ] = 1000;
        iMaxZoneIfOtherCanSeeNormal = 0;
        iMaxZoneIfOtherCanSeeBulletSync = 0;
        UpdateCachedValues ();
    }

    void SetMedium ( void )
    {
        EnableLightSync ( false );
        fZone0Radius = 50;
        fZone1Angle = 135;
        fZone2Angle = 225;
        ZoneUpdateIntervals [ ZONE0 ] = 0;
        ZoneUpdateIntervals [ ZONE1 ] = 200;
        ZoneUpdateIntervals [ ZONE2 ] = 400;
        ZoneUpdateIntervals [ ZONE3 ] = 1500;
        iMaxZoneIfOtherCanSeeNormal = 1;
        iMaxZoneIfOtherCanSeeBulletSync = 0;
        UpdateCachedValues ();
    }

    void SetMaximum ( void )
    {
        EnableLightSync ( true );
        fZone0Radius = 20;
        fZone1Angle = 115;
        fZone2Angle = 185;
        ZoneUpdateIntervals [ ZONE0 ] = 0;
        ZoneUpdateIntervals [ ZONE1 ] = 500;
        ZoneUpdateIntervals [ ZONE2 ] = 1000;
        ZoneUpdateIntervals [ ZONE3 ] = 2000;
        iMaxZoneIfOtherCanSeeNormal = 2;
        iMaxZoneIfOtherCanSeeBulletSync = 1;
        UpdateCachedValues ();
    }

    void EnableLightSync ( bool bEnable )
    {
        bLightSyncEnabled = bEnable;
    }

    void NotifyBulletSyncEnabled ( bool bEnabled )
    {
        if ( bBulletSyncEnabled != bEnabled )
        {
            bBulletSyncEnabled = bEnabled;
            UpdateCachedValues();
        }
    }

    void UpdateCachedValues ( void )
    {
        fZone1Dot = cos ( fZone1Angle / 180.f * PI * 0.5f );
        fZone2Dot = cos ( fZone2Angle / 180.f * PI * 0.5f );
        fZone0RadiusSq = fZone0Radius * fZone0Radius;
        iMaxZoneIfOtherCanSee = bBulletSyncEnabled ? iMaxZoneIfOtherCanSeeBulletSync : iMaxZoneIfOtherCanSeeNormal;
    }

    float fZone1Dot;
    float fZone2Dot;
    float fZone0RadiusSq;
    SFixedArray < int, ZONE_MAX > ZoneUpdateIntervals;   // First is always 0, next is Zone1 interval then Zone2 and Zone3
    int iMaxZoneIfOtherCanSee;
    bool bLightSyncEnabled;

protected:
    float fZone0Radius;                     // 10 - 200     Use zone1&2 only if other player is at least this distance away
    float fZone1Angle;                      // 90 - 350     Use zone1 if angle to other player is greater than this
    float fZone2Angle;                      // 100 - 360    Use zone2 if angle to other player is greater than this
    int iMaxZoneIfOtherCanSeeNormal;        // Zone limit if other player can see us
    int iMaxZoneIfOtherCanSeeBulletSync;    // Zone limit if other player can see us when bullet sync is enabled
    bool bBulletSyncEnabled;                // Cache flag set by game
};

extern CBandwidthSettings* g_pBandwidthSettings;
