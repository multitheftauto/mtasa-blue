/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEntitySA.cpp
 *  PURPOSE:     Base entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"
#include "BoneNode_cSA.h"

extern CGameSA* pGame;

unsigned long CEntitySA::FUNC_CClumpModelInfo__GetFrameFromId;
unsigned long CEntitySA::FUNC_RwFrameGetLTM;

void CEntitySAInterface::TransformFromObjectSpace(CVector& outPosn, CVector const& offset)
{
    ((void(__thiscall*)(CEntitySAInterface*, CVector&, CVector const&))0x533560)(this, outPosn, offset);
}

CVector* CEntitySAInterface::GetBoundCentre(CVector* pOutCentre)
{
    return ((CVector * (__thiscall*)(CEntitySAInterface*, CVector*))0x534250)(this, pOutCentre);
}

void CEntitySAInterface::UpdateRW()
{
    ((void(__thiscall*)(CEntitySAInterface*))0x446F90)(this);
}

void CEntitySAInterface::UpdateRpHAnim()
{
    ((void(__thiscall*)(CEntitySAInterface*))0x532B20)(this);
}

CRect* CEntitySAInterface::GetBoundRect_(CRect* pRect)
{
    CColModelSAInterface* colModel = CModelInfoSAInterface::GetModelInfo(m_nModelIndex)->pColModel;
    CVector               vecMin = colModel->m_bounds.m_vecMin;
    CVector               vecMax = colModel->m_bounds.m_vecMax;
    CRect                 rect;
    CVector               point;
    TransformFromObjectSpace(point, vecMin);
    rect.StretchToPoint(point.fX, point.fY);
    TransformFromObjectSpace(point, vecMax);
    rect.StretchToPoint(point.fX, point.fY);
    float maxX = vecMax.fX;
    vecMax.fX = vecMin.fX;
    vecMin.fX = maxX;
    TransformFromObjectSpace(point, vecMin);
    rect.StretchToPoint(point.fX, point.fY);
    TransformFromObjectSpace(point, vecMax);
    rect.StretchToPoint(point.fX, point.fY);
    *pRect = rect;
    pRect->FixIncorrectTopLeft();            // Fix #1613: custom map collision crashes in CPhysical class (infinite loop)
    return pRect;
}

void CEntitySAInterface::StaticSetHooks()
{
    HookInstall(0x534120, &CEntitySAInterface::GetBoundRect_);
}

CEntitySA::CEntitySA()
{
    // Set these variables to a constant state
    m_pInterface = NULL;
    internalID = 0;
    BeingDeleted = false;
    DoNotRemoveFromGame = false;
    m_pStoredPointer = NULL;
    m_ulArrayID = INVALID_POOL_ARRAY_ID;
}

void CEntitySA::UpdateRpHAnim()
{
    m_pInterface->UpdateRpHAnim();
}

bool CEntitySA::SetScaleInternal(const CVector& scale)
{
    m_pInterface->UpdateRW();
    RpClump* clump = GetRpClump();
    if (!clump)
        return false;
    RwFrame* frame = reinterpret_cast<RwFrame*>(clump->object.parent);
    RwMatrixScale((RwMatrix*)&frame->modelling, (RwV3d*)&scale, TRANSFORM_BEFORE);
    RwFrameUpdateObjects(frame);
    return true;
}

/*VOID CEntitySA::SetModelAlpha ( int iAlpha )
{
    this->internalInterface->ModelClump->SetAlpha(iAlpha);
}*/
VOID CEntitySA::SetPosition(float fX, float fY, float fZ)
{
    // Remove & add to world?
    DEBUG_TRACE("VOID CEntitySA::SetPosition(float fX, float fY, float fZ)");
    CVector* vecPos;
    if (m_pInterface->Placeable.matrix)
    {
        OnChangingPosition(CVector(fX, fY, fZ));
        vecPos = &m_pInterface->Placeable.matrix->vPos;
    }
    else
    {
        vecPos = &m_pInterface->Placeable.m_transform.m_translate;
    }

    if (vecPos)
    {
        vecPos->fX = fX;
        vecPos->fY = fY;
        vecPos->fZ = fZ;
        m_LastGoodPosition = *vecPos;
    }

    WORD wModelID = GetModelIndex();
    if (wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449)
    {
        // If it's a train, recalculate its rail position parameter (does not affect derailed state)
        DWORD dwThis = (DWORD)m_pInterface;
        DWORD dwFunc = FUNC_CTrain_FindPositionOnTrackFromCoors;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
    }
    if (m_pInterface->nType == ENTITY_TYPE_OBJECT)
    {
        ((CObjectSAInterface*)m_pInterface)->bUpdateScale = true;
    }
}

