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

    inline unsigned long        GetColor                 ( void )                        { return m_ucColorRed | m_ucColorGreen << 8 | m_ucColorBlue << 16 | m_ucColorAlpha << 24; };
    void                        SetColor                 ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha );
    
    void                        AttachTo                 ( CElement* pElement );

private:
    class CBlipManager*         m_pBlipManager;
    CVector                     m_vecPosition;

public:
    unsigned char               m_ucSize;
    unsigned char               m_ucIcon;
    unsigned char               m_ucColorRed;
    unsigned char               m_ucColorGreen;
    unsigned char               m_ucColorBlue;
    unsigned char               m_ucColorAlpha;
    short                       m_sOrdering;
};

#endif
