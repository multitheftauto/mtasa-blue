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

    inline unsigned int             GetMarkerType                       ( void ) const                      { return CClientMarkerCommon::CLASS_CORONA; };

    bool                            IsHit                               ( const CVector& vecPosition ) const;

    inline void                     GetPosition                         ( CVector& vecPosition ) const      { vecPosition = m_vecPosition; };
    void                            SetPosition                         ( const CVector& vecPosition );

    inline bool                     IsVisible                           ( void ) const                      { return m_bVisible; };
    inline void                     SetVisible                          ( bool bVisible )                   { m_bVisible = bVisible; };

    inline unsigned long            GetColor                        ( void )                            { return m_rgbaColor; };
    void                            GetColor                        ( unsigned char& Red, unsigned char& Green, unsigned char& Blue, unsigned char& Alpha ) const;
    inline unsigned char            GetColorRed                     ( void ) const                      { return m_rgbaColor.R; };
    inline unsigned char            GetColorGreen                   ( void ) const                      { return m_rgbaColor.G; };
    inline unsigned char            GetColorBlue                    ( void ) const                      { return m_rgbaColor.B; };
    inline unsigned char            GetColorAlpha                   ( void ) const                      { return m_rgbaColor.A; };
    void                            SetColor                        ( unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha );
    void                            SetColor                        ( unsigned long ulColor );

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
    CVector                         m_vecPosition;
    bool                            m_bVisible;
    float                           m_fSize;
    RGBA                            m_rgbaColor;
    CCoronas *                      m_pCoronas;
};

#endif
