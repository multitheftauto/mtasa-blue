/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CNametags.h
 *  PURPOSE:     Header for nametags class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CHud;
class CClientManager;
class CClientPlayer;
class CClientPlayerManager;
class CClientPedManager;
class CClientStreamer;

class CNametags
{
public:
    CNametags(CClientManager* pManager);
    ~CNametags();

    bool GetDrawHealth() { return m_bDrawHealth; };
    void SetDrawHealth(bool bDrawHealth) { m_bDrawHealth = bDrawHealth; };

    void DoPulse();

    void DrawTagForPlayer(CClientPlayer* pPlayer, unsigned char ucAlpha);

    unsigned int GetDimension() { return m_usDimension; }
    void         SetDimension(unsigned short usDimension) { m_usDimension = usDimension; }

    bool IsVisible() { return m_bVisible; }
    void SetVisible(bool bVisible) { m_bVisible = bVisible; }

    unsigned char m_ucInterior = 0;

private:
    static bool CompareNametagDistance(CClientPlayer* p1, CClientPlayer* p2);

    CClientPlayerManager* m_pPlayerManager;
    CClientStreamer*      m_pPlayerStreamer;
    bool                  m_bDrawHealth;
    CHud*                 m_pHud;
    unsigned short        m_usDimension;
    bool                  m_bVisible;
    CTextureItem*         m_pConnectionTroubleIcon;
};
