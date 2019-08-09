#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

enum gta2dEffectType
{
	EFFECT_LIGHT,
	EFFECT_PARTICLE,
	EFFECT_PEDATTRACTOR = 3,
	EFFECT_SUNGLARE,
	EFFECT_ENTEREXIT = 6,
	EFFECT_ROADSIGN,
	EFFECT_SLOTMACHINEWHEEL,
	EFFECT_COVERPOINT,
	EFFECT_ESCALATOR
};

enum gta2dEffectLightFlags1
{
	LIGHT1_CORONA_CHECK_OBSTACLES = 0x1,
	LIGHT1_WITHOUT_CORONA = 0x8,
	LIGHT1_CORONA_ONLY_AT_LONG_DISTANCE = 0x10,
	LIGHT1_AT_DAY = 0x20,
	LIGHT1_AT_NIGHT = 0x40,
	LIGHT1_BLINKING1 = 0x80,
};

enum gta2dEffectLightFlags2
{
	LIGHT2_CORONA_ONLY_FROM_BELOW = 0x1,
	LIGHT2_BLINKING2 = 0x2,
	LIGHT2_UDPDATE_HEIGHT_ABOVE_GROUND = 0x4,
	LIGHT2_CHECK_VIEW_VECTOR = 0x8,
	LIGHT2_BLINKING3 = 0x10
};

enum gta2dEffectCoronaShowMode
{
	CORONA_DEFAULT = 0,
	CORONA_ALWAYS_AT_WET_WEATHER = 2,
	CORONA_TRAFFICLIGHT = 7,
	CORONA_TRAINCROSSLIGHT = 8,
	CORONA_AT_RAIN_ONLY = 10
};

enum gta2dEffectPedAttractorType
{
	PED_ATM_ATTRACTOR,
	PED_SEAT_ATTRACTOR,
	PED_STOP_ATTRACTOR,
	PED_PIZZA_ATTRACTOR,
	PED_SHELTER_ATTRACTOR,
	PED_TRIGGER_SCRIPT_ATTRACTOR,
	PED_LOOK_AT_ATTRACTOR,
	PED_SCRIPTED_ATTRACTOR,
	PED_PARK_ATTRACTOR,
	PED_STEP_ATTRACTOR
};

enum gta2dEffectEnterExitFlags1
{
	ENEX1_UNKNOWN_INTERIOR = 0x1,
	ENEX1_UNKNOWN_PAIRING = 0x2,
	ENEX1_CREATE_LINKED_PAIRING = 0x4,
	ENEX1_REWARD_INTERIOR = 0x8,
	ENEX1_USED_REWARD_ENTRANCE = 0x10,
	ENEX1_CARS_AND_AIRCRAFT = 0x20,
	ENEX1_BIKES_AND_MOTORCYCLES = 0x40,
	ENEX1_DISABLE_ON_FOOT = 0x80
};

enum gta2dEffectEnterExitFlags2
{
	ENEX2_ACCEPT_NPC_GROUP = 0x1,
	ENEX2_FOOD_DATE_FLAG = 0x2,
	ENEX2_UNKNOWN_BURGLARY = 0x4,
	ENEX2_DISABLE_EXIT = 0x8,
	ENEX2_BURGLARY_ACCESS = 0x10,
	ENEX2_ENTERED_WITHOUT_EXIT = 0x20,
	ENEX2_ENABLE_ACCESS = 0x40,
	ENEX2_DELETE_ENEX = 0x80
};

enum gta2dEffectRoadsignLinesNumber
{
	ROADSIGN_4_LINES,
	ROADSIGN_1_LINE,
	ROADSIGN_2_LINES,
	ROADSIGN_3_LINES
};

enum gta2dEffectRoadsignSymbolsInLine
{
	ROADSIGN_16_SYMBOLS,
	ROADSIGN_2_SYMBOLS,
	ROADSIGN_4_SYMBOLS,
	ROADSIGN_8_SYMBOLS
};

enum gta2dEffectRoadsignTextColor
{
	ROADSIGN_COLOR_WHITE,
	ROADSIGN_COLOR_BLACK,
	ROADSIGN_COLOR_GREY,
	ROADSIGN_COLOR_RED
};

enum gta2dEffectEscalatorDirection
{
	ESCALATOR_DIRECTION_DOWN,
	ESCALATOR_DIRECTION_UP
};

#pragma pack(push, 1)
struct gta2dEffectEntry
{
	gtaRwV3d offset;
	gtaRwUInt32 type;
	gtaRwUInt32 size;

