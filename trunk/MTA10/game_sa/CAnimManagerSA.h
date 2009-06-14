/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CAnimManagerSA.h
*  PURPOSE:		Header file for animation manager class
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAnimManagerSA_H
#define __CAnimManagerSA_H

#include <game/CAnimManager.h>
#include <game/CAnimBlendAssociation.h>
#include <game/CAnimBlendHierarchy.h>
#include <game/CAnimBlock.h>

#include "Common.h"
#include <list>

#define FUNC_CAnimManager_Initialize                        0x5bf6b0
#define FUNC_CAnimManager_Shutdown                          0x4d4130
#define FUNC_CAnimManager_GetAnimationBlock_int             0x406f90
#define FUNC_CAnimManager_GetAnimationBlock_str             0x4d3940
#define FUNC_CAnimManager_GetAnimationBlockIndex            0x4d3990
#define FUNC_CAnimManager_RegisterAnimBlock                 0x4d3e50
#define FUNC_CAnimManager_GetAnimBlendAssoc                 0x45b0e0
#define FUNC_CAnimManager_GetFirstAssocGroup                0x4d39b0
#define FUNC_CAnimManager_GetAnimation_int                  0x4cde50
#define FUNC_CAnimManager_GetAnimation_str_block            0x4d42f0
#define FUNC_CAnimManager_GetAnimation_int_block            0x4d39f0
#define FUNC_CAnimManager_GetAnimGroupName                  0x4d3a20
#define FUNC_CAnimManager_GetAnimBlockName                  0x4d3a30
#define FUNC_CAnimManager_AddAnimBlockRef                   0x4d3fb0
#define FUNC_CAnimManager_RemoveAnimBlockRef                0x4d3fd0
#define FUNC_CAnimManager_RemoveAnimBlockRefWithoutDelete   0x4d3ff0
#define FUNC_CAnimManager_GetNumRefsToAnimBlock             0x4d4010
#define FUNC_CAnimManager_RemoveAnimBlock                   0x4d3f40
#define FUNC_CAnimManager_RemoveLastAnimFile                0x4d3ed0
#define FUNC_CAnimManager_AddAnimation                      0x4d3aa0
#define FUNC_CAnimManager_AddAnimation_hier                 0x4d4330
#define FUNC_CAnimManager_BlendAnimation                    0x4d4610
#define FUNC_CAnimManager_BlendAnimation_hier               0x4d4410
#define FUNC_CAnimManager_AddAnimationAndSync               0x4d3b30
#define FUNC_CAnimManager_CreateAnimAssociation             0x4d3a40
#define FUNC_CAnimManager_GetAnimAssociation                0x4d3a60
#define FUNC_CAnimManager_GetAnimAssociation_str            0x4d3a80
#define FUNC_CAnimManager_AddAnimAssocDefinition            0x4d3ba0
#define FUNC_CAnimManager_ReadAnimAssociationDefinitions    0x5bc910
#define FUNC_CAnimManager_CreateAnimAssocGroups             0x4d3cc0
#define FUNC_CAnimManager_UncompressAnimation               0x4d41c0
#define FUNC_CAnimManager_RemoveFromUncompressedCache       0x4d42a0
#define FUNC_CAnimManager_LoadAnimFile                      0x4d55d0
#define FUNC_CAnimManager_LoadAnimFile_stream               0x4d47f0
#define FUNC_CAnimManager_LoadAnimFiles                     0x4d5620
#define ARRAY_CAnimManager_AnimAssocGroups                  0xb4ea34
#define ARRAY_CAnimManager_Animations                       0xb4ea40
#define ARRAY_CAnimManager_AnimBlocks                       0xb5d4a0
#define VAR_CAnimManager_NumAnimAssocDefinitions            0xb4ea28
#define VAR_CAnimManager_NumAnimations                      0xb4ea2c
#define VAR_CAnimManager_NumAnimBlocks                      0xb4ea30
// Non members
#define FUNC_HasAnimGroupLoaded                             0x45b130
#define FUNC_RpAnimBlendClumpGetFirstAssociation            0x4d15e0
#define FUNC_RpAnimBlendClumpGetAssociation_str             0x4d6870
#define FUNC_RpAnimBlendClumpGetAssociation_int             0x4d68b0
#define FUNC_RpAnimBlendGetNextAssociation                  0x4d6ab0
#define FUNC_RpAnimBlendClumpGetNumAssociations             0x4d6b60
#define FUNC_RpAnimBlendClumpUpdateAnimations               0x4d34f0

class CAnimManagerSAInterface
{
public:
};

class CAnimManagerSA : public CAnimManager
{
public:
                                CAnimManagerSA                          ( void );
                                ~CAnimManagerSA                         ( void );

    void                        Initialize                              ( void );
    void                        Shutdown                                ( void );

    int                         GetNumAnimations                        ( void );
    int                         GetNumAnimBlocks                        ( void );
    int                         GetNumAnimAssocDefinitions              ( void );

