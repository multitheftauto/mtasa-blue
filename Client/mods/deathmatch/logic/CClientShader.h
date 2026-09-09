/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientShader.h
 *  PURPOSE:
 *
 *****************************************************************************/

#include "enums/ElementType.h"

class CClientShader : public CClientMaterial
{
    DECLARE_CLASS(CClientShader, CClientMaterial)
public:
    CClientShader(CClientManager* pManager, ElementID ID, CShaderItem* pShaderItem);

    ElementType::Enum GetType() const { return ElementType::SHADER; }

    // CClientShader methods
    CShaderItem* GetShaderItem() { return (CShaderItem*)m_pRenderItem; }
};
