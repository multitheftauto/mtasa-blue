/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayer.h
*  PURPOSE:     Player ped entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayer;

#ifndef __CPLAYER_H
#define __CPLAYER_H

#include "CPed.h"
#include "CCommon.h"
#include "CClient.h"
#include "CElement.h"
#include "CPlayerTextManager.h"
#include "CTeam.h"
#include "CPad.h"
#include "CObject.h"
#include "packets/CPacket.h"

class CKeyBinds;
class CPlayerCamera;

enum
{
    STATUS_CONNECTED,
    STATUS_VERIFYING,
    STATUS_JOINED,
};

enum eVoiceState
{
    VOICESTATE_IDLE = 0,
    VOICESTATE_TRANSMITTING,
};

struct SNearInfo
{
    int iCount;
    long long llLastUpdateTime;
};


class CPlayer : public CPed, public CClient
{
    friend class CElement;
    friend class CScriptDebugging;

public:
                                                CPlayer                     ( class CPlayerManager* pPlayerManager, class CScriptDebugging* pScriptDebugging, const NetServerPlayerID& PlayerSocket );
                                                ~CPlayer                    ( void );

    void                                        DoPulse                     ( void );

    void                                        Unlink                      ( void );
    bool                                        ReadSpecialData             ( void )            { return true; };

    inline bool                                 DoNotSendEntities           ( void )            { return m_bDoNotSendEntities; };
    inline void                                 SetDoNotSendEntities        ( bool bDont )      { m_bDoNotSendEntities = bDont; };

    inline int                                  GetClientType               ( void )                                { return CClient::CLIENT_PLAYER; };
    inline unsigned long                        GetTimeConnected            ( void ) const                          { return m_ulTimeConnected; };

    inline const char*                          GetNick                     ( void )                                { return m_szNick; };
    void                                        SetNick                     ( const char* szNick );

    inline int                                  GetGameVersion              ( void )                                { return m_iGameVersion; };
    inline void                                 SetGameVersion              ( int iGameVersion )                    { m_iGameVersion = iGameVersion; };
    inline unsigned short                       GetMTAVersion               ( void )                                { return m_usMTAVersion; };
    inline void                                 SetMTAVersion               ( unsigned short usMTAVersion )         { m_usMTAVersion = usMTAVersion; };
    inline unsigned short                       GetBitStreamVersion         ( void )                                { return m_usBitStreamVersion; };
    inline void                                 SetBitStreamVersion         ( unsigned short usBitStreamVersion )   { m_usBitStreamVersion = usBitStreamVersion; };
    void                                        SetPlayerVersion            ( const SString& strPlayerVersion )     { m_strPlayerVersion = strPlayerVersion; };
    const SString&                              GetPlayerVersion            ( void )                                { return m_strPlayerVersion; };

    inline bool                                 IsMuted                     ( void )                                { return m_bIsMuted; };
    inline void                                 SetMuted                    ( bool bSetMuted )                      { m_bIsMuted = bSetMuted; };

    inline int                                  GetStatus                   ( void )                                { return m_iStatus; };
    inline void                                 SetStatus                   ( int iStatus )                         { m_iStatus = iStatus; };

    inline bool                                 IsIngame                    ( void )                                { return m_iStatus >= STATUS_VERIFYING; };
    inline bool                                 IsJoined                    ( void )                                { return m_iStatus == STATUS_JOINED; };

    inline float                                GetCameraRotation           ( void )                                { return m_fCameraRotation; };
    inline void                                 SetCameraRotation           ( float fRotation )                     { m_fCameraRotation = fRotation; };

    inline long                                 GetMoney                    ( void )                                { return m_lMoney; };
    inline void                                 SetMoney                    ( long lMoney )                         { m_lMoney = lMoney; };
    
