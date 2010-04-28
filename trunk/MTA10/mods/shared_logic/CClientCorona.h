/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCorona.h
*  PURPOSE:     Corona marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTCORONA_H
#define __CCLIENTCORONA_H

#include "CClientMarker.h"
class CClientMarker;

class CClientCorona : public CClientMarkerCommon
{
public:
                                    CClientCorona                       ( CClientMarker * pThis );
                                    ~CClientCorona                      ( void );

    inline unsigned int             GetMarkerType                       ( void ) const                      { return CClientMarkerCommon::CLASS_CORONA; }

    bool                            IsHit                               ( const CVector& vecPosition ) const;

    inline void                     GetPosition                         ( CVector& vecPosition ) const      { vecPosition = m_Matrix.vPos; }
    inline void                     SetPosition                         ( const CVector& vecPosition )      { m_Matrix.vPos = vecPosition; }

    inline void                     GetMatrix                           ( CMatrix & mat )                   { mat = m_Matrix; }
    inline void                     SetMatrix                           ( CMatrix & mat )                   { m_Matrix = mat; }

    inline bool                     IsVisible                           ( void ) const                      { return m_bVisible; };
    inline void                     SetVisible                          ( bool bVisible )                   { m_bVisible = bVisible; };

    inline SColor                   GetColor                            ( void ) const                      { return m_Color; }
    inline void                     SetColor                            ( const SColor color )              { m_Color = color; }

    inline float                    GetSize                             ( void ) const                      { return m_fSize; };
    inline void                     SetSize                             ( float fSize )                     { m_fSize = fSize; };

protected:
    bool                            IsStreamedIn                        ( void )                            { return m_bStreamedIn; };
    void                            StreamIn                            ( void );
    void                            StreamOut                           ( void );

    void                            DoPulse                             ( void );

private:
    CClientMarker *                 m_pThis;
    bool                            m_bStreamedIn;
    unsigned long                   m_ulIdentifier;
    CMatrix                         m_Matrix;
    bool                            m_bVisible;
    float                           m_fSize;
    SColor                          m_Color;
    CCoronas *                      m_pCoronas;
};

#endif
