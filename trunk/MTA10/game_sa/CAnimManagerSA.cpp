/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAnimManagerSA.cpp
*  PURPOSE:     Animation manager
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CAnimManagerSA::CAnimManagerSA ( void )
{
    memset ( m_pAnimAssocGroups, 0, sizeof ( m_pAnimAssocGroups ) );
    memset ( m_pAnimations, 0, sizeof ( m_pAnimations ) );
    memset ( m_pAnimBlocks, 0, sizeof ( m_pAnimBlocks ) );
}


CAnimManagerSA::~CAnimManagerSA ( void )
{
    for ( unsigned int i = 0 ; i < MAX_ANIM_GROUPS ; i++ )
        if ( m_pAnimAssocGroups [ i ] ) delete m_pAnimAssocGroups [ i ];
    for ( unsigned int i = 0 ; i < MAX_ANIMATIONS ; i++ )
        if ( m_pAnimations [ i ] ) delete m_pAnimations [ i ];
    for ( unsigned int i = 0 ; i < MAX_ANIM_BLOCKS ; i++ )
        if ( m_pAnimBlocks [ i ] ) delete m_pAnimBlocks [ i ];
}


void CAnimManagerSA::Initialize ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_Initialize;
    _asm
    {
        call    dwFunc
    }
}


void CAnimManagerSA::Shutdown ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_Shutdown;
    _asm
    {
        call    dwFunc
    }
}


int CAnimManagerSA::GetNumAnimations ( void )
{
    return * ( int * ) ( VAR_CAnimManager_NumAnimations );
}


int CAnimManagerSA::GetNumAnimBlocks ( void )
{
    return * ( int * ) ( VAR_CAnimManager_NumAnimBlocks );
}


int CAnimManagerSA::GetNumAnimAssocDefinitions ( void )
{
    return * ( int * ) ( VAR_CAnimManager_NumAnimAssocDefinitions );
}