VOID CEntitySA::Teleport(float fX, float fY, float fZ)
{
    DEBUG_TRACE("VOID CEntitySA::Teleport ( float fX, float fY, float fZ )");
    if (m_pInterface->Placeable.matrix)
    {
        SetPosition(fX, fY, fZ);

        DWORD dwFunc = m_pInterface->vtbl->Teleport;
        DWORD dwThis = (DWORD)m_pInterface;
        _asm
        {
            mov     ecx, dwThis
            push    1
            push    fZ
            push    fY
            push    fX
            call    dwFunc
        }
    }
    else
    {
        SetPosition(fX, fY, fZ);
    }
}

VOID CEntitySA::ProcessControl()
{
    DEBUG_TRACE("VOID CEntitySA::ProcessControl ( void )");
    DWORD dwFunc = m_pInterface->vtbl->ProcessControl;
    DWORD dwThis = (DWORD)m_pInterface;
    if (dwFunc)
    {
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}

VOID CEntitySA::SetupLighting()
{
    DEBUG_TRACE("VOID CEntitySA::SetupLighting ( )");
    DWORD dwFunc = m_pInterface->vtbl->SetupLighting;
    DWORD dwThis = (DWORD)m_pInterface;
    if (dwFunc)
    {
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}

VOID CEntitySA::Render()
{
    DEBUG_TRACE("VOID CEntitySA::Render ( )");
    DWORD dwFunc = 0x59F180;            // m_pInterface->vtbl->Render;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    /*  DWORD dwFunc = 0x553260;
        DWORD dwThis = (DWORD) m_pInterface;

        _asm
        {
            push    dwThis
            call    dwFunc
            add     esp, 4
        }*/
}

VOID CEntitySA::SetOrientation(float fX, float fY, float fZ)
{
    DEBUG_TRACE("VOID CEntitySA::SetOrientation ( float fX, float fY, float fZ )");
    pGame->GetWorld()->Remove(this, CEntity_SetOrientation);
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_SetOrientation;
    _asm
    {
        // ChrML: I've switched the X and Z at this level because that's how the real rotation
        //        is. GTA has kinda swapped them in this function.

        push    fZ
        push    fY
        push    fX
        mov     ecx, dwThis
        call    dwFunc
    }

    dwFunc = 0x446F90;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    dwFunc = 0x532B00;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    if (m_pInterface->nType == ENTITY_TYPE_OBJECT)
    {
        ((CObjectSAInterface*)m_pInterface)->bUpdateScale = true;
    }

    pGame->GetWorld()->Add(this, CEntity_SetOrientation);
}

VOID CEntitySA::FixBoatOrientation()
{
    DEBUG_TRACE("VOID CEntitySA::FixBoatOrientation ( void )");
    pGame->GetWorld()->Remove(this, CEntity_FixBoatOrientation);
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x446F90;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    dwFunc = 0x532B00;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    pGame->GetWorld()->Add(this, CEntity_FixBoatOrientation);
}

VOID CEntitySA::SetPosition(CVector* vecPosition)
{
    DEBUG_TRACE("VOID CEntitySA::SetPosition( CVector * vecPosition )");
    /*  FLOAT fX = vecPosition->fX;
        FLOAT fY = vecPosition->fY;
        FLOAT fZ = vecPosition->fZ;
        DWORD dwFunc = 0x5A17B0;
        DWORD dwThis = (DWORD) m_pInterface;
        _asm
        {
            mov     ecx, dwThis
            push 0
            push fZ
            push fY
            push fX
            call    dwFunc
        }*/

    if (vecPosition)
        SetPosition(vecPosition->fX, vecPosition->fY, vecPosition->fZ);
}

void CEntitySA::RestoreLastGoodPhysicsState()
{
    // Validate m_LastGoodPosition
    if (!IsValidPosition(m_LastGoodPosition))
        m_LastGoodPosition = CVector(6000, (float)(rand() % 2000), 1000);

    CMatrix matrix;
    matrix.vPos = m_LastGoodPosition;
    SetMatrix(&matrix);
    SetPosition(&m_LastGoodPosition);
}

//
// Get entity position. Fixes bad numbers
//
CVector* CEntitySA::GetPosition()
{
    CVector* pPosition = GetPositionInternal();
    if (!IsValidPosition(*pPosition))
    {
        RestoreLastGoodPhysicsState();
        pPosition = GetPositionInternal();
    }
    m_LastGoodPosition = *pPosition;
    return pPosition;
}

CVector* CEntitySA::GetPositionInternal()
{
    DEBUG_TRACE("CVector * CEntitySA::GetPosition( )");
    if (m_pInterface->Placeable.matrix)
        return &m_pInterface->Placeable.matrix->vPos;
    else
        return &m_pInterface->Placeable.m_transform.m_translate;
}

//
// Get entity matrix. Fixes bad numbers
//
CMatrix* CEntitySA::GetMatrix(CMatrix* matrix)
{
    CMatrix* pMatrix = GetMatrixInternal(matrix);
    if (pMatrix)
    {
        if (!IsValidMatrix(*pMatrix))
        {
            RestoreLastGoodPhysicsState();
            pMatrix = GetMatrixInternal(matrix);
        }
        m_LastGoodPosition = pMatrix->vPos;
    }
    return pMatrix;
}

CMatrix* CEntitySA::GetMatrixInternal(CMatrix* matrix)
{
    DEBUG_TRACE("CMatrix * CEntitySA::GetMatrix ( CMatrix * matrix )");
    if (m_pInterface->Placeable.matrix && matrix)
    {
        MemCpyFast(&matrix->vFront, &m_pInterface->Placeable.matrix->vFront, sizeof(CVector));
        MemCpyFast(&matrix->vPos, &m_pInterface->Placeable.matrix->vPos, sizeof(CVector));
        MemCpyFast(&matrix->vUp, &m_pInterface->Placeable.matrix->vUp, sizeof(CVector));
        MemCpyFast(&matrix->vRight, &m_pInterface->Placeable.matrix->vRight, sizeof(CVector));
        return matrix;
    }
    else
    {
        return NULL;
    }
}

VOID CEntitySA::SetMatrix(CMatrix* matrix)
{
    DEBUG_TRACE("VOID CEntitySA::SetMatrix ( CMatrix * matrix )");

    if (m_pInterface->Placeable.matrix && matrix)
    {
        OnChangingPosition(matrix->vPos);

        MemCpyFast(&m_pInterface->Placeable.matrix->vFront, &matrix->vFront, sizeof(CVector));
        MemCpyFast(&m_pInterface->Placeable.matrix->vPos, &matrix->vPos, sizeof(CVector));
        MemCpyFast(&m_pInterface->Placeable.matrix->vUp, &matrix->vUp, sizeof(CVector));
        MemCpyFast(&m_pInterface->Placeable.matrix->vRight, &matrix->vRight, sizeof(CVector));

        m_pInterface->Placeable.m_transform.m_translate = matrix->vPos;
        m_LastGoodPosition = matrix->vPos;

        /*
        WORD wModelID = GetModelIndex();
        if ( wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449 )
        {
            DWORD dwThis = (DWORD) m_pInterface;
            DWORD dwFunc = 0x6F6CC0;
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }

            //OutputDebugString ( "Set train position on tracks (matrix)!\n" );
        }
        */

        pGame->GetWorld()->Remove(this, CEntity_SetMatrix);
        DWORD dwThis = (DWORD)m_pInterface;
        DWORD dwFunc = 0x446F90;            // CEntity::UpdateRwMatrix
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }

        dwFunc = 0x532B00;            // CEntity::UpdateRwFrame
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }

        if (m_pInterface->nType == ENTITY_TYPE_OBJECT)
        {
            ((CObjectSAInterface*)m_pInterface)->bUpdateScale = true;
        }

        pGame->GetWorld()->Add(this, CEntity_SetMatrix);
    }
}

