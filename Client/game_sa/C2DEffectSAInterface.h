/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C2DEffectSAInterface.h
 *  PURPOSE:     Header file for 2dfx game interface layer
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "game/RenderWare.h"
#include "game/CModelInfo.h"
#include "game/C2DEffects.h"
#include "CObjectSA.h"

struct t2dEffectLight
{
    RwColor color;
    float   coronaFarClip;
    float   pointLightRange;
    float   coronaSize;
    float   shadowSize;

    // Flags
    union
    {
        struct
        {
            // Flags 1
            std::uint16_t checkObstacles : 1;
            std::uint16_t fogType : 1;
            std::uint16_t fogType2 : 1;
            std::uint16_t withoutCorona : 1;
            std::uint16_t onlyLongDistance : 1;
            std::uint16_t atDay : 1;
            std::uint16_t atNight : 1;
            std::uint16_t blinking1 : 1;

            // Flags 2
            std::uint16_t onlyFromBelow : 1;
            std::uint16_t blinking2 : 1;
            std::uint16_t updateHeightAboveGround : 1;
            std::uint16_t checkDirection : 1;
            std::uint16_t blinking3 : 1;
        };

        std::uint16_t flags;
    };

    e2dCoronaFlashType coronaFlashType;            // show mode
    bool               coronaEnableReflection;
    std::uint8_t       coronaFlareType;            // lens flare effect 0 - off, 1 - on
    std::uint8_t       shadowColorMultiplier;
    std::int8_t        shadowZDistance;
    std::int8_t        offsetX;
    std::int8_t        offsetY;
    std::int8_t        offsetZ;
    std::uint8_t       field_1E[2];
    RwTexture*         coronaTex;
    RwTexture*         shadowTex;
    std::int32_t       field_28;
    std::int32_t       field_2C;
};

// The particle effect name is an entry in effects.fxp
struct t2dEffectParticle
{
    char szName[24];
};

// It`s used for spawning peds (Like on ticketbooth, Windows of shops, Blackjack-tables)
// It includes information about the External Script ped is going to use when spawned, it`s facing angle and it`s behaviour
struct t2dEffectAttractor
{
    RwV3d            queueDirection;
    RwV3d            useDirection;
    RwV3d            forwardDirection;
    e2dAttractorType attractorType;
    std::uint8_t     pedExistingProbability;
    std::uint8_t     field_26;
    std::uint8_t     flags;
    char             szScriptName[8];
};

// entry-exit markers similar to ipl version
struct t2dEffectEnex
{
    float        enterAngle;            // Rotation angle enter-marker (relative to the object)
    RwV3d        size;                  // The radius of the approximation to the marker
    RwV3d        exitPosn;              // The position of exit-marker (offset relative to enter position)
    float        exitAngle;             // angle of rotation exit-marker (relative to the object)
    std::int16_t interiorId;
    std::uint8_t flags1;            // Unknown flags
    std::uint8_t skyColor;
    char         szInteriorName[8];
    std::uint8_t timeOn;
    std::uint8_t timeOff;

    // Flags 2
    union
    {
        struct
        {
            std::uint8_t unknown1 : 1;
            std::uint8_t unknown2 : 1;
            std::uint8_t timedEffect : 1;
        };
        std::uint8_t flags2;
    };

    std::uint8_t field_2F;
};

struct t2dEffectRoadsign
{
    RwV2d size;
    RwV3d rotation;

    // Flags
    union
    {
        struct
        {
            std::uint8_t numOfLines : 2;
            std::uint8_t symbolsPerLine : 2;
            std::uint8_t textColor : 2;
        };

        std::uint8_t flags;
    };

    std::uint8_t field_16[2];
    char*        text;            // size 64
    RpAtomic*    atomic;
};

// Section defines a place where peds can cover during firefights
struct t2dEffectCoverPoint
{
    RwV2d        direction;
    std::uint8_t type;
    std::uint8_t field_9[3];
};

// Example in vgseesc01.dff
struct t2dEffectEscalator
{
    RwV3d        bottom;
    RwV3d        top;
    RwV3d        end;                  // Z pos, matches top Z if escalator goes up, bottom Z if it goes down
    std::uint8_t direction;            // 0 - down, 1 - up
    std::uint8_t field_25[3];
};

// Example in kb_bandit_u.dff
// Used to determine additional coordinates that can be used in scripts
struct t2dEffectTriggerPoint
{
    std::int32_t id;
};

// Some interiors stuff, probably unused?
struct t2dEffectFurniture
{
    std::uint8_t type;
    std::int8_t  groupId;
    // size
    std::uint8_t width;
    std::uint8_t depth;
    std::uint8_t height;
    // doors
    std::int8_t door;
    std::int8_t l_door[2];            // start, end
    std::int8_t r_door[2];            // start, end
    std::int8_t t_door[2];            // start, end
    // windows
    std::int8_t l_window[2];            // start, end
    std::int8_t r_window[2];            // start, end
    std::int8_t t_window[2];            // start, end
    // something like offsets?
    std::int8_t goLeft[3];              // x,y,z?
    std::int8_t goBottom[3];            // x,y,z?
    std::int8_t goWidth[3];             // x,y,z?
    std::int8_t goDepth[3];             // x,y,z?

    std::uint8_t seed;
    std::uint8_t status;
    float        rotation;
};

union t2dEffectUnion
{
    t2dEffectLight        light;
    t2dEffectParticle     particle;
    t2dEffectAttractor    attractor;
    t2dEffectEnex         enex;
    t2dEffectRoadsign     roadsign;
    t2dEffectCoverPoint   coverPoint;
    t2dEffectEscalator    escalator;
    t2dEffectTriggerPoint triggerPoint;            // slot machine, k.a.a.c gates, basketball hoop
    t2dEffectFurniture    furniture;
};

class C2DEffectSAInterface
{
public:
    RwV3d         position;
    e2dEffectType type;
    std::uint8_t  field_D[3];

    t2dEffectUnion effect;
};

class C2DEffectInfoStoreSAInterface
{
public:
    std::uint32_t        objCount;
    C2DEffectSAInterface objects[100];
};

class C2DEffectPluginDataSAInterface
{
public:
    std::uint32_t        count;
    C2DEffectSAInterface objects[];
};

class CEscalatorSAInterface
{
public:
    RwV3d               startPos;
    RwV3d               bottomPos;
    RwV3d               topPos;
    RwV3d               endPos;
    std::uint8_t        rotation[72];            // CMatrixSAInterface
    bool                exist;
    bool                objectCreated;
    bool                moveDown;
    std::uint8_t        field_7B;            // pad
    std::int32_t        numIntermediatePlanes;
    std::uint32_t       numBottomPlanes;
    std::uint32_t       numTopPlanes;
    std::uint8_t        field_88[8];            // unused field
    RwSphere            bounding;
    float               currentPosition;
    CEntitySAInterface* entity;
    CObjectSAInterface* objects[42];
};
