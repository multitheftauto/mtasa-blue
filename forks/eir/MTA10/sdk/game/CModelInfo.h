/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CModelInfo.h
*  PURPOSE:     Entity model info interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_MODELINFO
#define __CGAME_MODELINFO

#include "Common.h"
#include <CVector.h>

#include <windows.h>
#include "CColModel.h"
class CPedModelInfo;
struct RwObject;

class CBoundingBox
{
public:
    CVector vecBoundMin;
    CVector vecBoundMax;
    CVector vecBoundOffset;
    float   fRadius;
};

struct CBounds2D
{
    CBounds2D( void )
    { }

    CBounds2D( float minX, float maxY, float maxX, float minY ) : m_minX( minX ), m_maxY( maxY ), m_maxX( maxX ), m_minY( minY )
    { }

    CBounds2D( const CBounds2D& right ) : m_minX( right.m_minX ), m_maxY( right.m_maxY ), m_maxX( right.m_maxX ), m_minY( right.m_minY )
    { }

    // Is bbox inside this?
    // Method used by R*.
    // Takes into account that min is lower vector and max is upper vector.
    inline bool IsInside( const CBounds2D& bbox ) const
    {
        return ( bbox.m_minX <= m_maxX && bbox.m_maxX >= m_minX &&
                 bbox.m_minY <= m_maxY && bbox.m_maxY >= m_minY );
    }

    inline bool IsInside( const CVector2D& pos ) const
    {
        return ( pos.fX >= m_minX && pos.fX <= m_maxX &&
                 pos.fY >= m_minY && pos.fY <= m_maxY );
    }

    // Binary offset: 0x004042D0
    inline bool ContainsPoint( const CVector2D& pos, float subtract ) const
    {
        return CBounds2D( m_minX - subtract, subtract + m_maxY, subtract + m_maxX, m_minY - subtract ).IsInside( pos );
    }

    // R*: fuck yea, let us confuse dem pplz by switching things 'round, and not using vectors.
    float m_minX, m_maxY;
    float m_maxX, m_minY;
};

enum eVehicleUpgradePosn
{
    VEHICLE_UPGRADE_POSN_BONET = 0,
    VEHICLE_UPGRADE_POSN_VENT,
    VEHICLE_UPGRADE_POSN_SPOILER,
    VEHICLE_UPGRADE_POSN_SIDESKIRT,
    VEHICLE_UPGRADE_POSN_FRONT_BULLBARS,
    VEHICLE_UPGRADE_POSN_REAR_BULLBARS,
    VEHICLE_UPGRADE_POSN_HEADLIGHTS,
    VEHICLE_UPGRADE_POSN_ROOF,
    VEHICLE_UPGRADE_POSN_NITRO,
    VEHICLE_UPGRADE_POSN_HYDRAULICS,
    VEHICLE_UPGRADE_POSN_STEREO,
    VEHICLE_UPGRADE_POSN_UNKNOWN,
    VEHICLE_UPGRADE_POSN_WHEELS,
    VEHICLE_UPGRADE_POSN_EXHAUST,
    VEHICLE_UPGRADE_POSN_FRONT_BUMPER,
    VEHICLE_UPGRADE_POSN_REAR_BUMPER,
    VEHICLE_UPGRADE_POSN_MISC,
};

enum eVehicleType : unsigned int
{
    VEHICLE_CAR,
    VEHICLE_MONSTERTRUCK,
    VEHICLE_QUADBIKE,
    VEHICLE_HELI,
    VEHICLE_PLANE,
    VEHICLE_BOAT,
    VEHICLE_TRAIN,
    VEHICLE_UNKNOWN,
    VEHICLE_FAKEPLANE,    // what is this?
    VEHICLE_BIKE,
    VEHICLE_BICYCLE,
    VEHICLE_AUTOMOBILETRAILER
};