    CAnimBlendHierarchy *       GetAnimation                            ( int ID );
    CAnimBlendHierarchy *       GetAnimation                            ( const char * szName, CAnimBlock * pBlock );
    CAnimBlendHierarchy *       GetAnimation                            ( unsigned int uiIndex, CAnimBlock * pBlock );
    
    CAnimBlock *                GetAnimationBlock                       ( int ID );
    CAnimBlock *                GetAnimationBlock                       ( const char * szName );
    int                         GetAnimationBlockIndex                  ( const char * szName );
    int                         RegisterAnimBlock                       ( const char * szName );
        
    CAnimBlendAssocGroup *      GetAnimBlendAssoc                       ( AssocGroupId groupID );
    AssocGroupId                GetFirstAssocGroup                      ( const char * szName );

    const char *                GetAnimGroupName                        ( AssocGroupId groupID );
    const char *                GetAnimBlockName                        ( AssocGroupId groupID );

    CAnimBlendAssociation *     CreateAnimAssociation                   ( AssocGroupId animGroup, AnimationId animID );
    CAnimBlendAssociation *     GetAnimAssociation                      ( AssocGroupId animGroup, AnimationId animID );
    CAnimBlendAssociation *     GetAnimAssociation                      ( AssocGroupId animGroup, const char * szAnimName );
    CAnimBlendAssociation *     AddAnimation                            ( RpClump * pClump, AssocGroupId animGroup, AnimationId animID );
    CAnimBlendAssociation *     AddAnimation                            ( RpClump * pClump, CAnimBlendHierarchy *, int flags );
    CAnimBlendAssociation *     AddAnimationAndSync                     ( RpClump * pClump, CAnimBlendAssociation * pAssociation, AssocGroupId animGroup, AnimationId animID );
    CAnimBlendAssociation *     BlendAnimation                          ( RpClump * pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta );
    CAnimBlendAssociation *     BlendAnimation                          ( RpClump * pClump, CAnimBlendHierarchy * pHierarchy, int flags, float fBlendDelta );
    
    void                        AddAnimBlockRef                         ( int ID );
    void                        RemoveAnimBlockRef                      ( int ID );
    void                        RemoveAnimBlockRefWithoutDelete         ( int ID );
    int                         GetNumRefsToAnimBlock                   ( int ID );
    void                        RemoveAnimBlock                         ( int ID );
    
    AnimAssocDefinition *       AddAnimAssocDefinition                  ( const char * szBlockName, const char * szAnimName, AssocGroupId animGroup, AnimationId animID, AnimDescriptor * pDescriptor );
    void                        ReadAnimAssociationDefinitions          ( void );
    void                        CreateAnimAssocGroups                   ( void );

    void                        UncompressAnimation                     ( CAnimBlendHierarchy * pHierarchy );
    void                        RemoveFromUncompressedCache             ( CAnimBlendHierarchy * pHierarchy );

    void                        LoadAnimFile                            ( const char * szFile );
    void                        LoadAnimFile                            ( RwStream * pStream, bool b1, const char * sz1 );
    void                        LoadAnimFiles                           ( void );
    void                        RemoveLastAnimFile                      ( void );

    // Non members
    bool                        HasAnimGroupLoaded                      ( AssocGroupId groupID );
    CAnimBlendAssociation *     RpAnimBlendClumpGetFirstAssociation     ( RpClump * pClump );
    CAnimBlendAssociation *     RpAnimBlendClumpGetAssociation          ( RpClump * pClump, const char * szAnimName );
    CAnimBlendAssociation *     RpAnimBlendClumpGetAssociation          ( RpClump * pClump, AnimationId animID );
    CAnimBlendAssociation *     RpAnimBlendGetNextAssociation           ( CAnimBlendAssociation * pAssociation );
    int                         RpAnimBlendClumpGetNumAssociations      ( RpClump * pClump );
    void                        RpAnimBlendClumpUpdateAnimations        ( RpClump * pClump, float f1, bool b1 );

    // MTA members
    CAnimBlendAssociation *     GetAnimBlendAssociation                 ( CAnimBlendAssociationSAInterface * pInterface );
    CAnimBlendAssocGroup *      GetAnimBlendAssocGroup                  ( CAnimBlendAssocGroupSAInterface * pInterface );
    CAnimBlock *                GetAnimBlock                            ( CAnimBlockSAInterface * pInterface );
    CAnimBlendHierarchy *       GetAnimBlendHierarchy                   ( CAnimBlendHierarchySAInterface * pInterface );
    
    CAnimBlendHierarchy *       GetAnimation                            ( const char * szName, const char * szBlockName );
    
    void                        ClearAnimBlendAssociation               ( CAnimBlendAssociationSAInterface * pInterface );
    void                        ClearAnimBlendHierarchy                 ( CAnimBlendHierarchySAInterface * pInterface );

private:
    CAnimBlendAssocGroup *      m_pAnimAssocGroups [ MAX_ANIM_GROUPS ];
    CAnimBlendHierarchy *       m_pAnimations [ MAX_ANIMATIONS ];
    CAnimBlock *                m_pAnimBlocks [ MAX_ANIM_BLOCKS ];
    std::list < CAnimBlendAssociation *  > m_Associations;
};

#endif