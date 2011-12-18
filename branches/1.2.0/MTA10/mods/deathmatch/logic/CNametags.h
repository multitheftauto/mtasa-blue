/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CNametags.h
*  PURPOSE:     Header for nametags class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNAMETAGS_H
#define __CNAMETAGS_H

class CHud;
class CClientManager;
class CClientPlayer;
class CClientPlayerManager;
class CClientPedManager;
class CClientStreamer;

class CNametags
{
public:
                            CNametags               ( CClientManager* pManager );
                            ~CNametags              ( void );

    inline bool             GetDrawHealth           ( void )                    { return m_bDrawHealth; };
    inline void             SetDrawHealth           ( bool bDrawHealth )        { m_bDrawHealth = bDrawHealth; };

    void                    DoPulse                 ( void );

    void                    DrawFromAim             ( void );
    void                    DrawDefault             ( void );

    void                    DrawTagForPlayer        ( CClientPlayer* pPlayer, unsigned char ucAlpha );

    inline unsigned int     GetDimension            ( void )                        { return m_usDimension; }
    inline void             SetDimension            ( unsigned short usDimension )  { m_usDimension = usDimension; }

    inline bool             IsVisible               ( void )                        { return m_bVisible; }
    inline void             SetVisible              ( bool bVisible )               { m_bVisible = bVisible; }

private:
    static bool             CompareNametagDistance  ( CClientPlayer * p1, CClientPlayer * p2 );

    CClientPlayerManager*   m_pPlayerManager;
    CClientStreamer*        m_pPlayerStreamer;
    bool                    m_bDrawHealth;
    CHud*                   m_pHud;    
    bool                    m_bDrawFromAim;
    unsigned short          m_usDimension;
    bool                    m_bVisible;
};

#endif
