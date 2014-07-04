/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBlip.h
*  PURPOSE:     Blip entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CBlip;

#ifndef __CBLIP_H
#define __CBLIP_H

#include "CPerPlayerEntity.h"

class CBlip : public CPerPlayerEntity
{
public:
                                CBlip                    ( CElement* pParent, CXMLNode* pNode, class CBlipManager* pBlipManager );
                                ~CBlip                   ( void );

    void                        Unlink                   ( void );
    bool                        ReadSpecialData          ( void );

    const CVector&              GetPosition              ( void );
    void                        SetPosition              ( const CVector& vecPosition );

    inline SColor               GetColor                 ( void ) const                     { return m_Color; }
    void                        SetColor                 ( const SColor color )             { m_Color = color; }
    
    void                        AttachTo                 ( CElement* pElement );

private:
    class CBlipManager*         m_pBlipManager;
    CVector                     m_vecPosition;

public:
    unsigned char               m_ucSize;
    unsigned char               m_ucIcon;
    SColor                      m_Color;
    short                       m_sOrdering;
    unsigned short              m_usVisibleDistance;
};

#endif
