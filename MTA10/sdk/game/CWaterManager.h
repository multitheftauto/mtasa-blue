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

#include "CWater.h"

class CWaterManager
{
public:
    virtual CWaterPoly*         GetPolyAtPoint    ( CVector& vecPosition ) = 0;
    virtual CWaterPoly*         CreateQuad        ( CVector& vecBL, CVector& vecBR, CVector& vecTL, CVector& vecTR, bool bShallow = false ) = 0;
    virtual CWaterPoly*         CreateTriangle    ( CVector& vec1, CVector& vec2, CVector& vec3, bool bShallow = false ) = 0;
    virtual bool                DeletePoly        ( CWaterPoly* pPoly ) = 0;

    virtual bool                GetWaterLevel     ( CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown ) = 0;
    virtual bool                SetWaterLevel     ( CVector* pvecPosition, float fLevel, void* pChangeSource = NULL ) = 0;
    virtual bool                TestLineAgainstWater ( CVector& vecStart, CVector& vecEnd, CVector* vecCollision ) = 0;

    virtual void                UndoChanges       ( void* pChangeSource = NULL ) = 0;
    virtual void                RebuildIndex      () = 0;       // Call this after moving a polygon's vertices
    virtual void                Reset             () = 0;       // Reset all water to SA default
};

#endif