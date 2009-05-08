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

    void                        Unlink                  ();
    bool                        ReadSpecialData         ();

    EWaterType                  GetWaterType            ()  { return m_WaterType; }
    int                         GetNumVertices          ()  { return m_WaterType == TRIANGLE ? 3 : 4; }
    bool                        GetVertex               ( int index, CVector& vecPosition );
    void                        SetVertex               ( int index, CVector& vecPosition );

    void                        SetLevel                ( float fLevel ) { m_fWaterLevel = fLevel; };
    float                       GetLevel                () { return m_fWaterLevel; };
    bool                        Valid                   ();
    bool                        HasWaterLevelChanged    () { return m_bWaterChanged; };
    void                        SetWaterLevelChanged    ( bool bChanged ) { m_bWaterChanged = bChanged; };

private:
    void                        RoundVertices           ();
    void                        RoundVertex             ( int index );
    bool                        IsVertexWithinWorld     ( int index );

    bool                        m_bWaterChanged;
    CWaterManager*              m_pWaterManager;

    CVector                     m_Vertices [ 4 ];
    EWaterType                  m_WaterType;
    float                       m_fWaterLevel;
};

#endif
