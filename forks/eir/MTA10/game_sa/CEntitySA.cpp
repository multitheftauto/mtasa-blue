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

CEntitySAInterface::CEntitySAInterface( void )
{
    // Overwrite vtbl
    *(DWORD**)this = (DWORD*)0x00863928;

    nStatus = 4;
    m_entityFlags = ENTITY_VISIBLE | ENTITY_BACKFACECULL;

    m_nScanCode = 0;

    m_nModelIndex = -1;
    m_pRwObject = NULL;

    m_iplIndex = 0;
    m_areaCode = 0;

    RandomSeed = rand();

    pReferences = NULL;
    m_streamingRef = NULL;
    
    numLodChildren = 0;
    numLodChildrenRendered = 0;
    m_pLod = NULL;
}

// Implement some virtual stuff, so we can construct entities.
// The compiler complains otherwise.
// We are not going to use these functions anyway.
void __thiscall CEntitySAInterface::AddRect( CBounds2D bounds )                         {}
bool __thiscall CEntitySAInterface::AddToWorld( void )                                  { return false; }
void __thiscall CEntitySAInterface::RemoveFromWorld( void )                             {}
void __thiscall CEntitySAInterface::SetStatic( bool enabled )                           {}
void __thiscall CEntitySAInterface::SetModelIndex( modelId_t id )                       {}
void __thiscall CEntitySAInterface::SetModelIndexNoCreate( modelId_t id )               {}
void __thiscall CEntitySAInterface::CreateRwObject( void )                              {}
void __thiscall CEntitySAInterface::DeleteRwObject( void )                              {}
const CBounds2D& __thiscall CEntitySAInterface::GetBoundingBox( CBounds2D& bounds )     { return bounds; }
void __thiscall CEntitySAInterface::ProcessControl( void )                              {}
void __thiscall CEntitySAInterface::ProcessCollision( void )                            {}
void __thiscall CEntitySAInterface::ProcessShift( void )                                {}
bool __thiscall CEntitySAInterface::TestCollision( void )                               { return false; }
void __thiscall CEntitySAInterface::Teleport( float x, float y, float z, int unk )      {}
void __thiscall CEntitySAInterface::PreFrame( void )                                    {}
bool __thiscall CEntitySAInterface::Frame( void )                                       { return true; }
void __thiscall CEntitySAInterface::PreRender( void )                                   {}
void __thiscall CEntitySAInterface::Render( void )                                      {}
unsigned char __thiscall CEntitySAInterface::SetupLighting( void )                      { return 0; }
void __thiscall CEntitySAInterface::RemoveLighting( unsigned char id )                  {}
void __thiscall CEntitySAInterface::Invalidate( void )                                  {}


void CEntitySAInterface::GetPosition( CVector& pos ) const
{
    pos = Placeable.GetPosition();
}

float CEntitySAInterface::GetBasingDistance( void ) const
{
    return GetColModel()->m_bounds.vecBoundMin.fZ;
}

static bool RpMaterialSetAlpha( RpMaterial *mat, unsigned char alpha )
{
    mat->color.a = alpha;
    return true;
}

static bool RpAtomicMaterialSetAlpha( RpAtomic *atom, unsigned char alpha )
{
    atom->geometry->ForAllMateria( RpMaterialSetAlpha, alpha );
    return true;
}

void CEntitySAInterface::SetAlpha( unsigned char alpha )
{
    RwObject *rwobj = GetRwObject();

    if ( !rwobj )
        return;

    if ( rwobj->type == RW_ATOMIC )
    {
        RpAtomicMaterialSetAlpha( (RpAtomic*)rwobj, alpha );
    }
    else if ( rwobj->type == RW_CLUMP )
    {
        ((RpClump*)rwobj)->ForAllAtomics( RpAtomicMaterialSetAlpha, alpha );
    }
}

