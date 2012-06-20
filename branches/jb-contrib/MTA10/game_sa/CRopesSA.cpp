/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopesSA.cpp
*  PURPOSE:     Rope entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

DWORD dwDurationAddress = (DWORD)0x558D1D;

int CRopesSA::CreateRopeForSwatPed ( const CVector & vecPosition, DWORD dwDuration )
{

    int iReturn;
    DWORD dwFunc = FUNC_CRopes_CreateRopeForSwatPed;
    CVector * pvecPosition = const_cast < CVector * > ( &vecPosition );
    // First Push @ 0x558D1D is the duration.
    MemPut((void*)(dwDurationAddress), dwDuration);
    _asm
    {
        push    pvecPosition
        call    dwFunc
        add     esp, 0x4
        mov     iReturn, eax
    }
    // Set it back for SA in case we ever do some other implementation.
    MemPut((DWORD*)(dwDurationAddress), 4000);
    return iReturn;
/*
    CEntity* pRopeEntity = reinterpret_cast<CEntity*>((*(CEntity*)0xB781F8) + 100);
    CRopesSAInterface* pRope = CRopeSA::Create(pRopeEntity, RopeTypeSwat, vecPosition, true, 0, 0, 0, dwDuration);
    if(pRope > 0)
    {
        *reinterpret_cast<CEntity*>(0xB781F8) = *reinterpret_cast<CEntity*>(0xB781F8) + 1;
        return (int)pRope;
    }
    else
    {
        return (int)-1;
    }
    */
}

// Move later into CCameraSA
bool AreVerticesVisible(CCameraSAInterface* pCamera, CVector& vecTargetPos, float fRange, CMatrix_Padded* pMat)
{
    uint32 uiRet;
    uint32 dwFunc = (uint32)0x420C40;
    CVector* pVecTargetPos = &vecTargetPos;
    __asm
    {
        push pMat
        push fRange
        push pVecTargetPos
        push pCamera
        call dwFunc
        mov uiRet, eax
    }
    return uiRet == 1;
}
#if 0
void CRopesSAInterface::Draw()
{
    CVector &vecRopeCenter = m_vecSegments[ROPES_COUNT / 2];
    CCameraSAInterface
        *pCameraInterface = g_pGame->GetCamera()->GetInterface();
    if(AreVerticesVisible(pCameraInterface, vecRopeCenter, 20.0f, &pCameraInterface->m_matInverse)
       || *(uint8*)(pCameraInterface + 0x970) && AreVerticesVisible(pCameraInterface, vecRopeCenter, 20.0f, &pCameraInterface->m_matMirrorInverse))
    {
        CMatrix matCamPos;
        CVector &pVecCameraPos = g_pGame->GetCamera()->GetMatrix(&matCamPos)->vPos;
        CVector vecRange = pVecCameraPos - m_vecSegments[0];
        if(vecRange.LengthSquared() < 120.0f * 120.0f)
        {
            {   // DrawSkyBottom
                uint32 dwFunc = (uint32)0x734650;
                __asm call dwFunc
            }
            // RxObjSpace3dVertex pVerts[32];
            RxObjSpace3DVertex *pVerts = (RxObjSpace3DVertex*)(*reinterpret_cast<uint32*>(0xC4D958));

            for(uint32 i = 0; i < SEGMENT_COUNT; i++)
            {
                pVerts[i].color = 0x80000000;
                pVerts[i].objVertex.x = m_vecSegments[i].fX;
                pVerts[i].objVertex.y = m_vecSegments[i].fY;
                pVerts[i].objVertex.z = m_vecSegments[i].fZ;
            }
            RwEngineInstance
                *pEngineInstance = reinterpret_cast<RwEngineInstance*>(*reinterpret_cast<uint32*>(0xC97B24));
            pEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)1); // check
            pEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
            pEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5);
            pEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6);
            pEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)2);
            pEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);
            if(RwIm3DTransform(&pVerts, 32, 0, 0))
            {
                pEngineInstance->dOpenDevice.fpIm3DRenderIndexedPrimitive(1, (RwImVertexIndex*)*reinterpret_cast<uint16*>(0x8CD818), 62);
                //RwIm3DRenderIndexedPrimitive(1, *reinterpret_cast<uint16*>(0x8CD818), 62);
                RwIm3DEnd();
            }
            if(m_ucRopeType == 6)
            {
                pVerts[0].color = 0x80000000;
                pVerts[0].objVertex.x = m_vecSegments[0].fX;
                pVerts[0].objVertex.y = m_vecSegments[0].fY;
                pVerts[0].objVertex.z = m_vecSegments[0].fZ;
                pVerts[1].color = 0x80000000;
                pVerts[1].objVertex.x = 709.32f;
                pVerts[1].objVertex.y = 916.2f;
                pVerts[1].objVertex.z = 53.0f;
                if(RwIm3DTransform(pVerts, 2, 0, 0))
                {
                    pEngineInstance->dOpenDevice.fpIm3DRenderIndexedPrimitive(1, (RwImVertexIndex*)*reinterpret_cast<uint16*>(0x8CD818), 2);
                    //RwIm3DRenderIndexedPrimitive(1, *reinterpret_cast<uint16*>(0x8CD818), 2);
                    RwIm3DEnd();
                }
            }
        }
    }
}
#endif