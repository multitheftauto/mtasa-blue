/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPointLightsSA.h
*  PURPOSE:     Point lights sdk header
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPointLights_H
#define __CPointLights_H

enum ePointLightType
{
    PLTYPE_POINTLIGHT=0,
};

class CPointLights
{
public:
    virtual void                        AddLight                ( unsigned char ucType, CVector vec_2, CVector vec_3, float f_4, float f_5, float f_6, float f_7, unsigned char uc_8, bool b_9, CEntity * pEntity ) = 0;
};

#endif