WORD CEntitySA::GetModelIndex()
{
    DEBUG_TRACE("WORD CEntitySA::GetModelIndex ()");
    return m_pInterface->m_nModelIndex;
}

eEntityType CEntitySA::GetEntityType()
{
    DEBUG_TRACE("eEntityType CEntitySA::GetEntityType ()");
    return (eEntityType)m_pInterface->nType;
}

FLOAT CEntitySA::GetDistanceFromCentreOfMassToBaseOfModel()
{
    DEBUG_TRACE("FLOAT CEntitySA::GetDistanceFromCentreOfMassToBaseOfModel()");
    DWORD dwFunc = FUNC_GetDistanceFromCentreOfMassToBaseOfModel;
    DWORD dwThis = (DWORD)m_pInterface;
    FLOAT fReturn;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }
    return fReturn;
}

VOID CEntitySA::SetEntityStatus(eEntityStatus bStatus)
{
    DEBUG_TRACE("VOID CEntitySA::SetEntityStatus( eEntityStatus bStatus )");
    m_pInterface->nStatus = bStatus;
}

eEntityStatus CEntitySA::GetEntityStatus()
{
    DEBUG_TRACE("eEntityStatus CEntitySA::GetEntityStatus( )");
    return (eEntityStatus)m_pInterface->nStatus;
}

