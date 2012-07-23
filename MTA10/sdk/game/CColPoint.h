/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CColPoint.h
*  PURPOSE:     Collision point interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CCOLPOINT
#define __CGAME_CCOLPOINT

#include <CVector.h>
class CColPointSAInterface;

// Collision stored surface type
struct EColSurface
{
    enum EColSurfaceValue
    {
        DEFAULT,
        TARMAC,
        TARMAC_FUCKED,
        TARMAC_REALLYFUCKED,
        PAVEMENT,
        PAVEMENT_FUCKED,
        GRAVEL,
        FUCKED_CONCRETE,
        PAINTED_GROUND,
        GRASS_SHORT_LUSH,
        GRASS_MEDIUM_LUSH,
        GRASS_LONG_LUSH,
        GRASS_SHORT_DRY,
        GRASS_MEDIUM_DRY,
        GRASS_LONG_DRY,
        GOLFGRASS_ROUGH,
        GOLFGRASS_SMOOTH,
        STEEP_SLIDYGRASS,
        STEEP_CLIFF,
        FLOWERBED,
        MEADOW,
        WASTEGROUND,
        WOODLANDGROUND,
        VEGETATION,
        MUD_WET,
        MUD_DRY,
        DIRT,
        DIRTTRACK,
        SAND_DEEP,
        SAND_MEDIUM,
        SAND_COMPACT,
        SAND_ARID,
        SAND_MORE,
        SAND_BEACH,
        CONCRETE_BEACH,
        ROCK_DRY,
        ROCK_WET,
        ROCK_CLIFF,
        WATER_RIVERBED,
        WATER_SHALLOW,
        CORNFIELD,
        HEDGE,
        WOOD_CRATES,
        WOOD_SOLID,
        WOOD_THIN,
        GLASS,
        GLASS_WINDOWS_LARGE,
        GLASS_WINDOWS_SMALL,
        EMPTY1,
        EMPTY2,
        GARAGE_DOOR,
        THICK_METAL_PLATE,
        SCAFFOLD_POLE,
        LAMP_POST,
        METAL_GATE,
        METAL_CHAIN_FENCE,
        GIRDER,
        FIRE_HYDRANT,
        CONTAINER,
        NEWS_VENDOR,
        WHEELBASE,
        CARDBOARDBOX,
        PED,
        CAR,
        CAR_PANEL,
        CAR_MOVINGCOMPONENT,
        TRANSPARENT_CLOTH,
        RUBBER,
        PLASTIC,
        TRANSPARENT_STONE,
        WOOD_BENCH,
        CARPET,
        FLOORBOARD,
        STAIRSWOOD,
        P_SAND,
        P_SAND_DENSE,
        P_SAND_ARID,
        P_SAND_COMPACT,
        P_SAND_ROCKY,
        P_SANDBEACH,
        P_GRASS_SHORT,
        P_GRASS_MEADOW,
        P_GRASS_DRY,
        P_WOODLAND,
        P_WOODDENSE,
        P_ROADSIDE,
        P_ROADSIDEDES,
        P_FLOWERBED,
        P_WASTEGROUND,
        P_CONCRETE,
        P_OFFICEDESK,
        P_711SHELF1,
        P_711SHELF2,
        P_711SHELF3,
        P_RESTUARANTTABLE,
        P_BARTABLE,
        P_UNDERWATERLUSH,
        P_UNDERWATERBARREN,
        P_UNDERWATERCORAL,
        P_UNDERWATERDEEP,
        P_RIVERBED,
        P_RUBBLE,
        P_BEDROOMFLOOR,
        P_KIRCHENFLOOR,
        P_LIVINGRMFLOOR,
        P_CORRIDORFLOOR,
        P_711FLOOR,
        P_FASTFOODFLOOR,
        P_SKANKYFLOOR,
        P_MOUNTAIN,
        P_MARSH,
        P_BUSHY,
        P_BUSHYMIX,
        P_BUSHYDRY,
        P_BUSHYMID,
        P_GRASSWEEFLOWERS,
        P_GRASSDRYTALL,
        P_GRASSLUSHTALL,
        P_GRASSGRNMIX,
        P_GRASSBRNMIX,
        P_GRASSLOW,
        P_GRASSROCKY,
        P_GRASSSMALLTREES,
        P_DIRTROCKY,
        P_DIRTWEEDS,
        P_GRASSWEEDS,
        P_RIVEREDGE,
        P_POOLSIDE,
        P_FORESTSTUMPS,
        P_FORESTSTICKS,
        P_FORRESTLEAVES,
        P_DESERTROCKS,
        P_FORRESTDRY,
        P_SPARSEFLOWERS,
        P_BUILDINGSITE,
        P_DOCKLANDS,
        P_INDUSTRIAL,
        P_INDUSTJETTY,
        P_CONCRETELITTER,
        P_ALLEYRUBISH,
        P_JUNKYARDPILES,
        P_JUNKYARDGRND,
        P_DUMP,
        P_CACTUSDENSE,
        P_AIRPORTGRND,
        P_CORNFIELD,
        P_GRASSLIGHT,
        P_GRASSLIGHTER,
        P_GRASSLIGHTER2,
        P_GRASSMID1,
        P_GRASSMID2,
        P_GRASSDARK,
        P_GRASSDARK2,
        P_GRASSDIRTMIX,
        P_RIVERBEDSTONE,
        P_RIVERBEDSHALLOW,
        P_RIVERBEDWEEDS,
        P_SEAWEED,
        DOOR,
        PLASTICBARRIER,
        PARKGRASS,
        STAIRSSTONE,
        STAIRSMETAL,
        STAIRSCARPET,
        FLOORMETAL,
        FLOORCONCRETE,
        BIN_BAG,
        THIN_METAL_SHEET,
        METAL_BARREL,
        PLASTIC_CONE,
        PLASTIC_DUMPSTER,
        METAL_DUMPSTER,
        WOOD_PICKET_FENCE,
        WOOD_SLATTED_FENCE,
        WOOD_RANCH_FENCE,
        UNBREAKABLE_GLASS,
        HAY_BALE,
        GORE,
        RAILTRACK,
    };

