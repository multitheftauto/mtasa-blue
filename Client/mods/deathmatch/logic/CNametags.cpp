/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CNametags.cpp
 *  PURPOSE:     Nametags renderer
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CColPoint.h>

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
#define DEFAULT_VIEW_RANGE_EXP      ((DEFAULT_VIEW_RANGE)*(DEFAULT_VIEW_RANGE))

const bool bRenderOwn = false;

CNametags::CNametags(CClientManager* pManager)
{
    m_pPlayerStreamer = pManager->GetPlayerStreamer();
    m_pPlayerManager = pManager->GetPlayerManager();
    m_bDrawHealth = true;
    m_pHud = g_pGame->GetHud();
    m_usDimension = 0;
    m_bVisible = true;
    m_pConnectionTroubleIcon =
        g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(CalcMTASAPath("MTA\\cgui\\images\\16-message-warn.png"), NULL, false);
}

CNametags::~CNametags()
{
    SAFE_RELEASE(m_pConnectionTroubleIcon);
}

void CNametags::DoPulse()
{
    if (!m_bVisible)
        return;

    // Grab the resolution width and height
    static float fResWidth = static_cast<float>(g_pCore->GetGraphics()->GetViewportWidth());
    static float fResHeight = static_cast<float>(g_pCore->GetGraphics()->GetViewportHeight());

    // Got any players that are not local?
    if (m_pPlayerManager->Count() <= 1 && !bRenderOwn)
        return;

    list<CClientPlayer*> playerTags;

    // Grab the local player
    CClientPlayer* pLocalPlayer = m_pPlayerManager->GetLocalPlayer();
    if (!pLocalPlayer)
        return;

    CClientVehicle* pSniperTargetedVehicle = NULL;
    CClientPlayer*  pSniperTargetedPlayer = NULL;

    // Grab our current weapon slot. Use screen center if melee or none
    eWeaponSlot eSlot = pLocalPlayer->GetCurrentWeaponSlot();
    if (eSlot >= WEAPONSLOT_TYPE_HANDGUN && eSlot <= WEAPONSLOT_TYPE_RIFLE)
    {
        CVector vecOrigin, vecTarget;
        pLocalPlayer->GetShotData(&vecOrigin, &vecTarget);

        // Ignore the local player for this
        pLocalPlayer->WorldIgnore(true);

        // Do the raycast
        CColPoint*        pColPoint = NULL;
        CEntity*          pEntity = NULL;
        SLineOfSightFlags flags;
        flags.bCheckBuildings = true;
        flags.bCheckVehicles = true;
        flags.bCheckPeds = true;
        flags.bCheckObjects = true;
        flags.bCheckDummies = true;
        flags.bSeeThroughStuff = true;
        flags.bIgnoreSomeObjectsForCamera = false;
        flags.bShootThroughStuff = true;
        g_pGame->GetWorld()->ProcessLineOfSight(&vecOrigin, &vecTarget, &pColPoint, &pEntity, flags);
        if (pColPoint)
            pColPoint->Destroy();

        // Un-ignore the local player
        pLocalPlayer->WorldIgnore(false);

        // Did we find an entity?
        if (pEntity)
        {
            // Grab the CClientEntity belonging to this game_sa entity
            CClientEntity* pClientEntity = reinterpret_cast<CClientEntity*>(pEntity->GetStoredPointer());
            if (pClientEntity)
            {
                // Is it a vehicle? Is it a ped?
                eClientEntityType EntityType = pClientEntity->GetType();
                switch (EntityType)
                {
                    case CCLIENTVEHICLE:
                    {
                        pSniperTargetedVehicle = static_cast<CClientVehicle*>(pClientEntity);
                        break;
                    }
                    case CCLIENTPLAYER:
                    {
                        pSniperTargetedPlayer = static_cast<CClientPlayer*>(pClientEntity);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }

    // Grab the local player vehicle
    CClientVehicle* pLocalVehicle = pLocalPlayer->GetOccupiedVehicle();
    CVehicle*       pLocalGameVehicle = NULL;
    if (pLocalVehicle)
        pLocalGameVehicle = pLocalVehicle->GetGameVehicle();

    CMatrix CameraMatrix;
    g_pGame->GetCamera()->GetMatrix(&CameraMatrix);

    // Remove collision from our local vehicle (if we have one)
    if (pLocalVehicle)
        pLocalVehicle->WorldIgnore(true);

    // Draw the nametags we need to
    CVector                                     vecPlayerPosition;
    CClientVehicle*                             pPlayerVehicle = NULL;
    float                                       fDistanceExp;
    bool                                        bCollision;
    CColPoint*                                  pColPoint = NULL;
    CEntity*                                    pGameEntity = NULL;
    CClientEntity*                              pEntity = NULL;
    CClientPlayer*                              pPlayer;
    CClientStreamElement*                       pElement;
    list<CClientStreamElement*>::const_iterator iter = m_pPlayerStreamer->ActiveElementsBegin();
    for (; iter != m_pPlayerStreamer->ActiveElementsEnd(); ++iter)
    {
        pElement = *iter;
        if (!pElement->IsStreamedIn())
            continue;
        if (pElement->GetType() != CCLIENTPLAYER)
            continue;
        pPlayer = static_cast<CClientPlayer*>(pElement);
        if (pPlayer->IsLocalPlayer() && !bRenderOwn)
            continue;

        // Get the distance from the camera
        pPlayer->GetPosition(vecPlayerPosition);
        fDistanceExp = pPlayer->GetExpDistance();
        pPlayerVehicle = pPlayer->GetOccupiedVehicle();

        // Is he in the same vehicle as the local player?
        if ((pSniperTargetedPlayer == pPlayer) || (pSniperTargetedVehicle && pSniperTargetedVehicle == pPlayerVehicle) ||
            (pLocalVehicle && pLocalVehicle == pPlayerVehicle) || (fDistanceExp < DEFAULT_VIEW_RANGE_EXP && pPlayer->IsOnScreen()))
        {
            SLineOfSightFlags flags;
            flags.bCheckBuildings = true;
            flags.bCheckVehicles = true;
            flags.bCheckPeds = false;
            flags.bCheckObjects = true;
            bCollision = g_pCore->GetGame()->GetWorld()->ProcessLineOfSight(&CameraMatrix.vPos, &vecPlayerPosition, &pColPoint, &pGameEntity, flags);
            if (!bCollision || (pGameEntity && pPlayerVehicle && pGameEntity == pPlayerVehicle->GetGameEntity()))
            {
                pPlayer->SetNametagDistance(sqrt(fDistanceExp));
                playerTags.push_front(pPlayer);
            }

            // Destroy the colpoint
            if (pColPoint)
                pColPoint->Destroy();
        }
    }

    // Readd collision from our local vehicle (if we have one)
    if (pLocalVehicle)
        pLocalVehicle->WorldIgnore(false);

    // Draw each player's nametag
    float                          fAlphaModifier;
    unsigned char                  ucAlpha;
    float                          fDistance;
    list<CClientPlayer*>::iterator iterTags = playerTags.begin();
    for (; iterTags != playerTags.end(); ++iterTags)
    {
        pPlayer = *iterTags;
        fDistance = pPlayer->GetNametagDistance();

        static float fFullAlphaDistance = 7.0f;
        if ((fDistance < fFullAlphaDistance) || (pSniperTargetedPlayer && pSniperTargetedPlayer == pPlayer) ||
            (pSniperTargetedVehicle && pSniperTargetedVehicle == pPlayer->GetOccupiedVehicle()))
        {
            fAlphaModifier = 1.0f;
        }
        else
        {
            fAlphaModifier = 1.0f - ((fDistance - fFullAlphaDistance) / (DEFAULT_VIEW_RANGE - fFullAlphaDistance));
        }

        // Calculate the alpha for the nametag
        ucAlpha = static_cast<unsigned char>(180.0f * fAlphaModifier);

        // Draw the tag
        DrawTagForPlayer(pPlayer, ucAlpha);
    }
}

void CNametags::DrawTagForPlayer(CClientPlayer* pPlayer, unsigned char ucAlpha)
{
    // If nametag is hidden, don't draw
    if (!pPlayer->IsNametagShowing())
        return;

    // If they aren't in the same dimension, don't draw
    if (pPlayer->GetDimension() != m_usDimension)
        return;

    // If they aren't in the same interior, don't draw
    if (pPlayer->GetInterior() != m_ucInterior)
        return;

    // Grab the resolution width and height
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();
    static float        fResWidth = static_cast<float>(pGraphics->GetViewportWidth());
    static float        fResHeight = static_cast<float>(pGraphics->GetViewportHeight());

    // Get the position
    CVector vecPosition;
    pPlayer->GetBonePosition(BONE_PELVIS, vecPosition);

    // Calculate where the player is on our screen
    CVector vecScreenPosition;
    vecPosition.fZ += 0.3f;
    pGraphics->CalcScreenCoors(&vecPosition, &vecScreenPosition);

    // Grab health and max health
    float fMaxHealth = pPlayer->GetMaxHealth();
    float fHealth = pPlayer->GetHealth();
    float fArmor = pPlayer->GetArmor();

    // Recalculate that to be within 0-100
    fHealth = fHealth / fMaxHealth * 100;
    if (fHealth > 100.0f)
        fHealth = 100.0f;

    // Some multiplier heh...
    fHealth *= 7.52f;

    // Allow up to 50 pixels off screen to avoid nametags suddenly disappearing
    if (fHealth > 0 && vecScreenPosition.fX > -50.0f && vecScreenPosition.fX < fResWidth + 50.f && vecScreenPosition.fY > -50.0f &&
        vecScreenPosition.fY < fResHeight + 50.f && vecScreenPosition.fZ > 0.1f)
    {
        // Grab the nick to show
        const char* szNick = pPlayer->GetNametagText();
        if (!szNick || !szNick[0])
            szNick = pPlayer->GetNick();

        // Draw his name
        unsigned char ucR, ucG, ucB;
        pPlayer->GetNametagColor(ucR, ucG, ucB);
        // Draw shadow first
        int iScreenPosX = static_cast<int>(vecScreenPosition.fX);
        int iScreenPosY = static_cast<int>(vecScreenPosition.fY);
        pGraphics->DrawString(iScreenPosX + 1, iScreenPosY + 1, iScreenPosX + 1, iScreenPosY + 1, COLOR_ARGB(255, 0, 0, 0), szNick, 1.0f, 1.0f,
                              DT_NOCLIP | DT_CENTER);
        pGraphics->DrawString(iScreenPosX, iScreenPosY, iScreenPosX, iScreenPosY, COLOR_ARGB(255, ucR, ucG, ucB), szNick, 1.0f, 1.0f, DT_NOCLIP | DT_CENTER);

        // We need to draw health tags?
        if (m_bDrawHealth)
        {
            fHealth = fHealth / (750.0f / 510.0f);
            long lRed = 0;
            long lGreen = 0;
            if (fHealth > 255)
            {
                lRed = static_cast<long>(512.0f - fHealth);
                if (lRed > 255)
                    lRed = 255;
                lGreen = 255;
            }
            else if (fHealth <= 255)
            {
                lRed = 255;
                lGreen = static_cast<long>(fHealth);
            }

            long lRedBlack = static_cast<long>(lRed * 0.33f);
            long lGreenBlack = static_cast<long>(lGreen * 0.33f);

            // TR - TL - BR - BL
            float fHeight = fResHeight * 0.011f;
            float fWidth = fResWidth * 0.060f;
            float fTopOffset = fResHeight * 0.025f;
            float fSizeIncreaseBorder = fResWidth * 0.003f;
            float fRemovedWidth = fWidth - (fHealth / 512.0f * fWidth);
            float fTopArmorOffset = fTopOffset + fHeight - 0.01f * fResWidth;
            float fMaxArmor = 100.0f;
            float fArmorAlpha = (fArmor / fMaxArmor) * (ucAlpha / 255.0f);            // 0->1

            unsigned char ucArmorAlpha = (unsigned char)(255.0f * fArmorAlpha);

            #define ARMOR_BORDER_COLOR COLOR_ABGR(ucArmorAlpha,167,177,179)

            // Base rectangle
            CVector vecTopLeftBase(vecScreenPosition.fX - fWidth * 0.5f, vecScreenPosition.fY + fTopOffset, 0);
            CVector vecBotRightBase(vecScreenPosition.fX + fWidth * 0.5f, vecScreenPosition.fY + fTopOffset + fHeight, 0);

            // background
            CVector vecTopLeft = vecTopLeftBase + CVector(-fSizeIncreaseBorder, -fSizeIncreaseBorder, 0);
            CVector vecBotRight = vecBotRightBase + CVector(+fSizeIncreaseBorder, +fSizeIncreaseBorder, 0);
            pGraphics->DrawRectangle(vecTopLeft.fX, vecTopLeft.fY, vecBotRight.fX - vecTopLeft.fX, vecBotRight.fY - vecTopLeft.fY, COLOR_ABGR(ucAlpha, 0, 0, 0),
                                     true);

            if (fArmor > 0.0f)
            {
                // Left side of armor indicator
                vecTopLeft = vecTopLeftBase + CVector(-fSizeIncreaseBorder, -fSizeIncreaseBorder, 0);
                vecBotRight = vecBotRightBase + CVector(-fWidth, +fSizeIncreaseBorder, 0);
                pGraphics->DrawRectangle(vecTopLeft.fX, vecTopLeft.fY, vecBotRight.fX - vecTopLeft.fX, vecBotRight.fY - vecTopLeft.fY, ARMOR_BORDER_COLOR,
                                         true);

                // Right side of armor indicator
                vecTopLeft = vecTopLeftBase + CVector(+fWidth, -fSizeIncreaseBorder, 0);
                vecBotRight = vecBotRightBase + CVector(+fSizeIncreaseBorder, +fSizeIncreaseBorder, 0);
                pGraphics->DrawRectangle(vecTopLeft.fX, vecTopLeft.fY, vecBotRight.fX - vecTopLeft.fX, vecBotRight.fY - vecTopLeft.fY, ARMOR_BORDER_COLOR,
                                         true);

                // Top armor indicator
                vecTopLeft = vecTopLeftBase + CVector(+0, -fSizeIncreaseBorder, 0);
                vecBotRight = vecBotRightBase + CVector(+0, -fHeight, 0);
                pGraphics->DrawRectangle(vecTopLeft.fX, vecTopLeft.fY, vecBotRight.fX - vecTopLeft.fX, vecBotRight.fY - vecTopLeft.fY, ARMOR_BORDER_COLOR,
                                         true);

                // Bottom armor indicator
                vecTopLeft = vecTopLeftBase + CVector(+0, +fHeight, 0);
                vecBotRight = vecBotRightBase + CVector(+0, +fSizeIncreaseBorder, 0);
                pGraphics->DrawRectangle(vecTopLeft.fX, vecTopLeft.fY, vecBotRight.fX - vecTopLeft.fX, vecBotRight.fY - vecTopLeft.fY, ARMOR_BORDER_COLOR,
                                         true);
            }

            // the colored bit
            vecTopLeft = vecTopLeftBase + CVector(+0, +0, 0);
            vecBotRight = vecBotRightBase + CVector(-fRemovedWidth, +0, 0);
            pGraphics->DrawRectangle(vecTopLeft.fX, vecTopLeft.fY, vecBotRight.fX - vecTopLeft.fX, vecBotRight.fY - vecTopLeft.fY,
                                     COLOR_ABGR(ucAlpha, 0, static_cast<unsigned char>(lGreen), static_cast<unsigned char>(lRed)), true);

            // the black bit
            vecTopLeft = vecTopLeftBase + CVector(+fWidth - fRemovedWidth, +0, 0);
            vecBotRight = vecBotRightBase + CVector(+0, +0, 0);
            pGraphics->DrawRectangle(vecTopLeft.fX, vecTopLeft.fY, vecBotRight.fX - vecTopLeft.fX, vecBotRight.fY - vecTopLeft.fY,
                                     COLOR_ABGR(ucAlpha, 0, static_cast<unsigned char>(lGreenBlack), static_cast<unsigned char>(lRedBlack)), true);

            // Draw the player status icon
            if (pPlayer->HasConnectionTrouble())
            {
                pGraphics->DrawTexture(m_pConnectionTroubleIcon, vecScreenPosition.fX - 20, vecScreenPosition.fY + 20);
            }
        }
    }
}

bool CNametags::CompareNametagDistance(CClientPlayer* p1, CClientPlayer* p2)
{
    return p1->GetNametagDistance() > p2->GetNametagDistance();
}
