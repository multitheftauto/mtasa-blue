/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPointLightsSA.h
*  PURPOSE:     Point lights class header
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPointLightsSA_H
#define __CPointLightsSA_H

#include <game/CPointLights.h>

#define FUNC_CPointLights_AddLight      0x7000e0

class CPointLightsSA : public CPointLights
{
public:
    void                        AddLight                ( unsigned char ucType, CVector vec_2, CVector vec_3, float fRed, float fGreen, float fBlue, float f_7, unsigned char uc_8, bool b_9, CEntity * pEntity );
};

#endif