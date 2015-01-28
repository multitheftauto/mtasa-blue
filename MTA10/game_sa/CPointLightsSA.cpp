/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPointLightsSA.cpp
*  PURPOSE:     PointLights entity
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CPointLightsSA::AddLight ( int iMode, const CVector vecPosition, CVector vecDirection, float fRadius, SColor color, unsigned char uc_8, bool bCreatesShadow, CEntity * pAffected )
{
    DWORD dwEntityInterface = 0;
    if ( pAffected ) dwEntityInterface = ( DWORD ) pAffected->GetInterface ();
    DWORD dwFunc = FUNC_CPointLights_AddLight;
    float fPosX = vecPosition.fX, fPosY = vecPosition.fY, fPosZ = vecPosition.fZ;
    float fDirX = vecDirection.fX, fDirY = vecDirection.fY, fDirZ = vecDirection.fZ;
    float fRed = (float)color.R / 255, fGreen = (float)color.G / 255, fBlue = (float)color.B / 255;
    _asm
    {
        push    dwEntityInterface
        push    bCreatesShadow
        push    uc_8
        push    fBlue
        push    fGreen
        push    fRed
        push    fRadius
        push    fDirZ
        push    fDirY
        push    fDirX
        push    fPosZ
        push    fPosY
        push    fPosX
        push    iMode
        call    dwFunc
        add     esp, 56
    }
}