	union{
		struct{
			gtaRwRGBA color;
			gtaRwReal coronaFarClip;
			gtaRwReal pointlightRange;
			gtaRwReal coronaSize;
			gtaRwReal shadowSize;
			gtaRwUInt8 coronaShowMode;
			gtaRwUInt8 coronaEnableReflection;
			gtaRwUInt8 coronaFlareType;
			gtaRwUInt8 shadowColorMultiplier;
			union{
				gtaRwUInt8 flags1;
				struct{
					gtaRwUInt8 coronaCheckObstacles : 1;
					gtaRwUInt8 fogType : 2;
					gtaRwUInt8 withoutCorona : 1;
					gtaRwUInt8 onlyAtLongDistance : 1;
					gtaRwUInt8 atDay : 1;
					gtaRwUInt8 atNight : 1;
					gtaRwUInt8 blinking1 : 1;
				};
			};
			gtaRwChar coronaTexName[24];
			gtaRwChar shadowTexName[24];
			gtaRwUInt8 shadowZDistance;
			union{
				gtaRwUInt8 flags2;
				struct{
					gtaRwUInt8 coronaOnlyFromBelow : 1;
					gtaRwUInt8 blinking2 : 1;
					gtaRwUInt8 updateHeightAboveGround : 1;
					gtaRwUInt8 checkViewVector : 1;
					gtaRwUInt8 blinking3 : 1;
				};
			};
			gtaRwInt8 viewVector[3];
			gtaRwInt8 padding[2];
		} light;

		struct{
			gtaRwChar particleName[24];
		} particle;

		struct{
			gtaRwUInt8 attractorType;
			gtaRwInt8 padding[3];
			gtaRwV3d right;
			gtaRwV3d top;
			gtaRwV3d at;
			gtaRwChar scriptName[8];
			gtaRwUInt8 existingProbability;
			gtaRwInt8 padding2[3];
			gtaRwInt8 unknown1;
			gtaRwInt8 padding3;
			gtaRwInt8 unknown2;
			gtaRwInt8 padding4;
		} pedAttractor;

		struct{
		} sunGlare;

		struct{
			gtaRwReal angle;
			gtaRwReal radiusX;
			gtaRwReal radiusY;
			gtaRwV3d exitPosn;
			gtaRwReal exitMarkerAngle;
			gtaRwUInt16 interiorId;
			union{
				gtaRwUInt8 flags1;
				struct{
					gtaRwUInt8 unknownInterior : 1;
					gtaRwUInt8 unknownPairing : 1;
					gtaRwUInt8 createLinkedPairing : 1;
					gtaRwUInt8 rewardInterior : 1;
					gtaRwUInt8 usedRewardIntrance : 1;
					gtaRwUInt8 carsAndAircraft : 1;
					gtaRwUInt8 bikesAndMotorcycles : 1;
					gtaRwUInt8 disableOnFoot : 1;
				};
			};
			gtaRwUInt8 skyColor;
			gtaRwChar interiorName[8];
			gtaRwUInt8 timeOn;
			gtaRwUInt8 timeOff;
			union{
				gtaRwUInt8 flags2;
				struct{
					gtaRwUInt8 acceptNpcGroup : 1;
					gtaRwUInt8 foodDateFlag : 1;
					gtaRwUInt8 unknownBurglary : 1;
					gtaRwUInt8 disableExit : 1;
					gtaRwUInt8 burglaryAccess : 1;
					gtaRwUInt8 enteredWithoutExit : 1;
					gtaRwUInt8 enableAccess : 1;
					gtaRwUInt8 deleteEnex : 1;
				};
			};
			gtaRwUInt8 padding;
		} enterExit;

		struct{
			gtaRwV2d scale;
			gtaRwV3d rotation;
			gtaRwUInt16 linesNumber : 2;
			gtaRwUInt16 symbolsInLine : 2;
			gtaRwUInt16 textColor : 2;
			gtaRwChar textLines[4][16];
/* FIXED 03.07.14 */
			gtaRwInt8 pad[2];
/* FIXED 03.07.14 */
		} roadsign;

		struct{
			gtaRwInt32 wheelModelIndex;
		} slotmachineWheel;

		struct{
			gtaRwV2d direction;
			gtaRwUInt32 coverType;
		} coverPoint;

		struct{
			gtaRwV3d bottom;
			gtaRwV3d top;
			gtaRwV3d end;
			gtaRwUInt32 direction;
		} escalator;
	};

	gta2dEffectEntry();