    inline const CVector&                       GetSniperSourceVector       ( void )                        { return m_vecSniperSource; };
    inline void                                 SetSniperSourceVector       ( const CVector& vecSource )    { m_vecSniperSource = vecSource; };
    inline void                                 GetTargettingVector         ( CVector& vecTarget )          { vecTarget = m_vecTargetting; };
    inline void                                 SetTargettingVector         ( const CVector& vecTarget )    { m_vecTargetting = vecTarget; };
    inline float                                GetAimDirection             ( void )                        { return m_fAimDirection; };
    inline void                                 SetAimDirection             ( float fDirection )            { m_fAimDirection = fDirection; };
    inline unsigned char                        GetDriveByDirection         ( void )                        { return m_ucDriveByDirection; };
    inline void                                 SetDriveByDirection         ( unsigned char ucDirection )   { m_ucDriveByDirection = ucDirection; };
    inline bool                                 IsAkimboArmUp               ( void )                        { return m_bAkimboArmUp; };
    inline void                                 SetAkimboArmUp              ( bool bUp )                    { m_bAkimboArmUp = bUp; };

    inline NetServerPlayerID&                   GetSocket                   ( void )                        { return m_PlayerSocket; };
    char*                                       GetSourceIP                 ( char* pBuffer );
    inline unsigned long                        GetSourceIP                 ( void )                        { return m_PlayerSocket.GetBinaryAddress (); };
    inline unsigned short                       GetSourcePort               ( void )                        { return m_PlayerSocket.GetPort (); };
    inline unsigned int                         GetPing                     ( void )                        { return g_pNetServer->GetPing ( m_PlayerSocket ); };

    inline unsigned char                        GetLoginAttempts            ( void )                        { return m_ucLoginAttempts; };
    inline void                                 SetLoginAttempts            ( unsigned char ucLoginAttempts )   { m_ucLoginAttempts = ucLoginAttempts; };

    inline time_t                               GetNickChangeTime           ( void )                        { return m_tNickChange; };
    inline void                                 SetNickChangeTime           ( time_t tNickChange )          { m_tNickChange = tNickChange; };

    void                                        Send                        ( const CPacket& Packet );
    void                                        SendEcho                    ( const char* szEcho );
    void                                        SendConsole                 ( const char* szEcho );

    inline CPlayerTextManager*                  GetPlayerTextManager        ( void )                        { return m_pPlayerTextManager; };

    void                                        AddSyncingVehicle           ( CVehicle* pVehicle );
    void                                        RemoveSyncingVehicle        ( CVehicle* pVehicle );
    void                                        RemoveAllSyncingVehicles    ( void );

    inline unsigned int                         CountSyncingVehicles        ( void )                        { return static_cast < unsigned int > ( m_SyncingVehicles.size () ); };
    inline std::list < CVehicle* > ::const_iterator IterSyncingVehicleBegin ( void )                        { return m_SyncingVehicles.begin (); };
    inline std::list < CVehicle* > ::const_iterator IterSyncingVehicleEnd   ( void )                        { return m_SyncingVehicles.end (); };

    void                                        AddSyncingPed               ( CPed* pPed );
    void                                        RemoveSyncingPed            ( CPed* pPed );
    void                                        RemoveAllSyncingPeds         ( void );

    inline unsigned int                         CountSyncingPeds            ( void )                        { return static_cast < unsigned int > ( m_SyncingPeds.size () ); };
    inline std::list < CPed* > ::const_iterator IterSyncingPedBegin         ( void )                        { return m_SyncingPeds.begin (); };
    inline std::list < CPed* > ::const_iterator IterSyncingPedEnd           ( void )                        { return m_SyncingPeds.end (); };

    void                                        AddSyncingObject            ( CObject* pObject );
    void                                        RemoveSyncingObject         ( CObject* pObject );
    void                                        RemoveAllSyncingObjects     ( void );

