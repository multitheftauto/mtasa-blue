/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColPolygon.h
*  PURPOSE:     Polygon-shaped collision entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOLPOLYGON_H
#define __CCOLPOLYGON_H

#include "CColShape.h"

#include <list>

class CColPolygon : public CColShape
{
public:
                                CColPolygon         ( CColManager* pManager, CElement* pParent, const CVector& vecPosition, CXMLNode* pNode = NULL );

    virtual CSphere         GetWorldBoundingSphere  ( void );

    eColShapeType               GetShapeType        ( void )                { return COLSHAPE_POLYGON; }

    bool                        DoHitDetection      ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius );

    void                        SetPosition         ( const CVector& vecPosition );

    void                        AddPoint            ( CVector2D vecPoint );

    unsigned int                                CountPoints         ( void ) const          { return static_cast < unsigned int > ( m_Points.size () ); };
    std::vector < CVector2D > ::const_iterator  IterBegin           ( void )                { return m_Points.begin (); };
    std::vector < CVector2D > ::const_iterator  IterEnd             ( void )                { return m_Points.end (); };

protected:

    std::vector < CVector2D >   m_Points;

    bool                        ReadSpecialData     ( void );

    bool                        IsInBounds          ( CVector vecPoint );

    float                       m_fRadius;

};

#endif
