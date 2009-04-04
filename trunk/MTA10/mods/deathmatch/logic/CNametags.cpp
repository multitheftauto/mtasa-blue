/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CNametags.cpp
*  PURPOSE:     Nametags renderer
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

#define MELEE_VISIBLE_RANGE         60.0f
#define AIM_VISIBLE_RANGE           300.0f
#define SNIPER_AIM_VISIBLE_RANGE    300.0f
#define WEAPON_RANGE_FACTOR         3.0f
#define NAMETAG_BEGIN_FADE_TIME     0
#define NAMETAG_END_FADE_TIME       700
#define NAMETAG_FONT_SIZE           0.8f
#define MAX_ALPHA                   180.0f // max value is 255

#define DEFAULT_VIEW_RANGE          45.0f
#define DEFAULT_VIEW_RANGE_EXP      DEFAULT_VIEW_RANGE*DEFAULT_VIEW_RANGE

CNametags::CNametags ( CClientManager* pManager )
{
    m_pPlayerStreamer = pManager->GetPlayerStreamer ();
    m_pPlayerManager = pManager->GetPlayerManager ();
    m_bDrawHealth = true;
    m_pHud = g_pGame->GetHud ();
    m_bDrawFromAim = false;
    m_usDimension = 0;
    m_bVisible = true;
}


CNametags::~CNametags ( void )
{
}


void CNametags::DoPulse ( void )
{
    if ( m_bVisible )
    {
        if ( m_bDrawFromAim )
            DrawFromAim ();
        else
            DrawDefault ();
    }
}


