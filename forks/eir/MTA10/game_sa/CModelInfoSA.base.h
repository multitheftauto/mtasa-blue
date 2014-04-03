/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelInfoSA.base.h
*  PURPOSE:     Header file for entity model information handler class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _MODELINFO_BASE_
#define _MODELINFO_BASE_

class CAtomicModelInfoSA;
class CDamageAtomicModelInfoSA;
class CLODAtomicModelInfoSA;
class CColModelSAInterface;

enum eModelType : unsigned char
{
    MODEL_ATOMIC = 1,
    MODEL_VEHICLE = 6,
    MODEL_PED
};

namespace ModelInfo
{
    struct timeInfo //size: 4
    {
        unsigned char       m_fromHour;     // 0
        unsigned char       m_toHour;       // 1
        short               m_model;        // 2
    };
};

// Abstract class for every model info class
class CBaseModelInfoSA abstract
{
public:
    virtual                                             ~CBaseModelInfoSA           ( void )    {}              // 0

    virtual CAtomicModelInfoSA* __thiscall              GetAtomicModelInfo          ( void ) = 0;               // 4
    virtual CDamageAtomicModelInfoSA* __thiscall        GetDamageAtomicModelInfo    ( void ) = 0;               // 8
    virtual CLODAtomicModelInfoSA* __thiscall           GetLODAtomicModelInfo       ( void ) = 0;               // 12
    virtual eModelType __thiscall                       GetModelType                ( void ) = 0;               // 16
    virtual ModelInfo::timeInfo* __thiscall             GetTimeInfo                 ( void ) = 0;               // 20
    virtual void __thiscall                             Init                        ( void ) = 0;               // 24
    virtual void __thiscall                             Shutdown                    ( void ) = 0;               // 28
    virtual void __thiscall                             DeleteRwObject              ( void ) = 0;               // 32
    virtual eRwType __thiscall                          GetRwModelType              ( void ) = 0;               // 36
    virtual RwObject* __thiscall                        CreateRwObjectEx            ( RwMatrix& mat ) = 0;      // 40
    virtual RwObject* __thiscall                        CreateRwObject              ( void ) = 0;               // 44
    virtual void __thiscall                             SetAnimFile                 ( const char *name ) = 0;   // 48
    virtual void __thiscall                             ConvertAnimFileIndex        ( void ) = 0;               // 52
    virtual int __thiscall                              GetAnimFileIndex            ( void ) = 0;               // 56
};

#define RENDER_PRERENDERED          0x0001
#define RENDER_LAST                 0x0002
#define RENDER_ADDITIVE             0x0004
#define RENDER_NOSHADOW             0x0008
#define RENDER_NOCULL               0x0010
#define RENDER_LOD                  0x0020
#define RENDER_BACKFACECULL         0x0040
#define RENDER_COLMODEL             0x0080
#define RENDER_STATIC               0x0100
#define RENDER_NOSKELETON           0x0200

#define COLL_AUDIO                  0x0001
#define COLL_SWAYINWIND             0x0002
#define COLL_STREAMEDWITHMODEL      0x0004
#define COLL_NOCOLLFLYER            0x0008
#define COLL_COMPLEX                0x0010
#define COLL_WETROADREFLECT         0x0020

// Interface which provides common functionality to model info deriviations.
class CBaseModelInfoSAInterface abstract : public CBaseModelInfoSA
{
public:
    CBaseModelInfoSAInterface                               ( void );
    ~CBaseModelInfoSAInterface                              ( void );

    CAtomicModelInfoSA*         GetAtomicModelInfo          ( void );
    CDamageAtomicModelInfoSA*   GetDamageAtomicModelInfo    ( void );
    CLODAtomicModelInfoSA*      GetLODAtomicModelInfo       ( void );
    void                        Init                        ( void );
    void                        Shutdown                    ( void );
    void __thiscall             SetCollision                ( CColModelSAInterface *col, bool isDynamic );
    void __thiscall             DeleteCollision             ( void );
    void                        DereferenceTXD              ( void );
    ModelInfo::timeInfo*        GetTimeInfo                 ( void );
    void                        Reference                   ( void );
    unsigned int                GetRefCount                 ( void ) const              { return usNumberOfRefs; }
    void                        Dereference                 ( void );

