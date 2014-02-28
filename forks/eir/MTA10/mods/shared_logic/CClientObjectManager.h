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

#ifndef __CCLIENTOBJECTMANAGER_H
#define __CCLIENTOBJECTMANAGER_H

class CClientManager;
class CClientObject;
class CObject;

class CClientObjectManager
{
public:
    ZERO_ON_NEW
                                            CClientObjectManager        ( class CClientManager* pManager );
                                            ~CClientObjectManager       ( void );

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

    void                                    AddToList                   ( CClientObject* pObject )      { m_Objects.push_back ( pObject ); }
    void                                    RemoveFromLists             ( CClientObject* pObject );

    void                                    OnCreation                  ( CClientObject * pObject );
    void                                    OnDestruction               ( CClientObject * pObject );

protected:

    void                                    UpdateLimitInfo             ( void );

    static int                              m_iEntryInfoNodeEntries;
    static int                              m_iPointerNodeSingleLinkEntries;
    static int                              m_iPointerNodeDoubleLinkEntries;

    CClientManager*                         m_pManager;
    bool                                    m_bCanRemoveFromList;
    CMappedArray < CClientObject* >         m_Objects;
    CMappedArray < CClientObject* >         m_StreamedIn;
};

#endif
