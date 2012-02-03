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

    bool                        IsVisibleToPlayer               ( class CPlayer& Player );

    list < CPlayer * >          &GetPlayersList                  ( void )                    { return m_Players; }

protected:
    virtual void                CreateEntity                    ( CPlayer* pPlayer );
    virtual void                DestroyEntity                   ( CPlayer* pPlayer );

    void                        BroadcastOnlyVisible            ( const CPacket& Packet );
    bool                        m_bIsSynced;

    list < CElement* >          m_ElementReferences;

private:
    void                        RemoveIdenticalEntries          ( list < class CPlayer* >& List1, list < class CPlayer* >& List2 );

    void                        AddPlayersBelow                 ( CElement* pElement, list < class CPlayer* >& Added );
    void                        RemovePlayersBelow              ( CElement* pElement, list < class CPlayer* >& Removed );

    inline void                 AddPlayerReference              ( class CPlayer* pPlayer )      { m_Players.push_back ( pPlayer ); m_PlayersMap[ pPlayer->GetID ( ) ] = pPlayer; };
    void                        RemovePlayerReference           ( class CPlayer* pPlayer );

    list < class CPlayer* >     m_PlayersAdded;
    list < class CPlayer* >     m_PlayersRemoved;
    list < class CPlayer* >     m_Players;
    map < ElementID, class CPlayer* >     m_PlayersMap;
};

#endif
