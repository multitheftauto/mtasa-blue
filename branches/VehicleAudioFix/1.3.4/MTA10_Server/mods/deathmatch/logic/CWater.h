/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWater.h
*  PURPOSE:     Water entity class
*  DEVELOPERS:  arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWATER_H
#define __CWATER_H

class CWaterManager;

class CWater : public CElement
{
public:
    enum EWaterType
    {
        TRIANGLE,
        QUAD
    };

                                CWater                  ( CWaterManager* pWaterManager, CElement* pParent, CXMLNode* pNode = NULL, EWaterType waterType = QUAD );
                                ~CWater                 ();

    bool                        IsEntity                ()  { return true; }

    const CVector&              GetPosition             ();
    void                        SetPosition             ( const CVector& vecPosition );
    float                       GetLevel                () const;
    void                        SetLevel                ( float fLevel );

    void                        Unlink                  ();
    bool                        ReadSpecialData         ();

    EWaterType                  GetWaterType            () const  { return m_WaterType; }
    int                         GetNumVertices          () const  { return m_WaterType == TRIANGLE ? 3 : 4; }
    bool                        GetVertex               ( int index, CVector& vecPosition ) const;
    void                        SetVertex               ( int index, CVector& vecPosition );

    bool                        Valid                   ();

private:
    void                        RoundVertices           ();
    void                        RoundVertex             ( int index );
    bool                        IsVertexWithinWorld     ( int index );

    CWaterManager*              m_pWaterManager;

    SFixedArray < CVector, 4 >  m_Vertices;
    EWaterType                  m_WaterType;
};

#endif
