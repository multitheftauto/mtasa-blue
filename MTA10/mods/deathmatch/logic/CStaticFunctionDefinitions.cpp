/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CStaticFunctionDefinitions.cpp
*  PURPOSE:     Scripting function processing
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Derek Abdine <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Peter Beverloo <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

static CLuaManager*                                 m_pLuaManager;
static CEvents*                                     m_pEvents;
static CCoreInterface*                              m_pCore;
static CGame*                                       m_pGame;
static CClientGame*                                 m_pClientGame;
static CClientManager*                              m_pManager;
static CClientEntity*                               m_pRootEntity;
static CClientPlayerManager*                        m_pPlayerManager;
static CClientVehicleManager*                       m_pVehicleManager;
static CClientObjectManager*                        m_pObjectManager;
static CClientColManager*                           m_pColManager;
static CClientTeamManager*                          m_pTeamManager;
static CGUI*                                        m_pGUI;
static CClientGUIManager*                           m_pGUIManager;
static CScriptKeyBinds*                             m_pScriptKeyBinds;
//static CScriptFontLoader*                           m_pScriptFontLoader;
static CClientMarkerManager*                        m_pMarkerManager;
static CClientPickupManager*                        m_pPickupManager;
static CMovingObjectsManager*                       m_pMovingObjectsManager;
static CBlendedWeather*                             m_pBlendedWeather;
static CRadarMap*                                   m_pRadarMap;
static CClientCamera*                               m_pCamera;
static CClientExplosionManager*                     m_pExplosionManager;
static CClientProjectileManager*                    m_pProjectileManager;
static CClientSoundManager*                         m_pSoundManager;
static CClientFontManager*                          m_pFontManager;

// Used to run a function on all the children of the elements too
#define RUN_CHILDREN CChildListType::const_iterator iter=Entity.IterBegin();for(;iter!=Entity.IterEnd();iter++)
#define RUN_CHILDREN_BACKWARDS CChildListType::const_reverse_iterator iter=pEntity->IterReverseBegin();for(;iter!=pEntity->IterReverseEnd();iter++)


CStaticFunctionDefinitions::CStaticFunctionDefinitions (
    CLuaManager* pLuaManager,
    CEvents* pEvents,
    CCoreInterface* pCore,
    CGame* pGame,
    CClientGame* pClientGame,
    CClientManager* pManager )
{
    m_pLuaManager = pLuaManager;
    m_pEvents = pEvents;
    m_pCore = pCore;
    m_pGame = pGame;
    m_pClientGame = pClientGame;
    m_pManager = pManager;
    m_pRootEntity = m_pClientGame->GetRootEntity ();
    m_pPlayerManager = pManager->GetPlayerManager ();
    m_pVehicleManager = pManager->GetVehicleManager ();
    m_pObjectManager = pManager->GetObjectManager ();
    m_pColManager = pManager->GetColManager ();
    m_pTeamManager = pManager->GetTeamManager ();
    m_pGUI = pCore->GetGUI ();
    m_pGUIManager = pManager->GetGUIManager ();
    m_pScriptKeyBinds = m_pClientGame->GetScriptKeyBinds ();
//    m_pScriptFontLoader = m_pClientGame->GetScriptFontLoader();
    m_pMarkerManager = pManager->GetMarkerManager ();
    m_pPickupManager = pManager->GetPickupManager ();
    m_pMovingObjectsManager = m_pClientGame->GetMovingObjectsManager ();
    m_pBlendedWeather = m_pClientGame->GetBlendedWeather ();
    m_pRadarMap = m_pClientGame->GetRadarMap ();
    m_pCamera = pManager->GetCamera ();
    m_pExplosionManager = pManager->GetExplosionManager ();
    m_pProjectileManager = pManager->GetProjectileManager ();
    m_pSoundManager = pManager->GetSoundManager ();
    m_pFontManager = pManager->GetFontManager ();
}


CStaticFunctionDefinitions::~CStaticFunctionDefinitions ( void )
{
}


bool CStaticFunctionDefinitions::AddEvent ( CLuaMain& LuaMain, const char* szName, bool bAllowRemoteTrigger )
{
    assert ( szName );

    // Valid name?
    if ( strlen ( szName ) > 0 )
    {
        // Add our event to CEvents
        return m_pEvents->AddEvent ( szName, "", &LuaMain, bAllowRemoteTrigger );
    }

    return false;
}


bool CStaticFunctionDefinitions::AddEventHandler ( CLuaMain& LuaMain, char* szName, CClientEntity& Entity, const CLuaFunctionRef& iLuaFunction, bool bPropagated )
{
    assert ( szName );
    
    // Is this an old event name? get its new one (added this for backwards compatibility)
    char szUpdatedName [ 64 ];
    if ( m_pEvents->IsExtinctEvent ( szName, szUpdatedName, 64 ) ) szName = szUpdatedName;

    // We got an event with that name?
    if ( m_pEvents->Exists ( szName ) )
    {
        // Add the event handler
        if ( Entity.AddEvent ( &LuaMain, szName, iLuaFunction, bPropagated ) )
            return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::RemoveEventHandler ( CLuaMain& LuaMain, char* szName, CClientEntity& Entity, const CLuaFunctionRef& iLuaFunction )
{
    assert ( szName );

    // Is this an old event name? get its new one (added this for backwards compatibility)
    char szUpdatedName [ 64 ];
    if ( m_pEvents->IsExtinctEvent ( szName, szUpdatedName, 64 ) ) szName = szUpdatedName;

    // We got an event and handler with that name?
    if ( m_pEvents->Exists ( szName ) )
    {
        // ACHTUNG: CHECK WHETHER THE LUA FUNCTION REF IS CORRECTLY FOUND
        if ( Entity.DeleteEvent ( &LuaMain, szName, iLuaFunction ) )
        {
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::TriggerEvent ( const char* szName, CClientEntity& Entity, const CLuaArguments& Arguments, bool& bWasCancelled )
{
    // There is such event?
    if ( m_pEvents->Exists ( szName ) )
    {
        // Call the event
        Entity.CallEvent ( szName, Arguments, true );
        bWasCancelled = m_pEvents->WasEventCancelled ();
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::TriggerServerEvent ( const char* szName, CClientEntity& CallWithEntity, CLuaArguments& Arguments )
{
    assert ( szName );

    if ( CallWithEntity.IsLocalEntity ()  )
        return false;

    NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
    if ( pBitStream )
    {
        unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
        pBitStream->WriteCompressed ( usNameLength );
        pBitStream->Write ( const_cast < char* > ( szName ), usNameLength );
        pBitStream->Write ( CallWithEntity.GetID () );
        if ( !Arguments.WriteToBitStream ( *pBitStream ) )
        {
            g_pNet->DeallocateNetBitStream ( pBitStream );
            return false;
        }
        g_pNet->SendPacket ( PACKET_ID_LUA_EVENT, pBitStream );
        g_pNet->DeallocateNetBitStream ( pBitStream );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::CancelEvent ( bool bCancel )
{
    m_pEvents->CancelEvent ( bCancel );
    return true;
}


bool CStaticFunctionDefinitions::WasEventCancelled ( void )
{
    return m_pEvents->WasEventCancelled ();
}


bool CStaticFunctionDefinitions::OutputConsole ( const char* szText )
{
    m_pCore->GetConsole ()->Print ( szText );
    return true;
}


bool CStaticFunctionDefinitions::OutputChatBox ( const char* szText, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, bool bColorCoded )
{
    CLuaArguments Arguments;
    Arguments.PushString ( szText );
    Arguments.PushNumber ( ucRed );
    Arguments.PushNumber ( ucGreen );
    Arguments.PushNumber ( ucBlue );
    g_pClientGame->GetRootEntity()->CallEvent ( "onClientChatMessage", Arguments, false );

    m_pCore->ChatPrintfColor ( "%s", bColorCoded, ucRed, ucGreen, ucBlue, szText );
    return true;
}


bool CStaticFunctionDefinitions::SetClipboard ( SString& strText )
{
    std::wstring strUTF = MbUTF8ToUTF16 ( strText );

    // Open and empty the clipboard
    OpenClipboard ( NULL );
    EmptyClipboard ();

    // Allocate the clipboard buffer and copy the data
    HGLOBAL hBuf = GlobalAlloc ( GMEM_DDESHARE, strUTF.length() * sizeof(wchar_t) + sizeof(wchar_t) );
    wchar_t* buf = reinterpret_cast < wchar_t* > ( GlobalLock ( hBuf ) );
    wcscpy ( buf , strUTF.c_str () );
    GlobalUnlock ( hBuf );

    // Copy the data into the clipboard
    SetClipboardData ( CF_UNICODETEXT , hBuf );

    // Close the clipboard
    CloseClipboard ();
    return true;
}


bool CStaticFunctionDefinitions::GetClipboard ( SString& strText )
{
    OpenClipboard( NULL );

    // Get the clipboard's data and put it into a char array
    const wchar_t * szBuffer = reinterpret_cast < const wchar_t* > ( GetClipboardData ( CF_UNICODETEXT ) );

    CloseClipboard ();

    if ( szBuffer )
    {
        strText = UTF16ToMbUTF8 ( szBuffer );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::ShowChat ( bool bShow )
{
    g_pCore->SetChatVisible ( bShow );
    return true;
}


CClientEntity* CStaticFunctionDefinitions::GetRootElement ( void )
{
    return m_pRootEntity;
}


CClientEntity* CStaticFunctionDefinitions::GetElementByID ( const char* szID, unsigned int uiIndex )
{
    assert ( szID );

    // Search it from root and down
    return m_pRootEntity->FindChild ( szID, uiIndex, true );
}


CClientEntity* CStaticFunctionDefinitions::GetElementByIndex ( const char* szType, unsigned int uiIndex )
{
    assert ( szType );

    // Search it from root and down
    //return m_pMapManager->GetRootElement ()->FindChildByType ( szType, uiIndex, true );
    return m_pRootEntity->FindChildByType ( szType, uiIndex, true );
}


CClientEntity* CStaticFunctionDefinitions::GetElementChild ( CClientEntity& Entity, unsigned int uiIndex )
{
    // Grab it
    unsigned int uiCurrent = 0;
    CChildListType ::const_iterator iter = Entity.IterBegin ();
    for ( ; iter != Entity.IterEnd (); iter++ )
    {
        if ( uiIndex == uiCurrent++ )
        {
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}


bool CStaticFunctionDefinitions::GetElementMatrix ( CClientEntity& Entity, CMatrix& matrix )
{
    return Entity.GetMatrix ( matrix );
}


bool CStaticFunctionDefinitions::GetElementPosition ( CClientEntity& Entity, CVector& vecPosition )
{
    Entity.GetPosition ( vecPosition );
    return true;
}


bool CStaticFunctionDefinitions::GetElementRotation ( CClientEntity& Entity, CVector& vecRotation, const char* szRotationOrder)
{
    eEulerRotationOrder desiredRotOrder = EulerRotationOrderFromString(szRotationOrder);
    if (desiredRotOrder == EULER_INVALID)
    {
        return false;
    }

    int iType = Entity.GetType ();
    switch ( iType )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            Ped.GetRotationDegrees ( vecRotation );

            // Correct the rotation
            vecRotation.fZ = fmod( -vecRotation.fZ, 360);
            if ( vecRotation.fZ < 0 )
                vecRotation.fZ = 360 + vecRotation.fZ;

            if (desiredRotOrder != EULER_DEFAULT)
            {
                //In get, ped is Z-Y-X wheras in set it's -Z-Y-Z
                //It's a bug in ped, but the goal here is not to fix it, so we hack
                vecRotation.fZ = -vecRotation.fZ;
                vecRotation = ConvertEulerRotationOrder(vecRotation, EULER_MINUS_ZYX, desiredRotOrder);
            }

            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            Vehicle.GetRotationDegrees ( vecRotation );
            if (desiredRotOrder != EULER_DEFAULT && desiredRotOrder != EULER_ZYX)
            {
                vecRotation = ConvertEulerRotationOrder(vecRotation, EULER_ZYX, desiredRotOrder);
            }
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            Object.GetRotationDegrees ( vecRotation );
            if (desiredRotOrder != EULER_DEFAULT && desiredRotOrder != EULER_ZXY)
            {
                vecRotation = ConvertEulerRotationOrder(vecRotation, EULER_ZXY, desiredRotOrder);
            }
            break;
        }
        case CCLIENTPROJECTILE:
        {
            CClientProjectile& Projectile = static_cast < CClientProjectile& > ( Entity );
            Projectile.GetRotationDegrees ( vecRotation );
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::GetElementVelocity ( CClientEntity& Entity, CVector& vecVelocity )
{
    int iType = Entity.GetType ();
    switch ( iType )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            Ped.GetMoveSpeed ( vecVelocity );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            Vehicle.GetMoveSpeed ( vecVelocity );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            Object.GetMoveSpeed ( vecVelocity );
            break;
        }
        case CCLIENTPROJECTILE:
        {
            CClientProjectile& Projectile = static_cast < CClientProjectile& > ( Entity );
            Projectile.GetVelocity ( vecVelocity );
            break;
        }
        case CCLIENTSOUND:
        {
            CClientSound& Sound = static_cast < CClientSound& > ( Entity );
            Sound.GetVelocity ( vecVelocity );
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::GetElementInterior ( CClientEntity& Entity, unsigned char& ucInterior )
{
    ucInterior = Entity.GetInterior ();
    return true;
}


bool CStaticFunctionDefinitions::GetElementBoundingBox ( CClientEntity& Entity, CVector &vecMin, CVector &vecMax )
{
    CModelInfo* pModelInfo = NULL;
    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            pModelInfo = g_pGame->GetModelInfo ( Ped.GetModel () );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            pModelInfo = g_pGame->GetModelInfo ( Vehicle.GetModel () );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            pModelInfo = g_pGame->GetModelInfo ( Object.GetModel () );
            break;
        }
    }

    if ( pModelInfo )
    {
        CBoundingBox* pBoundingBox = pModelInfo->GetBoundingBox ();
        if ( pBoundingBox )
        {
            vecMin = pBoundingBox->vecBoundMin;
            vecMin.fX += pBoundingBox->vecBoundOffset.fX;
            vecMin.fY += pBoundingBox->vecBoundOffset.fY;
            vecMin.fZ += pBoundingBox->vecBoundOffset.fZ;

            vecMax = pBoundingBox->vecBoundMax;
            vecMax.fX += pBoundingBox->vecBoundOffset.fX;
            vecMax.fY += pBoundingBox->vecBoundOffset.fY;
            vecMax.fZ += pBoundingBox->vecBoundOffset.fZ;

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetElementRadius ( CClientEntity& Entity, float &fRadius )
{
    CModelInfo* pModelInfo = NULL;
    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            pModelInfo = g_pGame->GetModelInfo ( Ped.GetModel () );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            pModelInfo = g_pGame->GetModelInfo ( Vehicle.GetModel () );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            pModelInfo = g_pGame->GetModelInfo ( Object.GetModel () );
            break;
        }
    }
    if ( pModelInfo )
    {
        CBoundingBox* pBoundingBox = pModelInfo->GetBoundingBox ();
        if ( pBoundingBox )
        {
            fRadius = pBoundingBox->fRadius;
            return true;
        }
    }

    return false;
}


CClientEntity* CStaticFunctionDefinitions::GetElementAttachedTo ( CClientEntity& Entity )
{
    CClientEntity* pEntityAttachedTo = Entity.GetAttachedTo();
    if ( pEntityAttachedTo )
    {        
        if ( pEntityAttachedTo->IsEntityAttached ( &Entity ) )
        {
            return pEntityAttachedTo;
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::GetElementDistanceFromCentreOfMassToBaseOfModel ( CClientEntity& Entity, float& fDistance )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            fDistance = static_cast < CClientPed& > ( Entity ).GetDistanceFromCentreOfMassToBaseOfModel ();
            return true;       
        }
        case CCLIENTVEHICLE:
        {
            fDistance = static_cast < CClientVehicle& > ( Entity ).GetDistanceFromCentreOfMassToBaseOfModel ();
            return true;
        }
        case CCLIENTOBJECT:
        {
            fDistance = static_cast < CClientObject& > ( Entity ).GetDistanceFromCentreOfMassToBaseOfModel ();
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetElementAttachedOffsets ( CClientEntity & Entity, CVector & vecPosition, CVector & vecRotation )
{
    Entity.GetAttachedOffsets ( vecPosition, vecRotation );
    return true;
}


bool CStaticFunctionDefinitions::GetElementAlpha ( CClientEntity& Entity, unsigned char& ucAlpha )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            ucAlpha = Ped.GetAlpha ();
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            ucAlpha = Vehicle.GetAlpha ();
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject & Object = static_cast < CClientObject & > ( Entity );
            ucAlpha = Object.GetAlpha ();
            break;
        }
        default: return false;
    }
    return true;
}


bool CStaticFunctionDefinitions::IsElementOnScreen ( CClientEntity& Entity, bool& bOnScreen )
{
    bOnScreen = Entity.IsOnScreen ();
    return true;
}


bool CStaticFunctionDefinitions::GetElementHealth ( CClientEntity& Entity, float& fHealth )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            fHealth = Ped.GetHealth ();
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            fHealth = Vehicle.GetHealth ();
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            fHealth = Object.GetHealth ();
            break;
        }
        default: return false;
    }
    return true;
}


bool CStaticFunctionDefinitions::GetElementModel ( CClientEntity& Entity, unsigned short & usModel )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            usModel = static_cast < unsigned short > ( Ped.GetModel () );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            usModel = Vehicle.GetModel ();
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            usModel = Object.GetModel ();
            break;
        }
        case CCLIENTPICKUP:
        {
            CClientPickup& pPickup = static_cast < CClientPickup& > ( Entity );
            usModel = pPickup.GetModel ();
            break;
        }
        default: return false;
    }
    return true;
}


bool CStaticFunctionDefinitions::IsElementInWater ( CClientEntity& Entity, bool & bInWater )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            bInWater = Ped.IsInWater ();
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            bInWater = Vehicle.IsInWater ();
            break;
        }
        default: return false;
    }
    return true;
}


bool CStaticFunctionDefinitions::IsElementSyncer ( CClientEntity& Entity, bool & bIsSyncer )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        {
            CClientPed* Ped = static_cast < CClientPed* > ( &Entity );
            if ( Ped )
                bIsSyncer = m_pClientGame->GetPedSync()->Exists ( Ped );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CDeathmatchVehicle* Vehicle = static_cast < CDeathmatchVehicle* > ( &Entity );
            if ( Vehicle )
                bIsSyncer = m_pClientGame->GetUnoccupiedVehicleSync()->Exists ( Vehicle );
            break;
        }
        case CCLIENTOBJECT:
        {
            CDeathmatchObject* pObject = static_cast < CDeathmatchObject* > ( &Entity );
            if ( pObject )
                bIsSyncer = m_pClientGame->GetObjectSync ()->Exists ( pObject );
            break;
        }
        default: return false;
    }
    return true;
}


bool CStaticFunctionDefinitions::IsElementCollidableWith ( CClientEntity& Entity, CClientEntity& ThisEntity, bool & bCanCollide )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPLAYER:
        case CCLIENTPED:
        case CCLIENTOBJECT:
        case CCLIENTVEHICLE:
        {
            switch ( ThisEntity.GetType () )
            {
                case CCLIENTPLAYER:
                case CCLIENTPED:
                case CCLIENTOBJECT:
                case CCLIENTVEHICLE:
                {    
                    bCanCollide = Entity.IsCollidableWith ( &ThisEntity );
                    return true;
                }
                default: break;
            }
            break;
        }
        default: break;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetElementCollisionsEnabled ( CClientEntity& Entity )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            return Vehicle.IsCollisionEnabled ( );
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            return Object.IsCollisionEnabled ( );
        }
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            return Ped.GetUsesCollision ( );
        }
        default: return false;
    }

    return false;
}


bool CStaticFunctionDefinitions::IsElementFrozen ( CClientEntity& Entity, bool& bFrozen )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPLAYER:
        case CCLIENTPED:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            bFrozen = Ped.IsFrozen ();
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            bFrozen = Vehicle.IsFrozen ();
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            bFrozen = Object.IsStatic ();
            break;
        }
        default: return false;
    }

    return true;
}


CClientDummy* CStaticFunctionDefinitions::CreateElement ( CResource& Resource, const char* szTypeName, const char* szID )
{
    assert ( szTypeName );
    assert ( szID );

    // Long enough typename and not an internal one?
    if ( strlen ( szTypeName ) > 0 && CClientEntity::GetTypeID ( szTypeName ) == CCLIENTUNKNOWN )
    {
        CClientDummy* pDummy = new CClientDummy ( m_pManager, INVALID_ELEMENT_ID,  szTypeName );
        pDummy->SetName ( (char*) szID );

        pDummy->SetParent ( Resource.GetResourceDynamicEntity() );
        return pDummy;
    }

    return NULL;
}


