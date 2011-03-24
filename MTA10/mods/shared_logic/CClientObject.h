/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientObject.h
*  PURPOSE:     Physical object entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

class CClientObject;

#ifndef __CCLIENTOBJECT_H
#define __CCLIENTOBJECT_H

#include "CClientStreamElement.h"

class CClientObject : public CClientStreamElement
{
    friend class CClientObjectManager;
    friend CClientPed;

public:
                                    CClientObject           ( class CClientManager* pManager, ElementID ID, unsigned short usModel );
                                    ~CClientObject          ( void );

    void                            Unlink                  ( void );
    
    inline eClientEntityType        GetType                 ( void ) const                      { return CCLIENTOBJECT; };

    inline CObject *                GetGameObject           ( void )                            { return m_pObject; }
    inline CEntity *                GetGameEntity           ( void )                            { return m_pObject; }
    inline const CEntity*           GetGameEntity           ( void ) const                      { return m_pObject; }

    void                            GetPosition             ( CVector& vecPosition ) const;
    void                            SetPosition             ( const CVector& vecPosition );
    virtual CSphere                 GetWorldBoundingSphere  ( void );

    void                            AttachTo                ( CClientEntity* pEntity );

    void                            GetRotationDegrees      ( CVector& vecRotation ) const;
    void                            GetRotationRadians      ( CVector& vecRotation ) const;
    void                            SetRotationDegrees      ( const CVector& vecRotation );
    virtual void                    SetRotationRadians      ( const CVector& vecRotation );

    void                            GetMoveSpeed            ( CVector& vecMoveSpeed ) const;
    void                            SetMoveSpeed            ( const CVector& vecMoveSpeed );

    void                            GetOrientation          ( CVector& vecPosition, CVector& vecRotationRadians );
    virtual void                    SetOrientation          ( const CVector& vecPosition, const CVector& vecRotationRadians );

    void                            ModelRequestCallback    ( CModelInfo* pModelInfo );

    float                           GetDistanceFromCentreOfMassToBaseOfModel ( void );

    inline bool                     IsVisible               ( void )                            { return m_bIsVisible; };
    void                            SetVisible              ( bool bVisible );

    inline unsigned short           GetModel                ( void ) const                      { return m_usModel; };
    void                            SetModel                ( unsigned short usModel );

    void                            Render                  ( void );

    inline bool                     IsStatic                ( void )                            { return m_bIsStatic; }
    void                            SetStatic               ( bool bStatic );
    
    inline unsigned char            GetAlpha                ( void )                            { return m_ucAlpha; }   
    void                            SetAlpha                ( unsigned char ucAlpha );
    void                            SetScale                ( float fScale );

    inline bool                     IsCollisionEnabled      ( void )                            { return m_bUsesCollision; };
    void                            SetCollisionEnabled     ( bool bCollisionEnabled );

    void                            ReCreate                ( void );
protected:
    void                            StreamIn                ( bool bInstantly );
    void                            StreamOut               ( void );

    void                            Create                  ( void );
    void                            Destroy                 ( void );

    void                            NotifyCreate            ( void );
    void                            NotifyDestroy           ( void );

    void                            StreamedInPulse         ( void );

    class CClientObjectManager*         m_pObjectManager;
    class CClientModelRequestManager*   m_pModelRequester;

    unsigned short                      m_usModel;

    CVector                             m_vecPosition;
    CVector                             m_vecRotation;
    bool                                m_bIsVisible;
    bool                                m_bIsStatic;
    bool                                m_bUsesCollision;
    unsigned char                       m_ucAlpha;
    float                               m_fScale;

    CVector                             m_vecMoveSpeed;

public:
    CObject*                            m_pObject;
};

#endif
