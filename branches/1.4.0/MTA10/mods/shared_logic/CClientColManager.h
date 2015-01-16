/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColManager.h
*  PURPOSE:     Collision entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTCOLMANAGER_H
#define __CCLIENTCOLMANAGER_H

#include "CClientCommon.h"
#include <list>

class CClientColShape;
class CClientEntity;

class CClientColManager
{
    friend class CClientColShape;
public:
                                    ~CClientColManager          ( void );

    void                            DoPulse                     ( void );
    void                            DoHitDetection              ( const CVector& vecNowPosition, float fRadius, CClientEntity* pElement, CClientColShape * pJustThis = NULL, bool bChildren = false );

    bool                            Exists                      ( CClientColShape* pShape );
    void                            DeleteAll                   ( void );

    inline unsigned int             Count                       ( void )                            { return static_cast < unsigned int > ( m_List.size () ); }

protected:
    inline void                     AddToList                   ( CClientColShape* pShape )         { m_List.push_back ( pShape ); };
    void                            RemoveFromList              ( CClientColShape* pShape );
    void                            DoHitDetectionOld           ( const CVector& vecNowPosition, float fRadius, CClientEntity* pElement, CClientColShape * pJustThis = NULL, bool bChildren = false );
    void                            DoHitDetectionNew           ( const CVector& vecNowPosition, float fRadius, CClientEntity* pElement, CClientColShape * pJustThis = NULL, bool bChildren = false );
    void                            DoHitDetectionForColShape   ( CClientColShape* pShape );
    void                            DoHitDetectionForEntity     ( const CVector& vecNowPosition, float fRadius, CClientEntity* pEntity );
    void                            HandleHitDetectionResult    ( bool bHit, CClientColShape* pShape, CClientEntity* pEntity );

    std::vector < CClientColShape* >  m_List;
};

#endif