	void SetupLight(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwUInt8 Red, gtaRwUInt8 Green, gtaRwUInt8 Blue, gtaRwReal CoronaFarClip, 
		gtaRwReal PointlightRange, gtaRwReal CoronaSize, gtaRwReal ShadowSize, gtaRwUInt8 CoronaShowMode, gtaRwUInt8 CoronaEnableReflection, 
		gtaRwUInt8 CoronaFlareType, gtaRwUInt8 ShadowColorMultiplier, gtaRwUInt8 Flags1, gtaRwChar *CoronaTexName, gtaRwChar *ShadowTexName,
		gtaRwUInt8 ShadowZDistance, gtaRwUInt8 Flags2);

	void SetupLightWithViewVector(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwUInt8 Red, gtaRwUInt8 Green, gtaRwUInt8 Blue, 
		gtaRwReal CoronaFarClip, gtaRwReal PointlightRange, gtaRwReal CoronaSize, gtaRwReal ShadowSize, gtaRwUInt8 CoronaShowMode, 
		gtaRwUInt8 CoronaEnableReflection, gtaRwUInt8 CoronaFlareType, gtaRwUInt8 ShadowColorMultiplier, gtaRwUInt8 Flags1, 
		gtaRwChar *CoronaTexName, gtaRwChar *ShadowTexName, gtaRwUInt8 ShadowZDistance, gtaRwUInt8 Flags2, gtaRwInt8 ViewX, 
		gtaRwInt8 ViewY, gtaRwInt8 ViewZ);

	void SetupParticle(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwChar *ParticleName);

	void SetupPedAttractor(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwUInt8 AttractorType, gtaRwV3d *Right, gtaRwV3d *Top, 
		gtaRwV3d *At, gtaRwChar *ScriptName, gtaRwUInt8 ExistingProbability, gtaRwInt8 Unknown1, gtaRwInt8 Unknown2);

	void SetupPedAttractor(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwUInt8 AttractorType, gtaRwV3d *Rotation, gtaRwChar *ScriptName, 
		gtaRwUInt8 ExistingProbability, gtaRwInt8 Unknown1, gtaRwInt8 Unknown2);

	void SetupSunGlare(gtaRwReal X, gtaRwReal Y, gtaRwReal Z);

	void SetupEnterExitWithTime(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwReal Angle, gtaRwReal RadiusX, gtaRwReal RadiusY, gtaRwReal ExitX,
		gtaRwReal ExitY, gtaRwReal ExitZ, gtaRwReal ExitMarkerAngle, gtaRwUInt16 InteriorId, gtaRwUInt8 Flags1, gtaRwUInt8 SkyColor, 
		gtaRwChar *InteriorName, gtaRwUInt8 TimeOn, gtaRwUInt8 TimeOff, gtaRwUInt8 Flags2);

	void SetupEnterExit(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwReal Angle, gtaRwReal RadiusX, gtaRwReal RadiusY, gtaRwReal ExitX,
		gtaRwReal ExitY, gtaRwReal ExitZ, gtaRwReal ExitMarkerAngle, gtaRwUInt16 InteriorId, gtaRwUInt8 Flags1, gtaRwUInt8 SkyColor, 
		gtaRwChar *InteriorName);

	void SetupRoadsign(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwReal ScaleX, gtaRwReal ScaleY, gtaRwReal RotationX, gtaRwReal RotationY, 
		gtaRwReal RotationZ, gtaRwUInt8 LinesNumber, gtaRwUInt8 SymbolsInLine, gtaRwUInt8 TextColor, gtaRwChar *Line1, gtaRwChar *Line2,
		gtaRwChar *Line3, gtaRwChar *Line4);

	void SetupSlotmachineWheel(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwInt32 WheelModelIndex);

	void SetupCoverPoint(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwReal DirectionX, gtaRwReal DirectionY, gtaRwUInt32 CoverType);

	void SetupEscalator(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwV3d *Bottom, gtaRwV3d *Top, gtaRwV3d *End, gtaRwUInt32 Direction);

	void Destroy();
};

struct gtaGeometry2dEffect : public gtaRwExtension
{
	gtaRwUInt32 effectCount;
	gta2dEffectEntry *effects;

	gtaGeometry2dEffect();

	gtaRwBool StreamWrite(gtaRwStream *Stream);

	gtaRwBool StreamRead(gtaRwStream *Stream);
	
	gtaRwUInt32 GetStreamSize();

	void Initialise(gtaRwUInt32 EffectCount);

	void Destroy();
};