RwFrame* CEntitySA::GetFrameFromId(int id)
{
    DWORD dwClump = (DWORD)m_pInterface->m_pRwObject;
    DWORD dwReturn;
    _asm
    {
        push    id
        push    dwClump
        call    FUNC_CClumpModelInfo__GetFrameFromId
        add     esp, 8
        mov     dwReturn, eax
    }
    return (RwFrame*)dwReturn;
}

RpClump* CEntitySA::GetRpClump()
{
    return m_pInterface->m_pRwObject;
}

RwMatrix* CEntitySA::GetLTMFromId(int id)
{
    DWORD    dwReturn;
    RwFrame* frame = GetFrameFromId(id);
    _asm
    {
        push    frame
        call    FUNC_RwFrameGetLTM
        add     esp, 4
        mov     dwReturn, eax
    }
    return (RwMatrix*)dwReturn;
}

VOID CEntitySA::SetAlpha(DWORD dwAlpha)
{
    DEBUG_TRACE("VOID CEntitySA::SetAlpha(DWORD dwAlpha)");
    DWORD dwFunc = FUNC_SetRwObjectAlpha;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    dwAlpha
        call    dwFunc
    }
}

bool CEntitySA::IsOnScreen()
{
    /**(BYTE *)0x534540 = 0x83;
    MemPut < BYTE > ( 0x534541, 0xEC );
    MemPut < BYTE > ( 0x534542, 0x10 );
*/
    DWORD dwFunc = FUNC_IsVisible;            // FUNC_IsOnScreen;
    DWORD dwThis = (DWORD)m_pInterface;
    bool  bReturn = false;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    /*
        MemPut < BYTE > ( 0x534540, 0xB0 );
        MemPut < BYTE > ( 0x534541, 0x01 );
        MemPut < BYTE > ( 0x534542, 0xC3 );
    */
    return bReturn;
}

bool CEntitySA::IsFullyVisible()
{
    if (m_pInterface->bDistanceFade)
        return false;

    if (m_pInterface->GetEntityVisibilityResult() != 1)
        return false;

    return true;
}

bool CEntitySA::IsVisible()
{
    return m_pInterface->bIsVisible;
}

void CEntitySA::SetVisible(bool bVisible)
{
    m_pInterface->bIsVisible = bVisible;
}

VOID CEntitySA::MatrixConvertFromEulerAngles(float fX, float fY, float fZ, int iUnknown)
{
    CMatrix_Padded* matrixPadded = m_pInterface->Placeable.matrix;
    if (matrixPadded)
    {
        DWORD dwFunc = FUNC_CMatrix__ConvertFromEulerAngles;
        _asm
        {
            push    iUnknown
            push    fZ
            push    fY
            push    fX
            mov     ecx, matrixPadded
            call    dwFunc
        }
    }
}

VOID CEntitySA::MatrixConvertToEulerAngles(float* fX, float* fY, float* fZ, int iUnknown)
{
    CMatrix_Padded* matrixPadded = m_pInterface->Placeable.matrix;
    if (matrixPadded)
    {
        DWORD dwFunc = FUNC_CMatrix__ConvertToEulerAngles;
        _asm
        {
            push    iUnknown
            push    fZ
            push    fY
            push    fX
            mov     ecx, matrixPadded
            call    dwFunc
        }
    }
}

bool CEntitySA::IsPlayingAnimation(char* szAnimName)
{
    DWORD dwReturn = 0;
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetAssociation;
    DWORD dwThis = (DWORD)m_pInterface->m_pRwObject;

    _asm
    {
        push    szAnimName
        push    dwThis
        call    dwFunc
        add     esp, 8
        mov     dwReturn, eax
    }
    if (dwReturn) return true;
    else return false;
}

