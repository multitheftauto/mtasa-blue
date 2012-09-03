/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientObjectManager.h
*  PURPOSE:     Physical object entity manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

class CClientObjectManager;

#ifndef __CCLIENTOBJECTMANAGER_H
#define __CCLIENTOBJECTMANAGER_H

#include "CClientCommon.h"
#include <list>

class CClientManager;
class CClientObject;
class CObject;

class CClientObjectManager
{
    friend class CClientManager;
    friend class CClientObject;

public:
    void                                    DoPulse                     ( void );

    void                                    DeleteAll                   ( void );

    inline unsigned int                     Count                       ( void )                        { return static_cast < unsigned int > ( m_Objects.size () ); };
    inline unsigned int                     CountCreatedObjects         ( void )                        { return static_cast < unsigned int > ( g_pGame->GetPools ()->GetObjectCount () ); };
    static CClientObject*                   Get                         ( ElementID ID );
    CClientObject*                          Get                         ( CObject* pObject, bool bValidatePointer );
    CClientObject*                          GetSafe                     ( CEntity * pEntity );

    static bool                             IsValidModel                ( unsigned long ulObjectModel );
    static bool                             IsBreakableModel            ( unsigned long ulObjectModel );
    bool                                    Exists                      ( CClientObject* pObject );
    bool                                    ObjectsAroundPointLoaded    ( const CVector& vecPosition, float fRadius, unsigned short usDimension, SString* pstrStatus = NULL );

    static bool                             IsObjectLimitReached        ( void );

    void                                    RestreamObjects             ( unsigned short usModel );

    std::list < CClientObject* > ::const_iterator           IterGet             ( CClientObject* pObject );
    std::list < CClientObject* > ::const_reverse_iterator   IterGetReverse      ( CClientObject* pObject );
    std::list < CClientObject* > ::const_iterator           IterBegin           ( void )                        { return m_Objects.begin (); };
    std::list < CClientObject* > ::const_iterator           IterEnd             ( void )                        { return m_Objects.end (); };
    std::list < CClientObject* > ::const_reverse_iterator   IterReverseBegin    ( void )                        { return m_Objects.rbegin (); };
    std::list < CClientObject* > ::const_reverse_iterator   IterReverseEnd      ( void )                        { return m_Objects.rend (); };

private:
                                            CClientObjectManager        ( class CClientManager* pManager );
                                            ~CClientObjectManager       ( void );

    inline void                             AddToList                   ( CClientObject* pObject )      { m_Objects.push_back ( pObject ); };
    void                                    RemoveFromList              ( CClientObject* pObject );

    void                                    OnCreation                  ( CClientObject * pObject );
    void                                    OnDestruction               ( CClientObject * pObject );

    void                                    UpdateLimitInfo             ( void );

    static int                              m_iEntryInfoNodeEntries;
    static int                              m_iPointerNodeSingleLinkEntries;
    static int                              m_iPointerNodeDoubleLinkEntries;

    class CClientManager*                   m_pManager;
    bool                                    m_bCanRemoveFromList;
    CMappedList < CClientObject* >          m_Objects;
    std::vector < CClientObject* >          m_StreamedIn;
    CMappedList < CClientObject* >          m_Attached;
};

#endif
