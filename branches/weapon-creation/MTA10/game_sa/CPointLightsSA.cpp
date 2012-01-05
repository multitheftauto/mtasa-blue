/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPointLightsSA.cpp
*  PURPOSE:     Point lights class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CPointLightsSA::AddLight ( unsigned char ucType, CVector vec_2, CVector vec_3, float f_4, float f_5, float f_6, float f_7, unsigned char uc_8, bool b_9, CEntity * pEntity )
{
    DWORD dwEntityInterface = 0;
    if ( pEntity ) dwEntityInterface = ( DWORD ) pEntity->GetInterface ();
    DWORD dwFunc = FUNC_CPointLights_AddLight;
    float f_3x = vec_3.fX, f_3y = vec_3.fY, f_3z = vec_3.fZ;
    float f_2x = vec_2.fX, f_2y = vec_2.fY, f_2z = vec_2.fZ;
    _asm
    {
        push    dwEntityInterface
        push    b_9
        push    uc_8
        push    f_7
        push    f_6
        push    f_5
        push    f_4
        push    f_3z
        push    f_3y
        push    f_3x
        push    f_2z
        push    f_2y
        push    f_2x
        push    ucType        
        call    dwFunc
        add     esp, 56
    }
}