/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarArea.h
*  PURPOSE:     Radar area entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTRADARAREA_H
#define __CCLIENTRADARAREA_H

#include "CClientEntity.h"
#include <gui/CGUI.h>


class CClientRadarArea : public CClientEntity
{
    friend class CClientRadarAreaManager;

public:
                                CClientRadarArea            ( class CClientManager* pManager, ElementID ID );
                                ~CClientRadarArea           ( void );

    void                        Unlink                      ( void );

    inline eClientEntityType    GetType                     ( void ) const                      { return CCLIENTRADARAREA; };

    inline const CVector2D&     GetPosition                 ( void ) const                      { return m_vecPosition; };
    inline void                 GetPosition                 ( CVector2D& vecBottomLeft ) const  { vecBottomLeft = m_vecPosition; };
    inline void                 GetPosition                 ( CVector& vecBottomLeft ) const    { vecBottomLeft = CVector ( m_vecPosition.fX, m_vecPosition.fY, 0.0f ); };
    inline void                 SetPosition                 ( const CVector2D& vecBottomLeft )  { m_vecPosition = vecBottomLeft; };
    inline void                 SetPosition                 ( const CVector& vecBottomLeft )    { m_vecPosition = CVector2D ( vecBottomLeft.fX, vecBottomLeft.fY ); };

    inline const CVector2D&     GetSize                     ( void ) const                      { return m_vecSize; };
    inline void                 GetSize                     ( CVector2D& vecSize )              { vecSize = m_vecSize; };
    inline void                 SetSize                     ( const CVector2D& vecSize )        { m_vecSize = vecSize; };

    inline unsigned long        GetColor                    ( void ) const                      { return m_ulColor; };
    void                        SetColor                    ( unsigned long ulColor );
    void                        SetColor                    ( unsigned char ucR, unsigned char ucG, unsigned char ucB, unsigned char ucA );

    inline bool                 IsFlashing                  ( void ) const                      { return m_bFlashing; };
    inline void                 SetFlashing                 ( bool bFlashing )                  { m_bFlashing = bFlashing; };

    void                        SetDimension                ( unsigned short usDimension );
    void                        RelateDimension             ( unsigned short usDimension );

protected:
    void                        DoPulse                     ( void );
	void						DoPulse						( bool bRender );

    CClientRadarAreaManager*    m_pRadarAreaManager;

    CVector2D                   m_vecPosition;
    CVector2D                   m_vecSize;
    unsigned long               m_ulColor;

    bool                        m_bStreamedIn;
    bool                        m_bFlashing;
    unsigned long               m_ulFlashCycleStart;

};

#endif