void CNametags::DrawFromAim ( void )
{
    unsigned long ulCurrentTime = CClientTime::GetTime ();

    // Got any players that are not local?
    if ( m_pPlayerManager->Count () > 1 )
    {
        // Grab the local player
        CClientPlayer* pLocalPlayer = m_pPlayerManager->GetLocalPlayer ();
        if ( pLocalPlayer )
        {
            // Grab the current time and the camera
            unsigned long ulCurrentTime = CClientTime::GetTime ();
            CCamera* pCamera = g_pGame->GetCamera ();

            // Grab our controller state
            CControllerState State;
            g_pGame->GetPad ()->GetCurrentControllerState ( &State );

            // Grab our current weapon slot. Use screen center if melee or none
            CVector vecStart;
            CVector vecTarget;
            eWeaponSlot eSlot = pLocalPlayer->GetCurrentWeaponSlot ();
            if ( eSlot == WEAPONSLOT_TYPE_UNARMED ||
                    eSlot == WEAPONSLOT_TYPE_MELEE ||
                    eSlot == WEAPONSLOT_TYPE_RIFLE ||
                    eSlot == WEAPONSLOT_TYPE_THROWN ||
                    eSlot == WEAPONSLOT_TYPE_SPECIAL ||
                    eSlot == WEAPONSLOT_TYPE_GIFT ||
                    eSlot == WEAPONSLOT_TYPE_PARACHUTE ||
                    eSlot == WEAPONSLOT_TYPE_DETONATOR )
            {
                // Grab the active cam
                CCamera* pCamera = g_pGame->GetCamera ();
                CCam* pActive = pCamera->GetCam ( pCamera->GetActiveCam () );

                // Grab the camera matrix
	            CMatrix matCamera;
	            pCamera->GetMatrix ( &matCamera );
                vecStart = matCamera.vPos;

                // Range
                float fRange;
                if ( eSlot == WEAPONSLOT_TYPE_RIFLE && State.RightShoulder1 )
                {
                    fRange = SNIPER_AIM_VISIBLE_RANGE;
                }
                else
                {
                    fRange = MELEE_VISIBLE_RANGE;
                }

                // Find the target position
                CVector vecFront = *pActive->GetFront ();
                vecFront.Normalize ();
                vecTarget = *pActive->GetSource () + vecFront * fRange;
            }
            else
            {
                // Grab the weapon and keysync state. If it exists and he holds Target down
                CWeapon* pPlayerWeapon = pLocalPlayer->GetWeapon ();
                if ( pPlayerWeapon && State.RightShoulder1 )
                {
                    // Grab the gun muzzle position
                    CWeaponInfo* pCurrentWeaponInfo = pPlayerWeapon->GetInfo ();
                    CVector vecGunMuzzle = *pCurrentWeaponInfo->GetFireOffset ();
                    pLocalPlayer->GetTransformedBonePosition ( BONE_RIGHTWRIST, vecGunMuzzle );

                    // Grab the target point
                    pCamera->Find3rdPersonCamTargetVector ( AIM_VISIBLE_RANGE, &vecGunMuzzle, &vecStart, &vecTarget );
                }
                else
                {
                    // Grab the active cam
                    CCam* pActive = pCamera->GetCam ( pCamera->GetActiveCam () );

                    // Grab the camera matrix
	                CMatrix matCamera;
	                pCamera->GetMatrix ( &matCamera );
                    vecStart = matCamera.vPos;

                    // Find the target position
                    CVector vecFront = *pActive->GetFront ();
                    vecFront.Normalize ();
                    vecTarget = *pActive->GetSource () + vecFront * MELEE_VISIBLE_RANGE;
                }
            }

            // Ignore the local player for this
            pLocalPlayer->WorldIgnore ( true );

            // Do the raycast
            CColPoint* pColPoint = NULL;
            CEntity* pEntity = NULL;
            g_pGame->GetWorld ()->ProcessLineOfSight ( &vecStart, &vecTarget, &pColPoint, &pEntity, true, true, true, true, true, true, false, true );
            if ( pColPoint ) pColPoint->Destroy (); 

            // Un-ignore the local player
            pLocalPlayer->WorldIgnore ( false );

            // Did we find an entity?
            if ( pEntity )
            {
                // Grab the CClientEntity belonging to this game_sa entity
                CClientEntity* pClientEntity = reinterpret_cast < CClientEntity* > ( pEntity->GetStoredPointer () );
                if ( pClientEntity )
                {
                    // Is it a vehicle? Is it a ped?
                    eClientEntityType EntityType = pClientEntity->GetType ();
                    if ( EntityType == CCLIENTVEHICLE )
                    {
                        CClientVehicle* pClientVehicle = static_cast < CClientVehicle* > ( pClientEntity );

                        // Set the current time as the last draw time for all players inside
                        CClientPed* pPed;
                        int i;
                        for ( i = 0; i < 8; i++ )
                        {
                            // Grab this seat's occupant and set its last nametag show time to now
                            pPed = pClientVehicle->GetOccupant ( i );
                            if ( pPed && pPed->GetType () == CCLIENTPLAYER )
                            {
                                static_cast < CClientPlayer* > ( pPed )->SetLastNametagShow ( ulCurrentTime );
                            }
                        }
                    }
                    else if ( EntityType == CCLIENTPLAYER )
                    {
                        // Grab the player this entity is
                        CClientPlayer* pClientPlayer = static_cast < CClientPlayer* > ( pClientEntity );
                        if ( pClientPlayer )
                        {
                            // Set now as the last time we had the cursor above him
                            pClientPlayer->SetLastNametagShow ( ulCurrentTime );
                        }
                    }
                }
            }

            // Grab the local player vehicle
            CClientVehicle* pLocalVehicle = pLocalPlayer->GetOccupiedVehicle ();

            // Draw the nametags we need to
            CClientPlayer* pPlayer;
            CClientStreamElement * pElement;
            list < CClientStreamElement* > ::const_iterator iter = m_pPlayerStreamer->ActiveElementsBegin ();
            for ( ; iter != m_pPlayerStreamer->ActiveElementsEnd (); iter++ )
            {
                pElement = *iter;
                if ( !pElement->IsStreamedIn () ) continue;
                if ( pElement->GetType () != CCLIENTPLAYER ) continue;
                pPlayer = static_cast < CClientPlayer * > ( pElement );
                if ( pPlayer->IsLocalPlayer () ) continue;

                // Is he in the same vehicle as the local player?
                if ( pLocalVehicle && pPlayer->GetOccupiedVehicle () == pLocalVehicle )
                {
                    pPlayer->SetLastNametagShow ( ulCurrentTime );
                }

                // Can we show this player's nametag
                unsigned long ulLastNametagShow = pPlayer->GetLastNametagShow ();
                if ( ulLastNametagShow != 0 && ulCurrentTime <= ulLastNametagShow + NAMETAG_END_FADE_TIME )
                {
                    unsigned long ulLastNametagShow = pPlayer->GetLastNametagShow ();
                    // Calculate the alpha modifier
                    float fAlphaTimeModifier;
                    if ( ulCurrentTime < ulLastNametagShow + NAMETAG_BEGIN_FADE_TIME )
                    {
                        fAlphaTimeModifier = 1.0f;
                    }
                    else
                    {
                        fAlphaTimeModifier = 1.0f - ( ulCurrentTime - ulLastNametagShow - NAMETAG_BEGIN_FADE_TIME ) / 1000.0f;
                    }

                    // Calculate the alpha for the nametag
                    unsigned char ucAlpha = static_cast < unsigned char > ( 180.0f * fAlphaTimeModifier );

                    // Draw it
                    DrawTagForPlayer ( pPlayer, ucAlpha );
                }
            }
        }
    }
}


