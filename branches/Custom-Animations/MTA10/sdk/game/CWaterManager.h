/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CWaterManager.h
*  PURPOSE:     Water interface
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
    virtual CWaterPoly*         GetPolyAtPoint    ( const CVector& vecPosition ) = 0;
    virtual CWaterPoly*         CreateQuad        ( const CVector& vecBL, const CVector& vecBR, const CVector& vecTL, const CVector& vecTR, bool bShallow = false ) = 0;
    virtual CWaterPoly*         CreateTriangle    ( const CVector& vec1, const CVector& vec2, const CVector& vec3, bool bShallow = false ) = 0;
    virtual bool                DeletePoly        ( CWaterPoly* pPoly ) = 0;

    virtual bool                GetWaterLevel           ( const CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown ) = 0;
    virtual bool                SetWorldWaterLevel      ( float fLevel, void* pChangeSource, bool bIncludeWorldNonSeaLevel ) = 0;
    virtual bool                SetPositionWaterLevel   ( const CVector& vecPosition, float fLevel, void* pChangeSource ) = 0;
    virtual bool                SetPolyWaterLevel       ( CWaterPoly* pPoly, float fLevel, void* pChangeSource ) = 0;
    virtual void                ResetWorldWaterLevel    ( void ) = 0;

    virtual float               GetWaveLevel            () = 0;
    virtual void                SetWaveLevel            ( float fWaveLevel ) = 0;

    virtual bool                TestLineAgainstWater ( const CVector& vecStart, const CVector& vecEnd, CVector* vecCollision ) = 0;

    virtual void                UndoChanges       ( void* pChangeSource = NULL ) = 0;
    virtual void                RebuildIndex      () = 0;       // Call this after moving a polygon's vertices
    virtual void                Reset             () = 0;       // Reset all water to SA default
};

#endif