CColModelSAInterface* CEntitySAInterface::GetColModel( void ) const
{
    CColModelSAInterface *colModel = NULL;

#if 0
    CEntitySA *entity = (CEntitySA*)pGame->GetPools()->GetEntity( const_cast <CEntitySAInterface*> ( this ) );

    if ( entity )
    {
        CColModelSA *col = entity->GetColModel();

        if ( col )
            colModel = col->GetInterface();
    }
#endif

    if ( !colModel )
    {
        if ( nType == ENTITY_TYPE_VEHICLE )
        {
            CVehicleSAInterface *veh = (CVehicleSAInterface*)this;
            unsigned char n = veh->m_nSpecialColModel;

            if ( n != 0xFF )
                colModel = (CColModelSAInterface*)VAR_CVehicle_SpecialColModels + n;
        }
    }

    if ( !colModel )
        colModel = GetModelInfo()->pColModel;
    
    assert( colModel != NULL );

    return colModel;
}

const CVector& CEntitySAInterface::GetCollisionOffset( CVector& out ) const
{
    GetOffset( out, GetColModel()->m_bounds.vecBoundOffset );
    return out;
}

const CBounds2D& CEntitySAInterface::_GetBoundingBox( CBounds2D& out ) const
{
    CVector pos;
    
    GetCollisionOffset( pos );

    float radius = GetRadius();

    out.m_minX = pos[0] - radius;
    out.m_maxY = pos[1] + radius;
    out.m_maxX = pos[0] + radius;
    out.m_minY = pos[1] - radius;
    return out;
}

void __thiscall CEntitySAInterface::GetCenterPoint( CVector& out ) const
{
    CColModelSAInterface *col = GetColModel();

    GetOffset( out, col->m_bounds.vecBoundOffset );
}

void __thiscall CEntitySAInterface::SetOrientation( float x, float y, float z )
{
    Placeable.SetRotation( x, y, z );
}

bool CEntitySAInterface::IsOnScreen( void ) const
{
    // Bugfix: no col -> not visible
    if ( GetColModel() )
    {
        CVector pos;

        GetCollisionOffset( pos );

        CCameraSAInterface& camera = Camera::GetInterface();

        // MTA fix: get the real entity radius.
        float entityRadius = GetRadius();

        bool isVisible = camera.IsSphereVisible( pos, entityRadius, camera.m_matInverse );

        if ( isVisible )
        {
            return true;
        }

        // Are mirrors enabled?
        if ( *(unsigned char*)0x00B6F998 )
        {
            // Check if we are visible on the mirror.
            isVisible = camera.IsSphereVisible( pos, entityRadius, camera.m_matMirrorInverse );
            
            if ( isVisible )
            {
                return true;
            }
        }
    }

    return false;
}

bool __thiscall CEntitySAInterface::CheckScreenValidity( void ) const
{
    bool retVal;

    __asm
    {
        mov eax,0x0071FAE0
        call eax
        mov retVal,al
    }

    return retVal;
}

void CEntitySAInterface::UpdateRwMatrix( void )
{
    if ( !GetRwObject() )
        return;

    Placeable.GetMatrix( GetRwObject()->parent->modelling );
}

