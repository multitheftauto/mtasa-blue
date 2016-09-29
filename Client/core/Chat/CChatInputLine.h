/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once 

class CChatInputLine : public CChatLine
{
public:
    void                        Draw(CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow);
    void                        Clear(void);

    CChatLineSection            m_Prefix;
    std::vector < CChatLine >   m_ExtraLines;
};