void CNametags::DrawDefault ( void )
{
    // Grab the resolution width and height
    static float fResWidth = static_cast < float > ( g_pCore->GetGraphics ()->GetViewportWidth () );
    static float fResHeight = static_cast < float > ( g_pCore->GetGraphics ()->GetViewportHeight () );
        
    // Got any players that are not local?
    if ( m_pPlayerManager->Count () <= 1 ) return;

    list < CClientPlayer * > playerTags;

    // Grab the local player
    CClientPlayer* pLocalPlayer = m_pPlayerManager->GetLocalPlayer ();
    if ( !pLocalPlayer ) return;

    CClientVehicle* pSniperTargetedVehicle = NULL;
    CClientPlayer* pSniperTargetedPlayer = NULL;

    // Grab our current weapon slot. Use screen center if melee or none                
    eWeaponSlot eSlot = pLocalPlayer->GetCurrentWeaponSlot ();
    if ( eSlot >= WEAPONSLOT_TYPE_HANDGUN && eSlot <= WEAPONSLOT_TYPE_RIFLE )
    {
        CVector vecOrigin, vecTarget;
        pLocalPlayer->GetShotData ( &vecOrigin, &vecTarget ); 

        // Ignore the local player for this
        pLocalPlayer->WorldIgnore ( true );

        // Do the raycast
        CColPoint* pColPoint = NULL;
        CEntity* pEntity = NULL;
        g_pGame->GetWorld ()->ProcessLineOfSight ( &vecOrigin, &vecTarget, &pColPoint, &pEntity, true, true, true, true, true, true, false, true );
        if ( pColPoint ) pColPoint->Destroy ();

        // Un-ignore the local player
        pLocalPlayer->WorldIgnore ( false );
        

        // Did we find an entity?
        if ( pEntity )
        {
            // Grab the CClientEntity belonging to this game_sa entity
            CClientEntity* pClientEntity = reinterpret_cast < CClientEntity* > ( pEntity->GetStoredPointer () );
            if ( pClientEntity )
            {
                // Is it a vehicle? Is it a ped?
                eClientEntityType EntityType = pClientEntity->GetType ();
                switch ( EntityType )
                {
                    case CCLIENTVEHICLE:
                    {
                        pSniperTargetedVehicle = static_cast < CClientVehicle* > ( pClientEntity );
                        break;
                    }
                    case CCLIENTPLAYER:
                    {
                        pSniperTargetedPlayer = static_cast < CClientPlayer* > ( pClientEntity );
                        break;
                    }
                    default: break;
                }
            }
        }
    }

    // Grab the local player vehicle
    CClientVehicle* pLocalVehicle = pLocalPlayer->GetOccupiedVehicle ();
    CVehicle * pLocalGameVehicle = NULL;
    if ( pLocalVehicle ) pLocalGameVehicle = pLocalVehicle->GetGameVehicle ();

    CMatrix CameraMatrix;
    g_pGame->GetCamera ()->GetMatrix ( &CameraMatrix );

    // Draw the nametags we need to
    CVector vecPlayerPosition;
    CClientVehicle * pPlayerVehicle = NULL;
    float fDistanceExp;
    bool bCollision;
    CColPoint * pColPoint = NULL;
    CEntity * pGameEntity = NULL;
    CClientEntity * pEntity = NULL;
    CClientPlayer* pPlayer;
    CClientStreamElement * pElement;
    list < CClientStreamElement* > ::const_iterator iter = m_pPlayerStreamer->ActiveElementsBegin ();
    for ( ; iter != m_pPlayerStreamer->ActiveElementsEnd (); iter++ )
    {
        pElement = *iter;
        if ( !pElement->IsStreamedIn () ) continue;
        if ( pElement->GetType () != CCLIENTPLAYER ) continue;
        pPlayer = static_cast < CClientPlayer * > ( pElement );
        if ( pPlayer->IsLocalPlayer () ) continue;

        if ( pPlayer->GetStatusIcon ()->IsVisible () )
            pPlayer->GetStatusIcon ()->SetVisible ( false );

        // Get the distance from the camera
        pPlayer->GetPosition ( vecPlayerPosition );
        fDistanceExp = pPlayer->GetExpDistance ();
        pPlayerVehicle = pPlayer->GetOccupiedVehicle ();
        
        // Is he in the same vehicle as the local player?
        if ( ( pSniperTargetedPlayer == pPlayer ) ||
                ( pSniperTargetedVehicle && pSniperTargetedVehicle == pPlayer->GetOccupiedVehicle () ) ||
                ( pLocalVehicle && pLocalVehicle == pPlayerVehicle ) ||
                ( fDistanceExp < DEFAULT_VIEW_RANGE_EXP && pPlayer->IsOnScreen () ) )
        {                
            // If he's in a vehicle, get the world to ignore it for line-of-sight
            if ( pPlayerVehicle ) pPlayerVehicle->WorldIgnore ( true );

            bCollision = g_pCore->GetGame ()->GetWorld ()->ProcessLineOfSight ( &CameraMatrix.vPos, &vecPlayerPosition, &pColPoint, &pGameEntity, true, true, false, true );
            if ( !bCollision || ( pGameEntity && pGameEntity == pLocalGameVehicle ) ) 
            {
                pPlayer->SetNametagDistance ( sqrt ( fDistanceExp ) );
                playerTags.push_front ( pPlayer );
            }

            // Destroy the colpoint
            if ( pColPoint ) pColPoint->Destroy ();

            if ( pPlayerVehicle ) pPlayerVehicle->WorldIgnore ( false );
        }            
    }

    // Draw each player's nametag
    float fAlphaModifier;
    unsigned char ucAlpha;
    float fDistance;
    list < CClientPlayer * > ::iterator iterTags = playerTags.begin ();
    for ( ; iterTags != playerTags.end () ; iterTags++ )
    {
		pPlayer = *iterTags;
        fDistance = pPlayer->GetNametagDistance ();

        static float fFullAlphaDistance = 7.0f;
        if ( ( fDistance < fFullAlphaDistance ) ||
                ( pSniperTargetedPlayer && pSniperTargetedPlayer == pPlayer ) ||
                ( pSniperTargetedVehicle && pSniperTargetedVehicle == pPlayer->GetOccupiedVehicle () ) )
        {
            fAlphaModifier = 1.0f;
        }
        else
        {
            fAlphaModifier = 1.0f - ( ( fDistance - fFullAlphaDistance ) / ( DEFAULT_VIEW_RANGE - fFullAlphaDistance ) );
        }


        // Calculate the alpha for the nametag
        ucAlpha = static_cast < unsigned char > ( 180.0f * fAlphaModifier );

        // Draw the tag
        DrawTagForPlayer ( pPlayer, ucAlpha );
    }
}


