/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColShape.h
*  PURPOSE:     Shaped collision entity base class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

class CClientColShape;

#ifndef __CCLIENTCOLSHAPE_H
#define __CCLIENTCOLSHAPE_H

#include "CClientEntity.h"

class CClientColCallback;
class CClientMarker;
class CClientPickup;

enum eColShapeType
{
    COLSHAPE_CIRCLE,
    COLSHAPE_CUBOID,
    COLSHAPE_SPHERE,
    COLSHAPE_RECTANGLE,
    COLSHAPE_TUBE,
};

class CClientColShape : public CClientEntity
{
    friend CClientMarker;
	friend CClientPickup;
public:
                                        CClientColShape                 ( class CClientManager* pManager, ElementID ID );
                                        ~CClientColShape                ( void );

    void                                Unlink                          ( void );

    virtual eColShapeType               GetShapeType                    ( void ) = 0;

    void                                DoPulse                         ( void );

    inline eClientEntityType            GetType                         ( void ) const                          { return CCLIENTCOLSHAPE; }

    inline void                         GetPosition                     ( CVector& vecPosition ) const          { vecPosition = m_vecPosition; };
    inline void                         SetPosition                     ( const CVector& vecPosition )          { m_vecPosition = vecPosition; };

    virtual bool                        DoHitDetection                  ( const CVector& vecNowPosition, float fRadius ) = 0;

    inline bool                         IsEnabled                       ( void )                                            { return m_bIsEnabled; };
    inline void                         SetEnabled                      ( bool bEnabled )                                   { m_bIsEnabled = bEnabled; };

    void                                CallHitCallback                 ( CClientEntity& Entity );
	void                                CallLeaveCallback               ( CClientEntity& Entity );
    inline CClientColCallback*          SetHitCallback                  ( CClientColCallback* pCallback )                   { return ( m_pCallback = pCallback ); };

    inline bool                         GetAutoCallEvent                ( void )                                            { return m_bAutoCallEvent; };
    inline void                         SetAutoCallEvent                ( bool bAutoCallEvent )                             { m_bAutoCallEvent = bAutoCallEvent; };

    void                                AddCollider                     ( CClientEntity* pEntity )                          { m_Colliders.push_back ( pEntity ); }
    void                                RemoveCollider                  ( CClientEntity* pEntity )                          { if ( !m_Colliders.empty() ) m_Colliders.remove ( pEntity ); }
    bool                                ColliderExists                  ( CClientEntity* pEntity );
    void                                RemoveAllColliders              ( bool bNotify );
    list < CClientEntity* > ::iterator  CollidersBegin                  ( void )                                            { return m_Colliders.begin (); }
    list < CClientEntity* > ::iterator  CollidersEnd                    ( void )                                            { return m_Colliders.end (); }

protected:
    CVector                             m_vecPosition;
    CClientMarker *                     m_pOwningMarker;
	CClientPickup *                     m_pOwningPickup;

private:
    bool                                m_bIsEnabled;
    class CClientColManager*            m_pColManager;
    CClientColCallback*                 m_pCallback;
    bool                                m_bAutoCallEvent;  
    list < CClientEntity* >             m_Colliders;    
};

#endif
