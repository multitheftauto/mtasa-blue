/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.cpp
*  PURPOSE:     Base entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Kent Simon <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA * pGame;

unsigned long CEntitySA::FUNC_CClumpModelInfo__GetFrameFromId;
unsigned long CEntitySA::FUNC_RwFrameGetLTM;

CEntitySA::CEntitySA ( void )
{
    // Set these variables to a constant state
    m_pInterface = NULL;
    internalID = 0;
    BeingDeleted = false;
    DoNotRemoveFromGame = false;
    m_pStoredPointer = NULL;
    m_ulArrayID = 0;
}

/*VOID CEntitySA::SetModelAlpha ( int iAlpha )
{
    this->internalInterface->ModelClump->SetAlpha(iAlpha);
}*/
VOID CEntitySA::SetPosition(float fX, float fY, float fZ)
{
    DEBUG_TRACE("VOID CEntitySA::SetPosition(float fX, float fY, float fZ)");
    CVector * vecPos;
    if ( m_pInterface->Placeable.matrix )
    {
        vecPos = &m_pInterface->Placeable.matrix->vPos;
    }
    else
    {
        vecPos = &m_pInterface->Placeable.m_transform.m_translate;
    }

    if ( vecPos )
    {
        vecPos->fX = fX;
        vecPos->fY = fY;
        vecPos->fZ = fZ;
    }

    WORD wModelID = GetModelIndex();
    if ( wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449 )
    {
        // If it's a train, recalculate its rail position parameter (does not affect derailed state)
        DWORD dwThis = (DWORD) m_pInterface;
        DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}

VOID CEntitySA::Teleport ( float fX, float fY, float fZ )
{
    DEBUG_TRACE("VOID CEntitySA::Teleport ( float fX, float fY, float fZ )");
    if ( m_pInterface->Placeable.matrix )
    {
        SetPosition ( fX, fY, fZ );

        DWORD dwFunc = m_pInterface->vtbl->Teleport;
        DWORD dwThis = (DWORD) m_pInterface;
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
        SetPosition ( fX, fY, fZ );
    }


}

VOID CEntitySA::ProcessControl ( void )
{
    DEBUG_TRACE("VOID CEntitySA::ProcessControl ( void )");
    DWORD dwFunc = m_pInterface->vtbl->ProcessControl;
    DWORD dwThis = (DWORD) m_pInterface;
    if ( dwFunc )
    {
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}

VOID CEntitySA::SetupLighting ( )
{
    DEBUG_TRACE("VOID CEntitySA::SetupLighting ( )");
    DWORD dwFunc = m_pInterface->vtbl->SetupLighting;
    DWORD dwThis = (DWORD) m_pInterface;
    if ( dwFunc )
    {
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}

VOID CEntitySA::Render ( )
{
    DEBUG_TRACE("VOID CEntitySA::Render ( )");
    DWORD dwFunc = 0x59F180; //m_pInterface->vtbl->Render;
    DWORD dwThis = (DWORD) m_pInterface;
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


VOID CEntitySA::SetOrientation ( float fX, float fY, float fZ )
{
    DEBUG_TRACE("VOID CEntitySA::SetOrientation ( float fX, float fY, float fZ )");
    pGame->GetWorld()->Remove ( this );
    DWORD dwThis = (DWORD) m_pInterface;
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

    pGame->GetWorld()->Add ( this );
}

VOID CEntitySA::FixBoatOrientation ( void )
{
    DEBUG_TRACE("VOID CEntitySA::FixBoatOrientation ( void )");
    pGame->GetWorld()->Remove ( this );
    DWORD dwThis = (DWORD) m_pInterface;
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

    pGame->GetWorld()->Add ( this );
}

VOID CEntitySA::SetPosition( CVector * vecPosition )
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

    if ( vecPosition )
        SetPosition ( vecPosition->fX, vecPosition->fY, vecPosition->fZ );
}

CVector * CEntitySA::GetPosition( )
{
    DEBUG_TRACE("CVector * CEntitySA::GetPosition( )");
    if ( m_pInterface->Placeable.matrix )
        return &m_pInterface->Placeable.matrix->vPos;
    else
        return &m_pInterface->Placeable.m_transform.m_translate; 
}


CMatrix * CEntitySA::GetMatrix ( CMatrix * matrix ) const
{
    DEBUG_TRACE("CMatrix * CEntitySA::GetMatrix ( CMatrix * matrix )");
    if ( m_pInterface->Placeable.matrix && matrix )
    {
        MemCpyFast (&matrix->vFront,     &m_pInterface->Placeable.matrix->vFront, sizeof(CVector));
        MemCpyFast (&matrix->vPos,           &m_pInterface->Placeable.matrix->vPos, sizeof(CVector));
        MemCpyFast (&matrix->vUp,            &m_pInterface->Placeable.matrix->vUp, sizeof(CVector));
        MemCpyFast (&matrix->vRight,         &m_pInterface->Placeable.matrix->vRight, sizeof(CVector));
        return matrix;
    }
    else
    {
        return NULL;
    }
}

VOID CEntitySA::SetMatrix ( CMatrix * matrix )
{
    DEBUG_TRACE("VOID CEntitySA::SetMatrix ( CMatrix * matrix )");
    if ( m_pInterface->Placeable.matrix && matrix )
    {
        MemCpyFast (&m_pInterface->Placeable.matrix->vFront,     &matrix->vFront, sizeof(CVector));
        MemCpyFast (&m_pInterface->Placeable.matrix->vPos,           &matrix->vPos, sizeof(CVector));
        MemCpyFast (&m_pInterface->Placeable.matrix->vUp,            &matrix->vUp, sizeof(CVector));
        MemCpyFast (&m_pInterface->Placeable.matrix->vRight,         &matrix->vRight, sizeof(CVector));

        m_pInterface->Placeable.m_transform.m_translate = matrix->vPos;

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

        pGame->GetWorld()->Remove ( this );
        DWORD dwThis = (DWORD) m_pInterface;
        DWORD dwFunc = 0x446F90;    // CEntity::UpdateRwMatrix
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }

        dwFunc = 0x532B00;          // CEntity::UpdateRwFrame
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
        pGame->GetWorld()->Add ( this );
    }
}

WORD CEntitySA::GetModelIndex ()
{
    DEBUG_TRACE("WORD CEntitySA::GetModelIndex ()");
    return m_pInterface->m_nModelIndex;
}

eEntityType CEntitySA::GetEntityType ()
{
    DEBUG_TRACE("eEntityType CEntitySA::GetEntityType ()");
    return (eEntityType)m_pInterface->nType;
}

FLOAT CEntitySA::GetDistanceFromCentreOfMassToBaseOfModel()
{
    DEBUG_TRACE("FLOAT CEntitySA::GetDistanceFromCentreOfMassToBaseOfModel()");
    DWORD dwFunc = FUNC_GetDistanceFromCentreOfMassToBaseOfModel;
    DWORD dwThis = (DWORD) m_pInterface;
    FLOAT fReturn;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }
    return fReturn;
}   

VOID CEntitySA::SetEntityStatus( eEntityStatus bStatus )
{
    DEBUG_TRACE("VOID CEntitySA::SetEntityStatus( eEntityStatus bStatus )");
    m_pInterface->nStatus = bStatus;
}

eEntityStatus CEntitySA::GetEntityStatus( )
{
    DEBUG_TRACE("eEntityStatus CEntitySA::GetEntityStatus( )");
    return (eEntityStatus) m_pInterface->nStatus;
}

RwFrame * CEntitySA::GetFrameFromId ( int id )
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
    return (RwFrame *)dwReturn;
}

