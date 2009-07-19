/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPickup.cpp
*  PURPOSE:     Pickup entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CPickup::CPickup ( CElement* pParent, CXMLNode* pNode, CPickupManager* pPickupManager, CColManager* pColManager ) : CElement ( pParent, pNode )
{
    // Init
    m_pPickupManager = pPickupManager;
    m_pCollision = new CColSphere ( pColManager, NULL, m_vecPosition, 2.0f, NULL, true );
    m_pCollision->SetCallback ( this );
    m_pCollision->SetAutoCallEvent ( false );

    // Add us to the pickup manager's list and grab an unique id
    pPickupManager->AddToList ( this );

    // Initialize our stuff to defaults
    m_iType = CElement::PICKUP;
    SetTypeName ( "pickup" );
    m_ucType = CPickup::WEAPON;
    m_ucWeaponType = CPickup::WEAPON_BRASSKNUCKLE;
    m_usAmmo = 0;
    m_fAmount = 0;
    m_ulRespawnIntervals = 30000;
    m_ulLastUsedTime = 0;
    m_usModel = CPickupManager::GetWeaponModel ( m_ucWeaponType );
    m_bVisible = true;
    m_bSpawned = true;

    m_bIsTypeRandom = false;
    m_bIsWeaponTypeRandom = false;
    m_bIsHealthRandom = false;
}


CPickup::~CPickup ( void )
{
    // Delete our collision object
    if ( m_pCollision )
        delete m_pCollision;

    // Unlink from manager
    Unlink ();
}


void CPickup::Unlink ( void )
{
    // Remove us from the pickup manager's list
    m_pPickupManager->RemoveFromList ( this );
}


