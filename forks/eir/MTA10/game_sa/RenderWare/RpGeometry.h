/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpGeometry.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_GEOMETRY_
#define _RENDERWARE_GEOMETRY_

#define RW_STREAMLINE_TASK_INIT_NORMALS         0x00000001

// MTA extensions
struct RpGeomStreamline
{
    unsigned int                m_tasks;
    RwListEntry <RpGeometry>    m_managerNode;
};

// Core class.
#define RW_GEOMETRY_NO_SKIN         0x00000001
#define RW_GEOMETRY_NORMALS         0x00000010
#define RW_GEOMETRY_GLOBALLIGHT     0x00000020

struct RpGeometry : public RwObject
{
    unsigned int            flags;                              // 8
    unsigned short          unknown1;                           // 12
    unsigned short          refs;                               // 14

    unsigned int            triangleSize;                       // 16
    unsigned int            verticeSize;                        // 20
    unsigned int            numMeshes;                          // 24
    unsigned int            texcoordSize;                       // 28

    RpMaterials             materials;                          // 32
    RpTriangle*             triangles;                          // 44
    RwColor*                colors;                             // 48
    RwTextureCoordinates*   texcoords[RW_MAX_TEXTURE_COORDS];   // 52
    RwLinkedMateria*        linkedMateria;                      // 84
    void*                   info;                               // 88
    RpGeomMesh*             meshes;                             // 92, allocated by count in array
    unsigned int            usageFlag;                          // 96
    RpSkeleton*             skeleton;                           // 100
    RwColor*                nightColor;                         // 104
    BYTE                    pad[12];                            // 108
    Rw2dfx*                 _2dfx;                              // 120

    // Our own extension
    RpGeomStreamline        streamline;                         // 124

    template <class type>
    bool                    ForAllMateria( bool (*callback)( RpMaterial *mat, type data ), type data )
    {
        for ( unsigned int n = 0; n < materials.entries; n++ )
        {
            if ( !callback( materials.data[n], data ) )
                return false;
        }

        return true;
    }
    bool                    IsAlpha( void );
    void                    UnlinkFX( void );
};

typedef RpGeometry RwGeometry;

#endif //_RENDERWARE_GEOMETRY_