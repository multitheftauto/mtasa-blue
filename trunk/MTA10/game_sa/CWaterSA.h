/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CWaterSA.h
*  PURPOSE:		Control the lakes and seas
*  DEVELOPERS:	arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWATERSA_H
#define __CWATERSA_H

// -------------------------------
// SA interfaces

class CWaterVertexSAInterface
{
public:
    short m_sX;
    short m_sY;
    float m_fZ;
    float m_fUnknown;
    float m_fHeight;
    char m_cFlowX;
    char m_cFlowY;
    WORD m_wPadding;   
};

class CWaterPolySAInterface
{
public:
    WORD m_wVertexIDs[3];
};

class CWaterQuadSAInterface : public CWaterPolySAInterface
{
public:
    WORD m_wFourthVertexIDDummy;
    WORD m_wFlags;
};

class CWaterTriangleSAInterface : public CWaterPolySAInterface
{
public:
    WORD m_wFlags;
};

// -------------------------------
// Interface wrappers

class CWaterVertexSA : public CWaterVertex
{
public:
                                     CWaterVertexSA    () { m_pInterface = NULL; }
                                     CWaterVertexSA    ( CWaterVertexSAInterface* pInterface ) { m_pInterface = pInterface; }

    CWaterVertexSAInterface*         GetInterface      () { return m_pInterface; }
    void                             SetInterface      ( CWaterVertexSAInterface* pInterface ) { m_pInterface = pInterface; }

    WORD                             GetID             ();

    void                             GetPosition       ( CVector& vec );
    bool                             SetPosition       ( CVector& vec, void* pChangeSource = NULL );

protected:
    CWaterVertexSAInterface*         m_pInterface;
};

class CWaterPolySA : public CWaterPoly
{
public:
    CWaterPolySAInterface*           GetInterface      () { return m_pInterface; }
    virtual void                     SetInterface      ( CWaterPolySAInterface* pInterface ) = 0;

    virtual EWaterPolyType           GetType           () = 0;
    virtual int                      GetNumVertices    () = 0;
    WORD                             GetID             () { return m_wID; }
    CWaterVertex*                    GetVertex         ( int index );
    bool                             ContainsPoint     ( float fX, float fY );

protected:
    CWaterPolySAInterface*           m_pInterface;
    WORD                             m_wID;
};

class CWaterQuadSA : public CWaterPolySA
{
public:
                                     CWaterQuadSA      () { m_pInterface = NULL; m_wID = ~0; }
                                     CWaterQuadSA      ( CWaterPolySAInterface* pInterface ) { SetInterface ( pInterface ); }

    CWaterQuadSAInterface*           GetInterface      () { return (CWaterQuadSAInterface *)m_pInterface; }
    void                             SetInterface      ( CWaterPolySAInterface* pInterface );

    EWaterPolyType                   GetType           () { return WATER_POLY_QUAD; }
    int                              GetNumVertices    () { return 4; }
};

class CWaterTriangleSA : public CWaterPolySA
{
public:
                                     CWaterTriangleSA  () { m_pInterface = NULL; m_wID = ~0; }
                                     CWaterTriangleSA  ( CWaterPolySAInterface* pInterface ) { SetInterface ( pInterface ); }

    CWaterTriangleSAInterface*       GetInterface      () { return (CWaterTriangleSAInterface *)m_pInterface; }
    void                             SetInterface      ( CWaterPolySAInterface* pInterface );

    EWaterPolyType                   GetType           () { return WATER_POLY_TRIANGLE; }
    int                              GetNumVertices    () { return 3; }
};

#endif
