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

#include "CElement.h"
#include "CEvents.h"
#include "Utils.h"

struct SObjectMoveData
{
    bool            bActive;
    CVector         vecStartPosition;
    CVector         vecStopPosition;
    CVector         vecStartRotation;
    CVector         vecStopRotation;
    unsigned long   ulTime;
    unsigned long   ulTimeStart;
    // Saves calculating more than once
    unsigned long   ulTimeStop;
};

class CObject : public CElement
{    

public:
                                CObject                 ( CElement* pParent, CXMLNode* pNode, class CObjectManager* pObjectManager );
                                CObject                 ( const CObject& Copy );
                                ~CObject                ( void );

    bool                        IsEntity                ( void )                    { return true; }

    void                        Unlink                  ( void );
    bool                        ReadSpecialData         ( void );

    const CVector&              GetPosition             ( void );
    void                        SetPosition             ( const CVector& vecPosition );

    const CVector&              GetRotation             ( void );
    void                        SetRotation             ( const CVector& vecRotatation );

    bool                        IsMoving                ( void );
    void                        Move                    ( const CVector& vecPosition, const CVector& vecRotation, unsigned long ulTime );
    void                        StopMoving              ( void );
    unsigned long               GetMoveTimeLeft         ( void );

    void                        AttachTo                ( CElement* pElement );

    inline unsigned char        GetAlpha                ( void )                        { return m_ucAlpha; }
    inline void                 SetAlpha                ( unsigned char ucAlpha )       { m_ucAlpha = ucAlpha; }

    inline unsigned short       GetModel                ( void )                        { return m_usModel; }
    inline void                 SetModel                ( unsigned short usModel )      { m_usModel = usModel; }

private:
    CObjectManager*             m_pObjectManager;
    char                        m_szName [MAX_ELEMENT_NAME_LENGTH + 1];
    CVector                     m_vecRotation;
    unsigned char               m_ucAlpha;
    unsigned short              m_usModel;

public:
    SObjectMoveData             m_moveData;
};

#endif
