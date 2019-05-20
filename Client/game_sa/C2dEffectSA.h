#pragma once

#include "CRenderWareSA.h"

enum e2dEffectType {
    EFFECT_LIGHT = 0,
    EFFECT_PARTICLE = 1,
    EFFECT_ATTRACTOR = 3,
    EFFECT_SUN_GLARE,
    EFFECT_FURNITUR,
    EFFECT_ENEX,
    EFFECT_ROADSIGN,
    EFFECT_SLOTMACHINE_WHEEL,
    EFFECT_COVER_POINT,
    EFFECT_ESCALATOR,
};

enum ePedAttractorType {
    PED_ATTRACTOR_ATM = 0, // Ped uses ATM(at day time only)
    PED_ATTRACTOR_SEAT = 1, // Ped sits(at day time only)
    PED_ATTRACTOR_STOP = 2, // Ped stands(at day time only)
    PED_ATTRACTOR_PIZZA = 3, // Ped stands for few seconds
    PED_ATTRACTOR_SHELTER = 4, // Ped goes away after spawning, but stands if weather is rainy
    PED_ATTRACTOR_TRIGGER_SCRIPT = 5, // Launches an external script
    PED_ATTRACTOR_LOOK_AT = 6, // Ped looks at object, then goes away
    PED_ATTRACTOR_SCRIPTED = 7, // This type is not valid
    PED_ATTRACTOR_PARK = 8, // Ped lays(at day time only, ped goes away after 6 PM)
    PED_ATTRACTOR_STEP = 9  // Ped sits on steps
};

struct tEffectLight {
    RwRGBA m_color;
    float m_fCoronaFarClip;
    float m_fPointlightRange;
    float m_fCoronaSize;
    float m_fShadowSize;
    unsigned short m_nFlags;
    unsigned char m_nCoronaFlashType;
    bool m_bCoronaEnableReflection;
    unsigned char m_nCoronaFlareType;
    unsigned char m_nShadowColorMultiplier;
    char m_nShadowZDistance;
    char offsetX;
    char offsetY;
    char offsetZ;
private:
    char _pad2E[2];
public:
    RwTexture * m_pCoronaTex;
    RwTexture *m_pShadowTex;
    int field_38;
    int field_3C;
};

struct tEffectParticle {
    char m_szName[24];
};

struct tEffectPedAttractor {
    RwV3d m_vecQueueDir; // +16
    RwV3d m_vecUseDir;   // +28
    RwV3d m_vecForwardDir; // +40
    unsigned char m_nAttractorType; // +52 see ePedAttractorType
    unsigned char m_nPedExistingProbability; // +53
    unsigned char field_36; // +54 the value might be signed char
    unsigned char m_nFlags; // +55
    char m_szScriptName[8]; // +56
};


struct tEffectEnEx 
{
    float m_fEnterAngle;
    float m_fRadiusX;
    float m_fRadiusY;
    RwV3d m_vecExitPosn;
    float m_fExitAngle;
    short m_nInteriorId;
    unsigned char m_nFlags1;
    unsigned char m_nSkyColor;
    char m_szInteriorName[8];
    unsigned char m_nTimeOn;
    unsigned char m_nTimeOff;
    unsigned char m_nFlags2;
    unsigned char padding;
};

struct tEffectRoadsign {
    RwV2d m_vecSize;
    RwV3d m_vecRotation;
    unsigned short m_nFlags;
private:
    char _pad26[2];
public:
    char *m_pText;
    RpAtomic *m_pAtomic;
};

struct tEffectCoverPoint {
    RwV2d m_vecDirection;
    unsigned char m_nType;
private:
    char _pad19[3];
public:
};

struct tEffectEscalator {
    RwV3d m_vecBottom;
    RwV3d m_vecTop;
    RwV3d m_vecEnd;
    unsigned char m_nDirection;
private:
    char _pad35[3];
public:
};

class C2dEffectSAInterface {
public:
    CVector m_vecPosn;
    unsigned char m_nType; // see e2dEffectType
    char padding[3];
    union {
        tEffectLight light;
        tEffectParticle particle;
        tEffectPedAttractor pedAttractor;
        tEffectEnEx enEx;
        tEffectRoadsign roadsign;
        int iSlotMachineIndex;
        tEffectCoverPoint coverPoint;
        tEffectEscalator escalator;
    };
};

static_assert(sizeof(C2dEffectSAInterface) == 0x40, "Incorrect class size: C2dEffectSAInterface");
