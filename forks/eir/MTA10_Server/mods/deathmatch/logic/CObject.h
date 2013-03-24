/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObject.h
*  PURPOSE:     Object entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#ifndef __COBJECT_H
#define __COBJECT_H

//Kayl: There is now too many includes here, try to make it work with StdInc.h if possible
#include "CElement.h"
#include "CEvents.h"
#include "CObjectManager.h"
#include "Utils.h"

#include "CEasingCurve.h"
#include "TInterpolation.h"
#include "CPositionRotationAnimation.h"

class CObject : public CElement
{
    friend class CPlayer;

public:
    explicit                    CObject                 ( CElement* pParent, CXMLNode* pNode, class CObjectManager* pObjectManager, bool bIsLowLod );
    explicit                    CObject                 ( const CObject& Copy );
                                ~CObject                ( void );

    bool                        IsEntity                ( void )                    { return true; }

    void                        Unlink                  ( void );
    bool                        ReadSpecialData         ( void );

    const CVector&              GetPosition             ( void );
    void                        SetPosition             ( const CVector& vecPosition );

    void                        GetRotation             ( CVector & vecRotation );
    void                        SetRotation             ( const CVector& vecRotation );

    bool                        IsMoving                ( void );
    void                        Move                    ( const CPositionRotationAnimation& a_rMoveAnimation );
    void                        StopMoving              ( void );
    const CPositionRotationAnimation*   GetMoveAnimation    ( );

    inline unsigned char        GetAlpha                ( void )                        { return m_ucAlpha; }
    inline void                 SetAlpha                ( unsigned char ucAlpha )       { m_ucAlpha = ucAlpha; }

    inline unsigned short       GetModel                ( void )                        { return m_usModel; }
    inline void                 SetModel                ( unsigned short usModel )      { m_usModel = usModel; }

    const CVector&              GetScale                ( void )                        { return m_vecScale; }
    inline void                 SetScale                ( const CVector& vecScale )     { m_vecScale = vecScale; }

    inline bool                 GetCollisionEnabled     ( void )                        { return m_bCollisionsEnabled; }
    inline void                 SetCollisionEnabled     ( bool bCollisionEnabled )      { m_bCollisionsEnabled = bCollisionEnabled; }

    inline bool                 IsStatic                ( void )                        { return m_bIsStatic; }
    inline void                 SetStatic               ( bool bStatic )                { m_bIsStatic = bStatic; }

    inline float                GetHealth               ( void )                        { return m_fHealth; }
    inline void                 SetHealth               ( float fHealth )               { m_fHealth = fHealth; }

    inline bool                 IsSyncable              ( void )                        { return m_bSyncable; }
    inline void                 SetSyncable             ( bool bSyncable )              { m_bSyncable = bSyncable; }

    inline CPlayer*             GetSyncer               ( void )                        { return m_pSyncer; }
    void                        SetSyncer               ( CPlayer* pPlayer );

    bool                        IsLowLod                ( void );
    bool                        SetLowLodObject         ( CObject* pLowLodObject );
    CObject*                    GetLowLodObject         ( void );

private:
    CObjectManager*             m_pObjectManager;
    CVector                     m_vecRotation;
    unsigned char               m_ucAlpha;
    unsigned short              m_usModel;
    CVector                     m_vecScale;
    bool                        m_bIsStatic;
    float                       m_fHealth;
    bool                        m_bBreakable;
    bool                        m_bSyncable;
    CPlayer*                    m_pSyncer;

protected:
    bool                        m_bCollisionsEnabled;

    const bool                  m_bIsLowLod;            // true if this object is low LOD
    CObject*                    m_pLowLodObject;        // Pointer to low LOD version of this object
    std::vector < CObject* >    m_HighLodObjectList;    // List of objects that use this object as a low LOD version

public:
    CPositionRotationAnimation* m_pMoveAnimation;
};

#endif