    inline unsigned int                         CountSyncingObjects         ( void )                        { return static_cast < unsigned int > ( m_SyncingObjects.size () ); };
    inline std::list < CObject* > ::const_iterator IterSyncingObjectBegin   ( void )                        { return m_SyncingObjects.begin (); };
    inline std::list < CObject* > ::const_iterator IterSyncingObjectEnd     ( void )                        { return m_SyncingObjects.end (); };

    inline unsigned int                         GetScriptDebugLevel         ( void )                        { return m_uiScriptDebugLevel; };
    bool                                        SetScriptDebugLevel         ( unsigned int uiLevel );

    void                                        SetDamageInfo               ( ElementID ElementID, unsigned char ucWeapon, unsigned char ucBodyPart );
    void                                        ValidateDamageInfo          ( void );
    ElementID                                   GetPlayerAttacker           ( void );
    unsigned char                               GetAttackWeapon             ( void );
    unsigned char                               GetAttackBodyPart           ( void );

    inline CTeam*                               GetTeam                     ( void )                        { return m_pTeam; }
    void                                        SetTeam                     ( CTeam* pTeam, bool bChangeTeam = false );

    inline CPad*                                GetPad                      ( void )                        { return m_pPad; }

    inline bool                                 IsDebuggerVisible           ( void )                        { return m_bDebuggerVisible; }
    inline void                                 SetDebuggerVisible          ( bool bVisible )               { m_bDebuggerVisible = bVisible; }

    inline unsigned int                         GetWantedLevel              ( void )                        { return m_uiWantedLevel; }
    inline void                                 SetWantedLevel              ( unsigned int uiWantedLevel )  { m_uiWantedLevel = uiWantedLevel; }

    inline bool                                 GetForcedScoreboard         ( void )                        { return m_bForcedScoreboard; }
    inline void                                 SetForcedScoreboard         ( bool bVisible )               { m_bForcedScoreboard = bVisible; }

    inline bool                                 GetForcedMap                ( void )                        { return m_bForcedMap; }
    inline void                                 SetForcedMap                ( bool bVisible )               { m_bForcedMap = bVisible; }

    void                                        Reset                       ( void );

    inline CPlayerCamera *                      GetCamera                   ( void )                        { return m_pCamera; }

    inline CKeyBinds*                           GetKeyBinds                 ( void )                        { return m_pKeyBinds; }

    inline bool                                 IsCursorShowing             ( void )                        { return m_bCursorShowing; }
    inline void                                 SetCursorShowing            ( bool bCursorShowing )         { m_bCursorShowing = bCursorShowing; }

    inline char*                                GetNametagText              ( void )                        { return m_szNametagText; }
    void                                        SetNametagText              ( const char* szText );
    void                                        GetNametagColor             ( unsigned char& ucR, unsigned char& ucG, unsigned char& ucB );
    void                                        SetNametagOverrideColor     ( unsigned char ucR, unsigned char ucG, unsigned char ucB );
    void                                        RemoveNametagOverrideColor  ( void );
    inline bool                                 IsNametagColorOverridden    ( void )                                                            { return m_bNametagColorOverridden; }

    inline bool                                 IsNametagShowing            ( void )                        { return m_bNametagShowing; }
    inline void                                 SetNametagShowing           ( bool bShowing )               { m_bNametagShowing = bShowing; }

    inline const std::string&                   GetSerial                   ( void )                        { return m_strSerial; }
    inline void                                 SetSerial                   ( const std::string& strSerial ){ m_strSerial = strSerial; };

    inline const std::string&                   GetSerialUser               ( void )                        { return m_strSerialUser; };
    inline void                                 SetSerialUser               ( const std::string& strUser )  { m_strSerialUser = strUser; };

    inline const std::string&                   GetCommunityID              ( void )                        { return m_strCommunityID; };
    inline void                                 SetCommunityID              ( const std::string& strID )    { m_strCommunityID = strID; };

