/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CPlayerRPCs.cpp
*  PURPOSE:     Player remote procedure calls
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CPlayerRPCs.h"

void CPlayerRPCs::LoadFunctions ( void )
{    
    AddHandler ( SET_PLAYER_MONEY, SetPlayerMoney, "SetPlayerMoney" );    
    AddHandler ( SHOW_PLAYER_HUD_COMPONENT, ShowPlayerHudComponent, "ShowPlayerHudComponent" );
    AddHandler ( FORCE_PLAYER_MAP, ForcePlayerMap, "ForcePlayerMap" );
    AddHandler ( SET_PLAYER_NAMETAG_TEXT, SetPlayerNametagText, "SetPlayerNametagText" );
    AddHandler ( REMOVE_PLAYER_NAMETAG_COLOR, RemovePlayerNametagColor, "RemovePlayerNametagColor" );
    AddHandler ( SET_PLAYER_NAMETAG_COLOR, SetPlayerNametagColor, "SetPlayerNametagColor" );
    AddHandler ( SET_PLAYER_NAMETAG_SHOWING, SetPlayerNametagShowing, "SetPlayerNametagShowing" ); 
}


void CPlayerRPCs::SetPlayerMoney ( NetBitStreamInterface& bitStream )
{
    // Read out the new money amount
    long lMoney;
    if ( bitStream.Read ( lMoney ) )
    {
        // Set it
        m_pClientGame->SetMoney ( lMoney );
    }
}


void CPlayerRPCs::ShowPlayerHudComponent ( NetBitStreamInterface& bitStream )
{
    unsigned char ucComponent, ucShow;
    if ( bitStream.Read ( ucComponent ) && bitStream.Read ( ucShow ) )
    {
        bool bDisabled = ( ucShow != 1 );
        enum eHudComponent { HUD_AMMO = 0, HUD_WEAPON, HUD_HEALTH, HUD_BREATH,
                             HUD_ARMOUR, HUD_MONEY, HUD_VEHICLE_NAME, HUD_AREA_NAME, HUD_RADAR, HUD_CLOCK, HUD_RADIO, HUD_WANTED, HUD_ALL };
        switch ( ucComponent )
        {
            case HUD_AMMO:
                g_pGame->GetHud ()->DisableAmmo ( bDisabled );
                break;
            case HUD_WEAPON:
                g_pGame->GetHud ()->DisableWeaponIcon ( bDisabled );
                break;
            case HUD_HEALTH:
                g_pGame->GetHud ()->DisableHealth ( bDisabled );
                break;
            case HUD_BREATH:
                g_pGame->GetHud ()->DisableBreath ( bDisabled );
                break;
            case HUD_ARMOUR:
                g_pGame->GetHud ()->DisableArmour ( bDisabled );
                break;
            case HUD_MONEY:
                g_pGame->GetHud ()->DisableMoney ( bDisabled );
                break;
            case HUD_VEHICLE_NAME:
                g_pGame->GetHud ()->DisableVehicleName ( bDisabled );
                break;
            case HUD_AREA_NAME:
                g_pClientGame->SetHudAreaNameDisabled ( bDisabled );
                g_pGame->GetHud ()->DisableAreaName ( bDisabled );
                break;
            case HUD_RADAR:
                g_pGame->GetHud ()->DisableRadar ( bDisabled );
                break;
            case HUD_CLOCK:
                g_pGame->GetHud ()->DisableClock ( bDisabled );
                break;
            case HUD_RADIO:
                g_pGame->GetHud ()->DisableRadioName ( bDisabled );
                break;
            case HUD_WANTED:
                g_pGame->GetHud ()->DisableWantedLevel ( bDisabled );
                break;
            case HUD_ALL:
                g_pClientGame->SetHudAreaNameDisabled ( bDisabled );
                g_pGame->GetHud ()->DisableAll ( bDisabled );
                break;
        }
    }        
}


void CPlayerRPCs::ForcePlayerMap ( NetBitStreamInterface& bitStream )
{
    unsigned char ucVisible;
    if ( bitStream.Read ( ucVisible ) )
    {
        bool bVisible = ( ucVisible == 1 );
        m_pClientGame->GetRadarMap ()->SetForcedState ( bVisible );        
    }
}


void CPlayerRPCs::SetPlayerNametagText ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned short usTextLength;
    if ( bitStream.Read ( usTextLength ) )
    {
        CClientPlayer* pPlayer = m_pPlayerManager->Get ( pSource->GetID () );
        if ( pPlayer )
        {
            char* szText = NULL;
            if ( usTextLength > 0 )
            {
                szText = new char [ usTextLength + 1 ];                
                szText [ usTextLength ] = NULL;
                if ( bitStream.Read ( szText, usTextLength ) )
                {
                    pPlayer->SetNametagText ( szText );
                }                
                delete [] szText;
            }
            else
                pPlayer->SetNametagText ( NULL );
        }
    }
}


void CPlayerRPCs::SetPlayerNametagColor ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucR, ucG, ucB;
    if ( bitStream.Read ( ucR ) &&
         bitStream.Read ( ucG ) &&
         bitStream.Read ( ucB ) )
    {
        CClientPlayer* pPlayer = m_pPlayerManager->Get ( pSource->GetID () );
        if ( pPlayer )
            pPlayer->SetNametagOverrideColor ( ucR, ucG, ucB );
    }
}


void CPlayerRPCs::RemovePlayerNametagColor ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    CClientPlayer* pPlayer = m_pPlayerManager->Get ( pSource->GetID () );
    if ( pPlayer )
        pPlayer->RemoveNametagOverrideColor ();
}


void CPlayerRPCs::SetPlayerNametagShowing ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucShowing;
    if ( bitStream.Read ( ucShowing ) )
    {
        CClientPlayer* pPlayer = m_pPlayerManager->Get ( pSource->GetID () );
        if ( pPlayer )
        {
            pPlayer->SetNametagShowing ( ( ucShowing == 1 ) );
        }
    }
}
