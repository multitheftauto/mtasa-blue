/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRendererSA.cpp
 *  PURPOSE:     Game renderer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRendererSA.h"
#include "CModelInfoSA.h"
#include "CMatrix.h"
#include "gamesa_renderware.h"

CRendererSA::CRendererSA()
{
}

CRendererSA::~CRendererSA()
{
}

void CRendererSA::RenderModel(CModelInfo* pModelInfo, const CMatrix& matrix)
{
    CBaseModelInfoSAInterface* pModelInfoSAInterface = pModelInfo->GetInterface();
    if (!pModelInfoSAInterface)
        return;

    RwObject* pRwObject = pModelInfoSAInterface->pRwObject;
    if (!pRwObject)
        return;

    RwFrame* pFrame = RpGetFrame(pRwObject);

    static RwMatrix rwMatrix;
    rwMatrix.right = (RwV3d&)matrix.vRight;
    rwMatrix.up = (RwV3d&)matrix.vFront;
    rwMatrix.at = (RwV3d&)matrix.vUp;
    rwMatrix.pos = (RwV3d&)matrix.vPos;
    RwFrameTransform(pFrame, &rwMatrix, rwCOMBINEREPLACE);    

    if (pRwObject->type == RP_TYPE_ATOMIC)
    {
        RpAtomic* pRpAtomic = reinterpret_cast<RpAtomic*>(pRwObject);
        pRpAtomic->renderCallback(reinterpret_cast<RpAtomic*>(pRwObject));
    }
    else
    {
        RpClump* pClump = reinterpret_cast<RpClump*>(pRwObject);
        RpClumpRender(pClump);
    }
}
