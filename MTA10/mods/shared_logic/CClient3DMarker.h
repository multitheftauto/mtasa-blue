/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClient3DMarker.h
*  PURPOSE:     3D marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

#ifndef __CCLIENT3DMARKER_H
#define __CCLIENT3DMARKER_H

#include "CClientMarker.h"
class CClientMarker;

class CClient3DMarker : public CClientMarkerCommon
{
public:
    enum
    {
        TYPE_CYLINDER,
        TYPE_ARROW,
        TYPE_INVALID,
    };

                                    CClient3DMarker                     ( CClientMarker * pThis );
                                    ~CClient3DMarker                    ( void );

    inline unsigned int             GetMarkerType                       ( void ) const                      { return CClientMarkerCommon::CLASS_3DMARKER; };

    unsigned long                   Get3DMarkerType                     ( void );
    void                            Set3DMarkerType                     ( unsigned long ulType );

    bool                            IsHit                               ( const CVector& vecPosition ) const;

    inline void                     GetPosition                         ( CVector& vecPosition ) const      { vecPosition = m_Matrix.vPos; };
    inline void                     SetPosition                         ( const CVector& vecPosition )      { m_Matrix.vPos = vecPosition; };

    inline void                     GetMatrix                           ( CMatrix & mat )                   { mat = m_Matrix; };
    inline void                     SetMatrix                           ( CMatrix & mat )                   { m_Matrix = mat; };

    inline bool                     IsVisible                           ( void ) const                      { return m_bVisible; };
    inline void                     SetVisible                          ( bool bVisible )                   { m_bVisible = bVisible; };

    inline SColor                   GetColor                            ( void ) const                      { return m_Color; }
    inline void                     SetColor                            ( const SColor color )              { m_Color = color; }

    inline float                    GetSize                             ( void ) const                      { return m_fSize; };
    inline void                     SetSize                             ( float fSize )                     { m_fSize = fSize; };

    inline float                    GetPulseFraction                    ( void )                            { return static_cast < float > ( m_pMarker->GetPulseFraction() ); };
    void                            SetPulseFraction                    ( float fFraction )                 { m_pMarker->SetPulseFraction ( fFraction ); };

protected:
    void                            StreamIn                            ( void );
    void                            StreamOut                           ( void );

    void                            DoPulse                             ( void );

private:
    CClientMarker *                 m_pThis;
    CMatrix                         m_Matrix;

    bool                            m_bVisible;
    DWORD                           m_dwType;
    float                           m_fSize;
    SColor                          m_Color;
    C3DMarker*                      m_pMarker;
    unsigned int                    m_ulIdentifier;
    bool                            m_bMarkerStreamedIn;
};

#endif
