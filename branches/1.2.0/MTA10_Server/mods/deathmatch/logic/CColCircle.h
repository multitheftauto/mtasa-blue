/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColCircle.h
*  PURPOSE:     Circle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOLCIRCLE_H
#define __CCOLCIRCLE_H

#include "CColShape.h"

class CColCircle : public CColShape
{
public:
                    CColCircle      ( CColManager* pManager, CElement* pParent, const CVector& vecPosition, float fRadius, CXMLNode* pNode = NULL, bool bIsPartnered = false );

    virtual CSphere         GetWorldBoundingSphere  ( void );

    eColShapeType   GetShapeType    ( void )            { return COLSHAPE_CIRCLE; }

    bool            DoHitDetection  ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius );

    float           GetRadius       ( void )            { return m_fRadius; };
    void            SetRadius       ( float fRadius )   { m_fRadius = fRadius; SizeChanged (); };

protected:
    bool            ReadSpecialData ( void );

    float           m_fRadius;
};

#endif
