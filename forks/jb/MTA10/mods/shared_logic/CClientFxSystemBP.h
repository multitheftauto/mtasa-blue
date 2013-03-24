/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSystemManagerSA.h
*  PURPOSE:     Header file for particle system manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CClientCommon.h"
#include "CClientEntity.h"

class CClientFxSystemBP
{
public:
    CClientFxSystemBP(class CFxSystemBPSA* pFxSystemBP) { m_pFxSystemBP = pFxSystemBP; }
    void SetName(const SString& strName) { m_strName = strName; }
    const SString& GetName() const { return m_strName; }
    void SetLength(float fLength) { m_fLength = fLength; }
    float GetLength() const { return m_fLength; }
    void SetIntervalMin(float fIntervalMin) { m_fIntervalMin = fIntervalMin; }
    float GetIntervalMin() const { return m_fIntervalMin; }
    void SetLength2(float fLength2) { m_fLength2 = fLength2; }
    float GetLength2() const { return m_fLength2; }
    void SetCullDistance(float fCullDistance) { m_fCullDistance = fCullDistance; }
    float GetCullDistance() const { return m_fCullDistance; }
    void SetPlayMode(uint8 ucPlayMode) { m_ucPlayMode = ucPlayMode; }
    uint8 GetPlayMode() const { return m_ucPlayMode; }
    void SetBoundingSphere(RwSphere boundingSphere) { m_boundingSphere = boundingSphere; }
    const RwSphere& GetBoundingSphere() const { return m_boundingSphere; }
private:
    SString m_strName;
    float m_fLength;
    float m_fIntervalMin;
    float m_fLength2;
    float m_fCullDistance;
    uint8 m_ucPlayMode;
    CMappedList<class CClientFxEmitterBP*> m_pFxEmitterBPArray;
    RwSphere m_boundingSphere;
public:
    class CFxSystemBPSA* m_pFxSystemBP;
};