    inline unsigned char                        GetBlurLevel                ( void )                        { return m_ucBlurLevel; }
    inline void                                 SetBlurLevel                ( unsigned char ucBlurLevel )   { m_ucBlurLevel = ucBlurLevel; }

    // Sync stuff
    inline void                                 SetSyncingVelocity          ( bool bSyncing )               { m_bSyncingVelocity = bSyncing; }
    inline bool                                 IsSyncingVelocity           ( void ) const                  { return m_bSyncingVelocity; }
    inline void                                 IncrementPuresync           ( void )                        { m_uiPuresyncPackets++; }
    inline unsigned int                         GetPuresyncCount            ( void ) const                  { return m_uiPuresyncPackets; }

    void                                        NotifyReceivedSync        ( void )                        { m_ulLastReceivedSyncTime = GetTickCount32 (); }
    unsigned long                               GetTicksSinceLastReceivedSync ( void ) const              { return GetTickCount32 () - m_ulLastReceivedSyncTime; }

    const std::string&                          GetAnnounceValue            ( const std::string& strKey ) const;
    void                                        SetAnnounceValue            ( const std::string& strKey, const std::string& strValue );

    // Checks
    void                                        SetWeaponCorrect            ( bool bWeaponCorrect );
    bool                                        GetWeaponCorrect            ( void );

    void                                        UpdateOthersNearList        ( void );
    void                                        RefreshNearPlayer           ( CPlayer* pOther );
    std::map < CPlayer*, SNearInfo >&           GetNearPlayerList           ( void )                        { return m_NearPlayerList; }
    std::map < CPlayer*, SNearInfo >&           GetFarPlayerList            ( void )                        { return m_FarPlayerList; }
    void                                        AddPlayerToDistLists        ( CPlayer* pOther );
    void                                        RemovePlayerFromDistLists   ( CPlayer* pOther );
    void                                        MovePlayerToNearList        ( CPlayer* pOther );
    void                                        MovePlayerToFarList         ( CPlayer* pOther );

public:
    struct SLightweightSyncData
    {
        SLightweightSyncData ()
        {
            health.uiContext = 0;
            health.bSync = false;
            vehicleHealth.uiContext = 0;
            vehicleHealth.bSync = false;
        }

        struct
        {
            float           fLastHealth;
            float           fLastArmor;
            bool            bSync;
            unsigned int    uiContext;
        } health;

        struct
        {
            CVehicle*       lastVehicle;
            float           fLastHealth;
            bool            bSync;
            unsigned int    uiContext;
        } vehicleHealth;
    };
    SLightweightSyncData&                       GetLightweightSyncData      ( void ) { return m_lightweightSyncData; }

    eVoiceState                                 GetVoiceState               ( void )                      { return m_VoiceState; }
    void                                        SetVoiceState               ( eVoiceState State )         { m_VoiceState = State; }

    std::list < CElement* > ::const_iterator    IterBroadcastListBegin      ( void )                      { return m_lstBroadcastList.begin (); };
    std::list < CElement* > ::const_iterator    IterBroadcastListEnd        ( void )                      { return m_lstBroadcastList.end (); };
    bool                                        IsVoiceMuted                ( void )                      { return m_lstBroadcastList.empty (); }
    void                                        SetVoiceBroadcastTo         ( CElement* pElement );
    void                                        SetVoiceBroadcastTo         ( const std::list < CElement* >& lstElements );

    void                                        SetVoiceIgnoredElement      ( CElement* pElement );
    void                                        SetVoiceIgnoredList         ( const std::list < CElement* >& lstElements );
    std::list < CElement* > ::const_iterator    IterIgnoredListBegin        ( void )                      { return m_lstIgnoredList.begin (); };
    std::list < CElement* > ::const_iterator    IterIgnoredListEnd          ( void )                      { return m_lstIgnoredList.end (); };
    bool                                        IsPlayerIgnoringElement     ( CElement* pElement );

