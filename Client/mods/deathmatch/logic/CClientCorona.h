/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientCorona.h
 *  PURPOSE:     Corona marker entity class
 *
 *****************************************************************************/

#ifndef __CCLIENTCORONA_H
#define __CCLIENTCORONA_H

#include "CClientMarker.h"
class CClientMarker;

class CClientCorona : public CClientMarkerCommon
{
public:
    CClientCorona(CClientMarker* pThis);
    ~CClientCorona(void);

    unsigned int GetMarkerType(void) const { return CClientMarkerCommon::CLASS_CORONA; }

    bool IsHit(const CVector& vecPosition) const;

    void GetPosition(CVector& vecPosition) const { vecPosition = m_Matrix.vPos; }
    void SetPosition(const CVector& vecPosition) { m_Matrix.vPos = vecPosition; }

    void GetMatrix(CMatrix& mat) { mat = m_Matrix; }
    void SetMatrix(CMatrix& mat) { m_Matrix = mat; }

    bool IsVisible(void) const { return m_bVisible; };
    void SetVisible(bool bVisible) { m_bVisible = bVisible; };

    SColor GetColor(void) const { return m_Color; }
    void   SetColor(const SColor& color) { m_Color = color; }

    float GetSize(void) const { return m_fSize; };
    void  SetSize(float fSize) { m_fSize = fSize; };

protected:
    bool IsStreamedIn(void) { return m_bStreamedIn; };
    void StreamIn(void);
    void StreamOut(void);

    void DoPulse(void);

private:
    CClientMarkerPtr m_pThis;
    bool             m_bStreamedIn;
    unsigned long    m_ulIdentifier;
    CMatrix          m_Matrix;
    bool             m_bVisible;
    float            m_fSize;
    SColor           m_Color;
    CCoronas*        m_pCoronas;
};

#endif
