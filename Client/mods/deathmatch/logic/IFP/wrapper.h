#include <Windows.h>
#include "GTASA_classes.h"
#include <vector>
#include <map>
#include <string>
#include <iterator>

// Animations = _CAnimBlendHierarchy
#define ARRAY_CAnimManager_AnimBlocks                       0xb5d4a0
#define ARRAY_CAnimManager_Animations                       0xB4EA40
#define VAR_CAnimManager_NumAnimations                      0xb4ea2c
#define VAR_CAnimManager_NumAnimBlocks                      0xb4ea30

int GetNumAnimations ( void );
int GetNumAnimBlocks ( void );


typedef char * ( __cdecl* hAddAnimAssocDefinition )
(
    const char *   BlockName, 
    const char *   AnimName,
    unsigned long  AnimationGroup,
    unsigned long  AnimationID,
    int            Descriptor
);


typedef int ( __cdecl* hRegisterAnimBlock )
(
    const char * szName
);



typedef void(WINAPI* hCreateAssociations_Clump)
(
    char const* szIfpFile,
    void * pClump,
    char** pszAnimations,
    int NumAnimations
);

typedef void(WINAPI* hCreateAssociations)
(
    char const* szIfpFile,
    char const* arg2,
    char const* arg3,
    int arg4
);


typedef CAnimBlendAssoc * (__cdecl* hRpAnimBlendClumpGetAssociation)
(
    void * pClump,
    const char * szAnimName
);

typedef CAnimBlendAssoc * (__cdecl* hRpAnimBlendClumpGetAssociationAnimId)
(
    //int clump,
    void * clump,
    unsigned int animId
);



typedef CAnimBlendAssoc * (__cdecl* hGetAnimAssociationAnimId)
(
    DWORD animGroup,
    DWORD animID
);


typedef const char * (__cdecl* hGetAnimGroupName)
( 
    DWORD groupID
);


typedef CAnimBlendAssoc * (__cdecl* hCreateAnimAssociation)
(
    DWORD animGroup,
    DWORD animID
);

typedef CAnimBlendAssoc * (__cdecl* hAddAnimation)
(
    void * pClump,
    DWORD animGroup,
    DWORD animID
);

typedef CAnimBlendAssoc * (__cdecl* hAddAnimationAndSync)
(
    void * pClump,
    CAnimBlendAssoc * pAnimAssociation,
    DWORD animGroup,
    DWORD animID
);


typedef CAnimBlendStaticAssoc * (__thiscall* hCAnimBlendStaticAssoc_Constructor) 
( CAnimBlendStaticAssoc * pThis );

typedef void (__thiscall* hCAnimBlendStaticAssoc_Init)
(
    CAnimBlendStaticAssoc * pThis,
    void* pClump,
    _CAnimBlendHierarchy* pAnimBlendHierarchy
);

typedef signed int (__thiscall* hCAnimBlendAssoc_Init_reference)
(
    CAnimBlendStaticAssoc& ReferenceStaticAssoc
);


typedef int(__thiscall* hCAnimBlendAssoc_Init)
(
    CAnimBlendAssoc * pThis,
    void* pClump, 
    _CAnimBlendHierarchy* pAnimBlendHier
);

typedef unsigned int(__thiscall* h_CAnimBlendHierarchy_SetName)
(
    void * pThis,
    char * string
);

typedef unsigned int(__cdecl* hGetUppercaseKey)
(
    char const * str
);

typedef int(__cdecl * hLoadAnimFile_stream)
(
    int pStream,
    bool b1,
    const char * sz1
);

typedef const char *(__cdecl * hGetAnimBlockName)
(
    DWORD groupID
);

typedef CAnimBlock *(__cdecl * hGetAnimationBlock)
(
    const char * szName
);

typedef _CAnimBlendHierarchy *(__cdecl * hGetAnimation)
(
    const char * szName,
    CAnimBlock * pBlock
);

typedef CAnimBlendAssoc *(__cdecl * hAddAnimation_hier)
(
    void * pClump, 
    _CAnimBlendHierarchy *, 
    int ID
);


typedef WORD *(__thiscall * h_CAnimBlendSequence_Constructor)(void);

typedef void(__thiscall * h_CAnimBlendSequence_Deconstructor)(void);

typedef int(__thiscall * h_CAnimBlendHierarchy_Constructor)(void);
//typedef int(WINAPI * h_CAnimBlendHierarchy_Constructor)(void);

typedef unsigned int(__thiscall * h_CAnimBlendSequence_SetName)
(
    void * pThis,
    char const* string
);