bool CStaticFunctionDefinitions::DestroyElement ( CClientEntity& Entity )
{
    // Run us on all its children
    CChildListType ::const_iterator iter = Entity.IterBegin ();
    while ( iter != Entity.IterEnd () )
    {
        if ( DestroyElement ( **iter ) )
            iter = Entity.IterBegin ();
        else
            ++iter;
    }

    // Are we already being deleted?
    if ( Entity.IsBeingDeleted () )
        return false;

    // We can't delete our root
    if ( &Entity == m_pRootEntity )
        return false;

    
    // Use the element deleter to delete it if it's local and not system
    if ( Entity.IsLocalEntity () && !Entity.IsSystemEntity () )
    {
        m_pClientGame->GetElementDeleter ()->Delete ( &Entity );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetElementID ( CClientEntity& Entity, const char* szID )
{
    assert ( szID );

    // Change ID only for client-created elements
    if ( Entity.IsLocalEntity () )
    {
        // Set the new ID
        Entity.SetName ( szID );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetElementData ( CClientEntity& Entity, const char* szName, CLuaArgument& Variable, CLuaMain& LuaMain, bool bSynchronize )
{
    assert ( szName );

    CLuaArgument * pCurrentVariable = Entity.GetCustomData ( szName, false );
    if ( !pCurrentVariable || Variable != *pCurrentVariable )
    {
        if ( bSynchronize )
        {
            // Allocate a bitstream
            NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
            if ( pBitStream )
            {
                // Write element ID, name length and the name. Also write the variable.
                pBitStream->Write ( Entity.GetID () );
                unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
                pBitStream->WriteCompressed ( usNameLength );
                pBitStream->Write ( const_cast < char* > ( szName ), usNameLength );    
                Variable.WriteToBitStream ( *pBitStream );

                // Send the packet and deallocate
                g_pNet->SendPacket ( PACKET_ID_CUSTOM_DATA, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE, PACKET_ORDERING_CHAT );
                g_pNet->DeallocateNetBitStream ( pBitStream );

                // Set its custom data
                Entity.SetCustomData ( szName, Variable, &LuaMain );

                return true;
            }
        }
        else
        {
            // Set its custom data
            Entity.SetCustomData ( szName, Variable, &LuaMain );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::RemoveElementData ( CClientEntity& Entity, const char* szName )
{
    // TODO
    return false;
}



bool CStaticFunctionDefinitions::SetElementPosition ( CClientEntity& Entity, const CVector& vecPosition, bool bWarp )
{
    RUN_CHILDREN SetElementPosition ( **iter, vecPosition );

    if ( bWarp )
        Entity.Teleport ( vecPosition );
    else
        Entity.SetPosition ( vecPosition );

    return true;
}


bool CStaticFunctionDefinitions::SetElementRotation ( CClientEntity& Entity, const CVector& vecRotation, const char* szRotationOrder )
{
    RUN_CHILDREN SetElementRotation ( **iter, vecRotation, szRotationOrder);

    eEulerRotationOrder argumentRotOrder = EulerRotationOrderFromString(szRotationOrder);
    if (argumentRotOrder == EULER_INVALID)
    {
        return false;
    }

    int iType = Entity.GetType ();
    switch ( iType )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            if (argumentRotOrder == EULER_DEFAULT || argumentRotOrder == EULER_MINUS_ZYX)
            {
                Ped.SetRotationDegrees ( vecRotation );
            }
            else
            {
                Ped.SetRotationDegrees ( ConvertEulerRotationOrder(vecRotation, argumentRotOrder, EULER_MINUS_ZYX ) );
            }
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            if (argumentRotOrder == EULER_DEFAULT || argumentRotOrder == EULER_ZYX)
            {
                Vehicle.SetRotationDegrees ( vecRotation );
            }
            else
            {
                Vehicle.SetRotationDegrees ( ConvertEulerRotationOrder(vecRotation, argumentRotOrder, EULER_ZYX ) );            
            }
			
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            if (argumentRotOrder == EULER_DEFAULT || argumentRotOrder == EULER_ZXY)
            {
                Object.SetRotationDegrees ( vecRotation );
            }
            else
            {
                Object.SetRotationDegrees ( ConvertEulerRotationOrder(vecRotation, argumentRotOrder, EULER_ZXY ) );
            }
            
            break;
        }
        case CCLIENTPROJECTILE:
        {
            //Didn't implement anything for projectiles since I couldn't really test (only non crashing element was satchel and its rotation is ugly)
            CClientProjectile& Projectile = static_cast < CClientProjectile& > ( Entity );
            Projectile.SetRotationDegrees ( const_cast < CVector& > ( vecRotation ) );
            break;
        }

        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetElementVelocity ( CClientEntity& Entity, const CVector& vecVelocity )
{
    RUN_CHILDREN SetElementVelocity ( **iter, vecVelocity );

    int iType = Entity.GetType ();
    switch ( iType )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            Ped.SetMoveSpeed ( vecVelocity );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            Vehicle.SetMoveSpeed ( vecVelocity );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            Object.SetMoveSpeed ( vecVelocity );
            break;
        }
        case CCLIENTPROJECTILE:
        {
            CClientProjectile& Projectile = static_cast < CClientProjectile& > ( Entity );
            Projectile.SetVelocity ( const_cast < CVector& > ( vecVelocity ) );
            break;
        }
        case CCLIENTSOUND:
        {
            CClientSound& Sound = static_cast < CClientSound& > ( Entity );
            Sound.SetVelocity ( const_cast < CVector& > ( vecVelocity ) );
            break;
        }

        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetElementParent ( CClientEntity& Entity, CClientEntity& Parent, CLuaMain* pLuaMain )
{
    if ( &Entity != &Parent && !Entity.IsMyChild ( &Parent, true ) )
    {
        if ( Entity.GetType () == CCLIENTGUI )
        {
            if ( Parent.GetType () == CCLIENTGUI ||
                 &Parent == pLuaMain->GetResource()->GetResourceGUIEntity() )
            {
                CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

                GUIElement.SetParent ( &Parent );
                return true;
            }
        }
        else
        {
            CClientEntity* pTemp = &Parent;
            CClientEntity* pRoot = m_pRootEntity;
            bool bValidParent = false;
            while ( pTemp != pRoot )
            {
                const char * szTypeName = pTemp->GetTypeName();
                if ( szTypeName && strcmp(szTypeName, "map") == 0 )
                {
                    bValidParent = true; // parents must be a map
                    break;
                }

                pTemp = pTemp->GetParent();
            }

            // Make sure the entity we move is a client entity or we get a problem
            if ( bValidParent && Entity.IsLocalEntity () )
            {
                // Set the new parent
                Entity.SetParent ( &Parent );
                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetElementInterior ( CClientEntity& Entity, unsigned char ucInterior, bool bSetPosition, CVector& vecPosition )
{
    RUN_CHILDREN SetElementInterior ( **iter, ucInterior, bSetPosition, vecPosition );

    Entity.SetInterior ( ucInterior );
    if ( bSetPosition )
        Entity.SetPosition ( vecPosition );

    return true;
}


bool CStaticFunctionDefinitions::SetElementDimension ( CClientEntity& Entity, unsigned short usDimension )
{
    switch ( Entity.GetType () )
    {
        // Client side elements
        case CCLIENTTEAM:
        {
            CClientTeam& Team = static_cast < CClientTeam& > ( Entity );
            list < CClientPlayer* > ::const_iterator iter = Team.IterBegin ();
            for ( ; iter != Team.IterEnd () ; iter++ )
            {
                (*iter)->SetDimension ( usDimension );
            }
        }

        case CCLIENTCOLSHAPE:
        case CCLIENTDUMMY:
        case CCLIENTVEHICLE:
        case CCLIENTOBJECT:
        case CCLIENTMARKER:
        case CCLIENTRADARMARKER:
        case CCLIENTPED:
        case CCLIENTPICKUP:
        case CCLIENTRADARAREA:
        case CCLIENTWORLDMESH:
        case CCLIENTSOUND:
        {
            Entity.SetDimension ( usDimension );

            return true;
        }

        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            if ( Ped.IsLocalPlayer () )
            {
                // Update all of our streamers/managers to the local player's dimension
                m_pClientGame->SetAllDimensions ( usDimension );
            }

            Ped.SetDimension ( usDimension );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::AttachElements ( CClientEntity& Entity, CClientEntity& AttachedToEntity, CVector& vecPosition, CVector& vecRotation )
{
    RUN_CHILDREN AttachElements ( **iter, AttachedToEntity, vecPosition, vecRotation );

    // Are they the same element?
    if ( &Entity != &AttachedToEntity )
    {
        // Make sure they aren't already attached to eachother in reverse
        if ( AttachedToEntity.GetAttachedTo () != &Entity )
        {
            // Can these elements be attached?
            if ( Entity.IsAttachToable () && AttachedToEntity.IsAttachable () && Entity.GetDimension() == AttachedToEntity.GetDimension() )
            {
                ConvertDegreesToRadians ( vecRotation );

                Entity.SetAttachedOffsets ( vecPosition, vecRotation );
                Entity.AttachTo ( &AttachedToEntity );

                return true;
            }
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::DetachElements ( CClientEntity& Entity, CClientEntity* pAttachedToEntity )
{
    RUN_CHILDREN DetachElements ( **iter, pAttachedToEntity );

    CClientEntity* pActualAttachedToEntity = Entity.GetAttachedTo ();
    if ( pActualAttachedToEntity )
    {
        if ( pAttachedToEntity == NULL || pActualAttachedToEntity == pAttachedToEntity )
        {
            Entity.AttachTo ( NULL );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetElementAttachedOffsets ( CClientEntity & Entity, CVector & vecPosition, CVector & vecRotation )
{
    RUN_CHILDREN SetElementAttachedOffsets ( **iter, vecPosition, vecRotation );

    Entity.SetAttachedOffsets ( vecPosition, vecRotation );
    return true;
}


bool CStaticFunctionDefinitions::SetElementAlpha ( CClientEntity& Entity, unsigned char ucAlpha )
{
    RUN_CHILDREN SetElementAlpha ( **iter, ucAlpha );

    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed & Ped = static_cast < CClientPed & > ( Entity );
            Ped.SetAlpha ( ucAlpha );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle & Vehicle = static_cast < CClientVehicle & > ( Entity );
            Vehicle.SetAlpha ( ucAlpha );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject & Object = static_cast < CClientObject & > ( Entity );
            Object.SetAlpha ( ucAlpha );
            break;
        }
        case CCLIENTMARKER:
        {
            CClientMarker & Marker = static_cast < CClientMarker & > ( Entity );
            SColor color = Marker.GetColor ();
            color.A = ucAlpha;
            Marker.SetColor ( color );
            break;
        }

        default: return false;
    }
    return true;
}


bool CStaticFunctionDefinitions::SetElementHealth ( CClientEntity& Entity, float fHealth )
{
    RUN_CHILDREN SetElementHealth ( **iter, fHealth );

    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            // Grab the model
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );

            // Limit to max health
            float fMaxHealth = Ped.GetMaxHealth ();
            if ( fHealth > fMaxHealth )
                fHealth = fMaxHealth;

            // Set the new health
            Ped.SetHealth ( fHealth );            
            return true;
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            Vehicle.SetHealth ( fHealth );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            Object.SetHealth ( fHealth );
            break;
        }
        default: return false;
    }
    return true;
}


bool CStaticFunctionDefinitions::SetElementModel ( CClientEntity& Entity, unsigned short usModel )
{
    RUN_CHILDREN SetElementModel ( **iter, usModel );

    switch ( Entity.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            // Grab the model
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );            
            if ( Ped.GetModel () == usModel ) return false;
            if ( !CClientPlayerManager::IsValidModel ( usModel ) ) return false;
            Ped.SetModel ( usModel );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            if ( Vehicle.GetModel () == usModel ) return false;
            if ( !CClientVehicleManager::IsValidModel ( usModel ) ) return false;
            Vehicle.SetModelBlocking ( usModel );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            if ( Object.GetModel () == usModel ) return false;
            if ( !CClientObjectManager::IsValidModel ( usModel ) ) return false;
            Object.SetModel ( usModel );
            break;
        }
        default: return false;
    }
    return true;
}


bool CStaticFunctionDefinitions::SetRadioChannel ( unsigned char& ucChannel )
{
    return m_pPlayerManager->GetLocalPlayer ()->SetCurrentRadioChannel ( ucChannel );
}


bool CStaticFunctionDefinitions::GetRadioChannel ( unsigned char& ucChannel )
{
    ucChannel = m_pPlayerManager->GetLocalPlayer ()->GetCurrentRadioChannel ();
    return true;
}


CClientPlayer* CStaticFunctionDefinitions::GetLocalPlayer ( void )
{
    return m_pPlayerManager->GetLocalPlayer ();
}


CClientEntity* CStaticFunctionDefinitions::GetPedTarget ( CClientPed& Ped )
{
    CVector vecOrigin, vecTarget;
    Ped.GetShotData ( &vecOrigin, &vecTarget );

    CColPoint* pCollision = NULL;
    CEntity* pCollisionGameEntity = NULL;
    CVector vecCollision = vecTarget;
    bool bCollision = g_pGame->GetWorld ()->ProcessLineOfSight ( &vecOrigin, &vecTarget, &pCollision, &pCollisionGameEntity );
    if ( bCollision && pCollision )
        vecCollision = *pCollision->GetPosition ();

    // Destroy the colpoint
    if ( pCollision )
    {
        pCollision->Destroy ();
    }

    CClientEntity* pCollisionEntity = NULL;
    if ( pCollisionGameEntity )
    {
        pCollisionEntity = m_pManager->FindEntity ( pCollisionGameEntity );

        return pCollisionEntity;
    }

    return NULL;
}


bool CStaticFunctionDefinitions::GetPedTargetCollision ( CClientPed& Ped, CVector& vecCollision )
{
    CVector vecOrigin, vecTarget;
    Ped.GetShotData ( &vecOrigin, &vecTarget );

    CColPoint* pCollision = NULL;
    CEntity* pCollisionGameEntity = NULL;
    bool bCollision = g_pGame->GetWorld ()->ProcessLineOfSight ( &vecOrigin, &vecTarget, &pCollision, &pCollisionGameEntity );
    if ( pCollision )
    {
        vecCollision = *pCollision->GetPosition ();            

        // Destroy the collision
        pCollision->Destroy ();
    }

    return bCollision;
}


bool CStaticFunctionDefinitions::GetPedTask ( CClientPed& Ped, bool bPrimary, unsigned int uiTaskType, std::vector < SString >& outTaskHierarchy )
{
    CTaskManager* pTaskManager = Ped.GetTaskManager ();
    if ( pTaskManager )
    {
        CTask* pTask = NULL;
        if ( bPrimary )
            pTask = pTaskManager->GetTask ( uiTaskType );
        else
            pTask = pTaskManager->GetTaskSecondary ( uiTaskType );

        while ( pTask )
        {
            outTaskHierarchy.push_back ( pTask->GetTaskName () );
            pTask = pTask->GetSubTask ();
        }

        return !outTaskHierarchy.empty ();
    }

    return false;
}


bool CStaticFunctionDefinitions::GetPedClothes ( CClientPed & Ped, unsigned char ucType, char* szTextureReturn, char* szModelReturn )
{
    SPlayerClothing* pClothing = Ped.GetClothes ()->GetClothing ( ucType );
    if ( pClothing )
    {
        if ( szTextureReturn )
            strcpy ( szTextureReturn, pClothing->szTexture );
        if ( szModelReturn )
            strcpy ( szModelReturn, pClothing->szModel );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetPedControlState ( CClientPed & Ped, const char * szControl, bool & bState )
{
    if ( Ped.GetType () == CCLIENTPLAYER )
    {
        CControllerState cs;
        Ped.GetControllerState ( cs );
        bool bOnFoot = ( !Ped.GetRealOccupiedVehicle () );
        bState = CClientPad::GetControlState ( szControl, cs, bOnFoot );
        return true;
    }
    if ( Ped.m_Pad.GetControlState ( szControl, bState ) )
    {
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::IsPedDoingGangDriveby ( CClientPed & Ped, bool & bDoingGangDriveby )
{
    bDoingGangDriveby = Ped.IsDoingGangDriveby ();
    return true;
}


bool CStaticFunctionDefinitions::GetPedAnimation ( CClientPed & Ped, char * szBlockName, char * szAnimName, unsigned int uiLength )
{
    if ( Ped.IsRunningAnimation () )
    {
        strncpy ( szBlockName, Ped.GetAnimationBlock ()->GetName (), uiLength );
        strncpy ( szAnimName, Ped.GetAnimationName (), uiLength );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetPedMoveAnim ( CClientPed & Ped, unsigned int& iMoveAnim )
{
    iMoveAnim = (unsigned int)Ped.GetMoveAnim();
    return true;
}

bool CStaticFunctionDefinitions::GetPedMoveState ( CClientPed & Ped, std::string& strMoveState )
{
    if ( Ped.GetMovementState(strMoveState) )
        return true;
    return false;
}


bool CStaticFunctionDefinitions::IsPedHeadless ( CClientPed & Ped, bool & bHeadless )
{
    bHeadless = Ped.IsHeadless ();
    return true;
}


bool CStaticFunctionDefinitions::IsPedFrozen ( CClientPed & Ped, bool & bFrozen )
{
    bFrozen = Ped.IsFrozen ();
    return true;
}


bool CStaticFunctionDefinitions::IsPedFootBloodEnabled ( CClientPed & Ped, bool & bHasFootBlood )
{
    bHasFootBlood = Ped.IsFootBloodEnabled ();
    return true;
}


bool CStaticFunctionDefinitions::GetPedCameraRotation ( CClientPed & Ped, float & fRotation )
{
    fRotation = ConvertRadiansToDegrees ( Ped.GetCameraRotation() );
    return true;
}

bool CStaticFunctionDefinitions::GetPedWeaponMuzzlePosition ( CClientPed& Ped, CVector& vecPosition )
{
    return Ped.GetShotData ( NULL, NULL, &vecPosition );
}

bool CStaticFunctionDefinitions::IsPedOnFire ( CClientPed & Ped, bool & bOnFire )
{
    bOnFire = Ped.IsOnFire ();
    return true;
}

bool CStaticFunctionDefinitions::IsPedInVehicle ( CClientPed & Ped, bool & bInVehicle )
{
    bInVehicle = ( Ped.GetRealOccupiedVehicle () != NULL );
    return true;
}

char* CStaticFunctionDefinitions::GetPedSimplestTask ( CClientPed& Ped )
{
    CTaskManager* pTaskManager = Ped.GetTaskManager ();
    if ( pTaskManager )
    {
        CTask* pTask = pTaskManager->GetSimplestActiveTask ();
        if ( pTask == NULL )
        {
            for ( unsigned int i = 0 ; i < TASK_PRIORITY_MAX ; i++ )
            {
                pTask = pTaskManager->GetSimplestTask ( i );
                if ( pTask )
                    break;
            }
        }
        CTask* pTempTask = pTask->GetSubTask ();
        while ( pTempTask )
        {
            pTask = pTempTask;
            pTempTask = pTempTask->GetSubTask ();
        }
        if ( pTask )
        {
            return pTask->GetTaskName ();
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::IsPedDoingTask ( CClientPed& Ped, const char* szTaskName, bool& bIsDoingTask )
{
    assert ( szTaskName );

    bIsDoingTask = false;

    CTaskManager* pTaskManager = Ped.GetTaskManager ();
    if ( pTaskManager )
    {
        CTask* pTask = NULL;
        for ( int i = 0 ; i < TASK_PRIORITY_MAX ; i++ )
        {
            pTask = pTaskManager->GetTask ( i );
            while ( pTask )
            {
                if ( stricmp ( pTask->GetTaskName (), szTaskName ) == 0 )
                {
                    bIsDoingTask = true;
                    return true;
                }
                pTask = pTask->GetSubTask ();
            }
        }
        for ( int i = 0 ; i < TASK_SECONDARY_MAX ; i++ )
        {
            pTask = pTaskManager->GetTaskSecondary ( i );
            while ( pTask )
            {
                if ( stricmp ( pTask->GetTaskName (), szTaskName ) == 0 )
                {
                    bIsDoingTask = true;
                    return true;
                }
                pTask = pTask->GetSubTask ();
            }
        }
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetPedBonePosition ( CClientPed & Ped, eBone bone, CVector & vecPosition )
{
    Ped.GetBonePosition ( bone, vecPosition );
    return true;
}


CClientPlayer* CStaticFunctionDefinitions::GetPlayerFromName ( const char* szNick )
{
    assert ( szNick );

    return m_pPlayerManager->Get ( szNick );
}


bool CStaticFunctionDefinitions::GetPlayerMoney ( long & lMoney )
{
    lMoney = g_pClientGame->GetMoney ();
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerWantedLevel ( char & cWanted )
{
    cWanted = g_pGame->GetPlayerInfo()->GetWanted()->GetWantedLevel ();
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerNametagText ( CClientPlayer & Player, char * szBuffer, unsigned int uiLength )
{
    assert ( szBuffer );
    assert ( uiLength );

    strncpy ( szBuffer, Player.GetNick (), uiLength );
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerNametagColor ( CClientPlayer & Player, unsigned char & ucR, unsigned char & ucG, unsigned char & ucB )
{
    Player.GetNametagColor ( ucR, ucG, ucB );
    return true;
}

bool CStaticFunctionDefinitions::SetPedWeaponSlot ( CClientEntity& Entity, int iSlot )
{
    RUN_CHILDREN SetPedWeaponSlot ( **iter, iSlot );

    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        return Ped.SetCurrentWeaponSlot ( (eWeaponSlot) iSlot );
    }

    return false;
}


bool CStaticFunctionDefinitions::ShowPlayerHudComponent ( unsigned char ucComponent, bool bShow )
{
    enum eHudComponent { HUD_AMMO = 0, HUD_WEAPON, HUD_HEALTH, HUD_BREATH,
                             HUD_ARMOUR, HUD_MONEY, HUD_VEHICLE_NAME, HUD_AREA_NAME, HUD_RADAR, HUD_CLOCK, HUD_RADIO, HUD_WANTED, HUD_ALL };
    switch ( ucComponent )
    {
        case HUD_AMMO:
            g_pGame->GetHud ()->DisableAmmo ( !bShow );
            return true;
        case HUD_WEAPON:
            g_pGame->GetHud ()->DisableWeaponIcon ( !bShow );
            return true;
        case HUD_HEALTH:
            g_pGame->GetHud ()->DisableHealth ( !bShow );
            return true;
        case HUD_BREATH:
            g_pGame->GetHud ()->DisableBreath ( !bShow );
            return true;
        case HUD_ARMOUR:
            g_pGame->GetHud ()->DisableArmour ( !bShow );
            return true;
        case HUD_MONEY:
            g_pGame->GetHud ()->DisableMoney ( !bShow );
            return true;
        case HUD_VEHICLE_NAME:
            g_pGame->GetHud ()->DisableVehicleName ( !bShow );
            return true;
        case HUD_AREA_NAME:
            g_pClientGame->SetHudAreaNameDisabled ( !bShow );
            g_pGame->GetHud ()->DisableAreaName ( !bShow );
            return true;
        case HUD_RADAR:
            g_pGame->GetHud ()->DisableRadar ( !bShow );
            return true;
        case HUD_CLOCK:
            g_pGame->GetHud ()->DisableClock ( !bShow );
            return true;
        case HUD_RADIO:
            g_pGame->GetHud ()->DisableRadioName ( !bShow );
            return true;
        case HUD_WANTED:
            g_pGame->GetHud ()->DisableWantedLevel ( !bShow );
            return true;
        case HUD_ALL:
            g_pGame->GetHud ()->DisableAll ( !bShow );
            return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPlayerMoney ( long lMoney )
{
    g_pClientGame->SetMoney ( lMoney );
    return true;
}


bool CStaticFunctionDefinitions::GivePlayerMoney ( long lMoney )
{
    g_pClientGame->SetMoney ( g_pClientGame->GetMoney () + lMoney );
    return true;
}


bool CStaticFunctionDefinitions::TakePlayerMoney ( long lMoney )
{
    g_pClientGame->SetMoney ( g_pClientGame->GetMoney () - lMoney );
    return true;
}


bool CStaticFunctionDefinitions::SetPlayerNametagText ( CClientEntity& Entity, char * szText )
{
    assert ( szText );
    RUN_CHILDREN SetPlayerNametagText ( **iter, szText );

    if ( IS_PLAYER ( &Entity ) && g_pClientGame->IsNametagValid ( szText ) )
    {
        CClientPlayer& Player = static_cast < CClientPlayer& > ( Entity );

        Player.SetNametagText ( szText );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPlayerNametagColor ( CClientEntity& Entity, unsigned char ucR, unsigned char ucG, unsigned char ucB )
{
    RUN_CHILDREN SetPlayerNametagColor ( **iter, ucR, ucG, ucB );

    if ( IS_PLAYER ( &Entity ) )
    {
        CClientPlayer& Player = static_cast < CClientPlayer& > ( Entity );

        Player.SetNametagOverrideColor ( ucR, ucG, ucB );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetPlayerNametagShowing ( CClientEntity& Entity, bool bShowing )
{
    RUN_CHILDREN SetPlayerNametagShowing ( **iter, bShowing );

    if ( IS_PED ( &Entity ) )
    {
        CClientPlayer& Player = static_cast < CClientPlayer& > ( Entity );

        Player.SetNametagShowing ( bShowing );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedRotation ( CClientEntity& Entity, float fRotation )
{
    RUN_CHILDREN SetPedRotation ( **iter, fRotation );

    if ( IS_PED ( &Entity ) )
    {    
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );

        // Convert the rotation to radians and set the new rotation
        float fRadians = ConvertDegreesToRadians ( fRotation );
        // Clamp it to -PI .. PI
        if ( fRadians < -PI )
        {
            do
            {
                fRadians += PI * 2.0f;
            } while ( fRadians < -PI );
        }
        else if ( fRadians > PI )
        {
            do
            {
                fRadians -= PI * 2.0f;
            } while ( fRadians > PI );
        }

        Ped.SetCurrentRotation ( fRadians );
        if ( !IS_PLAYER ( &Entity ) )
            Ped.SetCameraRotation ( -fRadians );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPedCanBeKnockedOffBike ( CClientEntity& Entity, bool bCanBeKnockedOffBike )
{
    RUN_CHILDREN SetPedCanBeKnockedOffBike ( **iter, bCanBeKnockedOffBike );

    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );

        if ( IS_PLAYER ( &Entity ) && !Ped.IsLocalPlayer() )
            return false;

        Ped.SetCanBeKnockedOffBike ( bCanBeKnockedOffBike );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedAnimation ( CClientEntity& Entity, const char * szBlockName, const char * szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInterruptable, bool bFreezeLastFrame )
{    
    RUN_CHILDREN SetPedAnimation ( **iter, szBlockName, szAnimName, iTime, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame );

    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        if ( szBlockName && szAnimName )
        {
            CAnimBlock * pBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( szBlockName );
            if ( pBlock )
            {
                Ped.RunNamedAnimation ( pBlock, szAnimName, iTime, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame );
                return true;
            }
        }
        else
        {
            Ped.KillAnimation ();
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPedMoveAnim ( CClientEntity& Entity, unsigned int iMoveAnim )
{    
    RUN_CHILDREN SetPedMoveAnim ( **iter, iMoveAnim );

    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        if ( ( iMoveAnim >= 54 && iMoveAnim <= 70 ) ||
             ( iMoveAnim >= 118 && iMoveAnim <= 138 ) )
        {
            Ped.SetMoveAnim ( (eMoveAnim)iMoveAnim );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::AddPedClothes ( CClientEntity& Entity, char* szTexture, char* szModel, unsigned char ucType )
{
    RUN_CHILDREN AddPedClothes ( **iter, szTexture, szModel, ucType );
    // Is he a player?
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.GetClothes()->AddClothes ( szTexture, szModel, ucType, false );
        Ped.RebuildModel ( true );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::RemovePedClothes ( CClientEntity& Entity, unsigned char ucType )
{
    RUN_CHILDREN RemovePedClothes ( **iter, ucType );
    // Is he a player?
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.GetClothes()->RemoveClothes ( ucType, false );
        Ped.RebuildModel ( true );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPedControlState ( CClientEntity & Entity, const char * szControl, bool bState )
{
    RUN_CHILDREN SetPedControlState ( **iter, szControl, bState );
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        if ( Ped.m_Pad.SetControlState ( szControl, bState ) )
        {
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedDoingGangDriveby ( CClientEntity & Entity, bool bGangDriveby )
{
    RUN_CHILDREN SetPedDoingGangDriveby ( **iter, bGangDriveby );
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.SetDoingGangDriveby ( bGangDriveby );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedLookAt ( CClientEntity & Entity, CVector & vecPosition, int iTime, int iBlend, CClientEntity * pTarget )
{
    RUN_CHILDREN SetPedLookAt ( **iter, vecPosition, iTime, iBlend, pTarget );
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.LookAt ( vecPosition, iTime, iBlend, pTarget );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedHeadless ( CClientEntity & Entity, bool bHeadless )
{
    RUN_CHILDREN SetPedHeadless ( **iter, bHeadless );
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.SetHeadless ( bHeadless );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedFrozen ( CClientEntity & Entity, bool bFrozen )
{
    RUN_CHILDREN SetPedFrozen ( **iter, bFrozen );
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.SetFrozen ( bFrozen );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedFootBloodEnabled ( CClientEntity & Entity, bool bHasFootBlood )
{
    RUN_CHILDREN SetPedFootBloodEnabled ( **iter, bHasFootBlood );
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.SetFootBloodEnabled ( bHasFootBlood );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedCameraRotation ( CClientEntity & Entity, float fRotation )
{
    RUN_CHILDREN SetPedCameraRotation ( **iter, fRotation );
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.SetCameraRotation ( ConvertDegreesToRadians ( fRotation ) );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedAimTarget ( CClientEntity & Entity, CVector & vecTarget )
{
    RUN_CHILDREN SetPedAimTarget ( **iter, vecTarget );
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );

        CVector vecOrigin;
        Ped.GetPosition ( vecOrigin );

        // Arm direction
        float fArmX = atan2 ( vecTarget.fX - vecOrigin.fX, vecTarget.fY - vecOrigin.fY ),
              fArmY = -atan2 ( vecTarget.fZ - vecOrigin.fZ, DistanceBetweenPoints2D ( vecTarget, vecOrigin ) );

        // TODO: use gun muzzle for origin
        Ped.SetTargetTarget ( TICK_RATE, vecOrigin, vecTarget );
        Ped.SetAim ( fArmX, fArmY, 0 );

        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedOnFire ( CClientEntity & Entity, bool bOnFire )
{
    if ( IS_PED ( &Entity ) )
    {
        CClientPed& Ped = static_cast < CClientPed& > ( Entity );
        Ped.SetOnFire( bOnFire );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetBodyPartName ( unsigned char ucID, char* szName )
{
    if ( ucID <= 10 )
    {
        // Grab the name and check it's length
        const char* szNamePointer = CClientPed::GetBodyPartName ( ucID );
        if ( strlen ( szNamePointer ) > 0 )
        {
            strncpy ( szName, szNamePointer, 256 );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetClothesByTypeIndex ( unsigned char ucType, unsigned char ucIndex, char* szTextureReturn, char* szModelReturn )
{
    SPlayerClothing* pPlayerClothing = CClientPlayerClothes::GetClothingGroup ( ucType );
    if ( pPlayerClothing )
    {
        if ( ucIndex < CClientPlayerClothes::GetClothingGroupMax ( ucType ) )
        {
            if ( szTextureReturn )
                strcpy ( szTextureReturn, pPlayerClothing [ ucIndex ].szTexture );
            if ( szModelReturn )
                strcpy ( szModelReturn, pPlayerClothing [ ucIndex ].szModel );
            
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetTypeIndexFromClothes ( char* szTexture, char* szModel, unsigned char& ucTypeReturn, unsigned char& ucIndexReturn )
{
    if ( szTexture == NULL && szModel == NULL )
        return false;

    for ( unsigned char ucType = 0 ; ucType < PLAYER_CLOTHING_SLOTS ; ucType++ )
    {
        SPlayerClothing* pPlayerClothing = CClientPlayerClothes::GetClothingGroup ( ucType );
        if ( pPlayerClothing )
        {
            for ( unsigned char ucIter = 0 ; pPlayerClothing [ ucIter ].szTexture != NULL ; ucIter++ )
            {
                if ( ( szTexture == NULL || strcmp ( szTexture, pPlayerClothing [ ucIter ].szTexture ) == 0 ) &&
                     ( szModel == NULL || strcmp ( szModel, pPlayerClothing [ ucIter ].szModel ) == 0 ) )
                {
                    ucTypeReturn = ucType;
                    ucIndexReturn = ucIter;

                    return true;
                }
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetClothesTypeName ( unsigned char ucType, char* szNameReturn )
{
    assert ( szNameReturn );

    char* szName = CClientPlayerClothes::GetClothingName ( ucType );
    if ( szName )
    {
        strcpy ( szNameReturn, szName );

        return true;
    }

    return false;
}


CClientPed* CStaticFunctionDefinitions::CreatePed ( CResource& Resource, unsigned long ulModel, const CVector& vecPosition, float fRotation )
{
    // Valid model?
    if ( CClientPlayerManager::IsValidModel ( ulModel ) )
    {
        // Convert the rotation to radians
        float fRotationRadians = ConvertDegreesToRadians ( fRotation );
        // Clamp it to -PI .. PI
        if ( fRotationRadians < -PI )
        {
            do
            {
                fRotationRadians += PI * 2.0f;
            } while ( fRotationRadians < -PI );
        }
        else if ( fRotationRadians > PI )
        {
            do
            {
                fRotationRadians -= PI * 2.0f;
            } while ( fRotationRadians > PI );
        }

        // Create it
        CClientPed* pPed = new CClientPed ( m_pManager, ulModel, INVALID_ELEMENT_ID );
        pPed->SetParent ( Resource.GetResourceDynamicEntity() );
        pPed->SetPosition ( vecPosition );
        pPed->SetCurrentRotation ( fRotationRadians );
        return pPed;
    }

    // Failed
    return NULL;
}


bool CStaticFunctionDefinitions::GetVehicleModelFromName ( const char* szName, unsigned short& usID )
{
    assert ( szName );

    unsigned long ulID = CVehicleNames::GetVehicleModel ( szName );
    if ( ulID != 0 )
    {
        usID = static_cast < unsigned short > ( ulID );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( unsigned char ucSlot, char* szName, unsigned short len )
{
    strncpy ( szName, CVehicleUpgrades::GetSlotName ( ucSlot ), len );
    if (len)
        szName[len-1] = '\0';
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( unsigned short usUpgrade, char* szName, unsigned short len )
{
    unsigned char ucSlot;
    if ( CVehicleUpgrades::GetSlotFromUpgrade ( usUpgrade, ucSlot ) )
    {
        strncpy ( szName, CVehicleUpgrades::GetSlotName ( ucSlot ), len );
        if (len)
            szName[len-1] = '\0';
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetVehicleNameFromModel ( unsigned short usModel, char* szName, unsigned short len )
{
    assert ( szName );

    strncpy ( szName, CVehicleNames::GetVehicleName ( usModel ), len );
    if (len)
        szName[len-1] = '\0';

    return true;
}


bool CStaticFunctionDefinitions::GetHelicopterRotorSpeed ( CClientVehicle& Vehicle, float& fSpeed )
{
    // It's a heli?
    if ( Vehicle.GetVehicleType() == CLIENTVEHICLE_HELI )
    {
        fSpeed = Vehicle.GetHeliRotorSpeed ();
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::IsVehicleDamageProof ( CClientVehicle& Vehicle, bool& bDamageProof )
{
    bDamageProof = !Vehicle.GetScriptCanBeDamaged ();
    return true;
}


bool CStaticFunctionDefinitions::IsTrainDerailed ( CClientVehicle& Vehicle, bool& bDerailed )
{
    if ( Vehicle.GetVehicleType () != CLIENTVEHICLE_TRAIN )
        return false;

    bDerailed = Vehicle.IsDerailed ();
    return true;
}

bool CStaticFunctionDefinitions::IsTrainDerailable ( CClientVehicle& Vehicle, bool& bIsDerailable )
{
    if ( Vehicle.GetVehicleType () != CLIENTVEHICLE_TRAIN )
        return false;

    bIsDerailable = Vehicle.IsDerailable ();
    return true;
}


bool CStaticFunctionDefinitions::GetTrainDirection ( CClientVehicle& Vehicle, bool& bDirection )
{
    if ( Vehicle.GetVehicleType () != CLIENTVEHICLE_TRAIN )
        return false;

    bDirection = Vehicle.GetTrainDirection ();
    return true;
}


bool CStaticFunctionDefinitions::GetTrainSpeed ( CClientVehicle& Vehicle, float& fSpeed )
{
    if ( Vehicle.GetVehicleType () != CLIENTVEHICLE_TRAIN )
        return false;

    fSpeed = Vehicle.GetTrainSpeed ();
    return true;
}


CClientVehicle* CStaticFunctionDefinitions::CreateVehicle ( CResource& Resource, unsigned short usModel, const CVector& vecPosition, const CVector& vecRotation, const char* szRegPlate )
{
    if ( CClientVehicleManager::IsValidModel ( usModel ) )
    {
        CClientVehicle* pVehicle = new CDeathmatchVehicle ( m_pManager, NULL, INVALID_ELEMENT_ID, usModel );
        if ( pVehicle )
        {
            pVehicle->SetParent ( Resource.GetResourceDynamicEntity() );
            pVehicle->SetPosition ( vecPosition );     

            pVehicle->SetRotationDegrees ( vecRotation );
            if ( szRegPlate )
                pVehicle->SetRegPlate ( szRegPlate );

            return pVehicle;
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::FixVehicle ( CClientEntity& Entity )
{
    RUN_CHILDREN FixVehicle ( **iter );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        // Repair it
        Vehicle.Fix ();
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::BlowVehicle ( CClientEntity& Entity )
{
    RUN_CHILDREN BlowVehicle ( **iter );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        Vehicle.Blow ( true );
        return true;
    }

    return false;
}
bool CStaticFunctionDefinitions::IsVehicleBlown ( CClientVehicle& Vehicle, bool& bBlown )
{
    bBlown = Vehicle.IsVehicleBlown();
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleHeadLightColor ( CClientVehicle& Vehicle, SColor& outColor )
{
    outColor = Vehicle.GetHeadLightColor ();
    return true;
}

bool CStaticFunctionDefinitions::GetVehicleCurrentGear ( CClientVehicle& Vehicle, unsigned short& currentGear )
{
    currentGear = Vehicle.GetCurrentGear();
    return true;
}


bool CStaticFunctionDefinitions::SetVehicleColor ( CClientEntity& Entity, const CVehicleColor& color )
{
    RUN_CHILDREN SetVehicleColor ( **iter, color );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
        Vehicle.SetColor ( color );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleLandingGearDown ( CClientEntity& Entity, bool bLandingGearDown )
{
    RUN_CHILDREN SetVehicleLandingGearDown ( **iter, bLandingGearDown );

    // Is this a vehicle?
    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        // Has landing gear and different than before?
        if ( Vehicle.HasLandingGear () &&
             bLandingGearDown != Vehicle.IsLandingGearDown () )
        {
            // Set the new state
            Vehicle.SetLandingGearDown ( bLandingGearDown );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleLocked ( CClientEntity& Entity, bool bLocked )
{
    RUN_CHILDREN SetVehicleLocked ( **iter, bLocked );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
        
        // Set the new locked state
        Vehicle.SetDoorsLocked ( bLocked );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleDoorsUndamageable ( CClientEntity& Entity, bool bDoorsUndamageable )
{
    RUN_CHILDREN SetVehicleDoorsUndamageable ( **iter, bDoorsUndamageable );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
        if ( bDoorsUndamageable != Vehicle.AreDoorsUndamageable () )
        {
            // Set the new doors undamageable state
            Vehicle.SetDoorsUndamageable ( bDoorsUndamageable );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleRotation ( CClientEntity& Entity, const CVector& vecRotation )
{
    RUN_CHILDREN SetVehicleRotation ( **iter, vecRotation );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        Vehicle.SetRotationDegrees ( vecRotation );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetVehicleSirensOn ( CClientEntity& Entity, bool bSirensOn )
{
    RUN_CHILDREN SetVehicleSirensOn ( **iter, bSirensOn );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        // Has Sirens and different state than before?
        if ( CClientVehicleManager::HasSirens ( Vehicle.GetModel () ) &&
             bSirensOn != Vehicle.IsSirenOrAlarmActive () )
        {
            // Set the new state
            Vehicle.SetSirenOrAlarmActive ( bSirensOn );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleTurnVelocity ( CClientEntity& Entity, const CVector& vecTurnVelocity )
{
    RUN_CHILDREN SetVehicleTurnVelocity ( **iter, vecTurnVelocity );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
        Vehicle.SetTurnSpeed ( vecTurnVelocity );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::AddVehicleUpgrade ( CClientEntity& Entity, unsigned short usUpgrade )
{
    RUN_CHILDREN AddVehicleUpgrade ( **iter, usUpgrade );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        CVehicleUpgrades* pUpgrades = Vehicle.GetUpgrades ();
        if ( pUpgrades )
        {
            if ( pUpgrades->IsUpgradeCompatible ( usUpgrade ) )
            {
                // Add to our upgrade list
                pUpgrades->AddUpgrade ( usUpgrade );
                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::AddAllVehicleUpgrades ( CClientEntity& Entity )
{
    RUN_CHILDREN AddAllVehicleUpgrades ( **iter );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        CVehicleUpgrades* pUpgrades = Vehicle.GetUpgrades ();
        if ( pUpgrades )
        {
            unsigned short* usUpgrades = pUpgrades->GetSlotStates ();

            pUpgrades->AddAllUpgrades ();
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::RemoveVehicleUpgrade ( CClientEntity& Entity, unsigned short usUpgrade )
{
    RUN_CHILDREN RemoveVehicleUpgrade ( **iter, usUpgrade );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        CVehicleUpgrades* pUpgrades = Vehicle.GetUpgrades ();
        if ( pUpgrades )
        {
            if ( pUpgrades->HasUpgrade ( usUpgrade ) )
            {
                unsigned char ucSlot;
                if ( pUpgrades->GetSlotFromUpgrade ( usUpgrade, ucSlot ) )
                {
                    pUpgrades->RemoveUpgrade ( usUpgrade );

                    return true;
                }
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleDoorState ( CClientEntity& Entity, unsigned char ucDoor, unsigned char ucState )
{
    RUN_CHILDREN SetVehicleDoorState ( **iter, ucDoor, ucState );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        if ( ucDoor < MAX_DOORS )
        {
            switch ( Vehicle.GetModel () )
            {
                case VT_BFINJECT:
                case VT_RCBANDIT:
                case VT_CADDY:
                case VT_RCRAIDER:
                case VT_BAGGAGE:
                case VT_DOZER:
                case VT_FORKLIFT:
                case VT_TRACTOR:
                case VT_RCTIGER:
                case VT_BANDITO:
                case VT_KART:
                case VT_MOWER:
                case VT_RCCAM:
                case VT_RCGOBLIN:
                    return false;
                    break;
                default: break;
            }

            Vehicle.SetDoorStatus ( ucDoor, ucState );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleWheelStates ( CClientEntity& Entity, int iFrontLeft, int iRearLeft, int iFrontRight, int iRearRight )
{
    RUN_CHILDREN SetVehicleWheelStates ( **iter, iFrontLeft, iRearLeft, iFrontRight, iRearRight );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        // Store the current and new states
        unsigned char ucFrontLeft, ucNewFrontLeft = ucFrontLeft = Vehicle.GetWheelStatus ( FRONT_LEFT_WHEEL );
        unsigned char ucRearLeft, ucNewRearLeft = ucRearLeft = Vehicle.GetWheelStatus ( REAR_LEFT_WHEEL );
        unsigned char ucFrontRight, ucNewFrontRight = ucFrontRight = Vehicle.GetWheelStatus ( FRONT_RIGHT_WHEEL );
        unsigned char ucRearRight, ucNewRearRight = ucRearRight = Vehicle.GetWheelStatus ( REAR_RIGHT_WHEEL );

        // If we have a new state for them, change it
        if ( iFrontLeft >= 0 && iFrontLeft <= DT_WHEEL_INTACT_COLLISIONLESS )
            ucNewFrontLeft = static_cast < unsigned char > ( iFrontLeft );
        if ( iRearLeft >= 0 && iRearLeft <= DT_WHEEL_INTACT_COLLISIONLESS )
            ucNewRearLeft = static_cast < unsigned char > ( iRearLeft );
        if ( iFrontRight >= 0 && iFrontRight <= DT_WHEEL_INTACT_COLLISIONLESS )
            ucNewFrontRight = static_cast < unsigned char > ( iFrontRight );
        if ( iRearRight >= 0 && iRearRight <= DT_WHEEL_INTACT_COLLISIONLESS )
            ucNewRearRight = static_cast < unsigned char > ( iRearRight );

        // If atleast 1 wheel state is different
        if ( ucNewFrontLeft != ucFrontLeft ||
             ucNewRearLeft != ucRearLeft ||
             ucNewFrontRight != ucFrontRight ||
             ucNewRearRight != ucRearRight )
        {
            Vehicle.SetWheelStatus ( FRONT_LEFT_WHEEL, ucNewFrontLeft, false );
            Vehicle.SetWheelStatus ( REAR_LEFT_WHEEL, ucNewRearLeft, false );
            Vehicle.SetWheelStatus ( FRONT_RIGHT_WHEEL, ucNewFrontRight, false );
            Vehicle.SetWheelStatus ( REAR_RIGHT_WHEEL, ucNewRearRight, false );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleLightState ( CClientEntity& Entity, unsigned char ucLight, unsigned char ucState )
{
    RUN_CHILDREN SetVehicleLightState ( **iter, ucLight, ucState );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        if ( ucLight < 4 )
        {       
            Vehicle.SetLightStatus ( ucLight, ucState );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehiclePanelState ( CClientEntity& Entity, unsigned char ucPanel, unsigned char ucState )
{
    RUN_CHILDREN SetVehiclePanelState ( **iter, ucPanel, ucState );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        if ( ucPanel < 7 )
        {
            Vehicle.SetPanelStatus ( ucPanel, ucState );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleOverrideLights ( CClientEntity& Entity, unsigned char ucLights )
{
    RUN_CHILDREN SetVehicleOverrideLights ( **iter, ucLights );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        if ( Vehicle.GetOverrideLights () != ucLights )
        {
            Vehicle.SetOverrideLights ( ucLights );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::AttachTrailerToVehicle ( CClientVehicle& Vehicle, CClientVehicle& Trailer )
{    
    // Are they both free to be attached?
    if ( !Vehicle.GetTowedVehicle () && !Trailer.GetTowedByVehicle () )
    {
        // Attach them
        return Vehicle.SetTowedVehicle ( &Trailer );
    }

    return false;
}


bool CStaticFunctionDefinitions::DetachTrailerFromVehicle ( CClientVehicle& Vehicle, CClientVehicle* pTrailer )
{
    // Is there a trailer attached, and does it match this one
    CClientVehicle* pTempTrailer = Vehicle.GetTowedVehicle ();
    if ( pTempTrailer && ( !pTrailer || pTempTrailer == pTrailer ) )
    {
        // Detach them
        Vehicle.SetTowedVehicle ( NULL );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleEngineState ( CClientEntity& Entity, bool bState )
{
    RUN_CHILDREN SetVehicleEngineState ( **iter, bState );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        Vehicle.SetEngineOn ( bState );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleDirtLevel ( CClientEntity& Entity, float fDirtLevel )
{
    RUN_CHILDREN SetVehicleDirtLevel ( **iter, fDirtLevel );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        Vehicle.SetDirtLevel ( fDirtLevel );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleDamageProof ( CClientEntity& Entity, bool bDamageProof )
{
    RUN_CHILDREN SetVehicleDamageProof ( **iter, bDamageProof );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        Vehicle.SetScriptCanBeDamaged ( !bDamageProof );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehiclePaintjob ( CClientEntity& Entity, unsigned char ucPaintjob )
{
    RUN_CHILDREN SetVehiclePaintjob ( **iter, ucPaintjob );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        if ( ucPaintjob != Vehicle.GetPaintjob () && ucPaintjob <= 3 )
        {
            Vehicle.SetPaintjob ( ucPaintjob );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleFuelTankExplodable ( CClientEntity& Entity, bool bExplodable )
{
    RUN_CHILDREN SetVehicleFuelTankExplodable ( **iter, bExplodable );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        Vehicle.SetCanShootPetrolTank ( bExplodable );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleFrozen ( CClientEntity& Entity, bool bFrozen )
{
    RUN_CHILDREN SetVehicleFrozen ( **iter, bFrozen );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        Vehicle.SetScriptFrozen ( bFrozen );
        Vehicle.SetFrozen ( bFrozen );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleAdjustableProperty ( CClientEntity& Entity, unsigned short usAdjustableProperty )
{
    RUN_CHILDREN SetVehicleAdjustableProperty ( **iter, usAdjustableProperty );

    if ( IS_VEHICLE ( &Entity ) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );

        Vehicle.SetAdjustablePropertyValue ( usAdjustableProperty );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetHelicopterRotorSpeed ( CClientVehicle& Vehicle, float fSpeed )
{
    // It's a heli?
    if ( Vehicle.GetVehicleType() == CLIENTVEHICLE_HELI )
    {
        Vehicle.SetHeliRotorSpeed ( fSpeed );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetTrainDerailed ( CClientVehicle& Vehicle, bool bDerailed )
{
    if ( Vehicle.GetVehicleType () != CLIENTVEHICLE_TRAIN )
        return false;

    Vehicle.SetDerailed ( bDerailed );
    return true;
}


bool CStaticFunctionDefinitions::SetTrainDerailable ( CClientVehicle& Vehicle, bool bDerailable )
{
    if ( Vehicle.GetVehicleType () != CLIENTVEHICLE_TRAIN )
        return false;

    Vehicle.SetDerailable ( bDerailable );
    return true;
}


bool CStaticFunctionDefinitions::SetTrainDirection ( CClientVehicle& Vehicle, bool bDirection )
{
    if ( Vehicle.GetVehicleType () != CLIENTVEHICLE_TRAIN )
        return false;

    Vehicle.SetTrainDirection ( bDirection );
    return true;
}


bool CStaticFunctionDefinitions::SetTrainSpeed ( CClientVehicle& Vehicle, float fSpeed )
{
    if ( Vehicle.GetVehicleType () != CLIENTVEHICLE_TRAIN )
        return false;

    Vehicle.SetTrainSpeed ( fSpeed );
    return true;
}


bool CStaticFunctionDefinitions::SetVehicleHeadLightColor ( CClientEntity& Entity, const SColor color )
{
    RUN_CHILDREN SetVehicleHeadLightColor ( **iter, color );

    if ( IS_VEHICLE(&Entity) )
    {
        CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
        Vehicle.SetHeadLightColor ( color );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetVehicleEngineState ( CClientVehicle & Vehicle, bool & bState )
{
    bState = Vehicle.IsEngineOn ();
    return true;
}

bool CStaticFunctionDefinitions::SetVehicleDoorOpenRatio ( CClientEntity& Entity, unsigned char ucDoor, float fRatio, unsigned long ulTime )
{
    if ( ucDoor <= 5 )
    {
        RUN_CHILDREN SetVehicleDoorOpenRatio ( **iter, ucDoor, fRatio, ulTime );

        if ( IS_VEHICLE(&Entity) )
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            Vehicle.SetDoorOpenRatio ( ucDoor, fRatio, ulTime, true );
            return true;
        }
    }

    return false;
}

bool CStaticFunctionDefinitions::SetElementCollisionsEnabled ( CClientEntity& Entity, bool bEnabled )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            Vehicle.SetCollisionEnabled ( bEnabled );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            Object.SetCollisionEnabled ( bEnabled );
            break;
        }
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            Ped.SetUsesCollision ( bEnabled );
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetElementCollidableWith ( CClientEntity & Entity, CClientEntity & ThisEntity, bool bCanCollide )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPLAYER:
        case CCLIENTPED:
        case CCLIENTOBJECT:
        case CCLIENTVEHICLE:
        {
            switch ( ThisEntity.GetType () )
            {
                case CCLIENTPLAYER:
                case CCLIENTPED:
                case CCLIENTOBJECT:
                case CCLIENTVEHICLE:
                {    
                    Entity.SetCollidableWith ( &ThisEntity, bCanCollide );
                    return true;
                }
                default: break;
            }
            break;
        }
        default: break;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetElementFrozen ( CClientEntity& Entity, bool bFrozen )
{
    switch ( Entity.GetType () )
    {
        case CCLIENTPLAYER:
        case CCLIENTPED:
        {
            CClientPed& Ped = static_cast < CClientPed& > ( Entity );
            Ped.SetFrozen ( bFrozen );
            break;
        }
        case CCLIENTVEHICLE:
        {
            CClientVehicle& Vehicle = static_cast < CClientVehicle& > ( Entity );
            Vehicle.SetFrozen ( bFrozen );
            break;
        }
        case CCLIENTOBJECT:
        {
            CClientObject& Object = static_cast < CClientObject& > ( Entity );
            Object.SetStatic ( bFrozen );
            break;
        }
        default: return false;
    }

    return true;
}


CClientObject* CStaticFunctionDefinitions::CreateObject ( CResource& Resource, unsigned short usModelID, const CVector& vecPosition, const CVector& vecRotation )
{
    if ( CClientObjectManager::IsValidModel ( usModelID ) )
    {
        CClientObject* pObject = new CDeathmatchObject ( m_pManager, m_pMovingObjectsManager, m_pClientGame->GetObjectSync (), INVALID_ELEMENT_ID, usModelID );
        if ( pObject )
        {
            pObject->SetParent ( Resource.GetResourceDynamicEntity () );
            pObject->SetPosition ( vecPosition );
            pObject->SetRotationDegrees ( vecRotation );

            return pObject;
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::IsObjectStatic ( CClientObject & Object, bool & bStatic )
{
    bStatic = Object.IsStatic ();
    return true;
}


bool CStaticFunctionDefinitions::GetObjectScale ( CClientObject & Object, float& fScale )
{
    fScale = Object.GetScale ();
    return true;
}


bool CStaticFunctionDefinitions::SetObjectRotation ( CClientEntity& Entity, const CVector& vecRotation )
{
    RUN_CHILDREN SetObjectRotation ( **iter, vecRotation );

    if ( IS_OBJECT ( &Entity ) )
    {
        CDeathmatchObject& Object = static_cast < CDeathmatchObject& > ( Entity );

        Object.SetRotationDegrees ( vecRotation );

        // Kayl: removed setTargetRotation, if we want consistency target WAS a delta not an absolute value and serverside doesn't allow rotation
        // change while moving so in theory it shouldn't have been used
    }

    return true;
}


bool CStaticFunctionDefinitions::MoveObject ( CClientEntity& Entity, unsigned long ulTime, const CVector& vecPosition, const CVector& vecDeltaRotation, const char* a_szEasingType, double a_fEasingPeriod, double a_fEasingAmplitude, double a_fEasingOvershoot )
{
    RUN_CHILDREN MoveObject ( **iter, ulTime, vecPosition, vecDeltaRotation, a_szEasingType, a_fEasingPeriod, a_fEasingAmplitude, a_fEasingOvershoot );

    if ( IS_OBJECT ( &Entity ) )
    {
        CDeathmatchObject& Object = static_cast < CDeathmatchObject& > ( Entity );

        // Grab the source position and rotation
        CVector vecSourcePosition, vecSourceRotation;
        Object.GetPosition ( vecSourcePosition );
        Object.GetRotationRadians ( vecSourceRotation );

        // Convert the target rotation given to radians (don't wrap around as these can be rotated more than 360)
        CVector vecDeltaRadians = vecDeltaRotation;
        ConvertDegreesToRadiansNoWrap ( vecDeltaRadians );

        CEasingCurve::eType easingType = CEasingCurve::GetEasingTypeFromString ( a_szEasingType );
        if (easingType == CEasingCurve::EASING_INVALID )
        {
            return false;
        }

        CPositionRotationAnimation animation;
        animation.SetSourceValue ( SPositionRotation ( vecSourcePosition, vecSourceRotation ) );
        animation.SetTargetValue ( SPositionRotation ( vecPosition, vecDeltaRadians ), true );
        animation.SetEasing ( easingType, a_fEasingPeriod, a_fEasingAmplitude, a_fEasingOvershoot );
        animation.SetDuration ( ulTime );

        Object.StartMovement ( animation );
    }

    return true;
}


bool CStaticFunctionDefinitions::StopObject ( CClientEntity& Entity )
{
    RUN_CHILDREN StopObject ( **iter );

    if ( IS_OBJECT ( &Entity ) )
    {
        CDeathmatchObject& Object = static_cast < CDeathmatchObject& > ( Entity );

        // Stop the movement
        Object.StopMovement ();
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetObjectScale ( CClientEntity& Entity, float fScale )
{
    RUN_CHILDREN SetObjectScale ( **iter, fScale );

    if ( IS_OBJECT ( &Entity ) )
    {
        CDeathmatchObject& Object = static_cast < CDeathmatchObject& > ( Entity );
        Object.SetScale ( fScale );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetObjectStatic ( CClientEntity& Entity, bool bStatic )
{
    RUN_CHILDREN SetObjectStatic ( **iter, bStatic );

    if ( IS_OBJECT ( &Entity ) )
    {
        CDeathmatchObject& Object = static_cast < CDeathmatchObject& > ( Entity );
        Object.SetStatic ( bStatic );
        return true;
    }

    return false;
}


CClientRadarArea* CStaticFunctionDefinitions::CreateRadarArea ( CResource& Resource, const CVector2D& vecPosition2D, const CVector2D& vecSize, const SColor color )
{
    // Create it
    CClientRadarArea* pRadarArea = new CClientRadarArea ( m_pManager, INVALID_ELEMENT_ID );
    if ( pRadarArea )
    {
        pRadarArea->SetParent ( Resource.GetResourceDynamicEntity () );
        pRadarArea->SetPosition ( vecPosition2D );
        pRadarArea->SetSize ( vecSize );        
        pRadarArea->SetColor ( color );

        return pRadarArea;
    }

    return NULL;
}


bool CStaticFunctionDefinitions::GetRadarAreaColor ( CClientRadarArea* RadarArea, SColor& outColor )
{
    if ( RadarArea )
    {
        outColor = RadarArea->GetColor ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetRadarAreaSize ( CClientRadarArea* RadarArea, CVector2D& vecSize )
{
    if ( RadarArea )
    {
        vecSize = RadarArea->GetSize();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::IsRadarAreaFlashing ( CClientRadarArea* RadarArea, bool &flashing )
{
    if ( RadarArea )
    {
        flashing = RadarArea->IsFlashing();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetRadarAreaColor ( CClientRadarArea* RadarArea, const SColor color )
{
    if ( RadarArea )
    {
        RadarArea->SetColor ( color );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetRadarAreaSize ( CClientRadarArea* RadarArea, CVector2D vecSize )
{
    if ( RadarArea )
    {
        RadarArea->SetSize ( vecSize );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetRadarAreaFlashing ( CClientRadarArea* RadarArea, bool flashing )
{
    if ( RadarArea )
    {
        RadarArea->SetFlashing ( flashing );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::IsInsideRadarArea ( CClientRadarArea* RadarArea, CVector2D vecPosition, bool& inside )
{
    if ( RadarArea )
    {
        CVector2D vecRadarPos = RadarArea->GetPosition();
        CVector2D vecRadarSize = RadarArea->GetSize();
        float fMaxX = vecRadarPos.fX + vecRadarSize.fX;
        float fMaxY = vecRadarPos.fY + vecRadarSize.fY;
        if ( vecPosition.fX >= vecRadarPos.fX && vecPosition.fX <= fMaxX )
        {
            if ( vecPosition.fY >= vecRadarPos.fY && vecPosition.fY <= fMaxY )
            {
                inside = true;
            }
        }
        return true;
    }
    return false;
}


CClientPickup* CStaticFunctionDefinitions::CreatePickup ( CResource& Resource, const CVector& vecPosition, unsigned char ucType, double dFive, unsigned long ulRespawnInterval, double dSix )
{
    CClientPickup* pPickup = NULL;
    unsigned short usModel = 0;
    switch ( ucType )
    {
        case CClientPickup::ARMOR:
        case CClientPickup::HEALTH:
        {
            if ( ucType == CClientPickup::ARMOR )
                usModel = CClientPickupManager::GetArmorModel ();
            else
                usModel = CClientPickupManager::GetHealthModel ();
        
            if ( dFive >= 0 && dFive <= 100 )
            {
                pPickup = new CClientPickup ( m_pManager, INVALID_ELEMENT_ID, usModel, vecPosition );   
                pPickup->m_fAmount = static_cast < float > ( dFive );
            }
            break;
        }
        case CClientPickup::WEAPON:
        {
            // Get the weapon id
            unsigned char ucWeaponID = static_cast < unsigned char > ( dFive );
            if ( CClientPickupManager::IsValidWeaponID ( ucWeaponID ) )
            {
                usModel = CClientPickupManager::GetWeaponModel ( ucWeaponID );
                // Limit ammo to 9999
                unsigned short usAmmo = static_cast < unsigned short > ( dSix );
                if ( dSix > 9999 )
                {
                    usAmmo = 9999;
                }

                pPickup = new CClientPickup ( m_pManager, INVALID_ELEMENT_ID, usModel, vecPosition );
                pPickup->m_ucWeaponType = ucWeaponID;
                pPickup->m_usAmmo = usAmmo;
            }
            break;
        }
        case CClientPickup::CUSTOM:
        {
            // Get the model id
            usModel = static_cast < unsigned short > ( dFive );
            if ( CClientObjectManager::IsValidModel ( usModel ) )
            {
                pPickup = new CClientPickup ( m_pManager, INVALID_ELEMENT_ID, usModel, vecPosition );
            }
            break;
        }
        default: break;
    }

    if ( pPickup )
    {
        pPickup->SetParent ( Resource.GetResourceDynamicEntity () );
        pPickup->m_ucType = ucType;
    }

    return pPickup;
}


bool CStaticFunctionDefinitions::SetPickupType ( CClientEntity& Entity, unsigned char ucType, double dThree, double dFour )
{
    RUN_CHILDREN SetPickupType ( **iter, ucType, dThree, dFour );

    if ( IS_PICKUP ( &Entity ) )
    {
        CClientPickup& Pickup = static_cast < CClientPickup& > ( Entity );

        if ( ucType == CClientPickup::ARMOR || ucType == CClientPickup::HEALTH )
        {
            // Is the third argument (health) a number between 0 and 100?
            if ( dThree >= 0 && dThree <= 100 )
            {
                if ( ucType == CClientPickup::ARMOR )
                    Pickup.SetModel ( CClientPickupManager::GetArmorModel () );
                else
                    Pickup.SetModel ( CClientPickupManager::GetHealthModel () );
                Pickup.m_ucType = ucType;
                Pickup.m_fAmount = static_cast < float > ( dThree );

                return true;
            }
        }
        else if ( ucType == CClientPickup::WEAPON )
        {
            // Get the weapon id
            unsigned char ucWeaponID = static_cast < unsigned char > ( dThree );
            if ( CClientPickupManager::IsValidWeaponID ( ucWeaponID ) )
            {
                // Limit ammo to 9999
                unsigned short usAmmo = static_cast < unsigned short > ( dFour );
                if ( dFour > 9999 )
                {
                    usAmmo = 9999;
                }

                Pickup.SetModel ( CClientPickupManager::GetWeaponModel ( ucWeaponID ) );
                Pickup.m_ucType = ucType;
                Pickup.m_ucWeaponType = ucWeaponID;
                Pickup.m_usAmmo = usAmmo;

                return true;
            }
        }
        else if ( ucType == CClientPickup::CUSTOM )
        {
            // Get the weapon id
            unsigned short usModel = static_cast < unsigned short > ( dThree );
            if ( CClientObjectManager::IsValidModel ( usModel ) )
            {
                Pickup.SetModel ( usModel );
                Pickup.m_ucType = ucType;
                Pickup.SetModel ( usModel );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::CreateExplosion ( CVector& vecPosition, unsigned char ucType, bool bMakeSound, float fCamShake, bool bDamaging )
{
    // TODO: add creators for kill credit
    eExplosionType explosionType = ( eExplosionType ) ucType;      
    m_pExplosionManager->Create ( explosionType, vecPosition, NULL, bMakeSound, fCamShake, !bDamaging );
        
    return true;
}

bool CStaticFunctionDefinitions::CreateFire ( CVector& vecPosition, float fSize )
{
    return g_pGame->GetFireManager ()->StartFire ( vecPosition, fSize ) != NULL;
}

bool CStaticFunctionDefinitions::PlayMissionAudio ( const CVector& vecPosition, unsigned short usSlot )
{
    // TODO: Position of the sound

    // Play the sound if it's loaded
    if ( g_pGame->GetAudio ()->GetMissionAudioLoadingStatus ( usSlot ) == 1 )
    {
        g_pGame->GetAudio ()->PlayLoadedMissionAudio ( usSlot );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::PlaySoundFrontEnd ( unsigned char ucSound )
{
    g_pGame->GetAudio ()->PlayFrontEndSound ( ucSound );
    return true;
}


bool CStaticFunctionDefinitions::PreloadMissionAudio ( unsigned short usSound, unsigned short usSlot )
{
    g_pCore->ChatPrintf ( "Preload %u into slot %u", false, usSound, usSlot );
    g_pGame->GetAudio ()->PreloadMissionAudio ( usSound, usSlot );
    return true;
}


CClientRadarMarker* CStaticFunctionDefinitions::CreateBlip ( CResource& Resource, const CVector& vecPosition, unsigned char ucIcon, unsigned char ucSize, const SColor color, short sOrdering, unsigned short usVisibleDistance )
{
    CClientRadarMarker* pBlip = new CClientRadarMarker ( m_pManager, INVALID_ELEMENT_ID, sOrdering, usVisibleDistance );
    if ( pBlip )
    {
        pBlip->SetParent ( Resource.GetResourceDynamicEntity () );
        pBlip->SetPosition ( vecPosition );
        pBlip->SetSprite ( ucIcon );
        pBlip->SetScale ( ucSize );
        pBlip->SetColor ( color );
    }
    return pBlip;
}


CClientRadarMarker* CStaticFunctionDefinitions::CreateBlipAttachedTo ( CResource& Resource, CClientEntity& Entity, unsigned char ucIcon, unsigned char ucSize, const SColor color, short sOrdering, unsigned short usVisibleDistance )
{
    CClientRadarMarker* pBlip = new CClientRadarMarker ( m_pManager, INVALID_ELEMENT_ID, sOrdering, usVisibleDistance );
    if ( pBlip )
    {
        pBlip->SetParent ( Resource.GetResourceDynamicEntity () );
        pBlip->AttachTo ( &Entity );
        pBlip->SetSprite ( ucIcon );
        pBlip->SetScale ( ucSize );
        pBlip->SetColor ( color );
    }
    return pBlip;
}


bool CStaticFunctionDefinitions::SetBlipIcon ( CClientEntity& Entity, unsigned char ucIcon )
{
    RUN_CHILDREN SetBlipIcon ( **iter, ucIcon );

    if ( IS_RADARMARKER ( &Entity ) )
    {
        CClientRadarMarker& Marker = static_cast < CClientRadarMarker& > ( Entity );

        Marker.SetSprite ( ucIcon );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetBlipSize ( CClientEntity& Entity, unsigned char ucSize )
{
    RUN_CHILDREN SetBlipSize ( **iter, ucSize );

    if ( IS_RADARMARKER ( &Entity ) )
    {
        CClientRadarMarker& Marker = static_cast < CClientRadarMarker& > ( Entity );

        Marker.SetScale ( ucSize );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetBlipColor ( CClientEntity& Entity, const SColor color )
{
    RUN_CHILDREN SetBlipColor ( **iter, color );

    if ( IS_RADARMARKER ( &Entity ) )
    {
        CClientRadarMarker& Marker = static_cast < CClientRadarMarker& > ( Entity );

        Marker.SetColor ( color );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetBlipOrdering ( CClientEntity& Entity, short sOrdering )
{
    RUN_CHILDREN SetBlipOrdering ( **iter, sOrdering );

    if ( IS_RADARMARKER ( &Entity ) )
    {
        CClientRadarMarker& Marker = static_cast < CClientRadarMarker& > ( Entity );

        Marker.SetOrdering ( sOrdering );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetBlipVisibleDistance ( CClientEntity& Entity, unsigned short usVisibleDistance )
{
    RUN_CHILDREN SetBlipVisibleDistance ( **iter, usVisibleDistance );

    if ( IS_RADARMARKER ( &Entity ) )
    {
        CClientRadarMarker& Marker = static_cast < CClientRadarMarker& > ( Entity );

        Marker.SetVisibleDistance ( usVisibleDistance );
        return true;
    }

    return false;
}


CClientMarker* CStaticFunctionDefinitions::CreateMarker ( CResource& Resource, const CVector& vecPosition, const char* szType, float fSize, const SColor color )
{
    assert ( szType );

    // Grab the type id
    unsigned char ucType = CClientMarker::StringToType ( szType );
    if ( ucType != CClientMarker::MARKER_INVALID )
    {
        // Create the marker
        CClientMarker* pMarker = new CClientMarker ( m_pManager, INVALID_ELEMENT_ID, ucType );

        // Set its parent and its properties
        pMarker->SetParent ( Resource.GetResourceDynamicEntity() );
        pMarker->SetPosition ( vecPosition );
        pMarker->SetColor ( color );
        pMarker->SetSize ( fSize );

        // Return it
        return pMarker;
    }

    // Bad type id
    return NULL;
}


bool CStaticFunctionDefinitions::GetMarkerTarget ( CClientMarker& Marker, CVector& vecTarget )
{
    CClientCheckpoint* pCheckpoint = Marker.GetCheckpoint ();
    if ( pCheckpoint )
    {
        if ( pCheckpoint->HasTarget () )
        {
            pCheckpoint->GetTarget ( vecTarget );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetMarkerType ( CClientEntity& Entity, const char* szType )
{
    assert ( szType );
    RUN_CHILDREN SetMarkerType ( **iter, szType );

    // Grab the new type ID
    unsigned char ucType = CClientMarker::StringToType ( szType );
    if ( ucType != CClientMarker::MARKER_INVALID )
    {
        // Is this a marker?
        if ( IS_MARKER ( &Entity ) )
        {
            // Set the new type
            CClientMarker& Marker = static_cast < CClientMarker& > ( Entity );
            Marker.SetMarkerType ( static_cast < CClientMarker::eMarkerType > ( ucType ) );
        }

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetMarkerSize ( CClientEntity& Entity, float fSize )
{
    RUN_CHILDREN SetMarkerSize ( **iter, fSize );

    // Is this a marker?
    if ( IS_MARKER ( &Entity ) )
    {
        // Set the new size
        CClientMarker& Marker = static_cast < CClientMarker& > ( Entity );
        Marker.SetSize ( fSize );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetMarkerColor ( CClientEntity& Entity, const SColor color )
{
    RUN_CHILDREN SetMarkerColor ( **iter, color );

    // Is this a marker?
    if ( IS_MARKER ( &Entity ) )
    {
        // Set the new color
        CClientMarker& Marker = static_cast < CClientMarker& > ( Entity );
        Marker.SetColor ( color );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetMarkerTarget ( CClientEntity& Entity, const CVector* pTarget )
{
    RUN_CHILDREN SetMarkerTarget ( **iter, pTarget );

    // Is this a marker?
    if ( IS_MARKER ( &Entity ) )
    {
        CClientMarker& Marker = static_cast < CClientMarker& > ( Entity );
        CClientCheckpoint* pCheckpoint = Marker.GetCheckpoint ();
        if ( pCheckpoint )
        {
            if ( pTarget )
            {
                pCheckpoint->SetNextPosition ( *pTarget );
                pCheckpoint->SetIcon ( CClientCheckpoint::ICON_ARROW );
                pCheckpoint->SetHasTarget ( true );
                pCheckpoint->SetTarget ( *pTarget );
            }
            else
            {
                pCheckpoint->SetIcon ( CClientCheckpoint::ICON_NONE );
                pCheckpoint->SetDirection ( CVector ( 1.0f, 0.0f, 0.0f ) );
                pCheckpoint->SetHasTarget ( false );
            }
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetMarkerIcon ( CClientEntity& Entity, const char* szIcon )
{
    assert ( szIcon );
    RUN_CHILDREN SetMarkerIcon ( **iter, szIcon );
    
    unsigned char ucIcon = CClientCheckpoint::StringToIcon ( szIcon );
    if ( ucIcon != CClientCheckpoint::ICON_INVALID )
    {
        // Is this a marker?
        if ( IS_MARKER ( &Entity ) )
        {
            // Grab the checkpoint
            CClientMarker& Marker = static_cast < CClientMarker& > ( Entity );
            CClientCheckpoint* pCheckpoint = Marker.GetCheckpoint ();
            if ( pCheckpoint )
            {
                pCheckpoint->SetIcon ( static_cast < unsigned int > ( ucIcon ) );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetCameraMatrix ( CVector& vecPosition, CVector& vecLookAt, float& fRoll, float& fFOV )
{
    m_pCamera->GetPosition ( vecPosition );
    m_pCamera->GetTarget ( vecLookAt );
    fRoll = m_pCamera->GetRoll ();
    fFOV = m_pCamera->GetFOV ();
    return true;
}


CClientEntity * CStaticFunctionDefinitions::GetCameraTarget ( void )
{
    if ( !m_pCamera->IsInFixedMode() )
        return m_pCamera->GetTargetEntity ();
    return NULL;
}


bool CStaticFunctionDefinitions::GetCameraInterior ( unsigned char & ucInterior )
{
    ucInterior = static_cast < unsigned char > ( g_pGame->GetWorld ()->GetCurrentArea () );
    return true;
}


bool CStaticFunctionDefinitions::SetCameraMatrix ( CVector& vecPosition, CVector* pvecLookAt, float fRoll, float fFOV )
{
    if ( !m_pCamera->IsInFixedMode () )        
    {
        m_pCamera->ToggleCameraFixedMode ( true );
    }

    // Put the camera there
    m_pCamera->SetPosition ( vecPosition );
    if ( pvecLookAt )
        m_pCamera->SetTarget ( *pvecLookAt );
    m_pCamera->SetRoll ( fRoll );
    m_pCamera->SetFOV ( fFOV );
    return true;
}


bool CStaticFunctionDefinitions::SetCameraTarget ( CClientEntity * pEntity )
{
    assert ( pEntity );

    // Save our current target for later
    CClientEntity * pPreviousTarget = m_pCamera->GetTargetEntity ();

    switch ( pEntity->GetType () )
    {
        case CCLIENTPLAYER:
        {
            CClientPlayer* pPlayer = static_cast < CClientPlayer* > ( pEntity );            
            if ( pPlayer->IsLocalPlayer () )
            {
                // Return the focus to the local player
                m_pCamera->SetFocusToLocalPlayer ();
            }
            else
            {
                // TODO: stream in the player here (needs to be done through the streamer)

                // Put the focus on that player
                m_pCamera->SetFocus ( pPlayer, MODE_BEHINDCAR, false );
            }
            break;
        }
        default:
            return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetCameraInterior ( unsigned char ucInterior )
{
    g_pGame->GetWorld ()->SetCurrentArea ( ucInterior );
    return true;
}


bool CStaticFunctionDefinitions::FadeCamera ( bool bFadeIn, float fFadeTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    CClientCamera* pCamera = m_pManager->GetCamera ();
    g_pClientGame->SetInitiallyFadedOut ( false );

    if ( bFadeIn )
    {
        pCamera->FadeIn ( fFadeTime );
        g_pGame->GetHud ()->DisableAreaName ( g_pClientGame->GetHudAreaNameDisabled () );
    }
    else
    {
        pCamera->FadeOut ( fFadeTime, ucRed, ucGreen, ucBlue );
        g_pGame->GetHud ()->DisableAreaName ( true );
    }

    return true;
}

bool CStaticFunctionDefinitions::SetCameraView ( unsigned short ucMode )
{
    m_pCamera->SetCameraView ( (eVehicleCamMode) ucMode );
    return true;
}

bool CStaticFunctionDefinitions::GetCameraView ( unsigned short& ucMode )
{
    ucMode = m_pCamera->GetCameraView();
    return true;
}

bool CStaticFunctionDefinitions::GetCursorPosition ( CVector2D& vecCursor, CVector& vecWorld )
{
    if ( m_pClientGame->AreCursorEventsEnabled () )
    {
        tagPOINT point;
        GetCursorPos ( &point );
        
        HWND hookedWindow = g_pCore->GetHookedWindow ();

        tagPOINT windowPos = { 0 };
        ClientToScreen( hookedWindow, &windowPos );

        CVector2D vecResolution = g_pCore->GetGUI ()->GetResolution ();
        point.x -= windowPos.x;
        point.y -= windowPos.y;
        if ( point.x < 0 )
            point.x = 0;
        else if ( point.x > ( long ) vecResolution.fX )
            point.x = ( long ) vecResolution.fX;
        if ( point.y < 0 )
            point.y = 0;
        else if ( point.y > ( long ) vecResolution.fY )
            point.y = ( long ) vecResolution.fY;

        vecCursor = CVector2D ( ( ( float ) point.x ) / vecResolution.fX,
                                ( ( float ) point.y ) / vecResolution.fY );

        CVector vecScreen ( ( float ) ( ( int ) point.x ), ( float ) ( ( int ) point.y ), 300.0f );
        g_pCore->GetGraphics ()->CalcWorldCoors ( &vecScreen, &vecWorld );

        return true;        
    }
    return false;
}


void CStaticFunctionDefinitions::DrawText ( int iLeft, int iTop,
                                 int iRight, int iBottom,
                                 unsigned long dwColor,
                                 const char* szText,
                                 float fScaleX,
                                 float fScaleY,
                                 unsigned long ulFormat,
                                 ID3DXFont* pDXFont,
                                 bool bPostGUI )
{
    g_pCore->GetGraphics ()->DrawTextQueued ( iLeft, iTop, iRight, iBottom, dwColor, szText, fScaleX, fScaleY, ulFormat, pDXFont, bPostGUI );
}

CClientFont* CStaticFunctionDefinitions::CreateFont ( const SString& strFullFilePath, uint uiSize, bool bBold, const SString& strMetaPath, CResource* pResource  )
{
    return m_pFontManager->CreateFont( strFullFilePath, uiSize, bBold, strMetaPath, pResource );
}

// Find custom font from an element, or a standard font from a name
ID3DXFont* CStaticFunctionDefinitions::ResolveDXFont ( const SString& strFontName, CClientFont* pFontElement, float fScaleX, float fScaleY )
{
    if ( pFontElement )
        return pFontElement->GetDXFont ( fScaleX, fScaleY );

    CGraphicsInterface* pGraphics = g_pCore->GetGraphics ();
    eFontType fontType = pGraphics->GetFontType ( strFontName );
    return pGraphics->GetFont ( fontType );
}

bool CStaticFunctionDefinitions::IsCursorShowing ( bool& bShowing )
{
    bShowing = m_pClientGame->AreCursorEventsEnabled () || m_pCore->IsCursorForcedVisible();
    return true;
}


bool CStaticFunctionDefinitions::GUIGetInputEnabled ( void )
{   // can't inline because statics are defined in .cpp not .h
    return m_pGUI->GetGUIInputEnabled ();
}

void CStaticFunctionDefinitions::GUISetInputMode ( eInputMode inputMode )
{
    m_pGUI->SetGUIInputMode ( inputMode );
}

eInputMode CStaticFunctionDefinitions::GUIGetInputMode ( void )
{
    return m_pGUI->GetGUIInputMode();
}

CClientGUIElement* CStaticFunctionDefinitions::GUICreateWindow ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative )
{
    CGUIElement *pElement = m_pGUI->CreateWnd ( NULL, szCaption );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );

    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( LuaMain.GetResource()->GetResourceGUIEntity()  );

    // set events
    pGUIElement->SetEvents ( "onClientGUIClose", "onClientGUIKeyDown" );
    static_cast < CGUIWindow* > ( pElement ) -> SetCloseClickHandler ( pGUIElement->GetCallback1 () );
    static_cast < CGUIWindow* > ( pElement ) -> SetKeyDownHandler ( pGUIElement->GetCallback2 () );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateStaticImage ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const SString& strPath, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateStaticImage ( pParent ? pParent->GetCGUIElement () : NULL );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );

    // Register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    // Check for a valid (and sane) file path
    if ( strPath )
    {
        // Load the image
        if ( !static_cast < CGUIStaticImage* > ( pElement ) -> LoadFromFile ( "", strPath ) ) {
            // If this fails, there's no reason to keep the widget (we don't have any IE-style "not found" icons yet)
            // So delete it and reset the pointer, so we return NULL
            delete pGUIElement;
            pGUIElement = NULL;
        }
    }    

    return pGUIElement;
}


bool CStaticFunctionDefinitions::GUIStaticImageLoadImage ( CClientEntity& Entity, const SString& strDir )
{
    RUN_CHILDREN GUIStaticImageLoadImage ( **iter, strDir );

    // Is this a gui element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a static image?
        if ( IS_CGUIELEMENT_STATICIMAGE ( &GUIElement ) )
        {
            CGUIElement *pCGUIElement = GUIElement.GetCGUIElement ();

            if ( strDir )
            {
                // load the image, if any
                return static_cast < CGUIStaticImage* > ( pCGUIElement ) -> LoadFromFile ( "", strDir );
            }
        }
    }

    return false;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateLabel ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent )
{
    // If there's a parent, make sure it's a GUI widget that can be a parent
    // ChrML: This has broken a lot of scripts
    //if ( pParent && !IS_CGUIELEMENT_VALID_PARENT ( pParent ) ) return NULL;

    CGUIElement *pElement = m_pGUI->CreateLabel ( pParent ? pParent->GetCGUIElement () : NULL, szCaption );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );

    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateButton ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateButton ( pParent ? pParent->GetCGUIElement () : NULL, szCaption );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );
   
    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity() );
  
    // set events
    pGUIElement->SetEvents ( "onClientGUIClicked" );
    static_cast < CGUIButton* > ( pElement ) -> SetClickHandler ( pGUIElement->GetCallback1 () );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateProgressBar ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateProgressBar ( pParent ? pParent->GetCGUIElement () : NULL );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );
    
    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );
  
    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateScrollBar ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bHorizontal, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateScrollBar ( bHorizontal, pParent ? pParent->GetCGUIElement () : NULL );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );
    
    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );
  
    // set events
    pGUIElement->SetEvents ( "onClientGUIScroll" ); 
    static_cast < CGUIScrollBar* > ( pElement ) -> SetOnScrollHandler ( pGUIElement->GetCallback1 () );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateCheckBox ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bChecked, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateCheckBox ( pParent ? pParent->GetCGUIElement () : NULL, szCaption, bChecked );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );
    
    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );
  
    // set events
    pGUIElement->SetEvents ( "onClientGUIStateChanged" );
    static_cast < CGUICheckBox* > ( pElement ) -> SetClickHandler ( pGUIElement->GetCallback1 () );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateRadioButton ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateRadioButton ( pParent ? pParent->GetCGUIElement () : NULL, szCaption );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );
    
    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );
  
    // set events
    pGUIElement->SetEvents ( "onClientGUIStateChanged" );
    static_cast < CGUIRadioButton* > ( pElement ) -> SetClickHandler ( pGUIElement->GetCallback1 () );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateEdit ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateEdit ( pParent ? pParent->GetCGUIElement () : NULL, szCaption );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );
    pElement->SetText ( szCaption );

    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    // set events
    pGUIElement->SetEvents ( "onClientGUIAccepted", "onClientGUIChanged" );
    static_cast < CGUIEdit* > ( pElement ) -> SetTextAcceptedHandler ( pGUIElement->GetCallback1 () );
    static_cast < CGUIEdit* > ( pElement ) -> SetTextChangedHandler ( pGUIElement->GetCallback2 () );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateMemo ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateMemo ( pParent ? pParent->GetCGUIElement () : NULL, szCaption );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );
    pElement->SetText ( szCaption );

    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    // set events
    pGUIElement->SetEvents ( "onClientGUIAccepted", "onClientGUIChanged" );
    static_cast < CGUIMemo* > ( pElement ) -> SetTextChangedHandler ( pGUIElement->GetCallback2 () );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateGridList ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateGridList ( pParent ? pParent->GetCGUIElement () : NULL, true );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );

    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateTabPanel ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateTabPanel ( pParent ? pParent->GetCGUIElement () : NULL );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );

    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    // set events
    pGUIElement->SetEvents ( "onClientGUITabSwitched" );
    static_cast < CGUITabPanel* > ( pElement ) -> SetSelectionHandler ( pGUIElement->GetCallback1 () );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateScrollPane ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateScrollPane ( pParent ? pParent->GetCGUIElement () : NULL );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );

    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUICreateTab ( CLuaMain& LuaMain, const char *szCaption, CClientGUIElement* pParent )
{
    if ( !pParent ) return NULL;

    CGUIElement *pGUIParent = pParent->GetCGUIElement ();
    CGUIElement *pTab = static_cast < CGUITabPanel* > ( pGUIParent ) -> CreateTab ( szCaption );
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pTab );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    return pGUIElement;
}


CClientGUIElement* CStaticFunctionDefinitions::GUIGetSelectedTab ( CClientEntity& Element )
{
    if ( IS_GUI ( &Element ) )
    {
        CClientGUIElement& GUIPanel = static_cast < CClientGUIElement& > ( Element );
        if ( IS_CGUIELEMENT_TABPANEL ( &GUIPanel ) )
        {
            CGUITab* pTab = static_cast < CGUITabPanel* > ( GUIPanel.GetCGUIElement () )->GetSelectedTab ();
            if ( pTab )
            {
                return m_pGUIManager->Get ( static_cast < CGUIElement* > ( pTab ) );
            }
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::GUISetSelectedTab ( CClientEntity& Element, CClientEntity& Tab )
{
    if ( IS_GUI ( &Element ) && IS_GUI ( &Tab ) )
    {
        CClientGUIElement& GUIPanel = static_cast < CClientGUIElement& > ( Element );
        CClientGUIElement& GUITab = static_cast < CClientGUIElement& > ( Tab );
        if ( IS_CGUIELEMENT_TABPANEL ( &GUIPanel ) && IS_CGUIELEMENT_TAB ( &GUITab ) )
        {
            static_cast < CGUITabPanel* > ( GUIPanel.GetCGUIElement () )->SetSelectedTab ( static_cast < CGUITab* > ( GUITab.GetCGUIElement () ) );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GUIDeleteTab ( CLuaMain& LuaMain, CClientGUIElement* pTab, CClientGUIElement* pParent )
{
    if ( !pParent || !pTab ) return false;

    CGUIElement *pGUIParent = pParent->GetCGUIElement ();
    static_cast < CGUITabPanel* > ( pGUIParent ) ->DeleteTab ( reinterpret_cast < CGUITab* > ( pTab->GetCGUIElement() ) );
    g_pClientGame->GetElementDeleter ()->Delete ( pTab );

    return true;
}

CClientGUIElement* CStaticFunctionDefinitions::GUICreateComboBox ( CLuaMain& LuaMain, float fX, float fY, float fWidth, float fHeight, const char* szCaption, bool bRelative, CClientGUIElement* pParent )
{
    CGUIElement *pElement = m_pGUI->CreateComboBox ( pParent ? pParent->GetCGUIElement () : NULL, szCaption );
    pElement->SetPosition ( CVector2D ( fX, fY ), bRelative );
    pElement->SetSize ( CVector2D ( fWidth, fHeight ), bRelative );
    
    // Disable editing of the box...
    //CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( pElement );
    static_cast < CGUIComboBox* > ( pElement ) -> SetReadOnly ( true );

    // register to the gui manager
    CClientGUIElement *pGUIElement = new CClientGUIElement ( m_pManager, &LuaMain, pElement );
    pGUIElement->SetParent ( pParent ? pParent : LuaMain.GetResource()->GetResourceGUIEntity()  );

    // set events
    pGUIElement->SetEvents ( "onClientGUIComboBoxAccepted" );
    static_cast < CGUIComboBox* > ( pElement ) -> SetSelectionHandler ( pGUIElement->GetCallback1 () );

    return pGUIElement;
}

int CStaticFunctionDefinitions::GUIComboBoxAddItem ( CClientEntity& Entity, const char* szText )
{
    RUN_CHILDREN GUIComboBoxAddItem ( **iter, szText );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a combobox?
        if ( IS_CGUIELEMENT_COMBOBOX ( &GUIElement ) )
        {
            // Add a new item.
            CGUIListItem* item = static_cast < CGUIComboBox* > ( GUIElement.GetCGUIElement () ) -> AddItem ( szText );
            // Return it's id + 1 so indexes start at 1.
            return static_cast < CGUIComboBox* > ( GUIElement.GetCGUIElement () ) -> GetItemIndex ( item );
        }
    }    

    return 0;
}

bool CStaticFunctionDefinitions::GUIComboBoxRemoveItem ( CClientEntity& Entity, int index )
{
    RUN_CHILDREN GUIComboBoxRemoveItem ( **iter, index );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a combobox?
        if ( IS_CGUIELEMENT_COMBOBOX ( &GUIElement ) )
        {
            // Call RemoveItem with index - 1 so indexes are compatible internally ...
            return static_cast < CGUIComboBox* > ( GUIElement.GetCGUIElement () ) -> RemoveItem( index  );
        }
    }    

    return false;
}

bool CStaticFunctionDefinitions::GUIComboBoxClear ( CClientEntity& Entity  )
{
    RUN_CHILDREN GUIComboBoxClear ( **iter );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a combobox?
        if ( IS_CGUIELEMENT_COMBOBOX ( &GUIElement ) )
        {
            // Clear the combobox
            static_cast < CGUIComboBox* > ( GUIElement.GetCGUIElement () ) ->Clear ( );
            return true;
        }
    }    

    return false;
}

int CStaticFunctionDefinitions::GUIComboBoxGetSelected ( CClientEntity& Entity )
{
    RUN_CHILDREN GUIComboBoxGetSelected ( **iter );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a combobox?
        if ( IS_CGUIELEMENT_COMBOBOX ( &GUIElement ) )
        {
            // return the selected + 1 so indexes start at 1...
            return static_cast < CGUIComboBox* > ( GUIElement.GetCGUIElement () ) ->GetSelectedItemIndex ( );
        }
    }    

    return 0;
}

bool CStaticFunctionDefinitions::GUIComboBoxSetSelected ( CClientEntity& Entity, int index )
{
    RUN_CHILDREN GUIComboBoxSetSelected ( **iter, index );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a combobox?
        if ( IS_CGUIELEMENT_COMBOBOX ( &GUIElement ) )
        {
            // Call SetSelectedItem with index - 1 so indexes are compatible internally ...
            return static_cast < CGUIComboBox* > ( GUIElement.GetCGUIElement () ) -> SetSelectedItemByIndex( index );
        }
    }    

    return false;
}

std::string CStaticFunctionDefinitions::GUIComboBoxGetItemText ( CClientEntity& Entity, int index )
{
    RUN_CHILDREN GUIComboBoxGetItemText ( **iter, index );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a combobox?
        if ( IS_CGUIELEMENT_COMBOBOX ( &GUIElement ) )
        {
            // Call GetItemText with index - 1 so indexes are compatible internally ...
            return static_cast < CGUIComboBox* > ( GUIElement.GetCGUIElement () ) -> GetItemText( index );
        }
    }    

    return NULL;
}

bool CStaticFunctionDefinitions::GUIComboBoxSetItemText ( CClientEntity& Entity, int index, const char* szText )
{
    RUN_CHILDREN GUIComboBoxSetItemText ( **iter, index, szText );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a combobox?
        if ( IS_CGUIELEMENT_COMBOBOX ( &GUIElement ) )
        {
            // Call SetItemText with index - 1 so indexes are compatible internally ...
            return static_cast < CGUIComboBox* > ( GUIElement.GetCGUIElement () ) -> SetItemText( index, szText );
        }
    }    

    return false;
}

void CStaticFunctionDefinitions::GUISetText ( CClientEntity& Entity, const char* szText )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Set the text
        GUIElement.GetCGUIElement ()->SetText ( szText );
    }
}

/*
bool CStaticFunctionDefinitions::GUISetFont ( CClientEntity& Entity, const char* szFont, CClientFont* pFontElement )
{
    bool bResult = false;

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        bResult = GUIElement.SetFont ( szFont, pFontElement );

        // Set the font
        //bResult = GUIElement.GetCGUIElement ()->SetFont ( szFont );
    }
    return bResult;
}
*/
//bool CStaticFunctionDefinitions::GUIUnloadFont ( std::string strFullFilePath, std::string strMetaPath, CResource* pResource )
//{
//    return m_pScriptFontLoader->UnloadFont(strFullFilePath, strMetaPath, pResource );
//}



void CStaticFunctionDefinitions::GUISetSize ( CClientEntity& Entity, const CVector2D& vecSize, bool bRelative )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );
    
        // Set the size
        GUIElement.GetCGUIElement ()->SetSize ( vecSize, bRelative );
    }
}


void CStaticFunctionDefinitions::GUISetPosition ( CClientEntity& Entity, const CVector2D& vecPosition, bool bRelative )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Set the positin
        GUIElement.GetCGUIElement ()->SetPosition ( vecPosition, bRelative );
    }
}


void CStaticFunctionDefinitions::GUISetVisible ( CClientEntity& Entity, bool bFlag )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );
    
        // Set the visibility
        GUIElement.GetCGUIElement ()->SetVisible ( bFlag );
    }
}


void CStaticFunctionDefinitions::GUISetEnabled ( CClientEntity& Entity, bool bFlag )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );
    
        // Set the visibility
        GUIElement.GetCGUIElement ()->SetEnabled ( bFlag );
    }
}


void CStaticFunctionDefinitions::GUISetProperty ( CClientEntity& Entity, const char* szProperty, const char* szValue )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );
    
        // Set the property
        GUIElement.GetCGUIElement ()->SetProperty ( szProperty, szValue );

        // HACK: If the property being set is AlwaysOnTop, move it to the back so it's not in front of the main menu
        if (  ( stricmp ( szProperty, "AlwaysOnTop" ) == 0 ) && 
              ( stricmp ( szValue, "True" ) == 0 ) )
        {
            GUIElement.GetCGUIElement ()->MoveToBack();
        }
    }
}


void CStaticFunctionDefinitions::GUISetAlpha ( CClientEntity& Entity, float fAlpha )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );
    
        // Set the alpha level
        GUIElement.GetCGUIElement ()->SetAlpha ( fAlpha );
    }
}


bool CStaticFunctionDefinitions::GUIBringToFront ( CClientEntity& Entity )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );
        // We don't allow AlwaysOnTop GUI to be brought to the front (so it doesn't appear on top of the main menu)
        std::string strValue = GUIElement.GetCGUIElement ()->GetProperty ( "AlwaysOnTop" );
        if ( strValue.compare ( "True" ) != 0 )
        {
            // Bring it to the front
            GUIElement.GetCGUIElement ()->BringToFront ();
            return true;
        }
    }
    return false;
}


void CStaticFunctionDefinitions::GUIMoveToBack ( CClientEntity& Entity )
{
    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Move it to the back
        GUIElement.GetCGUIElement ()->MoveToBack ();
    }
}


void CStaticFunctionDefinitions::GUICheckBoxSetSelected ( CClientEntity& Entity, bool bFlag )
{
    RUN_CHILDREN GUICheckBoxSetSelected ( **iter, bFlag );

    // Are we a CGUI element and checkpox?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a checkbox?
        if ( IS_CGUIELEMENT_CHECKBOX ( &GUIElement ) )
        {
            // Set its selected state
            static_cast < CGUICheckBox* > ( GUIElement.GetCGUIElement () ) -> SetSelected ( bFlag );
        }
    }
}


void CStaticFunctionDefinitions::GUIRadioButtonSetSelected ( CClientEntity& Entity, bool bFlag )
{
    RUN_CHILDREN GUIRadioButtonSetSelected ( **iter, bFlag );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a radiobutton?
        if ( IS_CGUIELEMENT_RADIOBUTTON ( &GUIElement ) )
        {
            // Set its selected state
            static_cast < CGUIRadioButton* > ( GUIElement.GetCGUIElement () ) -> SetSelected ( bFlag );
        }
    }
}


void CStaticFunctionDefinitions::GUIProgressBarSetProgress ( CClientEntity& Entity, int iProgress )
{
    RUN_CHILDREN GUIProgressBarSetProgress ( **iter, iProgress );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a progressbar?
        if ( IS_CGUIELEMENT_PROGRESSBAR ( &GUIElement ) )
        {
            // Set the progress
            static_cast < CGUIProgressBar* > ( GUIElement.GetCGUIElement () ) -> SetProgress ( ( float ) ( iProgress / 100.0f ) );
        }
    }
}


void CStaticFunctionDefinitions::GUIScrollBarSetScrollPosition ( CClientEntity& Entity, int iProgress )
{
    RUN_CHILDREN GUIScrollBarSetScrollPosition ( **iter, iProgress );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a progressbar?
        if ( IS_CGUIELEMENT_SCROLLBAR ( &GUIElement ) )
        {
            // Set the progress
            static_cast < CGUIScrollBar* > ( GUIElement.GetCGUIElement () ) -> SetScrollPosition ( ( float ) ( iProgress / 100.0f ) );
        }
    }
}


void CStaticFunctionDefinitions::GUIScrollPaneSetHorizontalScrollPosition ( CClientEntity& Entity, int iProgress )
{
    RUN_CHILDREN GUIScrollPaneSetHorizontalScrollPosition ( **iter, iProgress );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a progressbar?
        if ( IS_CGUIELEMENT_SCROLLPANE ( &GUIElement ) )
        {
            // Set the progress
            static_cast < CGUIScrollPane* > ( GUIElement.GetCGUIElement () ) -> SetHorizontalScrollPosition ( ( float ) ( iProgress / 100.0f ) );
        }
    }
}


void CStaticFunctionDefinitions::GUIScrollPaneSetScrollBars ( CClientEntity& Entity, bool bH, bool bV )
{
    RUN_CHILDREN GUIScrollPaneSetScrollBars ( **iter, bH, bV );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a gridlist?
        if ( IS_CGUIELEMENT_SCROLLPANE ( &GUIElement ) )
        {
            // Set the scrollbars
            static_cast < CGUIScrollPane* > ( GUIElement.GetCGUIElement () ) -> SetHorizontalScrollBar ( bH );
            static_cast < CGUIScrollPane* > ( GUIElement.GetCGUIElement () ) -> SetVerticalScrollBar ( bV );
        }
    }
}


void CStaticFunctionDefinitions::GUIScrollPaneSetVerticalScrollPosition ( CClientEntity& Entity, int iProgress )
{
    RUN_CHILDREN GUIScrollPaneSetVerticalScrollPosition ( **iter, iProgress );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a progressbar?
        if ( IS_CGUIELEMENT_SCROLLPANE ( &GUIElement ) )
        {
            // Set the progress
            static_cast < CGUIScrollPane* > ( GUIElement.GetCGUIElement () ) -> SetVerticalScrollPosition ( ( float ) ( iProgress / 100.0f ) );
        }
    }
}


void CStaticFunctionDefinitions::GUIEditSetReadOnly ( CClientEntity& Entity, bool bFlag )
{
    RUN_CHILDREN GUIEditSetReadOnly ( **iter, bFlag );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a editbox?
        if ( IS_CGUIELEMENT_EDIT ( &GUIElement ) )
        {
            // Set its read only state
            static_cast < CGUIEdit* > ( GUIElement.GetCGUIElement () ) -> SetReadOnly ( bFlag );
        }
    }
}


void CStaticFunctionDefinitions::GUIMemoSetReadOnly ( CClientEntity& Entity, bool bFlag )
{
    RUN_CHILDREN GUIMemoSetReadOnly ( **iter, bFlag );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a editbox?
        if ( IS_CGUIELEMENT_MEMO ( &GUIElement ) )
        {
            // Set its read only state
            static_cast < CGUIMemo* > ( GUIElement.GetCGUIElement () ) -> SetReadOnly ( bFlag );
        }
    }
}


void CStaticFunctionDefinitions::GUIEditSetMasked ( CClientEntity& Entity, bool bFlag )
{
    RUN_CHILDREN GUIEditSetMasked ( **iter, bFlag );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Is this an edit?
        if ( IS_CGUIELEMENT_EDIT ( &GUIElement ) )
        {
            // Set its masked flag
            static_cast < CGUIEdit* > ( GUIElement.GetCGUIElement () ) -> SetMasked ( bFlag );
        }
    }
}


void CStaticFunctionDefinitions::GUIEditSetMaxLength ( CClientEntity& Entity, unsigned int iLength )
{
    RUN_CHILDREN GUIEditSetMaxLength ( **iter, iLength );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we an editbox element?
        if ( IS_CGUIELEMENT_EDIT ( &GUIElement ) )
        {
            // Set its max length
            static_cast < CGUIEdit* > ( GUIElement.GetCGUIElement () ) -> SetMaxLength ( iLength );
        }
    }
}


void CStaticFunctionDefinitions::GUIEditSetCaratIndex ( CClientEntity& Entity, unsigned int iCarat )
{
    RUN_CHILDREN GUIEditSetCaratIndex ( **iter, iCarat );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we an edit?
        if ( IS_CGUIELEMENT_EDIT ( &GUIElement ) )
        {
            // Set its carat index
            static_cast < CGUIEdit* > ( GUIElement.GetCGUIElement () ) -> SetCaratIndex ( iCarat );
        }
    }
}


void CStaticFunctionDefinitions::GUIMemoSetCaratIndex ( CClientEntity& Entity, unsigned int iCarat )
{
    RUN_CHILDREN GUIMemoSetCaratIndex ( **iter, iCarat );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we an edit?
        if ( IS_CGUIELEMENT_MEMO ( &GUIElement ) )
        {
            // Set its carat index
            static_cast < CGUIMemo* > ( GUIElement.GetCGUIElement () ) -> SetCaratIndex ( iCarat );
        }
    }
}


void CStaticFunctionDefinitions::GUIGridListSetSortingEnabled ( CClientEntity& Entity, bool bEnabled )
{
    RUN_CHILDREN GUIGridListSetSortingEnabled ( **iter, bEnabled );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a gridlist?
        if ( IS_CGUIELEMENT_GRIDLIST ( &GUIElement ) )
        {
            // Set sorting is enabled
            static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetSorting ( bEnabled );
        }
    }
}


void CStaticFunctionDefinitions::GUIGridListSetScrollBars ( CClientEntity& Entity, bool bH, bool bV )
{
    RUN_CHILDREN GUIGridListSetScrollBars ( **iter, bH, bV );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a gridlist?
        if ( IS_CGUIELEMENT_GRIDLIST ( &GUIElement ) )
        {
            // Set the scrollbars
            static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetHorizontalScrollBar ( bH );
            static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetVerticalScrollBar ( bV );
        }
    }
}


void CStaticFunctionDefinitions::GUIGridListClear ( CClientEntity& Entity )
{
    RUN_CHILDREN GUIGridListClear ( **iter );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a gridlist?
        if ( IS_CGUIELEMENT_GRIDLIST ( &GUIElement ) )
        {
            // Clear the gridlist
            static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) ->Clear ();
        }
    }
}

void CStaticFunctionDefinitions::GUIGridListSetItemData ( CClientGUIElement& GUIElement, int iRow, int iColumn, CLuaArgument* Variable )
{
    //Delete any old data we might have
    CLuaArgument* pVariable = reinterpret_cast < CLuaArgument* > (
        static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> GetItemData (
            iRow, 
            iColumn
        )
    );
    if ( pVariable )
        delete pVariable;

    static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetItemData ( 
        iRow, 
        iColumn, 
        (void*)Variable, 
        CGUICallback<void,void*>( &CStaticFunctionDefinitions::GUIItemDataDestroyCallback )
    ); 
}


void CStaticFunctionDefinitions::GUIItemDataDestroyCallback ( void* data )
{
    delete (CLuaArgument*)(data);
}


void CStaticFunctionDefinitions::GUIGridListSetSelectionMode ( CClientEntity& Entity, unsigned int uiMode )
{
    RUN_CHILDREN GUIGridListSetSelectionMode ( **iter, uiMode );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a gridlist?
        if ( IS_CGUIELEMENT_GRIDLIST ( &GUIElement ) )
        {
            // Set the selection mode
            static_cast < CGUIGridList* > ( GUIElement.GetCGUIElement () ) -> SetSelectionMode ( (SelectionMode) uiMode );
        }
    }
}


void CStaticFunctionDefinitions::GUIWindowSetMovable ( CClientEntity& Entity, bool bFlag )
{
    RUN_CHILDREN GUIWindowSetMovable ( **iter, bFlag );

    // Are we a GUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a window?
        if ( IS_CGUIELEMENT_WINDOW ( &GUIElement ) )
        {
            // Set the windows movability
            static_cast < CGUIWindow* > ( GUIElement.GetCGUIElement () ) -> SetMovable ( bFlag );
        }
    }
}


void CStaticFunctionDefinitions::GUIWindowSetSizable ( CClientEntity& Entity, bool bFlag )
{
    RUN_CHILDREN GUIWindowSetSizable ( **iter, bFlag );

    // Are we a GUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a window?
        if ( IS_CGUIELEMENT_WINDOW ( &GUIElement ) )
        {
            // Set the windows sizability
            static_cast < CGUIWindow* > ( GUIElement.GetCGUIElement () ) -> SetSizingEnabled ( bFlag );
        }
    }
}


void CStaticFunctionDefinitions::GUILabelSetColor ( CClientEntity& Entity, int iR, int iG, int iB )
{
    RUN_CHILDREN GUILabelSetColor ( **iter, iR, iG, iB );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a CGUI label?
        if ( IS_CGUIELEMENT_LABEL ( &GUIElement ) )
        {
            // Set the label color
            static_cast < CGUILabel* > ( GUIElement.GetCGUIElement () ) -> SetTextColor ( iR, iG, iB );
        }
    }
}


void CStaticFunctionDefinitions::GUILabelSetVerticalAlign ( CClientEntity& Entity, CGUIVerticalAlign eAlign )
{
    RUN_CHILDREN GUILabelSetVerticalAlign ( **iter, eAlign );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a CGUI label?
        if ( IS_CGUIELEMENT_LABEL ( &GUIElement ) )
        {
            // Set the vertical align
            static_cast < CGUILabel* > ( GUIElement.GetCGUIElement () ) ->  SetVerticalAlign ( eAlign );
        }
    }
}


void CStaticFunctionDefinitions::GUILabelSetHorizontalAlign ( CClientEntity& Entity, CGUIHorizontalAlign eAlign )
{
    RUN_CHILDREN GUILabelSetHorizontalAlign ( **iter, eAlign );

    // Are we a CGUI element?
    if ( IS_GUI ( &Entity ) )
    {
        CClientGUIElement& GUIElement = static_cast < CClientGUIElement& > ( Entity );

        // Are we a CGUI label?
        if ( IS_CGUIELEMENT_LABEL ( &GUIElement ) )
        {
            // Set the horizontal align
            static_cast < CGUILabel* > ( GUIElement.GetCGUIElement () ) -> SetHorizontalAlign ( eAlign );
        }
    }
}


bool CStaticFunctionDefinitions::GetTime ( unsigned char &ucHour, unsigned char &ucMin )
{
    g_pGame->GetClock ()->Get ( &ucHour, &ucMin );

    return true;
}


bool CStaticFunctionDefinitions::SetTime ( unsigned char ucHour, unsigned char ucMin )
{
    g_pGame->GetClock ()->Set ( ucHour, ucMin );

    return true;
}


bool CStaticFunctionDefinitions::ProcessLineOfSight ( CVector& vecStart, CVector& vecEnd, bool& bCollision, CColPoint** pColPoint, CClientEntity** pColEntity, bool bCheckBuildings, bool bCheckVehicles, bool bCheckPeds, bool bCheckObjects, bool bCheckDummies, bool bSeeThroughStuff, bool bIgnoreSomeObjectsForCamera, bool bShootThroughStuff, CEntity* pIgnoredEntity )
{
    assert ( pColPoint );
    assert ( pColEntity );

    if ( pIgnoredEntity )
        g_pGame->GetWorld ()->IgnoreEntity ( pIgnoredEntity );

    CEntity* pColGameEntity = 0;
    bCollision = g_pGame->GetWorld ()->ProcessLineOfSight ( &vecStart, &vecEnd, pColPoint, &pColGameEntity, bCheckBuildings, bCheckVehicles, bCheckPeds, bCheckObjects, bCheckDummies, bSeeThroughStuff, bIgnoreSomeObjectsForCamera, bShootThroughStuff );
    
    if ( pIgnoredEntity )
        g_pGame->GetWorld ()->IgnoreEntity ( NULL );
    
    *pColEntity = m_pManager->FindEntity ( pColGameEntity );

    return true;
}


bool CStaticFunctionDefinitions::IsLineOfSightClear ( CVector& vecStart, CVector& vecEnd, bool& bIsClear, bool bCheckBuildings, bool bCheckVehicles, bool bCheckPeds, bool bCheckObjects, bool bCheckDummies, bool bSeeThroughStuff, bool bIgnoreSomeObjectsForCamera, CEntity* pIgnoredEntity )
{
    if ( pIgnoredEntity )
        g_pGame->GetWorld ()->IgnoreEntity ( pIgnoredEntity );

    bIsClear = g_pGame->GetWorld ()->IsLineOfSightClear ( &vecStart, &vecEnd, bCheckBuildings, bCheckVehicles, bCheckPeds, bCheckObjects, bCheckDummies, bSeeThroughStuff, bIgnoreSomeObjectsForCamera );
    
    if ( pIgnoredEntity )
        g_pGame->GetWorld ()->IgnoreEntity ( NULL );

    return true;
}


bool CStaticFunctionDefinitions::TestLineAgainstWater ( CVector& vecStart, CVector& vecEnd, CVector& vecCollision )
{
    return g_pGame->GetWaterManager ()->TestLineAgainstWater ( vecStart, vecEnd, &vecCollision );
}


CClientWater* CStaticFunctionDefinitions::CreateWater ( CResource& resource, CVector* pV1, CVector* pV2, CVector* pV3, CVector* pV4, bool bShallow )
{
    CClientWater* pWater;
    if ( pV4 )
        pWater = new CClientWater ( g_pClientGame->GetManager (), INVALID_ELEMENT_ID, *pV1, *pV2, *pV3, *pV4, bShallow );
    else
        pWater = new CClientWater ( g_pClientGame->GetManager (), INVALID_ELEMENT_ID, *pV1, *pV2, *pV3, bShallow );
    if ( !pWater->Valid () ) {
        delete pWater;
        return NULL;
    }

    pWater->SetParent ( resource.GetResourceDynamicEntity () );
    return pWater;
}


bool CStaticFunctionDefinitions::GetWaterLevel ( CVector& vecPosition, float& fWaterLevel, bool bCheckWaves, CVector& vecUnknown )
{
    return g_pGame->GetWaterManager ()->GetWaterLevel ( vecPosition, &fWaterLevel, bCheckWaves, &vecUnknown );
}


bool CStaticFunctionDefinitions::GetWaterLevel ( CClientWater* pWater, float& fLevel )
{
    CVector vecPosition;
    pWater->GetPosition ( vecPosition );
    fLevel = vecPosition.fZ;
    return true;
}

bool CStaticFunctionDefinitions::GetWaterVertexPosition ( CClientWater* pWater, int iVertexIndex, CVector& vecPosition )
{
    return pWater->GetVertexPosition ( iVertexIndex - 1, vecPosition );
}


bool CStaticFunctionDefinitions::SetWaterLevel ( CVector* pvecPosition, float fLevel, void* pChangeSource )
{
    return g_pGame->GetWaterManager ()->SetWaterLevel ( pvecPosition, fLevel, pChangeSource );
}


bool CStaticFunctionDefinitions::SetWaterLevel ( CClientWater* pWater, float fLevel, void* pChangeSource )
{
    if ( pWater )
    {
        CVector vecPosition;
        pWater->GetPosition ( vecPosition );
        vecPosition.fZ = fLevel;
        pWater->SetPosition ( vecPosition );
        return true;
    }
    else
    {
        return g_pGame->GetWaterManager ()->SetWaterLevel ( (CVector *)NULL, fLevel, pChangeSource );
    }
}


bool CStaticFunctionDefinitions::SetWaterVertexPosition ( CClientWater* pWater, int iVertexIndex, CVector& vecPosition )
{
    return pWater->SetVertexPosition ( iVertexIndex - 1, vecPosition );
}


bool CStaticFunctionDefinitions::GetWorldFromScreenPosition ( CVector& vecScreen, CVector& vecWorld )
{
    g_pCore->GetGraphics ()->CalcWorldCoors ( &vecScreen, &vecWorld );

    return true;
}


bool CStaticFunctionDefinitions::GetScreenFromWorldPosition ( CVector& vecWorld, CVector& vecScreen, float fEdgeTolerance, bool bRelative )
{
    g_pCore->GetGraphics ()->CalcScreenCoors ( &vecWorld, &vecScreen );

    float fResWidth  = static_cast < float > ( g_pCore->GetGraphics ()->GetViewportWidth () );
    float fResHeight = static_cast < float > ( g_pCore->GetGraphics ()->GetViewportHeight () );

    // Calc relative values if required
    float fToleranceX = bRelative ? fEdgeTolerance * fResWidth  : fEdgeTolerance;
    float fToleranceY = bRelative ? fEdgeTolerance * fResHeight : fEdgeTolerance;

    // Keep within a reasonable range
    fToleranceX = Clamp ( 0.f, fToleranceX, fResWidth * 10 );
    fToleranceY = Clamp ( 0.f, fToleranceY, fResHeight * 10 );

    if ( vecScreen.fX >= -fToleranceX && vecScreen.fX <= fResWidth + fToleranceX && vecScreen.fY >= -fToleranceY && vecScreen.fY <= fResHeight + fToleranceY && vecScreen.fZ > 0.1f )
    {
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetWeather ( unsigned char& ucWeather, unsigned char& ucWeatherBlendingTo )
{
    // Grab the current weather
    CBlendedWeather* pWeather = g_pClientGame->GetBlendedWeather ();;
    ucWeather = pWeather->GetWeather ();

    // If we're blending to some weather, also return which. Otherwize just return 0xFF which will return false
    if ( pWeather->GetBlendedStopHour () != 0xFF )
        ucWeatherBlendingTo = pWeather->GetWeatherBlendingTo ();
    else
        ucWeatherBlendingTo = 0xFF;

    return true;
}


bool CStaticFunctionDefinitions::GetZoneName ( CVector& vecPosition, char* szBuffer, unsigned int uiBufferLength, bool bCitiesOnly )
{
    assert ( szBuffer );
    assert ( uiBufferLength );

    const char* szZone = NULL;
    if ( bCitiesOnly )
        szZone = m_pClientGame->GetZoneNames ()->GetCityName ( vecPosition );
    else
        szZone = m_pClientGame->GetZoneNames ()->GetZoneName ( vecPosition );

    strncpy ( szBuffer, szZone, uiBufferLength );
    if (uiBufferLength)
        szBuffer [ uiBufferLength - 1 ] = 0;
    return true;
}


bool CStaticFunctionDefinitions::GetGravity ( float& fGravity )
{
    fGravity = g_pGame->GetGravity ();
    return true;
}


bool CStaticFunctionDefinitions::GetGameSpeed ( float& fSpeed )
{
    fSpeed = g_pGame->GetGameSpeed ();
    return true;
}

bool CStaticFunctionDefinitions::GetMinuteDuration ( unsigned long& ulDelay )
{
    ulDelay = g_pGame->GetMinuteDuration ();
    return true;
}

bool CStaticFunctionDefinitions::GetWaveHeight ( float& fHeight )
{
    fHeight = g_pGame->GetWaterManager ()->GetWaveLevel ();
    return true;
}

bool CStaticFunctionDefinitions::IsGarageOpen ( unsigned char ucGarageID, bool& bIsOpen )
{
    CGarage* pGarage = g_pCore->GetGame()->GetGarages()->GetGarage ( ucGarageID );

    if ( pGarage )
    {
        bIsOpen = pGarage->IsOpen ();
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetGaragePosition ( unsigned char ucGarageID, CVector& vecPosition )
{
    CGarage* pGarage = g_pCore->GetGame()->GetGarages()->GetGarage ( ucGarageID );

    if ( pGarage )
    {
        pGarage->GetPosition ( vecPosition );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetGarageSize ( unsigned char ucGarageID, float& fHeight, float& fWidth, float& fDepth )
{
    CGarage* pGarage = g_pCore->GetGame()->GetGarages()->GetGarage ( ucGarageID );

    if ( pGarage )
    {
        pGarage->GetSize ( fHeight, fWidth, fDepth );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetGarageBoundingBox ( unsigned char ucGarageID, float& fLeft, float& fRight, float& fFront, float& fBack )
{
    CGarage* pGarage = g_pCore->GetGame()->GetGarages()->GetGarage ( ucGarageID );

    if ( pGarage )
    {
        pGarage->GetBoundingBox ( fLeft, fRight, fFront, fBack );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetBlurLevel ( unsigned char ucLevel )
{
    g_pGame->SetBlurLevel ( ucLevel );
    return true;
}

bool CStaticFunctionDefinitions::SetJetpackMaxHeight ( float fHeight )
{
    if ( fHeight >= -20 )
    {
        g_pGame->GetWorld ()->SetJetpackMaxHeight ( fHeight );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetAircraftMaxHeight ( float fHeight )
{
    if ( fHeight >= 0 && fHeight <= 9001 )
    {
        g_pGame->GetWorld ()->SetAircraftMaxHeight ( fHeight );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetTrafficLightState ( unsigned char ucState )
{
    if ( ucState >= 0 && ucState < 10 )
    {
        g_pMultiplayer->SetTrafficLightState ( ucState );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetTrafficLightsLocked ( bool bLocked )
{
    g_pMultiplayer->SetTrafficLightsLocked ( bLocked );
    return true;
}

bool CStaticFunctionDefinitions::SetWindVelocity ( float fX, float fY, float fZ )
{
    g_pMultiplayer->SetWindVelocity ( fX, fY, fZ );
    return true;
}

bool CStaticFunctionDefinitions::RestoreWindVelocity ( void )
{
    g_pMultiplayer->RestoreWindVelocity ( );
    return true;
}

bool CStaticFunctionDefinitions::GetWindVelocity ( float& fX, float& fY, float& fZ )
{
    g_pMultiplayer->GetWindVelocity ( fX, fY, fZ );
    return true;
}

bool CStaticFunctionDefinitions::IsWorldSpecialPropertyEnabled ( const char* szPropName )
{
    return g_pGame->IsCheatEnabled ( szPropName );
}

bool CStaticFunctionDefinitions::GetTrafficLightState ( unsigned char& ucState )
{
    ucState = g_pMultiplayer->GetTrafficLightState ();
    return true;
}

bool CStaticFunctionDefinitions::AreTrafficLightsLocked ( bool& bLocked )
{
    bLocked = g_pMultiplayer->GetTrafficLightsLocked ();
    return true;
}


bool CStaticFunctionDefinitions::GetSkyGradient ( unsigned char& ucTopRed, unsigned char& ucTopGreen, unsigned char& ucTopBlue, unsigned char& ucBottomRed, unsigned char& ucBottomGreen, unsigned char& ucBottomBlue )
{
    g_pMultiplayer->GetSkyColor ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue );
    return true;
}


bool CStaticFunctionDefinitions::SetSkyGradient ( unsigned char ucTopRed, unsigned char ucTopGreen, unsigned char ucTopBlue, unsigned char ucBottomRed, unsigned char ucBottomGreen, unsigned char ucBottomBlue )
{
    g_pMultiplayer->SetSkyColor ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue );

    return true;
}


bool CStaticFunctionDefinitions::ResetSkyGradient ( void )
{
    g_pMultiplayer->ResetSky ();
    return true;
}


bool CStaticFunctionDefinitions::GetHeatHaze ( SHeatHazeSettings& settings )
{
    g_pMultiplayer->GetHeatHaze ( settings );
    return true;
}


bool CStaticFunctionDefinitions::SetHeatHaze ( const SHeatHazeSettings& settings )
{
    g_pMultiplayer->SetHeatHaze ( settings );
    return true;
}


bool CStaticFunctionDefinitions::ResetHeatHaze ( void )
{
    g_pMultiplayer->ResetHeatHaze ();
    return true;
}


bool CStaticFunctionDefinitions::GetWaterColor ( float& fWaterRed, float& fWaterGreen, float& fWaterBlue, float& fWaterAlpha )
{
    g_pMultiplayer->GetWaterColor ( fWaterRed, fWaterGreen, fWaterBlue, fWaterAlpha );
    return true;
}

bool CStaticFunctionDefinitions::SetWaterColor ( float fWaterRed, float fWaterGreen, float fWaterBlue, float fWaterAlpha )
{
    g_pMultiplayer->SetWaterColor ( fWaterRed, fWaterGreen, fWaterBlue, fWaterAlpha );

    return true;
}


bool CStaticFunctionDefinitions::ResetWaterColor ( void )
{
    g_pMultiplayer->ResetWater ();
    return true;
}


bool CStaticFunctionDefinitions::SetWeather ( unsigned char ucWeather )
{
    // Verify it's within the max valid weather id
    if ( ucWeather <= MAX_VALID_WEATHER )
    {
        // Set the weather
        m_pBlendedWeather->SetWeather ( ucWeather );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetWeatherBlended ( unsigned char ucWeather )
{
    // Verify it's within the max valid weather id
    if ( ucWeather <= MAX_VALID_WEATHER )
    {
        // Get the next hour
        unsigned char ucHour, ucMin;
        m_pGame->GetClock ()->Get ( &ucHour, &ucMin );
        ++ucHour;
        if ( ucHour > 23 ) ucHour = 0;

        // Set the weather
        m_pBlendedWeather->SetWeatherBlended ( ucWeather, ucHour );
        return true;
    }

    return false;
}

#define MAX_GRAVITY 1.0f
#define MIN_GRAVITY -1.0f

bool CStaticFunctionDefinitions::SetGravity ( float fGravity )
{
    if ( fGravity >= MIN_GRAVITY && fGravity <= MAX_GRAVITY )
    {
        g_pMultiplayer->SetGlobalGravity ( fGravity );
        g_pCore->GetMultiplayer ()->SetLocalPlayerGravity ( fGravity );

        return true;
    }
    
    return false;
}


bool CStaticFunctionDefinitions::SetGameSpeed ( float fSpeed )
{
    if ( fSpeed >= 0.0f && fSpeed <= 10.0f )
    {
        m_pClientGame->SetGameSpeed ( fSpeed );
        
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetMinuteDuration ( unsigned long ulDelay )
{
    if ( ulDelay > 0 && ulDelay <= 4294967296 )
    {
        m_pClientGame->SetMinuteDuration ( ulDelay );

        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetWaveHeight ( float fHeight )
{
    if ( fHeight >= -1.0f && fHeight <= 100.0f )
    {
        g_pGame->GetWaterManager ()->SetWaveLevel ( fHeight );
        
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetGarageOpen ( unsigned char ucGarageID, bool bIsOpen )
{
    CGarage* pGarage = g_pCore->GetGame()->GetGarages()->GetGarage ( ucGarageID );

    if ( pGarage )
    {
        pGarage->SetOpen ( bIsOpen );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetWorldSpecialPropertyEnabled ( const char* szPropName, bool bEnabled )
{
    return g_pGame->SetCheatEnabled ( szPropName, bEnabled );
}
bool CStaticFunctionDefinitions::SetCloudsEnabled ( bool bEnabled )
{
    g_pMultiplayer->SetCloudsEnabled ( bEnabled );
    g_pClientGame->SetCloudsEnabled ( bEnabled );
    return true;
}
bool CStaticFunctionDefinitions::GetCloudsEnabled ( )
{
    return g_pClientGame->GetCloudsEnabled ();
}


bool CStaticFunctionDefinitions::BindKey ( const char* szKey, const char* szHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments )
{
    assert ( szKey );
    assert ( szHitState );
    assert ( pLuaMain );

    bool bSuccess = false;

    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    SScriptBindableKey* pScriptKey = NULL;
    if ( pKey )
        pScriptKey = m_pScriptKeyBinds->GetBindableFromKey ( szKey );
    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
    SScriptBindableGTAControl* pScriptControl = NULL;
    if ( pControl )
        pScriptControl = m_pScriptKeyBinds->GetBindableFromControl ( szKey );

    bool bHitState = true;
    if ( !stricmp ( szHitState, "down" ) || !stricmp ( szHitState, "both" ) )
    {
        if ( ( pKey &&
               m_pScriptKeyBinds->AddKeyFunction ( pScriptKey, bHitState, pLuaMain, iLuaFunction, Arguments ) &&
               ( pKeyBinds->FunctionExists ( pKey, CClientGame::StaticProcessClientKeyBind, true, bHitState ) || pKeyBinds->AddFunction ( pKey, CClientGame::StaticProcessClientKeyBind, bHitState, false ) ) ) ||
             ( pControl &&
               m_pScriptKeyBinds->AddControlFunction ( pScriptControl, bHitState, pLuaMain, iLuaFunction, Arguments ) &&
               ( pKeyBinds->ControlFunctionExists ( pControl, CClientGame::StaticProcessClientControlBind, true, bHitState ) || pKeyBinds->AddControlFunction ( pControl, CClientGame::StaticProcessClientControlBind, bHitState ) ) ) )
        {
            bSuccess = true;
        }
    }
    bHitState = false;
    if ( !stricmp ( szHitState, "up" ) || !stricmp ( szHitState, "both" ) )
    {
        if ( ( pKey &&
               m_pScriptKeyBinds->AddKeyFunction ( pScriptKey, bHitState, pLuaMain, iLuaFunction, Arguments ) &&
               ( pKeyBinds->FunctionExists ( pKey, CClientGame::StaticProcessClientKeyBind, true, bHitState ) || pKeyBinds->AddFunction ( pKey, CClientGame::StaticProcessClientKeyBind, bHitState, false ) ) ) ||
             ( pControl &&
               m_pScriptKeyBinds->AddControlFunction ( pScriptControl, bHitState, pLuaMain, iLuaFunction, Arguments ) &&
               ( pKeyBinds->ControlFunctionExists ( pControl, CClientGame::StaticProcessClientControlBind, true, bHitState ) || pKeyBinds->AddControlFunction ( pControl, CClientGame::StaticProcessClientControlBind, bHitState ) ) ) )
        {
            bSuccess = true;
        }
    }

    return bSuccess;
}

bool CStaticFunctionDefinitions::BindKey ( const char* szKey, const char* szHitState, const char* szCommandName, const char* szArguments, const char* szResource )
{
    assert ( szKey );
    assert ( szHitState );

    bool bSuccess = false;

    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    bool bKey = pKeyBinds->IsKey ( szKey );
    if ( bKey )
    {
        bool bHitState = true;
        //Activate all keys for this command
        pKeyBinds->SetAllCommandsActive ( szResource, true, szCommandName, bHitState, szArguments, true );
        //Check if its binded already (dont rebind)
        if ( pKeyBinds->CommandExists ( szKey, szCommandName, true, bHitState, szArguments, szResource ) )
            return true;

        if ( ( !stricmp ( szHitState, "down" ) || !stricmp ( szHitState, "both" ) ) &&
             pKeyBinds->AddCommand ( szKey, szCommandName, szArguments, bHitState, szResource ) )
        {
            pKeyBinds->SetCommandActive ( szKey, szCommandName, bHitState, szArguments, szResource, true, true );
            bSuccess = true;
        }

        bHitState = false;
        pKeyBinds->SetAllCommandsActive ( szResource, true, szCommandName, bHitState, szArguments, true );
        if ( pKeyBinds->CommandExists ( szKey, szCommandName, true, bHitState, szArguments, szResource ) )
            return true;

        if ( ( !stricmp ( szHitState, "up" ) || !stricmp ( szHitState, "both" ) ) &&
             pKeyBinds->AddCommand ( szKey, szCommandName, szArguments, bHitState, szResource ) )
        {
            pKeyBinds->SetCommandActive ( szKey, szCommandName, bHitState, szArguments, szResource, true, true  );
            bSuccess = true;
        }
    }
    return bSuccess;
}

bool CStaticFunctionDefinitions::UnbindKey ( const char* szKey, CLuaMain* pLuaMain, const char* szHitState, const CLuaFunctionRef& iLuaFunction )
{
    assert ( szKey );
    assert ( pLuaMain );

    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    SScriptBindableKey* pScriptKey = NULL;
    if ( pKey )
        pScriptKey = m_pScriptKeyBinds->GetBindableFromKey ( szKey );
    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
    SScriptBindableGTAControl* pScriptControl = NULL;
    if ( pControl )
        pScriptControl = m_pScriptKeyBinds->GetBindableFromControl ( szKey );

    bool bCheckHitState = false, bHitState = true;
    if ( szHitState )
    {
        if ( stricmp ( szHitState, "down" ) == 0 )
        {
            bCheckHitState = true, bHitState = true;
        }
        else if ( stricmp ( szHitState, "up" ) == 0 )
        {
            bCheckHitState = true, bHitState = false;
        }
    }

    if (
         (
           pKey &&
           ( m_pScriptKeyBinds->RemoveKeyFunction ( pScriptKey, pLuaMain, bCheckHitState, bHitState, iLuaFunction ) ||
             pKeyBinds->RemoveFunction ( pKey, CClientGame::StaticProcessClientKeyBind, bCheckHitState, bHitState ) )
         )
         ||
         (
           pControl &&
           ( m_pScriptKeyBinds->RemoveControlFunction ( pScriptControl, pLuaMain, bCheckHitState, bHitState, iLuaFunction ) ||
             pKeyBinds->RemoveControlFunction ( pControl, CClientGame::StaticProcessClientControlBind, true, bHitState ) )
         )
       )
    {
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::UnbindKey ( const char* szKey, const char* szHitState, const char* szCommandName, const char* szResource )
{
    assert ( szKey );
    assert ( szHitState );

    bool bSuccess = false;

    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    bool bKey = pKeyBinds->IsKey ( szKey );
    if ( bKey )
    {
        bool bCheckHitState = false, bHitState = true;
        if ( szHitState )
        {
            if ( stricmp ( szHitState, "down" ) == 0 )
            {
                bCheckHitState = true, bHitState = true;
            }
            else if ( stricmp ( szHitState, "up" ) == 0 )
            {
                bCheckHitState = true, bHitState = false;
            }
        }
        if ( ( !stricmp ( szHitState, "down" ) || !stricmp ( szHitState, "both" ) ) &&
             pKeyBinds->SetCommandActive ( szKey, szCommandName, bHitState, NULL, szResource, false, true  ) )
        {
            pKeyBinds->SetAllCommandsActive ( szResource, false, szCommandName, bHitState, NULL, true );
            bSuccess = true;
        }
        bHitState = false;
        if ( ( !stricmp ( szHitState, "up" ) || !stricmp ( szHitState, "both" ) ) &&
             pKeyBinds->SetCommandActive ( szKey, szCommandName, bHitState, NULL, szResource, false, true  ) )
        {
            pKeyBinds->SetAllCommandsActive ( szResource, false, szCommandName, bHitState, NULL, true );
            bSuccess = true;
        }
    }
    return bSuccess;
}

bool CStaticFunctionDefinitions::GetKeyState ( const char* szKey, bool& bState )
{
    assert ( szKey );
    
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    if ( pKey )
    {
        HWND hActiveWindow = GetActiveWindow ();
        if ( hActiveWindow == g_pCore->GetHookedWindow () )
        {
            bState = ( ::GetKeyState ( pKey->ulCode ) & 0x8000  ) ? true:false;
            
            return true;
        }
    }
    
    return false;
}


bool CStaticFunctionDefinitions::GetControlState ( const char* szControl, bool& bState )
{
    assert ( szControl );
    
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szControl );
    if ( pControl )
    {
        bState = pControl->bState;
        
        return true;
    }
    
    return false;
}

bool CStaticFunctionDefinitions::GetAnalogControlState ( const char * szControl, float & fState )
{
    CControllerState cs;
    CClientPlayer* localPlayer = m_pPlayerManager->GetLocalPlayer ();
    localPlayer->GetControllerState( cs );
    bool bOnFoot = ( !localPlayer->GetRealOccupiedVehicle () );
    if ( CClientPad::GetAnalogControlState ( szControl, cs, bOnFoot, fState ) )
    {
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::IsControlEnabled ( const char* szControl, bool& bEnabled )
{
    assert ( szControl );

    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szControl );
    if ( pControl )
    {
        bEnabled = pControl->bEnabled;

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetControlState ( const char* szControl, bool bState )
{
    assert ( szControl );

    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();    
    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szControl );
    if ( pControl )
    {
        pControl->bState = bState;
        
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::ToggleControl ( const char* szControl, bool bEnabled )
{
    assert ( szControl );
      
    if ( g_pCore->GetKeyBinds ()->SetControlEnabled ( szControl, bEnabled ) )
    {        
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::ToggleAllControls ( bool bGTAControls, bool bMTAControls, bool bEnabled )
{
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();    
    pKeyBinds->SetAllControlsEnabled ( bGTAControls, bMTAControls, bEnabled );

    return true;
}


CClientProjectile * CStaticFunctionDefinitions::CreateProjectile ( CResource& Resource, CClientEntity& Creator, unsigned char ucWeaponType, CVector& vecOrigin, float fForce, CClientEntity* pTarget, CVector* pvecRotation, CVector* pvecVelocity, unsigned short usModel )
{
    // Do we have a rotation vector?
    if ( pvecRotation )
    {
        // It should be in degrees, so convert it to radians
        ConvertDegreesToRadians ( *pvecRotation );
    }

    // Valid creator type?
    switch ( Creator.GetType () )
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        case CCLIENTVEHICLE:
        {
            eWeaponType weaponType = ( eWeaponType ) ucWeaponType;

            // Valid weapon type?
            switch ( weaponType )
            {
                case WEAPONTYPE_GRENADE:
                case WEAPONTYPE_TEARGAS:
                case WEAPONTYPE_MOLOTOV:
                case WEAPONTYPE_ROCKET:
                case WEAPONTYPE_ROCKET_HS:
                case WEAPONTYPE_FREEFALL_BOMB:
                case WEAPONTYPE_REMOTE_SATCHEL_CHARGE:
                {
                    CClientProjectile * pProjectile = m_pProjectileManager->Create ( &Creator, weaponType, vecOrigin, fForce, NULL, pTarget );
                    if ( pProjectile )
                    {
                        // Set our intiation data, which will be used on the next frame
                        pProjectile->Initiate ( &vecOrigin, pvecRotation, pvecVelocity, usModel );
                        pProjectile->SetParent ( Resource.GetResourceDynamicEntity() );
                        return pProjectile;
                    }
                    break;
                }
                default: break;
            }

            break;
        }
        default: break;
    }

    return NULL;
}


CClientColCircle* CStaticFunctionDefinitions::CreateColCircle ( CResource& Resource, const CVector& vecPosition, float fRadius )
{
    CClientColCircle* pShape = new CClientColCircle ( m_pManager, INVALID_ELEMENT_ID, vecPosition, fRadius );
    pShape->SetParent ( Resource.GetResourceDynamicEntity () );
    //CStaticFunctionDefinitions::RefreshColShapeColliders ( pShape );   ** Not applied to maintain compatibility with existing scrips **
    return pShape;
}


CClientColCuboid* CStaticFunctionDefinitions::CreateColCuboid ( CResource& Resource, const CVector& vecPosition, const CVector& vecSize )
{
    CClientColCuboid* pShape = new CClientColCuboid ( m_pManager, INVALID_ELEMENT_ID, vecPosition, vecSize );
    pShape->SetParent ( Resource.GetResourceDynamicEntity () );
    //CStaticFunctionDefinitions::RefreshColShapeColliders ( pShape );   ** Not applied to maintain compatibility with existing scrips **
    return pShape;
}


CClientColSphere* CStaticFunctionDefinitions::CreateColSphere ( CResource& Resource, const CVector& vecPosition, float fRadius )
{
    CClientColSphere* pShape = new CClientColSphere ( m_pManager, INVALID_ELEMENT_ID, vecPosition, fRadius );
    pShape->SetParent ( Resource.GetResourceDynamicEntity () );
    //CStaticFunctionDefinitions::RefreshColShapeColliders ( pShape );   ** Not applied to maintain compatibility with existing scrips **
    return pShape;
}


CClientColRectangle* CStaticFunctionDefinitions::CreateColRectangle ( CResource& Resource, const CVector& vecPosition, const CVector2D& vecSize )
{
    CClientColRectangle* pShape = new CClientColRectangle ( m_pManager, INVALID_ELEMENT_ID, vecPosition, vecSize );
    pShape->SetParent ( Resource.GetResourceDynamicEntity () );
    //CStaticFunctionDefinitions::RefreshColShapeColliders ( pShape );   ** Not applied to maintain compatibility with existing scrips **
    return pShape;
}


CClientColPolygon* CStaticFunctionDefinitions::CreateColPolygon ( CResource& Resource, const CVector& vecPosition )
{
    CClientColPolygon * pShape = new CClientColPolygon ( m_pManager, INVALID_ELEMENT_ID, vecPosition );
    pShape->SetParent ( Resource.GetResourceDynamicEntity () );
    //CStaticFunctionDefinitions::RefreshColShapeColliders ( pShape );   ** Not applied to maintain compatibility with existing scrips **
    return pShape;
}


CClientColTube* CStaticFunctionDefinitions::CreateColTube ( CResource& Resource, const CVector& vecPosition, float fRadius, float fHeight )
{
    CClientColTube* pShape = new CClientColTube ( m_pManager, INVALID_ELEMENT_ID, vecPosition, fRadius, fHeight );
    pShape->SetParent ( Resource.GetResourceDynamicEntity () );
    //CStaticFunctionDefinitions::RefreshColShapeColliders ( pShape );   ** Not applied to maintain compatibility with existing scrips **
    return pShape;
}


// Make sure all colliders for a colshape are up to date
void CStaticFunctionDefinitions::RefreshColShapeColliders ( CClientColShape * pColShape )
{
    CVector vecRootPosition;
    m_pRootEntity->GetPosition ( vecRootPosition );
    m_pColManager->DoHitDetection ( vecRootPosition, 0.0f, m_pRootEntity, pColShape, true );
}


CClientColShape* CStaticFunctionDefinitions::GetElementColShape ( CClientEntity* pEntity )
{
    assert ( pEntity );

    CClientColShape* pColShape = NULL;
    switch ( pEntity->GetType () )
    {
        case CCLIENTMARKER:
            pColShape = static_cast < CClientMarker* > ( pEntity )->GetColShape ();
            break;
        case CCLIENTPICKUP:
            pColShape = static_cast < CClientPickup* > ( pEntity )->GetColShape ();
            break;
    }
    return pColShape;
}


bool CStaticFunctionDefinitions::GetWeaponNameFromID ( unsigned char ucID, char* szName, unsigned short len )
{
    if ( ucID <= 59 )
    {
        // Grab the name and check it's length
        const char* szNamePointer = CWeaponNames::GetWeaponName ( ucID );
        if ( strlen ( szNamePointer ) > 0 )
        {
            strncpy ( szName, szNamePointer, len );
            if (len)
                szName[len-1] = '\0';
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetWeaponIDFromName ( const char* szName, unsigned char& ucID )
{
    // Grab the weapon id
    ucID = CWeaponNames::GetWeaponID ( szName );
    return ucID != 0xFF;
}


bool CStaticFunctionDefinitions::GetTickCount_ ( double& dCount )
{
    dCount = static_cast < double > ( GetTickCount64_() );
    return true;
}


bool CStaticFunctionDefinitions::IsPlayerMapForced ( bool & bForced )
{
    bForced = m_pRadarMap->GetForcedState ();
    return true;
}


bool CStaticFunctionDefinitions::IsPlayerMapVisible ( bool & bVisible )
{
    bVisible = m_pRadarMap->IsRadarShowing ();
    return true;
}

bool CStaticFunctionDefinitions::GetPlayerMapBoundingBox ( CVector &vecMin, CVector &vecMax )
{
    if ( m_pRadarMap->GetBoundingBox ( vecMin, vecMax ) )
    {
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::FxAddBlood ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness )
{
    g_pGame->GetFx ()->AddBlood ( vecPosition, vecDirection, iCount, fBrightness );
    return true;
}


bool CStaticFunctionDefinitions::FxAddWood ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness )
{
    g_pGame->GetFx ()->AddWood ( vecPosition, vecDirection, iCount, fBrightness );
    return true;
}


bool CStaticFunctionDefinitions::FxAddSparks ( CVector & vecPosition, CVector & vecDirection, float fForce, int iCount, CVector vecAcrossLine, bool bBlur, float fSpread, float fLife )
{
    g_pGame->GetFx ()->AddSparks ( vecPosition, vecDirection, fForce, iCount, vecAcrossLine, ( bBlur ) ? 0 : 1, fSpread, fLife );
    return true;
}


bool CStaticFunctionDefinitions::FxAddTyreBurst ( CVector & vecPosition, CVector & vecDirection )
{
    g_pGame->GetFx ()->AddTyreBurst ( vecPosition, vecDirection );
    return true;
}


bool CStaticFunctionDefinitions::FxAddBulletImpact ( CVector & vecPosition, CVector & vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity )
{
    g_pGame->GetFx ()->AddBulletImpact ( vecPosition, vecDirection, iSmokeSize, iSparkCount, fSmokeIntensity );
    return true;
}


bool CStaticFunctionDefinitions::FxAddPunchImpact ( CVector & vecPosition, CVector & vecDirection )
{
    g_pGame->GetFx ()->AddPunchImpact ( vecPosition, vecDirection, 1 );
    return true;
}


bool CStaticFunctionDefinitions::FxAddDebris ( CVector & vecPosition, RwColor & rwColor, float fScale, int iCount )
{
    g_pGame->GetFx ()->AddDebris ( vecPosition, rwColor, fScale, iCount );
    return true;
}


bool CStaticFunctionDefinitions::FxAddGlass ( CVector & vecPosition, RwColor & rwColor, float fScale, int iCount )
{
    g_pGame->GetFx ()->AddGlass ( vecPosition, rwColor, fScale, iCount );
    return true;
}


bool CStaticFunctionDefinitions::FxAddWaterHydrant ( CVector & vecPosition )
{
    g_pGame->GetFx ()->TriggerWaterHydrant ( vecPosition );
    return true;
}


bool CStaticFunctionDefinitions::FxAddGunshot ( CVector & vecPosition, CVector & vecDirection, bool bIncludeSparks )
{
    g_pGame->GetFx ()->TriggerGunshot ( NULL, vecPosition, vecDirection, bIncludeSparks );
    return true;
}


bool CStaticFunctionDefinitions::FxAddTankFire ( CVector & vecPosition, CVector & vecDirection )
{
    g_pGame->GetFx ()->TriggerTankFire ( vecPosition, vecDirection );
    return true;
}


bool CStaticFunctionDefinitions::FxAddWaterSplash ( CVector & vecPosition )
{
    g_pGame->GetFx ()->TriggerWaterSplash ( vecPosition );
    return true;
}


bool CStaticFunctionDefinitions::FxAddBulletSplash ( CVector & vecPosition )
{
    g_pGame->GetFx ()->TriggerBulletSplash ( vecPosition );
    return true;
}


bool CStaticFunctionDefinitions::FxAddFootSplash ( CVector & vecPosition )
{
    g_pGame->GetFx ()->TriggerFootSplash ( vecPosition );
    return true;
}


CClientSound* CStaticFunctionDefinitions::PlaySound ( CResource* pResource, const SString& strSound, bool bIsURL, bool bLoop )
{
    CClientSound* pSound = m_pSoundManager->PlaySound2D ( strSound, bIsURL, bLoop );
    if ( pSound ) pSound->SetParent ( pResource->GetResourceDynamicEntity() );
    return pSound;
}


CClientSound* CStaticFunctionDefinitions::PlaySound3D ( CResource* pResource, const SString& strSound, bool bIsURL, const CVector& vecPosition, bool bLoop )
{
    CClientSound* pSound = m_pSoundManager->PlaySound3D ( strSound, bIsURL, vecPosition, bLoop );
    if ( pSound ) pSound->SetParent ( pResource->GetResourceDynamicEntity() );
    return pSound;
}


bool CStaticFunctionDefinitions::StopSound ( CClientSound& Sound )
{
    Sound.Stop ();
    return true;
}


bool CStaticFunctionDefinitions::SetSoundPosition ( CClientSound& Sound, unsigned int uiPosition )
{
    Sound.SetPlayPosition ( uiPosition );
    return true;
}


bool CStaticFunctionDefinitions::GetSoundPosition ( CClientSound& Sound, unsigned int& uiPosition )
{
    uiPosition = Sound.GetPlayPosition ();
    return true;
}


bool CStaticFunctionDefinitions::GetSoundLength ( CClientSound& Sound, unsigned int& uiLength )
{
    uiLength = Sound.GetLength ();
    return true;
}


bool CStaticFunctionDefinitions::SetSoundPaused ( CClientSound& Sound, bool bPaused )
{
    Sound.SetPaused ( bPaused );
    return true;
}


bool CStaticFunctionDefinitions::IsSoundPaused ( CClientSound& Sound, bool& bPaused )
{
    bPaused = Sound.IsPaused ();
    return true;
}


bool CStaticFunctionDefinitions::SetSoundVolume ( CClientSound& Sound, float fVolume )
{
    Sound.SetVolume ( fVolume );
    return true;
}


bool CStaticFunctionDefinitions::GetSoundVolume ( CClientSound& Sound, float& fVolume )
{
    fVolume = Sound.GetVolume ();
    return true;
}


bool CStaticFunctionDefinitions::SetSoundSpeed ( CClientSound& Sound, float fSpeed )
{
    Sound.SetPlaybackSpeed ( fSpeed );
    return true;
}


bool CStaticFunctionDefinitions::GetSoundSpeed ( CClientSound& Sound, float& fSpeed )
{
    fSpeed = Sound.GetPlaybackSpeed ();
    return true;
}


bool CStaticFunctionDefinitions::SetSoundMinDistance ( CClientSound& Sound, float fDistance )
{
    Sound.SetMinDistance ( fDistance );
    return true;
}


bool CStaticFunctionDefinitions::GetSoundMinDistance ( CClientSound& Sound, float& fDistance )
{
    fDistance = Sound.GetMinDistance ();
    return true;
}


bool CStaticFunctionDefinitions::SetSoundMaxDistance ( CClientSound& Sound, float fDistance )
{
    Sound.SetMaxDistance ( fDistance );
    return true;
}


bool CStaticFunctionDefinitions::GetSoundMaxDistance ( CClientSound& Sound, float& fDistance )
{
    fDistance = Sound.GetMaxDistance ();
    return true;
}

bool CStaticFunctionDefinitions::GetSoundMetaTags ( CClientSound& Sound, const SString& strFormat, SString& strMetaTags )
{
    strMetaTags = Sound.GetMetaTags ( strFormat );
    return true;
}

bool CStaticFunctionDefinitions::SetSoundEffectEnabled ( CClientSound& Sound, const SString& strEffectName, bool bEnable )
{
    int iFxEffect = m_pSoundManager->GetFxEffectFromName ( strEffectName );

    if ( iFxEffect >= 0 )
        if ( Sound.SetFxEffect ( iFxEffect, bEnable ) )
            return true;

    return false;
}

#ifdef MTA_VOICE
/**
 * Used by clients to set the enabled state of the voice system.  
 * Enabling the voice system starts recording of voice data from the
 * microphone.
 * 
 * @param bEnabled The enabled state of the mic.
 *
 * @return This method always returns true.
 */
bool CStaticFunctionDefinitions::SetVoiceInputEnabled ( bool bEnabled )
{
    CVoice * pVoice = m_pClientGame->GetVoice();

    // Voice module may not be loaded.
    if ( NULL != pVoice )
    {
        pVoice->EnableInput ( bEnabled );
    }

    return true;
}

/**
 * Sets all players muted.
 * 
 * @param bEnabled The muted enable state.
 *
 * @return This function always returns true.
 */

bool CStaticFunctionDefinitions::SetVoiceMuteAllEnabled ( bool bEnabled )
{
    CVoice * pVoice = m_pClientGame->GetVoice();

    // Voice module may not be loaded.
    if ( NULL != pVoice )
    {
        pVoice->SetMuted ( bEnabled );
    }

    return true;
}
#endif

/** Version functions **/
unsigned long CStaticFunctionDefinitions::GetVersion ()
{
    return MTA_DM_VERSION;
}

const char* CStaticFunctionDefinitions::GetVersionString ()
{
    return MTA_DM_VERSIONSTRING;
}

const char* CStaticFunctionDefinitions::GetVersionName ()
{
    return MTA_DM_FULL_STRING;
}

SString CStaticFunctionDefinitions::GetVersionBuildType ()
{
    SString strResult = MTA_DM_BUILDTYPE;
    strResult[0] = toupper ( strResult[0] );
    return strResult;
}

unsigned long CStaticFunctionDefinitions::GetNetcodeVersion ()
{
    return MTA_DM_NETCODE_VERSION;
}

const char* CStaticFunctionDefinitions::GetOperatingSystemName ()
{
    return MTA_OS_STRING;
}

const char* CStaticFunctionDefinitions::GetVersionBuildTag ()
{
    return MTA_DM_BUILDTAG_LONG;
}

SString CStaticFunctionDefinitions::GetVersionSortable ()
{
    unsigned short usNetRev = g_pCore->GetNetwork ()->GetNetRev ();
    return SString ( "%d.%d.%d-%d.%05d.%d"
                            ,MTASA_VERSION_MAJOR
                            ,MTASA_VERSION_MINOR
                            ,MTASA_VERSION_MAINTENANCE
                            ,MTASA_VERSION_TYPE
                            ,MTASA_VERSION_BUILD
                            ,usNetRev
                            );
}


/* Handling functions */

eHandlingProperty CStaticFunctionDefinitions::GetVehicleHandlingEnum ( std::string strProperty )
{
    eHandlingProperty eProperty = g_pGame->GetHandlingManager ( )->GetPropertyEnumFromName ( strProperty );
    if ( eProperty )
    {
        return eProperty;
    }
    return HANDLING_MAX;
}

bool CStaticFunctionDefinitions::GetVehicleHandling ( CClientVehicle* pVehicle, eHandlingProperty eProperty, CVector& vecValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( eProperty == HANDLING_CENTEROFMASS )
    {
        vecValue = pEntry->GetCenterOfMass ();
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GetVehicleHandling ( CClientVehicle* pVehicle, eHandlingProperty eProperty, float &fValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( GetEntryHandling ( pEntry, eProperty, fValue ) )
        return true;

    return false;
}


bool CStaticFunctionDefinitions::GetVehicleHandling ( CClientVehicle* pVehicle, eHandlingProperty eProperty, std::string& strValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( GetEntryHandling ( pEntry, eProperty, strValue ) )
        return true;

    return false;
}

bool CStaticFunctionDefinitions::GetVehicleHandling ( CClientVehicle* pVehicle, eHandlingProperty eProperty, unsigned int& uiValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( GetEntryHandling ( pEntry, eProperty, uiValue ) )
        return true;

    return false;
}

bool CStaticFunctionDefinitions::GetVehicleHandling ( CClientVehicle* pVehicle, eHandlingProperty eProperty, unsigned char& ucValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( GetEntryHandling ( pEntry, eProperty, ucValue ) )
        return true;

    return false;
}

bool CStaticFunctionDefinitions::GetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, float &fValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_MASS:
                fValue = pEntry->GetMass ();
                break;
            case HANDLING_TURNMASS:
                fValue = pEntry->GetTurnMass ();
                break;
            case HANDLING_DRAGCOEFF:
                fValue = pEntry->GetDragCoeff ();
                break;
            case HANDLING_TRACTIONMULTIPLIER:
                fValue = pEntry->GetTractionMultiplier ();
                break;
            case HANDLING_ENGINEACCELERATION:
                fValue = pEntry->GetEngineAcceleration ();
                break;
            case HANDLING_ENGINEINERTIA:
                fValue = pEntry->GetEngineInertia ();
                break;
            case HANDLING_MAXVELOCITY:
                fValue = pEntry->GetMaxVelocity ();
                break;
            case HANDLING_BRAKEDECELERATION:
                fValue = pEntry->GetBrakeDeceleration ();
                break;
            case HANDLING_BRAKEBIAS:
                fValue = pEntry->GetBrakeBias ();
                break;
            case HANDLING_STEERINGLOCK:
                fValue = pEntry->GetSteeringLock ();
                break;
            case HANDLING_TRACTIONLOSS:
                fValue = pEntry->GetTractionLoss ();
                break;
            case HANDLING_TRACTIONBIAS:
                fValue = pEntry->GetTractionBias ();
                break;
            case HANDLING_SUSPENSION_FORCELEVEL:
                fValue = pEntry->GetSuspensionForceLevel ();
                break;
            case HANDLING_SUSPENSION_DAMPING:
                fValue = pEntry->GetSuspensionDamping ();
                break;
            case HANDLING_SUSPENSION_HIGHSPEEDDAMPING:
                fValue = pEntry->GetSuspensionHighSpeedDamping ();
                break;
            case HANDLING_SUSPENSION_UPPER_LIMIT:
                fValue = pEntry->GetSuspensionUpperLimit ();
                break;
            case HANDLING_SUSPENSION_LOWER_LIMIT:
                fValue = pEntry->GetSuspensionLowerLimit ();
                break;
            case HANDLING_SUSPENSION_FRONTREARBIAS:
                fValue = pEntry->GetSuspensionFrontRearBias ();
                break;
            case HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER:
                fValue = pEntry->GetSuspensionAntiDiveMultiplier ();
                break;
            case HANDLING_COLLISIONDAMAGEMULTIPLIER:
                fValue = pEntry->GetCollisionDamageMultiplier ();
                break;
            case HANDLING_SEATOFFSETDISTANCE:
                fValue = pEntry->GetSeatOffsetDistance ();
                break;
            case HANDLING_ABS: // bool
                fValue = (float)(pEntry->GetABS () ? 1 : 0);
                break;
            default:
                return false;
        }
    }
    return true;
}

bool CStaticFunctionDefinitions::GetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned int &uiValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_PERCENTSUBMERGED: // unsigned int
                uiValue = pEntry->GetPercentSubmerged ();
                break;
            case HANDLING_MONETARY:
                uiValue = pEntry->GetMonetary ();
                break;
            case HANDLING_HANDLINGFLAGS:
                uiValue = pEntry->GetHandlingFlags ();
                break;
            case HANDLING_MODELFLAGS:
                uiValue = pEntry->GetModelFlags ();
                break;
            default:
                return false;
        }
    }
    return true;
}

bool CStaticFunctionDefinitions::GetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned char &ucValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_NUMOFGEARS:
                ucValue = pEntry->GetNumberOfGears ();
                break;
            case HANDLING_ANIMGROUP:
                ucValue = pEntry->GetAnimGroup ();
                break;
            default:
                return false;
        }
    }
    return true;
}

bool CStaticFunctionDefinitions::GetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, std::string& strValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_DRIVETYPE:
            {
                CHandlingEntry::eDriveType eDriveType = pEntry->GetCarDriveType ();
                if ( eDriveType == CHandlingEntry::FWD )
                    strValue = "fwd";
                else if ( eDriveType == CHandlingEntry::RWD )
                    strValue = "rwd";
                else if (eDriveType == CHandlingEntry::FOURWHEEL )
                    strValue = "awd";
                else
                    return false;
                break;
            }
            case HANDLING_ENGINETYPE:
            {
                CHandlingEntry::eEngineType eEngineType = pEntry->GetCarEngineType ();
                if ( eEngineType == CHandlingEntry::PETROL )
                    strValue = "petrol";
                else if ( eEngineType == CHandlingEntry::DIESEL )
                    strValue = "diesel";
                else if ( eEngineType == CHandlingEntry::ELECTRIC )
                    strValue = "electric";
                else
                    return false;
                break;
            }
            case HANDLING_HEADLIGHT:
            {
                CHandlingEntry::eLightType eHeadType = pEntry->GetHeadLight ();
                if ( eHeadType == CHandlingEntry::SMALL )
                    strValue = "small";
                else if ( eHeadType == CHandlingEntry::LONG )
                    strValue = "long";
                else if ( eHeadType == CHandlingEntry::BIG )
                    strValue = "big";
                else if ( eHeadType == CHandlingEntry::TALL )
                    strValue = "tall";
                else
                    return false;
                break;
            }
            case HANDLING_TAILLIGHT:
            {
                CHandlingEntry::eLightType eTailType = pEntry->GetTailLight ();
                if ( eTailType == CHandlingEntry::SMALL )
                    strValue = "small";
                else if ( eTailType == CHandlingEntry::LONG )
                    strValue = "long";
                else if ( eTailType == CHandlingEntry::BIG )
                    strValue = "big";
                else if ( eTailType == CHandlingEntry::TALL )
                    strValue = "tall";
                else
                    return false;
                break;
            }
            default:
                return false;
        }
    }
    else
        return false;

    return true;
}
