/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpLight.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_LIGHT_
#define _RENDERWARE_LIGHT_

struct RpLight : public RwObjectFrame
{
    typedef RwNodeList <RwSector*> sectorNode;

    float                   radius;             // 20
    RwColorFloat            color;              // 24
    float                   coneAngle;          // 40
    RwList <sectorNode>     sectors;            // 44
    RwListEntry <RpLight>   sceneLights;        // 52
    unsigned short          frame;              // 60
    unsigned short          unknown2;           // 62
    RwScene*                scene;              // 64
    float                   unk;                // 68
    RpClump*                clump;              // 72
    RwListEntry <RpLight>   clumpLights;        // 76

    // Start of D3D9Light plugin
    int                     lightIndex;         // 84, may be 0-7
    CVector                 attenuation;        // 88

    // Methods.
    void                    SetLightActive          ( bool active )         { BOOL_FLAG( flags, 0x01, active ); }
    bool                    IsLightActive           ( void )                { return IS_ANY_FLAG( flags, 0x01 ); }

    void                    SetLightIndex           ( unsigned int idx );
    unsigned int            GetLightIndex           ( void ) const          { return lightIndex; }

    void                    AddToClump              ( RpClump *clump );
    void                    RemoveFromClump         ( void );

    void                    AddToScene_Local        ( RwScene *scene );
    void                    AddToScene_Global       ( RwScene *scene );

    void                    AddToScene              ( RwScene *scene );
    void                    RemoveFromScene         ( void );

    void                    SetColor                ( const RwColorFloat& color );
};

// Light API.
RpLight*            RpLightCreate( unsigned char type );        // US exe: 0x00752110

// NEW RenderWare functions
RpLight*            RpLightClone( const RpLight *src );

#endif //_RENDERWARE_LIGHT_