typedef int(__thiscall * h_CAnimBlendSequence_SetBoneTag)
(
    void * pThis,
    int hash
);

typedef void *(__thiscall * h_CAnimBlendSequence_SetNumFrames)
(
    void * pThis,
    size_t frameCount,
    bool isRoot, 
    bool compressed, 
    unsigned char* st
);


typedef int(__thiscall *  h_CAnimBlendHierarchy_RemoveQuaternionFlips) ( void );

typedef int(__thiscall *  h_CAnimBlendHierarchy_CalcTotalTime) (void);

typedef int(__cdecl *  hCreateAnimAssocGroups)
(
    void
);

typedef int(__cdecl *  hLoadPedAnimIFPFile)
(
    void
);

typedef int * (__cdecl *  hRwStreamOpen)
(int a1, int a2, DWORD *a3);

typedef BOOL(__cdecl *  hRwStreamClose)
(DWORD *a1, DWORD *a2);

typedef int(__cdecl *  hGetFirstAssocGroup)
(char *a1);


typedef CAnimBlendAssoc *(__thiscall *  hCAnimBlendAssocGroup_CopyAnimation)
(
DWORD * pThis, 
int AnimID
);

typedef int(__thiscall *  hCAnimBlendAssoc_SyncAnimation)
(
CAnimBlendAssoc* pThis, 
CAnimBlendAssoc* a2
);

typedef int(__thiscall *  hCAnimBlendAssoc_Start)
(
CAnimBlendAssoc* pThis,
float a2
);


typedef CAnimBlendAssoc * (__thiscall *  hCAnimBlendAssoc_Constructor)
( 
CAnimBlendAssoc * pThis, 
void * pClump, 
_CAnimBlendHierarchy * pAnimBlendHierarchy 
);


typedef void (__cdecl *  hUncompressAnimation)
(
_CAnimBlendHierarchy * pAnimBlendHierarchy 
);

typedef CAnimBlendAssoc * (__thiscall *  hCAnimBlendAssoc_Constructor_staticAssocRef)
(
CAnimBlendAssoc * pThis,
CAnimBlendStaticAssoc& StaticAssociationByReference
);


char * __cdecl NEW_AddAnimAssocDefinition
(
    const char *   BlockName,
    const char *   AnimName,
    unsigned long  AnimationGroup,
    unsigned long  AnimationID,
    int            Descriptor
);

int __cdecl NEW_RegisterAnimBlock
(
    const char * szName
);


void WINAPI NEW_CreateAssociations_Clump
(
    char const* szIfpFile,
    void * pClump, 
    char** pszAnimations,
    int NumAnimations
);

void WINAPI NEW_CreateAssociations
(
    char const* szIfpFile,
    char const* arg2, 
    char const* arg3, 
    int arg4
);

CAnimBlendAssoc * NEW_CreateAnimAssociation
(
    DWORD animGroup,
    DWORD animID
);

CAnimBlendAssoc * NEW_AddAnimation
(
    void * pClump,
    DWORD animGroup,
    DWORD animID
);

CAnimBlendAssoc * NEW_AddAnimationAndSync
(
    void * pClump,
    CAnimBlendAssoc * pAnimAssociation,
    DWORD animGroup,
    DWORD animID
);

void __fastcall NEW_CAnimBlendStaticAssoc_Init
(
    CAnimBlendStaticAssoc * pThis,
    void * padding,
    void* pClump, 
    _CAnimBlendHierarchy* pAnimBlendHierarchy
);

unsigned int WINAPI NEW__CAnimBlendHierarchy_SetName
(
    char * string
);

unsigned int __cdecl NEW_GetUppercaseKey
(
    char const * str
);


int __cdecl NEW_LoadAnimFile_stream
(
    int pStream,
    bool b1, 
    const char * sz1
);

int NEW_LoadPedAnimIFPFile
(
    void
);


void __cdecl NEW_UncompressAnimation
(
_CAnimBlendHierarchy * pAnimBlendHierarchy 
);


const char * GetNameFromHash(DWORD Hash);
int GetNumAnimations(void);
void printHierarchies(void);
void printAnimBlocks(void);
bool isAnimationHierarchyLoaded(const char * AnimationName);
_CAnimBlendHierarchy * GetAnimHierachyBy_GroupId_AnimId (DWORD GroupID, DWORD AnimID);
CAnimBlendStaticAssoc * GetAnimStaticAssocBy_GroupId_AnimId(DWORD GroupID, DWORD AnimID);
