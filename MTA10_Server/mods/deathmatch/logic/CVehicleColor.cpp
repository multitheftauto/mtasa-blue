/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVehicleColor.cpp
*  PURPOSE:     Vehicle entity color class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVehicleColor::CVehicleColor ( void )
{
    // Init
    m_ucColor1 = 0;
    m_ucColor2 = 0;
    m_ucColor3 = 0;
    m_ucColor4 = 0;
}


CVehicleColor::CVehicleColor ( unsigned char ucColor1, unsigned char ucColor2, unsigned char ucColor3, unsigned char ucColor4 )
{
    m_ucColor1 = ucColor1;
    m_ucColor2 = ucColor2;
    m_ucColor3 = ucColor3;
    m_ucColor4 = ucColor4;
}


void CVehicleColor::SetColor ( unsigned char ucColor1, unsigned char ucColor2, unsigned char ucColor3, unsigned char ucColor4 )
{
    m_ucColor1 = ucColor1;
    m_ucColor2 = ucColor2;
    m_ucColor3 = ucColor3;
    m_ucColor4 = ucColor4;
}