void CEntitySAInterface::UpdateRwFrame( void )
{
    if ( !GetRwObject() )
        return;

    GetRwObject()->parent->Update();
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00407000
bool __thiscall CEntitySAInterface::IsInStreamingArea( void ) const
{
    return Streaming::IsValidStreamingArea( m_areaCode );
}

// Entity referencing system.
// Should prevent entities that are marked by the system to be destroyed in crucial areas.
// Otherwise the system will crash.
static entityReferenceCallback_t _entityAddRef = NULL;
static entityReferenceCallback_t _entityRemoveRef = NULL;

bool CEntitySAInterface::Reference( void )
{
    if ( _entityAddRef )
    {
        CEntitySA *mtaEntity = Pools::GetEntity( this );

        if ( mtaEntity )
        {
            return _entityAddRef( mtaEntity );
        }
    }

    return false;
}

void CEntitySAInterface::Dereference( void )
{
    if ( _entityRemoveRef )
    {
        CEntitySA *mtaEntity = Pools::GetEntity( this );

        if ( mtaEntity )
            _entityRemoveRef( mtaEntity );
    }
}

void Entity::SetReferenceCallbacks( entityReferenceCallback_t addRef, entityReferenceCallback_t delRef )
{
    _entityAddRef = addRef;
    _entityRemoveRef = delRef;
}

void Entity_Init( void )
{
    HookInstall( 0x00535300, h_memFunc( &CEntitySAInterface::GetColModel ), 5 );
    HookInstall( 0x00534540, h_memFunc( &CEntitySAInterface::IsOnScreen ), 5 );
    HookInstall( 0x00534250, h_memFunc( &CEntitySAInterface::GetCollisionOffset ), 5 );
    HookInstall( 0x005449B0, h_memFunc( &CEntitySAInterface::_GetBoundingBox ), 5 );
    HookInstall( 0x00534290, h_memFunc( &CEntitySAInterface::GetCenterPoint ), 5 );
    HookInstall( 0x00446F90, h_memFunc( &CEntitySAInterface::UpdateRwMatrix ), 5 );
    HookInstall( 0x00532B00, h_memFunc( &CEntitySAInterface::UpdateRwFrame ), 5 );
    HookInstall( 0x00536BE0, h_memFunc( &CEntitySAInterface::GetBasingDistance ), 5 );

    EntityRender_Init();
}

void Entity_Shutdown( void )
{
    EntityRender_Shutdown();
}

CEntitySA::CEntitySA ( void )
{
    // Set these variables to a constant state
    m_pInterface = NULL;
    internalID = 0;
    BeingDeleted = false;
    DoNotRemoveFromGame = false;
    m_pStoredPointer = NULL;
    m_ulArrayID = -1;
}

/*VOID CEntitySA::SetModelAlpha ( int iAlpha )
{
    this->internalInterface->ModelClump->SetAlpha(iAlpha);
}*/
VOID CEntitySA::SetPosition(float fX, float fY, float fZ)
{
    // Remove & add to world?
    DEBUG_TRACE("VOID CEntitySA::SetPosition(float fX, float fY, float fZ)");
    CVector * vecPos;
    if ( m_pInterface->Placeable.matrix )
    {
        OnChangingPosition ( CVector ( fX, fY, fZ ) );
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
        m_LastGoodPosition = *vecPos;
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
    if ( m_pInterface->nType == ENTITY_TYPE_OBJECT )
    {
        ((CObjectSAInterface*)m_pInterface)->bUpdateScale = true;
    }
}

VOID CEntitySA::Teleport ( float fX, float fY, float fZ )
{
    DEBUG_TRACE("VOID CEntitySA::Teleport ( float fX, float fY, float fZ )");
    if ( m_pInterface->Placeable.matrix )
    {
        SetPosition ( fX, fY, fZ );

        DWORD dwFunc = (*(CEntitySAInterfaceVTBL**)m_pInterface)->Teleport;
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
    DWORD dwFunc = (*(CEntitySAInterfaceVTBL**)m_pInterface)->ProcessControl;
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
    DWORD dwFunc = (*(CEntitySAInterfaceVTBL**)m_pInterface)->SetupLighting;
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
    pGame->GetWorld()->Remove ( this, CEntity_SetOrientation );
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

    if ( m_pInterface->nType == ENTITY_TYPE_OBJECT )
    {
        ((CObjectSAInterface*)m_pInterface)->bUpdateScale = true;
    }

    pGame->GetWorld()->Add ( this, CEntity_SetOrientation );
}

VOID CEntitySA::FixBoatOrientation ( void )
{
    DEBUG_TRACE("VOID CEntitySA::FixBoatOrientation ( void )");
    pGame->GetWorld()->Remove ( this, CEntity_FixBoatOrientation );
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

    pGame->GetWorld()->Add ( this, CEntity_FixBoatOrientation );
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


void CEntitySA::RestoreLastGoodPhysicsState ( void )
{
    // Validate m_LastGoodPosition
    if ( !IsValidPosition ( m_LastGoodPosition ) )
        m_LastGoodPosition = CVector ( 6000, (float)( rand () % 2000 ), 1000 );

    CMatrix matrix;
    matrix.vPos = m_LastGoodPosition;
    SetMatrix ( &matrix );
    SetPosition ( &m_LastGoodPosition );
}


//
// Get entity position. Fixes bad numbers
//
CVector* CEntitySA::GetPosition ( void )
{
    CVector* pPosition = GetPositionInternal ();
    if ( !IsValidPosition ( *pPosition ) )
    {
        RestoreLastGoodPhysicsState ();
        pPosition = GetPositionInternal ();
    }
    m_LastGoodPosition = *pPosition;
    return pPosition;
}

CVector * CEntitySA::GetPositionInternal ( )
{
    DEBUG_TRACE("CVector * CEntitySA::GetPosition( )");
    if ( m_pInterface->Placeable.matrix )
        return &m_pInterface->Placeable.matrix->vPos;
    else
        return &m_pInterface->Placeable.m_transform.m_translate; 
}


//
// Get entity matrix. Fixes bad numbers
//
CMatrix* CEntitySA::GetMatrix ( CMatrix * matrix )
{
    CMatrix* pMatrix = GetMatrixInternal ( matrix );
    if ( pMatrix )
    {
        if ( !IsValidMatrix ( *pMatrix ) )
        {
            RestoreLastGoodPhysicsState ();
            pMatrix = GetMatrixInternal ( matrix );
        }       
        m_LastGoodPosition = pMatrix->vPos;
    }
    return pMatrix;
}

CMatrix * CEntitySA::GetMatrixInternal ( CMatrix * matrix )
{
    DEBUG_TRACE("CMatrix * CEntitySA::GetMatrix ( CMatrix * matrix )");
    if ( m_pInterface->Placeable.matrix && matrix )
    {
        *matrix = *m_pInterface->Placeable.matrix;
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
        OnChangingPosition ( matrix->vPos );

        *m_pInterface->Placeable.matrix = *matrix;

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

        pGame->GetWorld()->Remove ( this, CEntity_SetMatrix );
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

        if ( m_pInterface->nType == ENTITY_TYPE_OBJECT )
        {
            ((CObjectSAInterface*)m_pInterface)->bUpdateScale = true;
        }

        pGame->GetWorld()->Add ( this, CEntity_SetMatrix );
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


bool CEntitySA::IsFullyVisible ( void )
{
    if ( m_pInterface->bDistanceFade )
        return false;

    if ( m_pInterface->GetEntityVisibilityResult () != 1 )
        return false;

    return true;
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
    RwMatrix * matrix = m_pInterface->Placeable.matrix;
    if ( matrix )
    {
        DWORD dwFunc = FUNC_CMatrix__ConvertFromEulerAngles;
        _asm
        {
            push    iUnknown
            push    fZ
            push    fY
            push    fX
            mov     ecx, matrix
            call    dwFunc
        }
    }
}

VOID CEntitySA::MatrixConvertToEulerAngles ( float * fX, float * fY, float * fZ, int iUnknown )
{
    RwMatrix * matrix = m_pInterface->Placeable.matrix;
    if ( matrix )
    {
        DWORD dwFunc = FUNC_CMatrix__ConvertToEulerAngles;
        _asm
        {
            push    iUnknown
            push    fZ
            push    fY
            push    fX
            mov     ecx, matrix
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

void CEntitySA::SetUnderwater ( bool bUnderwater )
{
    m_pInterface->bUnderwater = bUnderwater;
}

bool CEntitySA::GetUnderwater ( void )
{
    return m_pInterface->bUnderwater;
}
