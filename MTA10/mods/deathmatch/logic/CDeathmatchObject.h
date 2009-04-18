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
public:
                                        CDeathmatchObject               ( CClientManager* pManager, class CMovingObjectsManager* pMovingObjectsManager, ElementID ID, unsigned short usModel );
                                        ~CDeathmatchObject              ( void );

    // CClientEntity interface
    void                                SetPosition                     ( const CVector& vecPosition );

    // CClientObject interface
    void                                SetRotationRadians              ( const CVector& vecRotation );
    void                                SetOrientation                  ( const CVector& vecPosition, const CVector& vecRotationRadians );

    // CDeathmatchObject functions
    inline const CVector&               GetStartPosition                ( void )        { return m_vecStartPosition; };
    inline const CVector&               GetStartRotation                ( void )        { return m_vecStartRotation; };
    inline const CVector&               GetTargetPosition               ( void )        { return m_vecTargetPosition; };
    inline const CVector&               GetTargetRotation               ( void )        { return m_vecTargetRotation; };
    inline void                         SetTargetRotation               ( const CVector& vecRotation ) { m_vecTargetRotation = vecRotation; }
    inline unsigned long                GetStartTime                    ( void )        { return m_ulStartTime; };
    inline unsigned long                GetTargetTime                   ( void )        { return m_ulTargetTime; };

    void                                StartMovement                   ( const CVector& vecTargetPosition, const CVector& vecTargetRotation, unsigned long ulTime );
    void                                StopMovement                    ( void );
    void                                FinishMovement                  ( void );
    void                                UpdateMovement                  ( void );
    void                                UpdateContactingBegin           ( const CVector& vecPreviousPosition, const CVector& vecPreviousRotation );
    void                                UpdateContacting                ( const CVector& vecCenterOfRotation, const CVector& vecFrameTranslation, const CVector& vecFrameRotation );

    inline bool                         IsMoving                        ( void )        { return m_ulStartTime != 0; };

protected:
    class CMovingObjectsManager*        m_pMovingObjectsManager;

    CVector                             m_vecStartPosition;
    CVector                             m_vecStartRotation;
    CVector                             m_vecTargetPosition;
    CVector                             m_vecTargetRotation;
    unsigned long                       m_ulStartTime;
    unsigned long                       m_ulTargetTime;
};

#endif