RwMatrixTag* CEntitySA::GetBoneRwMatrix(eBone boneId)
{
    RpClump* clump = GetRpClump();
    if (!clump)
        return nullptr;
    RpHAnimHierarchy* hAnimHier = GetAnimHierarchyFromSkinClump(clump);
    if (hAnimHier)
    {
        int boneAnimIdIndex = RpHAnimIDGetIndex(hAnimHier, boneId);
        if (boneAnimIdIndex != -1)
            return &RpHAnimHierarchyGetMatrixArray(hAnimHier)[boneAnimIdIndex];
    }
    return nullptr;
}

bool CEntitySA::SetBoneMatrix(eBone boneId, const CMatrix& matrix)
{
    RwMatrixTag* rwBoneMatrix = GetBoneRwMatrix(boneId);
    if (rwBoneMatrix)
    {
        CMatrixSAInterface boneMatrix(rwBoneMatrix, false);
        boneMatrix.SetMatrix(matrix.vRight, matrix.vFront, matrix.vUp, matrix.vPos);
        boneMatrix.UpdateRW();
        return true;
    }
    return false;
}

bool CEntitySA::GetBoneRotation(eBone boneId, float& yaw, float& pitch, float& roll)
{
    RpClump* clump = GetRpClump();
    if (clump)
    {
        // updating the bone frame orientation will also update its children
        // This rotation is only applied when UpdateElementRpHAnim is called
        CAnimBlendClumpDataSAInterface* clumpDataInterface = *pGame->GetClumpData(clump);
        AnimBlendFrameData*             frameData = clumpDataInterface->GetFrameDataByNodeId(boneId);
        if (frameData)
        {
            RtQuat* boneOrientation = &frameData->m_pIFrame->orientation;
            RwV3d   angles = {yaw, roll, pitch};
            BoneNode_cSAInterface::QuatToEuler(boneOrientation, &angles);
            yaw = angles.x;
            roll = angles.y;
            pitch = angles.z;
            return true;
        }
    }
    return false;
}

bool CEntitySA::SetBoneRotation(eBone boneId, float yaw, float pitch, float roll)
{
    RpClump* clump = GetRpClump();
    if (clump)
    {
        // updating the bone frame orientation will also update its children
        // This rotation is only applied when UpdateElementRpHAnim is called
        CAnimBlendClumpDataSAInterface* clumpDataInterface = *pGame->GetClumpData(clump);
        AnimBlendFrameData*             frameData = clumpDataInterface->GetFrameDataByNodeId(boneId);
        if (frameData)
        {
            RtQuat* boneOrientation = &frameData->m_pIFrame->orientation;
            RwV3d   angles = {yaw, roll, pitch};
            BoneNode_cSAInterface::EulerToQuat(&angles, boneOrientation);
            CEntitySAInterface* theInterface = GetInterface();
            if (theInterface)
            {
                theInterface->bDontUpdateHierarchy = false;
            }
            return true;
        }
    }
    return false;
}

bool CEntitySA::GetBonePosition(eBone boneId, CVector& position)
{
    RwMatrixTag* rwBoneMatrix = GetBoneRwMatrix(boneId);
    if (rwBoneMatrix)
    {
        const RwV3d& pos = rwBoneMatrix->pos;
        position = {pos.x, pos.y, pos.z};
        return true;
    }
    return false;
}

// NOTE: The position will be reset if UpdateElementRpHAnim is called after this.
bool CEntitySA::SetBonePosition(eBone boneId, const CVector& position)
{
    RwMatrixTag* rwBoneMatrix = GetBoneRwMatrix(boneId);
    if (rwBoneMatrix)
    {
        CMatrixSAInterface boneMatrix(rwBoneMatrix, false);
        boneMatrix.SetTranslateOnly(position);
        boneMatrix.UpdateRW();
        return true;
    }
    return false;
}

BYTE CEntitySA::GetAreaCode()
{
    return m_pInterface->m_areaCode;
}

void CEntitySA::SetAreaCode(BYTE areaCode)
{
    m_pInterface->m_areaCode = areaCode;
}

void CEntitySA::SetUnderwater(bool bUnderwater)
{
    m_pInterface->bUnderwater = bUnderwater;
}

bool CEntitySA::GetUnderwater()
{
    return m_pInterface->bUnderwater;
}
