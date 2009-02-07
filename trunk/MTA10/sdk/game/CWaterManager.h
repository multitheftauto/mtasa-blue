/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CWaterManager.h
*  PURPOSE:		Water interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWATERMANAGER_H
#define __CWATERMANAGER_H

enum EWaterPolyType
{
    WATER_POLY_NONE       = 0,
    WATER_POLY_QUAD       = 1,
    WATER_POLY_TRIANGLE   = 2,
    WATER_POLY_LIST       = 3
};

#define WATER_VISIBLE 1
#define WATER_SHALLOW 2

class CWaterVertex
{
public:
    virtual WORD                GetID             () = 0;
    virtual void                GetPosition       ( CVector& vec ) = 0;
    virtual void                SetPosition       ( CVector& vec, void* pChangeSource = NULL ) = 0;
};

class CWaterPoly
{
public:
    virtual EWaterPolyType      GetType           () = 0;
    virtual WORD                GetID             () = 0;
    virtual int                 GetNumVertices    () = 0;
    virtual CWaterVertex*       GetVertex         ( int index ) = 0;
    virtual bool                ContainsPoint     ( float fX, float fY ) = 0;
};



class CWaterManager
{
public:
    virtual CWaterPoly*         GetPolyAtPoint    ( CVector& vecPosition ) = 0;
    virtual CWaterPoly*         CreateQuad        ( CVector& vec1, CVector& vec2, CVector& vec3, CVector& vec4, bool bShallow = false, void* pChangeSource = NULL ) = 0;
    virtual CWaterPoly*         CreateTriangle    ( CVector& vec1, CVector& vec2, CVector& vec3, bool bShallow = false, void* pChangeSource = NULL ) = 0;
    virtual bool                DeletePoly        ( CWaterPoly* pPoly ) = 0;

    virtual bool                GetWaterLevel     ( CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown ) = 0;
    virtual bool                SetWaterLevel     ( CVector& vecPosition, float fLevel, void* pChangeSource = NULL ) = 0;
    virtual bool                TestLineAgainstWater ( CVector& vecStart, CVector& vecEnd, CVector* vecCollision ) = 0;

    virtual void                UndoChanges       ( void* pChangeSource = NULL ) = 0;
    virtual void                Reset             () = 0;
};

#endif