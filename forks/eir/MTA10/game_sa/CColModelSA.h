/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColModelSA.h
*  PURPOSE:     Header file for collision model entity class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_COLMODELSA
#define __CGAMESA_COLMODELSA

#include <windows.h>
#include <game/CColModel.h>

#include "CQuadTreeSA.h"

//#include "CStreamingSA.h"

#define FUNC_CColModel_Constructor      0x40FB60
#define FUNC_CColModel_Destructor       0x40F700

typedef struct
{
    CVector     vecMin;
    CVector     vecMax;
    CVector     vecOffset;
    FLOAT       fRadius;
} CBoundingBoxSA;


typedef struct
{
    CVector     vecCenter;
    float       fRadius;
} CColSphereSA;


typedef struct
{
    CVector     min;
    CVector     max;
} CColBoxSA;


typedef struct
{
    unsigned short  v1;
    unsigned short  v2;
    unsigned short  v3;
    EColSurface     material;
    CColLighting    lighting;
} CColTriangleSA;


typedef struct
{
    BYTE pad0 [ 12 ];
} CColTrianglePlaneSA;


typedef struct
{
    union
    {
        char version[4];
        unsigned int checksum;
    };

    DWORD size;
    char name[22];
    unsigned short modelId;
} ColModelFileHeader;

class CColFileSA
{
public:
    CColFileSA( const char *name ) : m_bounds( MAX_REGION, -MAX_REGION, -MAX_REGION, MAX_REGION )
    {
        m_loaded = false;
        m_sectorLoad = false;
        m_isProcedural = false;
        m_isInterior = false;

        m_rangeStart = 0x7FFF;
        m_rangeEnd = -0;
        m_refs = 0;

        // Do not store the name, but check it for properties.
        // Apparently these properties are hardcoded.
        if ( stricmp( name, "procobj" ) == 0 || stricmp( name, "proc_int" ) == 0 || stricmp( name, "proc_int2" ) == 0 )
        {
            m_isProcedural = true;
        }

        if ( strnicmp( name, "int_la", 6 ) == 0 ||
             strnicmp( name, "int_sf", 6 ) == 0 ||
             strnicmp( name, "int_veg", 7 ) == 0 ||
             strnicmp( name, "int_cont", 8 ) == 0 ||
             strnicmp( name, "gen_int1", 8 ) == 0 ||
             strnicmp( name, "gen_int2", 8 ) == 0 ||
             strnicmp( name, "gen_int3", 8 ) == 0 ||
             strnicmp( name, "gen_int4", 8 ) == 0 ||
             strnicmp( name, "gen_int5", 8 ) == 0 ||
             strnicmp( name, "gen_intb", 8 ) == 0 ||
             strnicmp( name, "savehous", 8 ) == 0 ||
             stricmp( name, "props" ) == 0 ||
             stricmp( name, "props2" ) == 0 ||      // Okay, I am unsure whether I caught all of the namechecking due to secuROM obfuscation
                                                    // If there is a filename missing, feel free to append it here!
             strnicmp( name, "levelmap", 8 ) == 0 ||
             strnicmp( name, "stadint", 7 ) == 0 )
        {
            m_isInterior = true;
        }
    }

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    // Sectorizer dependencies.
    inline bool     IsSectorFlagged( void ) const           { return m_refs > 0 || m_sectorLoad; }
    inline void     FlagSector( bool flagged )              { m_sectorLoad = flagged; }
    inline bool     IsLoaded( void ) const                  { return m_loaded; }
    inline bool     IsStreamingDisabled( void ) const       { return false; }

    inline void     OnStreamOut( void )                     {}

    CBounds2D                       m_bounds;           // 0

    BYTE                            m_pad[18];          // 16

    short                           m_rangeStart;       // 34
    short                           m_rangeEnd;         // 36
    unsigned short                  m_refs;             // 38

    bool                            m_loaded;           // 40
    bool                            m_sectorLoad;       // 41, set by streaming to indicate that it wants to load it
    bool                            m_isProcedural;     // 42
    bool                            m_isInterior;       // 43
};

typedef struct
{
    unsigned short                  numSpheres;         // 0
    unsigned short                  numBoxes;           // 2
    unsigned short                  numTriangles;       // 4
    BYTE                            ucNumWheels;        // 6
    BYTE                            pad2;               // 7
    CColSphereSA*                   pColSpheres;        // 8
    CColBoxSA*                      pColBoxes;          // 12
    void*                           pSuspensionLines;   // 16
    void*                           unk;                // 20
    CColTriangleSA*                 pColTriangles;      // 24
    CColTrianglePlaneSA*            pColTrianglePlanes; // 28
} CColDataSA;

class CColModelSAInterface
{
public:
                                    CColModelSAInterface    ( void );
                                    ~CColModelSAInterface   ( void );

    void __thiscall                 AllocateData            ( void );
    void __thiscall                 ReleaseData             ( void );

    void*   operator new ( size_t );
    void    operator delete ( void *ptr );

    CBoundingBox                    m_bounds;           // 0
    unsigned char                   m_colPoolIndex;     // 40
    bool                            m_unk : 1;          // 41
    BYTE                            m_pad[2];           // 42
    CColDataSA*                     pColData;           // 44
};

// Utility functions.
void __cdecl Collision_Preload( void );

// Quad tree of all collision files.
typedef CQuadTreeNodeSAInterface <CColFileSA> colFileQuadTreeNode_t;

namespace Collision
{
    // The idea of a collision quad tree conflicts with our dynamical collision assignment system.
    // We have to update the GTA:SA logic so that it does not unload colFiles which we are still
    // using.
    inline colFileQuadTreeNode_t*   GetCollisionQuadTree( void )
    {
        return *(colFileQuadTreeNode_t**)0x0096555C;
    }
};

// Module Initialization.
void ColModel_Init( void );
void ColModel_Shutdown( void );


class CColModelSA : public CColModel
{
    friend class CStreamingSA;
public:
                                    CColModelSA         ( void );
                                    CColModelSA         ( CColModelSAInterface *pInterface, bool destroy = false );
                                    ~CColModelSA        ( void );

    inline CColModelSAInterface*    GetInterface        ( void )                        { return m_pInterface; }

    bool                            Replace             ( unsigned short id );
    bool                            IsReplaced          ( unsigned short id ) const;
    bool                            Restore             ( unsigned short id );
    void                            RestoreAll          ( void );

    const imports_t&                GetImportList       ( void ) const                  { return m_imported; }

    void                            SetOriginal         ( CColModelSAInterface *col, bool isDynamic )   { m_original = col; m_originalDynamic = isDynamic; }
    CColModelSAInterface*           GetOriginal         ( void )                        { return m_original; }

    bool                            IsOriginalDynamic   ( void )                        { return m_originalDynamic; }

private:
    CColModelSAInterface*           m_pInterface;
    CColModelSAInterface*           m_original;
    bool                            m_originalDynamic;
    bool                            m_bDestroyInterface;

    imports_t                       m_imported;
};


typedef void    (__cdecl*SetCachedCollision_t)              ( unsigned int id, CColModelSAInterface *col );
extern SetCachedCollision_t     SetCachedCollision;

#endif
