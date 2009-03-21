/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCheckpoint.h
*  PURPOSE:     Checkpoint marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTCHECKPOINT_H
#define __CCLIENTCHECKPOINT_H

#include "CClientMarkerCommon.h"
#include <list>

class CClientMarker;

class CClientCheckpoint : public CClientMarkerCommon
{
    friend class CClientMarkerManager;

public:
    enum
    {
        TYPE_NORMAL,
        TYPE_RING,
        TYPE_INVALID,
    };

    enum
    {
        ICON_NONE,
        ICON_ARROW,
        ICON_FINISH,
    };

                                    CClientCheckpoint               ( CClientMarker * pThis );
                                    ~CClientCheckpoint              ( void );

    inline unsigned int             GetMarkerType                   ( void ) const                      { return CClientMarkerCommon::CLASS_CHECKPOINT; };

    unsigned long                   GetCheckpointType               ( void ) const;
    void                            SetCheckpointType               ( unsigned long ulType );

    bool                            IsHit                           ( const CVector& vecPosition ) const;
    
    inline void                     GetPosition                     ( CVector& vecPosition ) const      { vecPosition = m_vecPosition; };
    void                            SetPosition                     ( const CVector& vecPosition );
    void                            SetDirection                    ( const CVector& vecDirection );
    void                            SetNextPosition                 ( const CVector& vecPosition );

    inline bool                     IsVisible                       ( void ) const                      { return m_bVisible; };
    void                            SetVisible                      ( bool bVisible );

    inline unsigned int             GetIcon                         ( void ) const                      { return m_uiIcon; };
    void                            SetIcon                         ( unsigned int uiIcon );
    
    inline unsigned long            GetColor                        ( void )                            { return m_rgbaColor; };
    void                            GetColor                        ( unsigned char& Red, unsigned char& Green, unsigned char& Blue, unsigned char& Alpha ) const;
    inline unsigned char            GetColorRed                     ( void ) const                      { return static_cast < unsigned char > ( m_rgbaColor ); };
    inline unsigned char            GetColorGreen                   ( void ) const                      { return static_cast < unsigned char > ( m_rgbaColor >> 8 ); };
    inline unsigned char            GetColorBlue                    ( void ) const                      { return static_cast < unsigned char > ( m_rgbaColor >> 16 ); };
    inline unsigned char            GetColorAlpha                   ( void ) const                      { return static_cast < unsigned char > ( m_rgbaColor >> 24 ); };
    void                            SetColor                        ( unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha );
    void                            SetColor                        ( unsigned long ulColor );

    inline float                    GetSize                         ( void ) const                      { return m_fSize; };
    void                            SetSize                         ( float fSize );

    inline bool                     HasTarget                       ( void )                            { return m_bHasTarget; }
    inline void                     SetHasTarget                    ( bool bHasTarget )                 { m_bHasTarget = bHasTarget; }
    inline void                     GetTarget                       ( CVector& vecTarget )              { vecTarget = m_vecTarget; }
    inline void                     SetTarget                       ( const CVector& vecTarget )              { m_vecTarget = vecTarget; }

    static unsigned char            StringToIcon                    ( const char* szString );
    static bool                     IconToString                    ( unsigned char ucIcon, char* szString );
    void                            ReCreateWithSameIdentifier      ( void );
protected:
    bool                            IsStreamedIn                    ( void )                            { return m_bStreamedIn; };
    void                            StreamIn                        ( void );
    void                            StreamOut                       ( void );

private:
    void                            Create                          ( unsigned long ulIdentifier = 0 );
    void                            Destroy                         ( void );
    void                            ReCreate                        ( void );

    CClientMarker *                 m_pThis;
    bool                            m_bStreamedIn;
    CVector                         m_vecPosition;
    CVector                         m_vecDirection;
    CVector                         m_vecTargetPosition;
    bool                            m_bVisible;
    unsigned int                    m_uiIcon;
    DWORD                           m_dwType;
    float                           m_fSize;
    RGBA                            m_rgbaColor;
    CCheckpoint*                    m_pCheckpoint;

    DWORD                           m_dwIdentifier;    
    bool                            m_bHasTarget;
    CVector                         m_vecTarget;
};

#endif
