/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedModelInfoSA.cpp
*  PURPOSE:     Modelinfo for ped entities
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPedModelInfoSAInterface::CPedModelInfoSAInterface ( void )
{
    memset ( this, 0, sizeof ( CPedModelInfoSAInterface ) );
    VFTBL = ( CBaseModelInfo_SA_VTBL * ) VAR_CPedModelInfo_VTBL;
    pColModel = ( CColModelSAInterface * ) VAR_CTempColModels_ModelPed1;
    * ( DWORD * ) &pad = 0xFFFFFFFF;
}

CPedModelInfoSA::CPedModelInfoSA ( void ) : CModelInfoSA ()
{
    m_pPedModelInterface = new CPedModelInfoSAInterface;
}

void CPedModelInfoSA::SetMotionAnimGroup ( AssocGroupId animGroup )
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = (DWORD)FUNC_SetMotionAnimGroup;
    _asm
    {
        mov     ecx, dwThis
        push    animGroup
        call    dwFunc
    }
}
