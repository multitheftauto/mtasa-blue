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

using CHeli_SearchLightCone_t = void(__cdecl *)(int handleId, CVector startPos, CVector endPos, float radius1, float unknownConstant,
    int unkown1, bool renderSpot, CVector* unkown3, CVector* unkown4, CVector* unknown5, int unknown6, float radius2);
using CHeli_PreSearchLightCone_t = int(__cdecl *)();
using CHeli_PostSearchLightCone_t = int(__cdecl *)();

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

void CPointLightsSA::PreRenderHeliLights ()
{
    auto CHeli_PreSearchLightCone = (CHeli_PreSearchLightCone_t)FUNC_CHeli_Pre_SearchLightCone;
    CHeli_PreSearchLightCone ();
}

void CPointLightsSA::PostRenderHeliLights ()
{
    auto CHeli_PostSearchLightCone = (CHeli_PostSearchLightCone_t)FUNC_CHeli_Post_SearchLightCone;
    CHeli_PostSearchLightCone ();
}

void CPointLightsSA::RenderHeliLight ( const CVector& vecStart, const CVector& vecEnd, float startRadius, float endRadius, bool renderSpot )
{
    auto CHeli_SearchLightCone = (CHeli_SearchLightCone_t)FUNC_CHeli_SearchLightCone;
    
    // 3x3 translation matrix (initialised by the game)
    CVector mat[] = { CVector(), CVector(), CVector() };

    // Set render states and render
    CHeli_SearchLightCone ( 0, vecStart, vecEnd, endRadius, 1.0f, 0, renderSpot, &mat[0], &mat[1], &mat[2], 1, startRadius );
}
