/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPointLightsSA.h
*  PURPOSE:     Header file for PointLights entity class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_POINTLIGHTS
#define __CGAMESA_POINTLIGHTS

#include <game/CPointLights.h>

#define FUNC_CPointLights_AddLight      0x7000e0

class CPointLightsSA : public CPointLights
{
public:
    void                        AddLight                ( int iMode, const CVector vecPosition, CVector vecDirection, float fRadius, SColor color, unsigned char uc_8, bool bCreatesShadow, CEntity * pAffected );
};

#endif