void CNametags::DrawTagForPlayer ( CClientPlayer* pPlayer, unsigned char ucAlpha )
{
    // Get the nametag widget
	CGUIStaticImage * pIcon = pPlayer->GetStatusIcon ();

	// If they aren't in the same dimension, dont draw
	if ( pPlayer->GetDimension () != m_usDimension || !pPlayer->IsNametagShowing () )
		return;

	// Grab the resolution width and height
    static float fResWidth = static_cast < float > ( g_pCore->GetGraphics ()->GetViewportWidth () );
    static float fResHeight = static_cast < float > ( g_pCore->GetGraphics ()->GetViewportHeight () );

    // Get the position
    CVector vecPosition;
    pPlayer->GetPosition ( vecPosition );

    // Calculate where the player is on our screen
    CVector vecScreenPosition;
    float fTempX = 0, fTempY = 0;
    vecPosition.fZ += 0.3f;
    m_pHud->CalcScreenCoors ( &vecPosition, &vecScreenPosition, &fTempX, &fTempY, true, true );

    // Grab health and max health
    float fMaxHealth = pPlayer->GetMaxHealth ();
    float fHealth = pPlayer->GetHealth ();

    // Recalculate that to be within 0-100
    fHealth = fHealth / fMaxHealth * 100;
    if ( fHealth > 100.0f ) fHealth = 100.0f;

    // Some multiplier heh...
    fHealth *= 7.52f;

	// We ignore the top 10.0f as that goes dodgy up there for some reason
    if ( fHealth > 0 && vecScreenPosition.fX >= 0.0f && vecScreenPosition.fX <= fResWidth && vecScreenPosition.fY >= 10.0f && vecScreenPosition.fY <= fResHeight && fTempX >= 0 && fTempY >= 0 )
    {
        // Draw the player nametag and status icon
        if ( pPlayer->HasConnectionTrouble () )
        {
            pIcon->SetVisible ( true );
		    pIcon->SetPosition ( CVector2D ( vecScreenPosition.fX - 20, vecScreenPosition.fY ), false );
        }

        // Grab the nick to show
        const char* szNick = pPlayer->GetNametagText ();
        if ( !szNick || !szNick [0] ) szNick = pPlayer->GetNickPointer ();

        // Draw his name
        unsigned char ucR, ucG, ucB;
        pPlayer->GetNametagColor ( ucR, ucG, ucB );
        if ( g_pCore->GetGraphics ()->GetCEGUIUsed () )
		{
            // Draw shadow first
            g_pCore->GetGraphics ()->DrawTextCEGUI ( ( int ) vecScreenPosition.fX + 1 - 17, ( int ) vecScreenPosition.fY + 1, ( int ) vecScreenPosition.fX + 1 - 17, ( int ) vecScreenPosition.fY + 1, COLOR_ARGB ( 255, 0, 0, 0 ), szNick, 1.0f, DT_NOCLIP | DT_CENTER );
		    g_pCore->GetGraphics ()->DrawTextCEGUI ( ( int ) vecScreenPosition.fX - 17, ( int ) vecScreenPosition.fY, ( int ) vecScreenPosition.fX - 17, ( int ) vecScreenPosition.fY, COLOR_ARGB ( 255, ucR, ucG, ucB ), szNick, 1.0f, DT_NOCLIP | DT_CENTER );
		}
		else
		{				
            // Draw shadow first
            g_pCore->GetGraphics ()->DrawText ( ( int ) vecScreenPosition.fX + 1, ( int ) vecScreenPosition.fY + 1, ( int ) vecScreenPosition.fX + 1, ( int ) vecScreenPosition.fY + 1, COLOR_ARGB ( 255, 0, 0, 0 ), szNick, 1.0f, 1.0f, DT_NOCLIP | DT_CENTER );
			g_pCore->GetGraphics ()->DrawText ( ( int ) vecScreenPosition.fX, ( int ) vecScreenPosition.fY, ( int ) vecScreenPosition.fX, ( int ) vecScreenPosition.fY, COLOR_ARGB ( 255, ucR, ucG, ucB ), szNick, 1.0f, 1.0f, DT_NOCLIP | DT_CENTER );
        }
        
        // We need to draw health tags?
        if ( m_bDrawHealth )
        {
            fHealth = fHealth / ( 750.0f / 510.0f );
            long lRed = 0;
            long lGreen = 0;
            if ( fHealth > 255 )
            {
                lRed = static_cast < long > ( 512.0f - fHealth );
                if ( lRed > 255 )
                    lRed = 255;
                lGreen = 255;
            }
            else if ( fHealth <= 255 )
            {
                lRed = 255;
                lGreen = static_cast < long > ( fHealth );
            }

            long lRedBlack = static_cast < long > ( static_cast < float > ( lRed ) * 0.33f );
            long lGreenBlack = static_cast < long > ( static_cast < float > ( lGreen ) * 0.33f );

            // TR - TL - BR - BL
            float fHeight = fResHeight * 0.011f;
            float fWidth = fResWidth * 0.060f;
            float fTopOffset = fResHeight * 0.025f;
            float fSizeIncreaseBorder = fResWidth * 0.003f;
            float fRemovedWidth = fWidth - (fHealth / 512.0f * fWidth);
            float fTopArmorOffset = fTopOffset + fHeight - 0.01f * fResWidth;
            float fArmor = pPlayer->GetArmor ();
            float fMaxArmor = 100.0f;
            float fArmorAlpha = ( fArmor / fMaxArmor ) * ( ( float ) ucAlpha / 255.0f ); // 0->1
            unsigned char ucArmorAlpha = ( unsigned char ) ( 255.0f * fArmorAlpha );

            #define ARMOR_BORDER_COLOR COLOR_ARGB(ucArmorAlpha,167,177,179)
                
            // background
            m_pHud->Draw2DPolygon ( 
                            vecScreenPosition.fX - fWidth / 2 - fSizeIncreaseBorder, 
                            vecScreenPosition.fY + fTopOffset - fSizeIncreaseBorder,
                            vecScreenPosition.fX + fWidth / 2 + fSizeIncreaseBorder,
                            vecScreenPosition.fY + fTopOffset - fSizeIncreaseBorder,

                            vecScreenPosition.fX - fWidth / 2 - fSizeIncreaseBorder, 
                            vecScreenPosition.fY + fTopOffset + fHeight + fSizeIncreaseBorder,
                            vecScreenPosition.fX + fWidth / 2 + fSizeIncreaseBorder,
                            vecScreenPosition.fY + fTopOffset + fHeight + fSizeIncreaseBorder,
                            COLOR_ARGB ( ucAlpha, 0, 0, 0 ) );

            // Left side of armor indicator
            m_pHud->Draw2DPolygon ( 
                            vecScreenPosition.fX - fWidth / 2 - fSizeIncreaseBorder, 
                            vecScreenPosition.fY + fTopOffset - fSizeIncreaseBorder,
                            vecScreenPosition.fX - fWidth / 2,
                            vecScreenPosition.fY + fTopOffset - fSizeIncreaseBorder,

                            vecScreenPosition.fX - fWidth / 2 - fSizeIncreaseBorder, 
                            vecScreenPosition.fY + fTopOffset + fHeight + fSizeIncreaseBorder,
                            vecScreenPosition.fX - fWidth / 2,
                            vecScreenPosition.fY + fTopOffset + fHeight + fSizeIncreaseBorder ,
                            ARMOR_BORDER_COLOR );

            // Right side of armor indicator
            m_pHud->Draw2DPolygon ( 
                            vecScreenPosition.fX + fWidth / 2, 
                            vecScreenPosition.fY + fTopOffset - fSizeIncreaseBorder,
                            vecScreenPosition.fX + fWidth / 2 + fSizeIncreaseBorder,
                            vecScreenPosition.fY + fTopOffset - fSizeIncreaseBorder,

                            vecScreenPosition.fX + fWidth / 2, 
                            vecScreenPosition.fY + fTopOffset + fHeight + fSizeIncreaseBorder,
                            vecScreenPosition.fX + fWidth / 2 + fSizeIncreaseBorder,
                            vecScreenPosition.fY + fTopOffset + fHeight + fSizeIncreaseBorder ,
                            ARMOR_BORDER_COLOR );

            // Top armor indicator
            m_pHud->Draw2DPolygon ( 
                            vecScreenPosition.fX - fWidth / 2,
                            vecScreenPosition.fY + fTopOffset - fSizeIncreaseBorder,
                            vecScreenPosition.fX - fWidth / 2 + fWidth,
                            vecScreenPosition.fY + fTopOffset - fSizeIncreaseBorder,

                            vecScreenPosition.fX - fWidth / 2, 
                            vecScreenPosition.fY + fTopOffset,
                            vecScreenPosition.fX - fWidth / 2 + fWidth,
                            vecScreenPosition.fY + fTopOffset,
                            ARMOR_BORDER_COLOR );

            // Bottom armor indicator
            m_pHud->Draw2DPolygon ( 
                            vecScreenPosition.fX - fWidth / 2,
                            vecScreenPosition.fY + fTopOffset + fHeight,
                            vecScreenPosition.fX - fWidth / 2 + fWidth,
                            vecScreenPosition.fY + fTopOffset + fHeight,

                            vecScreenPosition.fX - fWidth / 2, 
                            vecScreenPosition.fY + fTopOffset + fHeight + fSizeIncreaseBorder,
                            vecScreenPosition.fX - fWidth / 2 + fWidth,
                            vecScreenPosition.fY + fTopOffset + fHeight + fSizeIncreaseBorder,
                            ARMOR_BORDER_COLOR );


            // the colored bit
            m_pHud->Draw2DPolygon ( 
                            vecScreenPosition.fX - fWidth / 2, 
                            vecScreenPosition.fY + fTopOffset,
                            vecScreenPosition.fX + fWidth / 2 - fRemovedWidth,
                            vecScreenPosition.fY + fTopOffset,
                            vecScreenPosition.fX - fWidth / 2, 
                            vecScreenPosition.fY + fTopOffset + fHeight,
                            vecScreenPosition.fX + fWidth / 2  - fRemovedWidth,
                            vecScreenPosition.fY + fTopOffset + fHeight,
                            COLOR_ARGB ( ucAlpha, 0, lGreen, lRed ) );

            // the black bit
            m_pHud->Draw2DPolygon ( 
                            vecScreenPosition.fX - fWidth / 2 + (fWidth - fRemovedWidth), 
                            vecScreenPosition.fY + fTopOffset,
                            vecScreenPosition.fX + fWidth / 2,
                            vecScreenPosition.fY + fTopOffset,
                            vecScreenPosition.fX - fWidth / 2 + (fWidth - fRemovedWidth), 
                            vecScreenPosition.fY + fTopOffset + fHeight,
                            vecScreenPosition.fX + fWidth / 2,
                            vecScreenPosition.fY + fTopOffset + fHeight,
                            COLOR_ARGB ( ucAlpha, 0, lGreenBlack, lRedBlack ) );
        }
 	}
}


bool CNametags::CompareNametagDistance ( CClientPlayer * p1, CClientPlayer * p2 )
{
    return p1->GetNametagDistance () > p2->GetNametagDistance ();
}