RpClump * CEntitySA::GetRpClump ()
{
    return m_pInterface->m_pRwObject;
}

RwMatrix * CEntitySA::GetLTMFromId ( int id )
{
    DWORD dwReturn;
    RwFrame * frame = GetFrameFromId(id);
    _asm
    {
        push    frame
        call    FUNC_RwFrameGetLTM
        add     esp, 4
        mov     dwReturn, eax
    }
    return (RwMatrix *)dwReturn;
}

VOID CEntitySA::SetAlpha(DWORD dwAlpha)
{
    DEBUG_TRACE("VOID CEntitySA::SetAlpha(DWORD dwAlpha)");
    DWORD dwFunc = FUNC_SetRwObjectAlpha;
    DWORD dwThis = (DWORD) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    dwAlpha
        call    dwFunc
    }
}

bool CEntitySA::IsOnScreen ()
{
    /**(BYTE *)0x534540 = 0x83;
    MemPut < BYTE > ( 0x534541, 0xEC );
    MemPut < BYTE > ( 0x534542, 0x10 );
*/
    DWORD dwFunc = FUNC_IsVisible; //FUNC_IsOnScreen;
    DWORD dwThis = (DWORD) m_pInterface;
    bool bReturn = false;
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


bool CEntitySA::IsVisible ( void )
{
    return m_pInterface->bIsVisible;
}


void CEntitySA::SetVisible ( bool bVisible )
{
    m_pInterface->bIsVisible = bVisible;
}

VOID CEntitySA::MatrixConvertFromEulerAngles ( float fX, float fY, float fZ, int iUnknown )
{
    CMatrix_Padded * matrixPadded = m_pInterface->Placeable.matrix;
    if ( matrixPadded )
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

VOID CEntitySA::MatrixConvertToEulerAngles ( float * fX, float * fY, float * fZ, int iUnknown )
{
    CMatrix_Padded * matrixPadded = m_pInterface->Placeable.matrix;
    if ( matrixPadded )
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

bool CEntitySA::IsPlayingAnimation ( char * szAnimName )
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
    if ( dwReturn )
        return true;
    else
        return false;
}

BYTE CEntitySA::GetAreaCode ( void )
{
    return m_pInterface->m_areaCode;
}

void CEntitySA::SetAreaCode ( BYTE areaCode )
{
    m_pInterface->m_areaCode = areaCode;
}


void CEntitySA::GetImmunities ( bool & bNoClip, bool & bFrozen, bool & bBulletProof, bool & bFlameProof, bool & bUnk, bool & bUnk2, bool & bCollisionProof, bool & bExplosionProof )
{
    unsigned char ucImmunities = m_pInterface->nImmunities;
    bNoClip = ( ucImmunities & 0x1 ) ? true:false;
    bFrozen = ( ucImmunities & 0x2 ) ? true:false;
    bBulletProof = ( ucImmunities & 0x4 ) ? true:false;
    bFlameProof = ( ucImmunities & 0x8 ) ? true:false;
    bUnk = ( ucImmunities & 0x10 ) ? true:false;
    bUnk2 = ( ucImmunities & 0x20 ) ? true:false;
    bCollisionProof = ( ucImmunities & 0x40 ) ? true:false;
    bExplosionProof = ( ucImmunities & 0x80 ) ? true:false;
}


void CEntitySA::SetUnderwater ( bool bUnderwater )
{
    m_pInterface->bUnderwater = bUnderwater;
}


bool CEntitySA::GetUnderwater ( void )
{
    return m_pInterface->bUnderwater;
}
