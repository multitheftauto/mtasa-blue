/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CClumpModelInfoSA.h
*  PURPOSE:     Clump model instance
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CClumpModelInfoSA_
#define _CClumpModelInfoSA_

#define MAX_BONES   64

/*===================================================
    CComponentHierarchySAInterface

    There are hardcoded vehicle description structures built into the engine,
    for every vehicle type (car, heli, plane, boat, bike, bicycle, train, etc).
    Every vehicle component is registered using this interface.

        m_name - szName of the frame which describes this component
        m_frameHierarchy - hierarchy id to set to the found frame
        m_flags - custom component flags

    The pointers to all of such component registry chains reside at 0x008A7740.
    Only CVehicleModelInfoSAInterface is known to use this interface.
===================================================*/

// Custom component flags, needs research
#define ATOMIC_HIER_ACTIVE          0x00000001
#define ATOMIC_HIER_FRONTSEAT       0x00000008
#define ATOMIC_HIER_DOOR            0x00000010
#define ATOMIC_HIER_SEAT            0x00000200
#define ATOMIC_HIER_UNKNOWN4        0x00010000
#define ATOMIC_HIER_UNKNOWN3        0x00020000
#define ATOMIC_HIER_UNKNOWN5        0x00100000
#define ATOMIC_HIER_UNKNOWN6        0x00200000

class CComponentHierarchySAInterface
{
public:
    const char*                     m_name;
    unsigned int                    m_frameHierarchy;
    unsigned int                    m_flags;
};

class CClumpModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
    virtual CColModelSAInterface* __thiscall        GetCollision( void );
    virtual void __thiscall                         SetClump    ( RpClump *clump );

    void                    Init                    ( void );
    void                    DeleteRwObject          ( void );
    eRwType                 GetRwModelType          ( void );
    RwObject*               CreateRwObjectEx        ( RwMatrix& mat );
    RwObject*               CreateRwObject          ( void );
    void                    SetAnimFile             ( const char *name );
    void                    ConvertAnimFileIndex    ( void );
    int                     GetAnimFileIndex        ( void );

    RpClump*                GetRwObject             ( void )                { return (RpClump*)pRwObject; }
    const RpClump*          GetRwObject             ( void ) const          { return (const RpClump*)pRwObject; }

    void                    AssignAtomics           ( CComponentHierarchySAInterface *atomics );

    int                     m_animBlock;                    // 32
};

#endif //_CClumpModelInfoSA_