    unsigned int                GetHashKey                  ( void ) const              { return ulHashKey; }

    float                       GetLODDistance              ( void ) const              { return fLodDistanceUnscaled; }
    void                        SetLODDistance              ( float dist )              { fLodDistanceUnscaled = dist; }

    unsigned char               GetEffectCount              ( void ) const              { return ucNumOf2DEffects; }

    void __thiscall             SetColModel                 ( CColModelSAInterface *col, bool isDynamic );
    bool                        IsDynamicCol                ( void ) const              { return IS_FLAG( renderFlags, RENDER_COLMODEL ); }
    void __thiscall             UnsetColModel               ( void );

    unsigned short              GetFlags                    ( void );

    RwObject*                   GetRwObject                 ( void )                    { return pRwObject; }
    const RwObject*             GetRwObject                 ( void ) const              { return pRwObject; }

    bool                        ValidateResource            ( modelId_t modelIndex, RwObject *obj );

    unsigned int                ulHashKey;                  // +4       Generated by CKeyGen::GetUppercaseKey(char const *) called by CBaseModelInfo::SetModelName(char const *)
    unsigned short              usNumberOfRefs;             // +8
    short                       usTextureDictionary;        // +10
    unsigned char               ucAlpha;                    // +12

    unsigned char               ucNumOf2DEffects;           // +13
    short                       usEffectID;                 // +14      Something with 2d effects

    short                       usDynamicIndex;             // +16      Index of attributes in g_dynObjData

    union
    {
        struct
        {
            unsigned short      renderFlags : 10;           // 18
            unsigned short      collFlags : 6;
        };
        struct
        {
            unsigned short      flags;                      // 18
        };
        struct
        {
            unsigned short      unkPad : 12;                // 18
            unsigned short      atomicType : 4;
        };
        struct
        {
            // Flags used by CBaseModelInfo (one way to represent them)
            unsigned char           bHasBeenPreRendered: 1;     // +18
            unsigned char           bAlphaTransparency: 1;
            unsigned char           bIsLod: 1;
            unsigned char           bDontCastShadowsOn: 1;
            unsigned char           bDontWriteZBuffer: 1;
            unsigned char           bDrawAdditive: 1;
            unsigned char           bDrawLast: 1;
            unsigned char           bDoWeOwnTheColModel: 1;

            unsigned char           bCollisionless: 1;          // +19
            unsigned char           dwUnknownFlag26: 1;
            unsigned char           dwUnknownFlag27: 1;
            unsigned char           bSwaysInWind: 1;
            unsigned char           bCollisionWasStreamedWithModel: 1;  // CClumpModelInfo::SetCollisionWasStreamedWithModel(unsigned int)
            unsigned char           bDontCollideWithFlyer: 1;           // CAtomicModelInfo::SetDontCollideWithFlyer(unsigned int)
            unsigned char           bHasComplexHierarchy: 1;            // CClumpModelInfo::SetHasComplexHierarchy(unsigned int)
            unsigned char           bWetRoadReflection: 1;              // CAtomicModelInfo::SetWetRoadReflection(unsigned int)
        };
    };

    CColModelSAInterface*       pColModel;                  // +20      CColModel: public CBoundingBox

    float                       fLodDistanceUnscaled;       // +24      Scaled is this value multiplied with flt_B6F118

    RwObject*                   pRwObject;                  // 28

    // CClumpModelInfo::SetCollisionWasStreamedWithModel(unsigned int)
    // CAtomicModelInfo::SetDontCollideWithFlyer(unsigned int)
    // CClumpModelInfo::SetHasComplexHierarchy(unsigned int)
    // CAtomicModelInfo::SetWetRoadReflection(unsigned int)

    // CWeaponModelInfo:
    // +36 = Weapon info as int
};

// So we do not have to redefine it and the headers can use it.
extern CBaseModelInfoSAInterface **ppModelInfo;

void ModelInfoBase_Init( void );
void ModelInfoBase_Shutdown( void );

#endif //_MODELINFO_BASE_