CAnimBlendHierarchy * CAnimManagerSA::GetAnimation ( int ID )
{
    CAnimBlendHierarchySAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimation_int;
    _asm
    {
        push    ID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlendHierarchy ( pInterface );
}


CAnimBlendHierarchy * CAnimManagerSA::GetAnimation ( const char * szName, CAnimBlock * pBlock )
{
    CAnimBlendHierarchySAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimation_str_block;
    CAnimBlockSAInterface * pBlockInterface = pBlock->GetInterface ();
    _asm
    {
        push    pBlockInterface
        push    szName
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendHierarchy ( pInterface );
}


CAnimBlendHierarchy * CAnimManagerSA::GetAnimation ( unsigned int uiIndex, CAnimBlock * pBlock )
{
    CAnimBlendHierarchySAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimation_int_block;
    CAnimBlockSAInterface * pBlockInterface = pBlock->GetInterface ();
    _asm
    {
        push    pBlockInterface
        push    uiIndex
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendHierarchy ( pInterface );
}


CAnimBlock * CAnimManagerSA::GetAnimationBlock ( int ID )
{
    CAnimBlockSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimationBlock_int;
    _asm
    {
        push    ID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlock ( pInterface );
}


CAnimBlock * CAnimManagerSA::GetAnimationBlock ( const char * szName )
{
    CAnimBlockSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimationBlock_str;
    _asm
    {
        push    szName
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlock ( pInterface );
}


int CAnimManagerSA::GetAnimationBlockIndex ( const char * szName )
{
    int iReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimationBlockIndex;
    _asm
    {
        push    szName
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}


int CAnimManagerSA::RegisterAnimBlock ( const char * szName )
{
    int iReturn;
    DWORD dwFunc = FUNC_CAnimManager_RegisterAnimBlock;
    _asm
    {
        push    szName
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}


CAnimBlendAssocGroup * CAnimManagerSA::GetAnimBlendAssoc ( AssocGroupId groupID )
{ 
    CAnimBlendAssocGroupSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimBlendAssoc;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlendAssocGroup ( pInterface );
}


AssocGroupId CAnimManagerSA::GetFirstAssocGroup ( const char * szName )
{
    AssocGroupId groupReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetFirstAssocGroup;
    _asm
    {
        push    szName
        call    dwFunc
        mov     groupReturn, eax
        add     esp, 0x4
    }
    return groupReturn;
}


const char * CAnimManagerSA::GetAnimGroupName ( AssocGroupId groupID )
{
    const char * szReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimGroupName;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     szReturn, eax
        add     esp, 0x4
    }
    return szReturn;
}


const char * CAnimManagerSA::GetAnimBlockName ( AssocGroupId groupID )
{
    const char * szReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimBlockName;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     szReturn, eax
        add     esp, 0x4
    }
    return szReturn;
}


CAnimBlendAssociation * CAnimManagerSA::CreateAnimAssociation ( AssocGroupId animGroup, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_CreateAnimAssociation;
    _asm
    {
        push    animID
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::GetAnimAssociation ( AssocGroupId animGroup, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimAssociation;
    _asm
    {
        push    animID
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::GetAnimAssociation ( AssocGroupId animGroup, const char * szAnimName )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimAssociation_str;
    _asm
    {
        push    szAnimName
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::AddAnimation ( RpClump * pClump, AssocGroupId animGroup, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_AddAnimation;
    _asm
    {
        push    animID
        push    animGroup
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0xC
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::AddAnimation ( RpClump * pClump, CAnimBlendHierarchy * pHierarchy, int ID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_AddAnimation_hier;
    CAnimBlendHierarchySAInterface * pHierarchyInterface = pHierarchy->GetInterface ();
    _asm
    {
        push    ID
        push    pHierarchyInterface
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0xC
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::AddAnimationAndSync ( RpClump * pClump, CAnimBlendAssociation * pAssociation, AssocGroupId animGroup, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_AddAnimationAndSync;
    CAnimBlendAssociationSAInterface * pAssociationInterface = pAssociation->GetInterface ();
    _asm
    {
        push    animID
        push    animGroup
        push    pAssociationInterface
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::BlendAnimation ( RpClump * pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_BlendAnimation;
    _asm
    {
        push    fBlendDelta
        push    animID
        push    animGroup
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::BlendAnimation ( RpClump * pClump, CAnimBlendHierarchy * pHierarchy, int ID, float fBlendDelta )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_BlendAnimation_hier;
    CAnimBlendHierarchySAInterface * pHierarchyInterface = pHierarchy->GetInterface ();
    _asm
    {
        push    fBlendDelta
        push    ID
        push    pHierarchyInterface
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
    return GetAnimBlendAssociation ( pInterface );
}


void CAnimManagerSA::AddAnimBlockRef ( int ID )
{
    DWORD dwFunc = FUNC_CAnimManager_AddAnimBlockRef;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::RemoveAnimBlockRef ( int ID )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlockRef;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::RemoveAnimBlockRefWithoutDelete ( int ID )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlockRefWithoutDelete;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}


int CAnimManagerSA::GetNumRefsToAnimBlock ( int ID )
{
    int iReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetNumRefsToAnimBlock;
    _asm
    {
        push    ID
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}


void CAnimManagerSA::RemoveAnimBlock ( int ID )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlock;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}


AnimAssocDefinition * CAnimManagerSA::AddAnimAssocDefinition ( const char * szBlockName, const char * szAnimName, AssocGroupId animGroup, AnimationId animID, AnimDescriptor * pDescriptor )
{
    AnimAssocDefinition * pReturn;
    DWORD dwFunc = FUNC_CAnimManager_AddAnimAssocDefinition;
    _asm
    {
        push    pDescriptor
        push    animID
        push    animGroup
        push    szAnimName
        push    szBlockName
        call    dwFunc
        mov     pReturn, eax
        add     esp, 0x14
    }
    return NULL;
}


void CAnimManagerSA::ReadAnimAssociationDefinitions ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_ReadAnimAssociationDefinitions;
    _asm
    {
        call    dwFunc
    }
}


void CAnimManagerSA::CreateAnimAssocGroups ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_CreateAnimAssocGroups;
    _asm
    {
        call    dwFunc
    }
}


void CAnimManagerSA::UncompressAnimation ( CAnimBlendHierarchy * pHierarchy )
{
    DWORD dwFunc = FUNC_CAnimManager_UncompressAnimation;
    CAnimBlendHierarchySAInterface * pHierarchyInterface = pHierarchy->GetInterface ();
    _asm
    {
        push    pHierarchyInterface
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::RemoveFromUncompressedCache ( CAnimBlendHierarchy * pHierarchy )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveFromUncompressedCache;
    CAnimBlendHierarchySAInterface * pHierarchyInterface = pHierarchy->GetInterface ();
    _asm
    {
        push    pHierarchyInterface
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::LoadAnimFile ( const char * szFile )
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFile;
    _asm
    {
        push    szFile
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::LoadAnimFile ( RwStream * pStream, bool b1, const char * sz1 )
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFile_stream;
    _asm
    {
        push    sz1
        push    b1
        push    pStream
        call    dwFunc
        add     esp, 0xC
    }
}


void CAnimManagerSA::LoadAnimFiles ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFiles;
    _asm
    {
        call    dwFunc
    }
}


void CAnimManagerSA::RemoveLastAnimFile ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveLastAnimFile;
    _asm
    {
        call    dwFunc
    }
}


bool CAnimManagerSA::HasAnimGroupLoaded ( AssocGroupId groupID )
{
    bool bReturn;
    DWORD dwFunc = FUNC_HasAnimGroupLoaded;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x4
    }
    return bReturn;
}



CAnimBlendAssociation * CAnimManagerSA::RpAnimBlendClumpGetFirstAssociation ( RpClump * pClump )
{
    CAnimBlendAssociationSAInterface * pInterface;    
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetFirstAssociation;
    _asm
    {
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::RpAnimBlendClumpGetAssociation ( RpClump * pClump, const char * szAnimName )
{
    CAnimBlendAssociationSAInterface * pInterface;    
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetAssociation_str;
    _asm
    {
        push    szAnimName
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::RpAnimBlendClumpGetAssociation ( RpClump * pClump, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;    
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetAssociation_int;
    _asm
    {
        push    animID
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::RpAnimBlendGetNextAssociation ( CAnimBlendAssociation * pAssociation )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_RpAnimBlendGetNextAssociation;
    CAnimBlendAssociationSAInterface * pAssociationInterface = pAssociation->GetInterface ();
    _asm
    {
        push    pAssociationInterface
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlendAssociation ( pInterface );
}


int CAnimManagerSA::RpAnimBlendClumpGetNumAssociations ( RpClump * pClump )
{
    int iReturn;
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetNumAssociations;
    _asm
    {
        push    pClump
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}


void CAnimManagerSA::RpAnimBlendClumpUpdateAnimations ( RpClump * pClump, float f1, bool b1 )
{
    DWORD dwFunc = FUNC_RpAnimBlendClumpUpdateAnimations;
    _asm
    {
        push    b1
        push    f1
        push    pClump
        call    dwFunc
        add     esp, 0xC
    }
}


CAnimBlendAssociation * CAnimManagerSA::GetAnimBlendAssociation ( CAnimBlendAssociationSAInterface * pInterface )
{
    if ( pInterface )
    {
        list < CAnimBlendAssociation * > ::iterator iter = m_Associations.begin ();
        for ( ; iter != m_Associations.end () ; iter++ )
        {
            if ( (*iter)->GetInterface () == pInterface )
            {
                return *iter;
            }
        }
        CAnimBlendAssociation * pAssociation = new CAnimBlendAssociationSA ( pInterface );
        m_Associations.push_back ( pAssociation );
        return pAssociation;
    }
    return NULL;
}


CAnimBlendAssocGroup * CAnimManagerSA::GetAnimBlendAssocGroup ( CAnimBlendAssocGroupSAInterface * pInterface )
{
    if ( pInterface )
    {        
        AssocGroupId groupID = pInterface->groupID;
        if ( !m_pAnimAssocGroups [ groupID ] )
        {
            m_pAnimAssocGroups [ groupID ] = new CAnimBlendAssocGroupSA ( pInterface );
        }
        return m_pAnimAssocGroups [ groupID ];
    }
    return NULL;
}


CAnimBlock * CAnimManagerSA::GetAnimBlock ( CAnimBlockSAInterface * pInterface )
{
    if ( pInterface )
    {
        unsigned int ID = pInterface->GetIndex ();
        if ( ID < MAX_ANIM_BLOCKS )
        {
            if ( !m_pAnimBlocks [ ID ] )
            {
                m_pAnimBlocks [ ID ] = new CAnimBlockSA ( pInterface );
            }
            return m_pAnimBlocks [ ID ];
        }
    }
    return NULL;
}


CAnimBlendHierarchy * CAnimManagerSA::GetAnimBlendHierarchy ( CAnimBlendHierarchySAInterface * pInterface )
{
    if ( pInterface )
    {
        unsigned int ID = pInterface->GetIndex ();
        if ( ID < MAX_ANIMATIONS )
        {
            if ( !m_pAnimations [ ID ] )
            {
                m_pAnimations [ ID ] = new CAnimBlendHierarchySA ( pInterface );
            }
            return m_pAnimations [ ID ];
        }
    }
    return NULL;
}
