/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColTube.h
*  PURPOSE:     Tube-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOLTUBE_H
#define __CCOLTUBE_H

#include "CColShape.h"

class CColTube : public CColShape
{
public:
                            CColTube        ( CColManager* pManager, CElement* pParent, CXMLNode* pNode = NULL );
                            CColTube        ( CColManager* pManager, CElement* pParent, const CVector& vecPosition, float fRadius, float fHeight, CXMLNode* pNode = NULL );

    eColShapeType           GetShapeType    ( void )            { return COLSHAPE_TUBE; }
    bool                    DoHitDetection  ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius );

    inline float            GetRadius       ( void )            { return m_fRadius; };
    inline void             SetRadius       ( float fRadius )   { m_fRadius = fRadius; };
    inline float            GetHeight       ( void )            { return m_fHeight; };
    inline void             SetHeight       ( float fHeight )   { m_fHeight = fHeight; };

protected:
    bool                    ReadSpecialData ( void );

    float                   m_fRadius;
    float                   m_fHeight;
};

#endif
