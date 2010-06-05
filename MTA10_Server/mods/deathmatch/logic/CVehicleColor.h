/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVehicleColor.h
*  PURPOSE:     Vehicle entity color class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVEHICLECOLOR_H
#define __CVEHICLECOLOR_H

class CVehicleColor
{
public:
                            CVehicleColor               ( void );
                            CVehicleColor               ( unsigned char ucColor1, unsigned char ucColor2, unsigned char ucColor3, unsigned char ucColor4 );

    inline unsigned long    GetColor                    ( void ) const              { return m_ucColor4 << 24 | m_ucColor3 << 16 | m_ucColor2 << 8 | m_ucColor1; };
    inline unsigned char    GetColor1                   ( void ) const              { return m_ucColor1; };
    inline unsigned char    GetColor2                   ( void ) const              { return m_ucColor2; };
    inline unsigned char    GetColor3                   ( void ) const              { return m_ucColor3; };
    inline unsigned char    GetColor4                   ( void ) const              { return m_ucColor4; };

    void                    SetColor                    ( unsigned char ucColor1, unsigned char ucColor2, unsigned char ucColor3, unsigned char ucColor4 );
    inline void             SetColor1                   ( unsigned char ucColor1 )  { m_ucColor1 = ucColor1; };
    inline void             SetColor2                   ( unsigned char ucColor2 )  { m_ucColor2 = ucColor2; };
    inline void             SetColor3                   ( unsigned char ucColor3 )  { m_ucColor3 = ucColor3; };
    inline void             SetColor4                   ( unsigned char ucColor4 )  { m_ucColor4 = ucColor4; };

private:
    unsigned char           m_ucColor1;
    unsigned char           m_ucColor2;
    unsigned char           m_ucColor3;
    unsigned char           m_ucColor4;
};

#endif