    void                                        SetCameraOrientation        ( const CVector& vecPosition, const CVector& vecFwd );
    bool                                        IsTimeToReceiveNearSyncFrom ( CPlayer* pOther, SNearInfo& nearInfo );
    int                                         GetSyncZone                 ( CPlayer* pOther );
    const CVector&                              GetCamPosition              ( void )            { return m_vecCamPosition; };
    const CVector&                              GetCamFwd                   ( void )            { return m_vecCamFwd; };



private:
    SLightweightSyncData                        m_lightweightSyncData;

    void                                        WriteCameraModePacket       ( void );
    void                                        WriteCameraPositionPacket   ( void );

    class CPlayerManager*                       m_pPlayerManager;
    class CScriptDebugging*                     m_pScriptDebugging;

    CPlayerTextManager*                         m_pPlayerTextManager;

    char                                        m_szNick [MAX_NICK_LENGTH + 1];
    bool                                        m_bDoNotSendEntities;
    int                                         m_iGameVersion;
    unsigned short                              m_usMTAVersion;
    unsigned short                              m_usBitStreamVersion;
    SString                                     m_strPlayerVersion;
    bool                                        m_bIsMuted;
    int                                         m_iStatus;
    
    bool                                        m_bNametagColorOverridden;
    
    float                                       m_fCameraRotation;
    long                                        m_lMoney;    
    CVector                                     m_vecSniperSource;
    CVector                                     m_vecTargetting;
    float                                       m_fAimDirection;
    unsigned char                               m_ucDriveByDirection;   
    
    bool                                        m_bAkimboArmUp;

    unsigned long                               m_ulTimeConnected;

    NetServerPlayerID                           m_PlayerSocket;

    time_t                                      m_tNickChange;

    unsigned char                               m_ucLoginAttempts;

    std::list < CVehicle* >                     m_SyncingVehicles;
    std::list < CPed* >                         m_SyncingPeds;
    std::list < CObject* >                      m_SyncingObjects;

    unsigned int                                m_uiScriptDebugLevel;   

    ElementID                                   m_PlayerAttackerID;
    unsigned char                               m_ucAttackWeapon;
    unsigned char                               m_ucAttackBodyPart;
    long long                                   m_llSetDamageInfoTime;

    CTeam*                                      m_pTeam;
    CPad*                                       m_pPad;
 
    bool                                        m_bDebuggerVisible;

    unsigned int                                m_uiWantedLevel;

    bool                                        m_bForcedScoreboard;
    bool                                        m_bForcedMap;    

    CPlayerCamera *                             m_pCamera;

    CKeyBinds*                                  m_pKeyBinds;

    bool                                        m_bCursorShowing;

    char*                                       m_szNametagText;
    unsigned char                               m_ucNametagR;
    unsigned char                               m_ucNametagG;
    unsigned char                               m_ucNametagB;
    bool                                        m_bNametagShowing;       

    std::string                                 m_strSerial;
    std::string                                 m_strSerialUser;
    std::string                                 m_strCommunityID;

    unsigned char                               m_ucBlurLevel;

    // Voice
    eVoiceState                                 m_VoiceState;
    std::list < CElement* >                     m_lstBroadcastList;
    std::list < CElement* >                     m_lstIgnoredList;

    // Sync stuff
    bool                                        m_bSyncingVelocity;
    unsigned int                                m_uiPuresyncPackets;

    unsigned long                               m_ulLastReceivedSyncTime;

    std::map < std::string, std::string >       m_AnnounceValues;

    uint                                        m_uiWeaponIncorrectCount;

    std::map < CPlayer*, SNearInfo >            m_NearPlayerList;
    std::map < CPlayer*, SNearInfo >            m_FarPlayerList;
    long long                                   m_llNearListUpdateTime;

    CVector                                     m_vecCamPosition;
    CVector                                     m_vecCamFwd;
    int                                         m_iLastZoneDebug;
};

#endif
