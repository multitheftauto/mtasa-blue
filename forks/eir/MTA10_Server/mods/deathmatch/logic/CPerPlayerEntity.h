/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerPlayerEntity.h
*  PURPOSE:     Per-player entity linking class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPERPLAYERENTITY_H
#define __CPERPLAYERENTITY_H

#include "CElement.h"
#include "packets/CPacket.h"

class CPerPlayerEntity : public CElement
{
    friend class CElement;

public:
                                CPerPlayerEntity                ( CElement* pParent, CXMLNode* pNode = NULL );
                                ~CPerPlayerEntity               ( void );

    bool                        IsEntity                        ( void )                    { return true; }
    bool                        IsPerPlayerEntity               ( void )                    { return true; };

    bool                        Sync                            ( bool bSync );
    inline bool                 IsSynced                        ( void )                    { return m_bIsSynced; };
    inline void                 SetIsSynced                     ( bool bIsSynced )          { m_bIsSynced = bIsSynced; };

    void                        OnReferencedSubtreeAdd          ( CElement* pElement );
    void                        OnReferencedSubtreeRemove       ( CElement* pElement );
    void                        UpdatePerPlayer                 ( void );

    bool                        AddVisibleToReference           ( CElement* pElement );
    bool                        RemoveVisibleToReference        ( CElement* pElement );
    void                        ClearVisibleToReferences        ( void );
    bool                        IsVisibleToReferenced           ( CElement* pElement );

    bool                        IsVisibleToPlayer               ( CPlayer& Player );

    const std::set < CPlayer * >&   GetPlayersList              ( void )                    { return m_Players; }

    static void                 StaticOnPlayerDelete            ( CPlayer* pPlayer );
    void                        OnPlayerDelete                  ( CPlayer* pPlayer );

protected:
    virtual void                CreateEntity                    ( CPlayer* pPlayer );
    virtual void                DestroyEntity                   ( CPlayer* pPlayer );

    void                        BroadcastOnlyVisible            ( const CPacket& Packet );
    bool                        m_bIsSynced;

    list < CElement* >          m_ElementReferences;

private:
    void                        RemoveIdenticalEntries          ( std::set < class CPlayer* >& List1, std::set < class CPlayer* >& List2 );

    void                        AddPlayersBelow                 ( CElement* pElement, std::set < class CPlayer* >& Added );
    void                        RemovePlayersBelow              ( CElement* pElement, std::set < class CPlayer* >& Removed );

    void                        AddPlayerReference              ( class CPlayer* pPlayer );
    void                        RemovePlayerReference           ( class CPlayer* pPlayer );

    std::set < CPlayer* >       m_PlayersAdded;
    std::set < CPlayer* >       m_PlayersRemoved;
    std::set < CPlayer* >       m_Players;

    static std::set < CPerPlayerEntity* > ms_AllPerPlayerEntityMap;
};

#endif