struct SVehicleSupportedUpgrades
{
    SVehicleSupportedUpgrades ( )
    {
        Reset ( );
    }
    void Reset ( )
    {
        m_bBonnet = false;
        m_bBonnet_Left = false;
        m_bBonnet_Left_dam = false;
        m_bBonnet_Right = false;
        m_bBonnet_Right_dam = false;
        m_bSpoiler = false;
        m_bSpoiler_dam = false;
        m_bVent = false;
        m_bSideSkirt_Right = false;
        m_bSideSkirt_Left = false;
        m_bFrontBullbars = false;
        m_bRearBullbars = false;
        m_bLamps = false;
        m_bLamps_dam = false;
        m_bRoof = false;
        m_bExhaust = false;
        m_bFrontBumper = false;
        m_bRearBumper = false;
        m_bMisc = false;
        m_bInitialised = false;
    }
    bool m_bBonnet;
    bool m_bBonnet_Left;
    bool m_bBonnet_Left_dam;
    bool m_bBonnet_Right;
    bool m_bBonnet_Right_dam;
    bool m_bSpoiler;
    bool m_bSpoiler_dam;
    bool m_bVent;
    bool m_bSideSkirt_Right;
    bool m_bSideSkirt_Left;
    bool m_bFrontBullbars;
    bool m_bRearBullbars;
    bool m_bLamps;
    bool m_bLamps_dam;
    bool m_bRoof;
    bool m_bExhaust;
    bool m_bFrontBumper;
    bool m_bRearBumper;
    bool m_bMisc;
    bool m_bInitialised;
};
class CModelInfo
{
public:
    virtual class CBaseModelInfoSAInterface *     GetInterface            ( void ) = 0;

    virtual DWORD           GetModel                () = 0;
    virtual BOOL            IsBoat                  () = 0;
    virtual BOOL            IsCar                   () = 0;
    virtual BOOL            IsTrain                 () = 0;
    virtual BOOL            IsHeli                  () = 0;
    virtual BOOL            IsPlane                 () = 0;
    virtual BOOL            IsBike                  () = 0;
    virtual BOOL            IsFakePlane             () = 0;
    virtual BOOL            IsMonsterTruck          () = 0;
    virtual BOOL            IsQuadBike              () = 0;
    virtual BOOL            IsBmx                   () = 0;
    virtual BOOL            IsTrailer               () = 0;
    virtual BOOL            IsVehicle               () = 0;

    virtual char*           GetNameIfVehicle        () = 0;

    virtual VOID            Request                 ( EModelRequestType requestType, const char* szTag/* = NULL*/ ) = 0;
    virtual BYTE            GetLevelFromPosition    ( CVector * vecPosition ) = 0;
    virtual BOOL            IsLoaded                () = 0;
    virtual BYTE            GetFlags                () = 0;
    virtual CBoundingBox*   GetBoundingBox          () = 0;
    virtual bool            IsValid                 () = 0;
    virtual unsigned short  GetTextureDictionaryID  () = 0;
    virtual float           GetLODDistance          () = 0;
    virtual void            SetLODDistance          ( float fDistance ) = 0;
    virtual void            RestreamIPL             () = 0;

    virtual void            ModelAddRef             ( EModelRequestType requestType, const char* szTag/* = NULL*/ ) = 0;
    virtual void            RemoveRef               ( bool bRemoveExtraGTARef = false ) = 0;
    virtual int             GetRefCount             () = 0;

    virtual float           GetDistanceFromCentreOfMassToBaseOfModel () = 0;

    virtual void            SetAlphaTransparencyEnabled ( BOOL bEnabled ) = 0;
    virtual bool            IsAlphaTransparencyEnabled () = 0;
    virtual void            ResetAlphaTransparency  () = 0;

    // ONLY use for CVehicleModelInfos
    virtual short           GetAvailableVehicleMod  ( unsigned short usSlot ) = 0;
    virtual bool            IsUpgradeAvailable      ( eVehicleUpgradePosn posn ) = 0;
    virtual void            SetCustomCarPlateText   ( const char * szText ) = 0;
    virtual unsigned int    GetNumRemaps            ( void ) = 0;
    virtual void*           GetVehicleSuspensionData( void ) = 0;
    virtual void*           SetVehicleSuspensionData( void* pSuspensionLines ) = 0;

    // Init the supported upgrades structure
    virtual void            InitialiseSupportedUpgrades ( RpClump * pClump ) = 0;

    // ONLY use for peds
    virtual void            GetVoice                ( short* psVoiceType, short* psVoice ) = 0;
    virtual void            GetVoice                ( const char** pszVoiceType, const char** szVoice ) = 0;
    virtual void            SetVoice                ( short sVoiceType, short sVoice ) = 0;
    virtual void            SetVoice                ( const char* szVoiceType, const char* szVoice ) = 0;

    // Custom collision related functions
    virtual void            SetCustomModel          ( RpClump* pClump ) = 0;
    virtual void            RestoreOriginalModel    ( void ) = 0;

    // Call this to make sure the custom vehicle models are being used after a load.
    virtual void            MakeCustomModel         ( void ) = 0;
    virtual RwObject*       GetRwObject             ( void ) = 0;


    virtual SVehicleSupportedUpgrades               GetVehicleSupportedUpgrades         ( void ) = 0;
    virtual void                                    ResetSupportedUpgrades              ( void ) = 0;
};

#endif