    // Enum in a byte
    EColSurface     ( void )                       {}
    EColSurface     ( EColSurfaceValue newValue )  { m_ucValue = newValue; }
    operator uchar  ( void ) const                 { return m_ucValue; }
    uchar   m_ucValue;
};
typedef EColSurface::EColSurfaceValue EColSurfaceValue;
C_ASSERT( EColSurfaceValue::RAILTRACK == 178 );
C_ASSERT( sizeof ( EColSurface ) == 1 );

// Collision stored lighting
struct CColLighting
{
    uchar   day:4;    // 0-15
    uchar   night:4;  // 0-15
};
C_ASSERT( sizeof ( CColLighting ) == 1 );

class CColPoint
{
public:
    virtual                         ~CColPoint                  ( void ) {};

    virtual CColPointSAInterface*   GetInterface                ( void ) = 0;
    virtual const CVector&          GetPosition                 ( void ) = 0;
    virtual void                    SetPosition                 ( const CVector& vecPosition ) = 0;

    virtual const CVector&          GetNormal                   ( void ) = 0;
    virtual void                    SetNormal                   ( const CVector& vecNormal ) = 0;

    virtual EColSurface             GetSurfaceTypeA             ( void ) = 0;
    virtual EColSurface             GetSurfaceTypeB             ( void ) = 0;

    virtual void                    SetSurfaceTypeA             ( EColSurface surfaceType ) = 0;
    virtual void                    SetSurfaceTypeB             ( EColSurface surfaceType ) = 0;

    virtual BYTE                    GetPieceTypeA               ( void ) = 0;
    virtual BYTE                    GetPieceTypeB               ( void ) = 0;

    virtual void                    SetPieceTypeA               ( BYTE ucPieceType ) = 0;
    virtual void                    SetPieceTypeB               ( BYTE ucPieceType ) = 0;

    virtual CColLighting            GetLightingA                ( void ) = 0;
    virtual CColLighting            GetLightingB                ( void ) = 0;

    virtual void                    SetLightingA                ( CColLighting lighting ) = 0;
    virtual void                    SetLightingB                ( CColLighting lighting ) = 0;

    virtual float                   GetDepth                    ( void ) = 0;
    virtual void                    SetDepth                    ( float fDepth ) = 0;

    virtual void                    Destroy                     ( void ) = 0;
    virtual float                   GetLightingForTimeOfDay     ( void ) = 0;
};

#endif
