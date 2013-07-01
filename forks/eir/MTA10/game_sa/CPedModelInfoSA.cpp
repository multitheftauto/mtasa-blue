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
    MemSetFast ( this, 0, sizeof ( CPedModelInfoSAInterface ) );
    *(DWORD*)this = (DWORD)VAR_CPedModelInfo_VTBL;
    pColModel = ( CColModelSAInterface * ) VAR_CTempColModels_ModelPed1;
    m_animBlock = -1;
}

CPedModelInfoSAInterface::~CPedModelInfoSAInterface( void )
{

}

eModelType CPedModelInfoSAInterface::GetModelType( void )
{
    return MODEL_PED;
}

void CPedModelInfoSAInterface::DeleteRwObject( void )
{
    CClumpModelInfoSAInterface::DeleteRwObject();

    if ( pColModel )
    {
        delete pColModel;

        pColModel = NULL;
    }
}

void CPedModelInfoSAInterface::SetAnimFile( const char *name )
{

}

void CPedModelInfoSAInterface::ConvertAnimFileIndex( void )
{

}

int CPedModelInfoSAInterface::GetAnimFileIndex( void )
{
    return -1;
}

CPedModelInfoSA::CPedModelInfoSA ( void ) : CModelInfoSA ()
{
    m_pPedModelInterface = new CPedModelInfoSAInterface;
}

void CPedModelInfoSA::SetMotionAnimGroup ( AssocGroupId animGroup )
{
    m_pPedModelInterface->motionAnimGroup = animGroup;
}
