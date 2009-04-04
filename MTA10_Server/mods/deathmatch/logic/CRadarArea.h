/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRadarArea.h
*  PURPOSE:     Radar area entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRADARAREA_H
#define __CRADARAREA_H

#include "CPerPlayerEntity.h"
#include <CVector2D.h>

class CRadarArea : public CPerPlayerEntity
{
    friend class CRadarAreaManager;

public:
                                ~CRadarArea                     ( void );

    void                        Unlink                          ( void );

    bool                        ReadSpecialData                 ( void );

    inline const CVector2D&     GetSize                         ( void )                            { return m_vecSize; };
    inline unsigned long        GetColor                        ( void )                            { return m_ulColor; };
    /* GTA radar area colors are in the ABGR format */
    inline unsigned char        GetColorRed                     ( void )                            { return static_cast < unsigned char > ( m_ulColor & 0xFF ); };
    inline unsigned char        GetColorGreen                   ( void )                            { return static_cast < unsigned char > ( ( m_ulColor >> 8 ) & 0xFF ); };
    inline unsigned char        GetColorBlue                    ( void )                            { return static_cast < unsigned char > ( ( m_ulColor >> 16 ) & 0xFF ); };
    inline unsigned char        GetColorAlpha                   ( void )                            { return static_cast < unsigned char > ( ( m_ulColor >> 24 ) & 0xFF ); };
    inline bool                 IsFlashing                      ( void )                            { return m_bIsFlashing; };

    void                        SetPosition                     ( const CVector& vecPosition );
    void                        SetSize                         ( const CVector2D& vecSize );
    void                        SetColor                        ( unsigned long ulColor );
    void                        SetColor                        ( unsigned char ucRed,
                                                                  unsigned char ucGreen,
                                                                  unsigned char ucBlue,
                                                                  unsigned char ucAlpha );
    void                        SetFlashing                     ( bool bFlashing );

private:
                                CRadarArea                      ( class CRadarAreaManager* pRadarAreaManager, CElement* pParent, CXMLNode* pNode );

    class CRadarAreaManager*    m_pRadarAreaManager;

    CVector2D                   m_vecSize;
    unsigned long               m_ulColor;
    bool                        m_bIsFlashing;
};

#endif
