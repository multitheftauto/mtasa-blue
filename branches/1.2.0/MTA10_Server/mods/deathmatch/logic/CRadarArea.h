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
    inline SColor               GetColor                        ( void ) const                            { return m_Color; };
    inline bool                 IsFlashing                      ( void )                            { return m_bIsFlashing; };

    void                        SetPosition                     ( const CVector& vecPosition );
    void                        SetSize                         ( const CVector2D& vecSize );
    void                        SetColor                        ( const SColor color );
    void                        SetFlashing                     ( bool bFlashing );

private:
                                CRadarArea                      ( class CRadarAreaManager* pRadarAreaManager, CElement* pParent, CXMLNode* pNode );

    class CRadarAreaManager*    m_pRadarAreaManager;

    CVector2D                   m_vecSize;
    SColor                      m_Color;
    bool                        m_bIsFlashing;
};

#endif
