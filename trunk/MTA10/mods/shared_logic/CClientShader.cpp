/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientShader.cpp
*  PURPOSE:
*  DEVELOPERS:  vidiot
*
*****************************************************************************/

#include <StdInc.h>


////////////////////////////////////////////////////////////////
//
// CClientShader::CClientShader
//
//
//
////////////////////////////////////////////////////////////////
CClientShader::CClientShader ( CClientManager* pManager, ElementID ID, SShaderItem* pShaderItem ) : ClassInit ( this ), CClientMaterial ( pManager, ID )
{
    SetTypeName ( "shader" );
    m_pRenderItem = pShaderItem;
}
