/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRendererSA.cpp
 *  PURPOSE:     Game renderer class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRendererSA.h"
#include "CModelInfoSA.h"
#include "CMatrix.h"
#include "gamesa_renderware.h"

#define SetLightColoursForPedsCarsAndObjects(fMult) ((RpLight * (__cdecl*)(float))0x735D90)(fMult)
#define SetAmbientColours()                         ((RpLight * (__cdecl*)())0x735D30)()

CRendererSA::CRendererSA()
{
}

CRendererSA::~CRendererSA()
{
}

void CRendererSA::RenderModel(CModelInfo* pModelInfo, const CMatrix& matrix, float lighting, bool doubleSided)
{
    CBaseModelInfoSAInterface* pModelInfoSAInterface = pModelInfo->GetInterface();
    if (!pModelInfoSAInterface)
        return;

    // Prevent GC from freeing RwObject during rendering
    pModelInfo->ModelAddRef(NON_BLOCKING, "CRendererSA::RenderModel");

    // Revalidate interface after AddRef
    pModelInfoSAInterface = pModelInfo->GetInterface();
    if (!pModelInfoSAInterface)
    {
        pModelInfo->RemoveRef();
        return;
    }

    // Check and cache pRwObject
    RwObject* pRwObject = pModelInfoSAInterface->pRwObject;
    if (!pRwObject)
    {
        // Release reference before early return to prevent leak
        pModelInfo->RemoveRef();
        return;
    }

    RwFrame* pFrame = RpGetFrame(pRwObject);

    static RwMatrix rwMatrix;
    rwMatrix.right = (RwV3d&)matrix.vRight;
    rwMatrix.up = (RwV3d&)matrix.vFront;
    rwMatrix.at = (RwV3d&)matrix.vUp;
    rwMatrix.pos = (RwV3d&)matrix.vPos;
    RwFrameTransform(pFrame, &rwMatrix, rwCOMBINEREPLACE);

    // Ensure reference released on exception
    try
    {
        // Setup ambient light multiplier
        SetLightColoursForPedsCarsAndObjects(lighting);

        RwCullMode currentCullMode;
        if (doubleSided)
        {
            RwRenderStateGet(rwRENDERSTATECULLMODE, &currentCullMode);
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
        }
      
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

        if (doubleSided)
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(currentCullMode));

        // Restore ambient light
        SetAmbientColours();
    }
    catch (...)
    {
        // Release reference on rendering exception
        pModelInfo->RemoveRef();
        throw;
    }  // Release reference - allow GC
    pModelInfo->RemoveRef();
}