bool CPickup::ReadSpecialData ( void )
{
	unsigned short usBuffer = 0;
    // Grab the "posX" data
    if ( !GetCustomDataFloat ( "posX", m_vecPosition.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posX' attribute in <pickup> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posY" data
    if ( !GetCustomDataFloat ( "posY", m_vecPosition.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posY' attribute in <pickup> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posZ" data
    if ( !GetCustomDataFloat ( "posZ", m_vecPosition.fZ, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posZ' attribute in <pickup> (line %u)\n", m_uiLine );
        return false;
    }

    // Put the collision object at the given xyz
    if ( m_pCollision )
        m_pCollision->SetPosition ( m_vecPosition );

    // Grab the "type" data
    char szBuffer [128];
    if ( GetCustomDataString ( "type", szBuffer, 128, true ) )
    {
        // Check what type it is
        m_bIsTypeRandom = false;
        if ( stricmp ( szBuffer, "health" ) == 0 )
        {
            m_ucType = HEALTH;
            m_usModel = CPickupManager::GetHealthModel ();
        }
        else if ( stricmp ( szBuffer, "armor" ) == 0 )
        {
            m_ucType = ARMOR;
            m_usModel = CPickupManager::GetArmorModel ();
        }
		else if ( IsNumericString ( szBuffer ) )
		{	// could be a weapon
			usBuffer = atoi ( szBuffer );
			if ( CPickupManager::IsValidWeaponID ( usBuffer ) )
			{ // its a weapon
				m_ucType = WEAPON;
				m_usModel = CPickupManager::GetWeaponModel ( m_ucWeaponType );
			}
		}
        else if ( stricmp ( szBuffer, "custom" ) == 0 )
        {
            m_ucType = CUSTOM;
            m_usModel = 1700;
        }
        else if ( stricmp ( szBuffer, "random" ) == 0 )
        {
            m_ucType = HEALTH;
            m_usModel = CPickupManager::GetHealthModel ();
            m_bIsTypeRandom = true;
        }
        else
        {
            CLogger::LogPrintf ( "WARNING: Unknown 'type' value in <pickup>; defaulting to \"random\" (line %u)\n", m_uiLine );

            m_ucType = HEALTH;
            m_usModel = CPickupManager::GetHealthModel ();
            m_bIsTypeRandom = true;
        }
    }
    else
    {
        CLogger::ErrorPrintf ( "Bad/missing 'type' attribute in <pickup> (line %u)\n", m_uiLine );
        return false;
    }

    // Is this a weapon pickup?
    if ( m_ucType == CPickup::WEAPON || m_bIsTypeRandom )
    {
            // Remember the weapon type
            m_ucWeaponType = static_cast < unsigned char > ( usBuffer );
            m_usModel = CPickupManager::GetWeaponModel ( m_ucWeaponType );
            m_bIsWeaponTypeRandom = false;
    }

    // Is this a health pickup?
    if ( m_ucType == CPickup::HEALTH || m_ucType == CPickup::ARMOR || m_bIsTypeRandom )
    {
        // Grab the "health" data
        if ( GetCustomDataString ( "amount", szBuffer, 128, true ) )
        {
            // Is it random?
            if ( strcmp ( szBuffer, "random" ) == 0 )
            {
                m_fAmount = 100.0f;
                m_bIsHealthRandom = true;
            }
            else
            {
                // Convert the health to an integer and limit it to 100
                m_fAmount = static_cast < float > ( atoi ( szBuffer ) );
                if ( m_fAmount > 100.0f ) m_fAmount = 100.0f;
            }
        }
        else
        {
            m_fAmount = 100.0f;
            m_bIsHealthRandom = false;
        }
    }

    
    // Is this a weapon pickup?
    int iTemp;
    if ( m_ucType == CPickup::WEAPON || m_bIsTypeRandom )
    {
        // Grab the "ammo" data
        if ( GetCustomDataInt ( "amount", iTemp, true ) )
        {
            // Limit it to 0-9999 if it was above
            if ( iTemp > 9999 )
                iTemp = 9999;
            else if ( iTemp < 0 )
                iTemp = 0;

            // Remember it
            m_usAmmo = static_cast < unsigned short > ( iTemp );
        }
        else
        {
            m_usAmmo = 100;
        }
    }

    // Grab the "respawn" data
    if ( GetCustomDataInt ( "respawn", iTemp, true ) )
    {
        // Make sure it's above 3 seconds
        if ( iTemp < 3000 )
            iTemp = 3000;

        // Remember it
        m_ulRespawnIntervals = static_cast < unsigned long > ( iTemp );
    }
    else
    {
        m_ulRespawnIntervals = 10000;
    }

    // Is this a custom pickup?
    if ( m_ucType == CPickup::CUSTOM )
    {
        // Grab the "model" data
        if ( GetCustomDataInt ( "model", iTemp, true ) )
        {
            // Valid id?
            if ( CObjectManager::IsValidModel ( iTemp ) || iTemp == 370 ) // 370 = jetpack - sort of a hack
            {
                // Set the object id
                m_usModel = static_cast < unsigned short > ( iTemp );
            }
            else
            {
                CLogger::ErrorPrintf ( "Bad 'model' id specified in <pickup> (line %u)\n", m_uiLine );
                return false;
            }
        }
        else
        {
            // Error out if custom is specified but no model id is
            CLogger::ErrorPrintf ( "Pickup type set to 'custom' but no 'model' id specified (line %u)\n", m_uiLine );
            return false;
        }
    }

	if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    // Success
    return true;
}


void CPickup::SetPosition ( const CVector& vecPosition )
{
    m_vecLastPosition = m_vecPosition;
    m_vecPosition = vecPosition;
    if ( m_pCollision )
        m_pCollision->SetPosition ( vecPosition );
}


void CPickup::SetPickupType ( unsigned char ucType )
{
    m_ucType = ucType;

    switch ( ucType )
    {
        case CPickup::HEALTH:
            m_usModel = CPickupManager::GetHealthModel ();
            break;

        case CPickup::ARMOR:
            m_usModel = CPickupManager::GetArmorModel ();
            break;

        case CPickup::WEAPON:
            m_usModel = CPickupManager::GetWeaponModel ( m_ucWeaponType );
            break;

        case CPickup::CUSTOM:
            if ( m_usModel == 0 )
            {
                m_usModel = 1700;
            }

            break;
    }
}


void CPickup::SetWeaponType ( unsigned char ucWeaponType )
{
    m_ucWeaponType = ucWeaponType;

    if ( m_ucType == CPickup::WEAPON )
    {
        m_usModel = CPickupManager::GetWeaponModel ( ucWeaponType );
    }
}


void CPickup::SetVisible ( bool bVisible )
{
    m_bVisible = bVisible;
    if ( m_pCollision )
        m_pCollision->SetEnabled ( bVisible );
}


void CPickup::Randomize ( void )
{
    // Randomize type
    if ( m_bIsTypeRandom )
    {
        m_ucType = static_cast < unsigned char > ( GetRandom ( 0, CPickup::CUSTOM - 1 ) );
        switch ( m_ucType )
        {
            case CPickup::HEALTH:
                m_usModel = 1240;
                break;

            case CPickup::ARMOR:
                m_usModel = 1242;
                break;
        }
    }

    // Randomize weapon and ammo if the type is "weapon"
    if ( m_bIsWeaponTypeRandom && m_ucType == CPickup::WEAPON )
    {
        // There are 42 different weapons
        m_ucWeaponType = static_cast < unsigned char > ( GetRandom ( 1, 42 ) );

        // Eventually skip the "rocket" weapons (skip 13, 19 and 20)
        if ( m_ucWeaponType > 12 )
        {
            ++m_ucWeaponType;
        }

        if ( m_ucWeaponType > 18 )
        {
            m_ucWeaponType += 3;
        }

        m_usModel = CPickupManager::GetWeaponModel ( m_ucWeaponType );
    }
    else
    {
        // Eventually randomize the health
        if ( m_bIsHealthRandom )
        {
            m_fAmount = static_cast < float > ( GetRandom ( 0, 100 ) );
        }
    }
}


bool CPickup::CanUse ( CPlayer& Player, bool bOnfootCheck )
{
    // Is the player on foot?
    if ( !bOnfootCheck || !Player.GetOccupiedVehicle () )
    {
        // Check if he can pick it up depending on the type
        switch ( m_ucType )
        {
            case CPickup::HEALTH:
                // TODO: calc max health from max_health stat
                return ( Player.GetHealth () < 200.0f );

            case CPickup::ARMOR:
                return ( Player.GetArmor () < 100.0f );

            case CPickup::WEAPON:
                return true;

            case CPickup::CUSTOM:
                return true;
        }
    }

    return false;
}


void CPickup::Use ( CPlayer& Player )
{
    // Call the onPickupUse event
    CLuaArguments Arguments;
    Arguments.PushElement ( &Player );
    if ( !CallEvent ( "onPickupUse", Arguments ) )
    {
        CLuaArguments Arguments2;
        Arguments2.PushElement ( this );      // pickup
        Player.CallEvent ( "onPlayerPickupUse", Arguments2 );
    }
    else
    {
        CLuaArguments Arguments2;
        Arguments2.PushElement ( this );      // pickup
        if ( Player.CallEvent ( "onPlayerPickupUse", Arguments2 ) )
        {
            // Tell all the other players to hide it if the respawn intervals are bigger than 0
            if ( m_ulRespawnIntervals > 0 )
            {
                // Save our last used time
                m_ulLastUsedTime = GetTime ();
                // Mark us as not spawned
                m_bSpawned = false;
        
                // Mark us as hidden
                SetVisible ( false );
            }
        
            // Tell him to play the sound and hide/show it
            Player.Send ( CPickupHitConfirmPacket ( this, true ) );
        
            // Tell everyone else to hide/show it as neccessary
            g_pGame->GetPlayerManager ()->BroadcastOnlyJoined ( CPickupHitConfirmPacket ( this, false ), &Player );
        
            // Handle it depending on the type
            switch ( m_ucType )
            {
                // Health pickup?
                case CPickup::HEALTH:
                {
                    float fHealth = Player.GetHealth ();
                    float fNewHealth = fHealth + m_fAmount;
                    if ( fNewHealth > 200.0f )
                        fNewHealth = 200.0f;

                    CStaticFunctionDefinitions::SetElementHealth ( &Player, fNewHealth );

                    break;
                }

                // Armor pickup?
                case CPickup::ARMOR:
                {
                    float fArmor = Player.GetArmor ();
                    float fNewArmor = fArmor + m_fAmount;
                    if ( fNewArmor > 100.0f )
                        fNewArmor = 100.0f;

                    CStaticFunctionDefinitions::SetPedArmor ( &Player, fNewArmor );

                    break;
                }

                // Weapon pickup?
                case CPickup::WEAPON:
                {
                    // Give him the weapon
                    CStaticFunctionDefinitions::GiveWeapon ( &Player, m_ucWeaponType, m_usAmmo );
                    break;
                }
                default: break;
            }
        }
    }
}


void CPickup::Callback_OnCollision ( CColShape& Shape, CElement& Element )
{
    if ( IS_PLAYER ( &Element ) )
    {
        CPlayer& Player = static_cast < CPlayer& > ( Element );

        // Is he alive?
        if ( !Player.IsDead () )
        {
            // Are they both in the same dimension?
            if ( GetDimension () == Element.GetDimension () )
            {
                // Call the onPickupHit event
                CLuaArguments Arguments;
                Arguments.PushElement ( &Player );
                bool bContinue1 = CallEvent ( "onPickupHit", Arguments );

                CLuaArguments Arguments2;
                Arguments2.PushElement ( this );       // pickup
                bool bContinue2 = Element.CallEvent ( "onPlayerPickupHit", Arguments2 );

                if ( bContinue1 && bContinue2 )
                {
                    // Does it still exist?
                    if ( !IsBeingDeleted () )
                    {
                        // Can we USE the pickup?
                        if ( CanUse ( Player ) )
                        {
                            // USE the pickup
                            Use ( Player );
                        }
                    }
                }
            }
        }
    }
}


void CPickup::Callback_OnLeave ( CColShape& Shape, CElement& Element )
{
    if ( IS_PLAYER ( &Element ) )
    {
        CPlayer& Player = static_cast < CPlayer& > ( Element );

        // Is he alive?
        if ( !Player.IsDead () )
        {
            // Call the onPickupHit event
            CLuaArguments Arguments;
            Arguments.PushElement ( &Player );
            Arguments.PushBoolean ( ( GetDimension () == Element.GetDimension () ) );
            CallEvent ( "onPickupLeave", Arguments );

            CLuaArguments Arguments2;
            Arguments2.PushElement ( this );       // pickup
            Arguments2.PushBoolean ( ( GetDimension () == Element.GetDimension () ) );
            Element.CallEvent ( "onPlayerPickupLeave", Arguments2 );            
        }
    }
}


void CPickup::Callback_OnCollisionDestroy ( CColShape* pCollision )
{
    if ( pCollision == m_pCollision )
        m_pCollision = NULL;
}