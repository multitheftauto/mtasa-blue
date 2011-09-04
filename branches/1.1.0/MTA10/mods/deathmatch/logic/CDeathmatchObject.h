/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDeathmatchObject.h
*  PURPOSE:     Header for deathmatch object class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDEATHMATCHOBJECT_H
#define __CDEATHMATCHOBJECT_H

#include "CClientObject.h"

class CDeathmatchObject : public CClientObject
{
    DECLARE_CLASS( CDeathmatchObject, CClientObject )
public:
#ifdef WITH_OBJECT_SYNC
                                        CDeathmatchObject               ( CClientManager* pManager, class CMovingObjectsManager* pMovingObjectsManager, class CObjectSync* pObjectSync, ElementID ID, unsigned short usModel );
#else
                                        CDeathmatchObject               ( CClientManager* pManager, class CMovingObjectsManager* pMovingObjectsManager, ElementID ID, unsigned short usModel );
#endif
                                        ~CDeathmatchObject              ( void );

    // CClientEntity interface
    void                                SetPosition                     ( const CVector& vecPosition );

    // CClientObject interface
    void                                SetRotationRadians              ( const CVector& vecRotation );
    void                                SetOrientation                  ( const CVector& vecPosition, const CVector& vecRotationRadians );

    void                                StartMovement                   ( const CPositionRotationAnimation& a_rMoveAnimation ); 
    void                                UpdateMovement                  ( void );
    void                                StopMovement                    ( void );
    
    inline bool                         IsMoving                        ( void )        { return m_pMoveAnimation != NULL; };

    void                                UpdateContactingBegin           ( const CVector& vecPreviousPosition, const CVector& vecPreviousRotation );
    void                                UpdateContacting                ( const CVector& vecCenterOfRotation, const CVector& vecFrameTranslation, const CVector& vecFrameRotation );

protected:
    class CMovingObjectsManager*        m_pMovingObjectsManager;
#ifdef WITH_OBJECT_SYNC
    class CObjectSync*                  m_pObjectSync;
#endif
    void                                _StopMovement                    ( bool a_bUnregister );
   
    CPositionRotationAnimation*         m_pMoveAnimation;
};

#endif
