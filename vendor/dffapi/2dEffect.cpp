#include "2dEffect.h"
#include "StdInc.h"

gta2dEffectEntry::gta2dEffectEntry()
{
	memset(this, 0, sizeof(gta2dEffectEntry));
}

void gta2dEffectEntry::SetupLight(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwUInt8 Red, gtaRwUInt8 Green, gtaRwUInt8 Blue, gtaRwReal CoronaFarClip, 
	gtaRwReal PointlightRange, gtaRwReal CoronaSize, gtaRwReal ShadowSize, gtaRwUInt8 CoronaShowMode, gtaRwUInt8 CoronaEnableReflection, 
	gtaRwUInt8 CoronaFlareType, gtaRwUInt8 ShadowColorMultiplier, gtaRwUInt8 Flags1, gtaRwChar *CoronaTexName, gtaRwChar *ShadowTexName,
	gtaRwUInt8 ShadowZDistance, gtaRwUInt8 Flags2)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_LIGHT;
	size = 76;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	light.color.r = Red;
	light.color.g = Green;
	light.color.b = Blue;
	light.color.a = 200;
	light.coronaFarClip = CoronaFarClip;
	light.pointlightRange = PointlightRange;
	light.coronaSize = CoronaSize;
	light.shadowSize = ShadowSize;
	light.coronaShowMode = CoronaShowMode;
	light.coronaEnableReflection = CoronaEnableReflection;
	light.coronaFlareType = CoronaFlareType;
	light.shadowColorMultiplier = ShadowColorMultiplier;
	light.flags1 = Flags1;
	if(CoronaTexName)
		strcpy(light.coronaTexName, CoronaTexName);
	if(ShadowTexName)
		strcpy(light.shadowTexName, ShadowTexName);
	light.shadowZDistance = ShadowZDistance;
	light.flags2 = Flags2;
}

void gta2dEffectEntry::SetupLightWithViewVector(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwUInt8 Red, gtaRwUInt8 Green, gtaRwUInt8 Blue, 
	gtaRwReal CoronaFarClip, gtaRwReal PointlightRange, gtaRwReal CoronaSize, gtaRwReal ShadowSize, gtaRwUInt8 CoronaShowMode, 
	gtaRwUInt8 CoronaEnableReflection, gtaRwUInt8 CoronaFlareType, gtaRwUInt8 ShadowColorMultiplier, gtaRwUInt8 Flags1, 
	gtaRwChar *CoronaTexName, gtaRwChar *ShadowTexName, gtaRwUInt8 ShadowZDistance, gtaRwUInt8 Flags2, gtaRwInt8 ViewX, 
	gtaRwInt8 ViewY, gtaRwInt8 ViewZ)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_LIGHT;
	size = 80;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	light.color.r = Red;
	light.color.g = Green;
	light.color.b = Blue;
	light.color.a = 200;
	light.coronaFarClip = CoronaFarClip;
	light.pointlightRange = PointlightRange;
	light.coronaSize = CoronaSize;
	light.shadowSize = ShadowSize;
	light.coronaShowMode = CoronaShowMode;
	light.coronaEnableReflection = CoronaEnableReflection;
	light.coronaFlareType = CoronaFlareType;
	light.shadowColorMultiplier = ShadowColorMultiplier;
	light.flags1 = Flags1;
	if(CoronaTexName)
		strcpy(light.coronaTexName, CoronaTexName);
	if(ShadowTexName)
		strcpy(light.shadowTexName, ShadowTexName);
	light.shadowZDistance = ShadowZDistance;
	light.flags2 = Flags2;
	light.viewVector[0] = ViewX;
	light.viewVector[1] = ViewY;
	light.viewVector[2] = ViewZ;
}

void gta2dEffectEntry::SetupParticle(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwChar *ParticleName)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_PARTICLE;
	size = 24;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	if(ParticleName)
		strcpy(particle.particleName, ParticleName);
}

void gta2dEffectEntry::SetupPedAttractor(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwUInt8 AttractorType, gtaRwV3d *Right, gtaRwV3d *Top, 
	gtaRwV3d *At, gtaRwChar *ScriptName, gtaRwUInt8 ExistingProbability, gtaRwInt8 Unknown1, gtaRwInt8 Unknown2)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_PEDATTRACTOR;
	size = 56;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	pedAttractor.attractorType = AttractorType;
	if(Right)
		memcpy(&pedAttractor.right, &Right, 12);
	if(Top)
		memcpy(&pedAttractor.top, &Top, 12);
	if(At)
		memcpy(&pedAttractor.at, &At, 12);
	if(ScriptName)
		strcpy(pedAttractor.scriptName, ScriptName);
	ExistingProbability = ExistingProbability;
	pedAttractor.unknown1 = Unknown1;
	pedAttractor.unknown2 = Unknown2;
}

void gta2dEffectEntry::SetupPedAttractor(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwUInt8 AttractorType, gtaRwV3d *Rotation, gtaRwChar *ScriptName, 
	gtaRwUInt8 ExistingProbability, gtaRwInt8 Unknown1, gtaRwInt8 Unknown2)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_PEDATTRACTOR;
	size = 56;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	pedAttractor.attractorType = AttractorType;
	if(Rotation)
		memcpy(&pedAttractor.right, &Rotation, 36);
	if(ScriptName)
		strcpy(pedAttractor.scriptName, ScriptName);
	ExistingProbability = ExistingProbability;
	pedAttractor.unknown1 = Unknown1;
	pedAttractor.unknown2 = Unknown2;
}

void gta2dEffectEntry::SetupSunGlare(gtaRwReal X, gtaRwReal Y, gtaRwReal Z)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_SUNGLARE;
	size = 0;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
}

void gta2dEffectEntry::SetupEnterExitWithTime(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwReal Angle, gtaRwReal RadiusX, gtaRwReal RadiusY, gtaRwReal ExitX,
	gtaRwReal ExitY, gtaRwReal ExitZ, gtaRwReal ExitMarkerAngle, gtaRwUInt16 InteriorId, gtaRwUInt8 Flags1, gtaRwUInt8 SkyColor, 
	gtaRwChar *InteriorName, gtaRwUInt8 TimeOn, gtaRwUInt8 TimeOff, gtaRwUInt8 Flags2)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_ENTEREXIT;
	size = 44;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	enterExit.angle = Angle;
	enterExit.radiusX = RadiusX;
	enterExit.radiusY = RadiusY;
	enterExit.exitPosn.x = ExitX;
	enterExit.exitPosn.y = ExitY;
	enterExit.exitPosn.z = ExitZ;
	enterExit.exitMarkerAngle = ExitMarkerAngle;
	enterExit.interiorId = InteriorId;
	enterExit.flags1 = Flags1;
	enterExit.skyColor = SkyColor;
	if(InteriorName)
		strcpy(enterExit.interiorName, InteriorName);
	enterExit.timeOn = TimeOn;
	enterExit.timeOff = TimeOff;
	enterExit.flags2 = Flags2;
}

void gta2dEffectEntry::SetupEnterExit(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwReal Angle, gtaRwReal RadiusX, gtaRwReal RadiusY, gtaRwReal ExitX,
	gtaRwReal ExitY, gtaRwReal ExitZ, gtaRwReal ExitMarkerAngle, gtaRwUInt16 InteriorId, gtaRwUInt8 Flags1, gtaRwUInt8 SkyColor, 
	gtaRwChar *InteriorName)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_ENTEREXIT;
	size = 40;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	enterExit.angle = Angle;
	enterExit.radiusX = RadiusX;
	enterExit.radiusY = RadiusY;
	enterExit.exitPosn.x = ExitX;
	enterExit.exitPosn.y = ExitY;
	enterExit.exitPosn.z = ExitZ;
	enterExit.exitMarkerAngle = ExitMarkerAngle;
	enterExit.interiorId = InteriorId;
	enterExit.flags1 = Flags1;
	enterExit.skyColor = SkyColor;
	if(InteriorName)
		strcpy(enterExit.interiorName, InteriorName);
}

void gta2dEffectEntry::SetupRoadsign(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwReal ScaleX, gtaRwReal ScaleY, gtaRwReal RotationX, gtaRwReal RotationY, 
	gtaRwReal RotationZ, gtaRwUInt8 LinesNumber, gtaRwUInt8 SymbolsInLine, gtaRwUInt8 TextColor, gtaRwChar *Line1, gtaRwChar *Line2,
	gtaRwChar *Line3, gtaRwChar *Line4)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_ROADSIGN;
	size = 88;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	roadsign.scale.x = ScaleX;
	roadsign.scale.y = ScaleY;
	roadsign.rotation.x = RotationX;
	roadsign.rotation.y = RotationY;
	roadsign.rotation.z = RotationZ;
	roadsign.linesNumber = LinesNumber;
	roadsign.symbolsInLine = SymbolsInLine;
	roadsign.textColor = TextColor;
	if(Line1)
		strcpy(roadsign.textLines[0], Line1);
	if(Line2)
		strcpy(roadsign.textLines[0], Line2);
	if(Line3)
		strcpy(roadsign.textLines[0], Line3);
	if(Line4)
		strcpy(roadsign.textLines[0], Line4);
}

void gta2dEffectEntry::SetupSlotmachineWheel(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwInt32 WheelModelIndex)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_SLOTMACHINEWHEEL;
	size = 4;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	slotmachineWheel.wheelModelIndex = WheelModelIndex;
}

void gta2dEffectEntry::SetupCoverPoint(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwReal DirectionX, gtaRwReal DirectionY, gtaRwUInt32 CoverType)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_COVERPOINT;
	size = 12;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	coverPoint.direction.x = DirectionX;
	coverPoint.direction.y = DirectionY;
	coverPoint.coverType = CoverType;
}

void gta2dEffectEntry::SetupEscalator(gtaRwReal X, gtaRwReal Y, gtaRwReal Z, gtaRwV3d *Bottom, gtaRwV3d *Top, gtaRwV3d *End, gtaRwUInt32 Direction)
{
	memset(this, 0, sizeof(gta2dEffectEntry));
	type = EFFECT_ESCALATOR;
	size = 40;
	offset.x = X;
	offset.y = Y;
	offset.z = Z;
	if(Bottom)
		memcpy(&escalator.bottom, Bottom, 12);
	if(Top)
		memcpy(&escalator.top, Top, 12);
	if(End)
		memcpy(&escalator.end, End, 12);
	escalator.direction = Direction;
}

void gta2dEffectEntry::Destroy()
{
	memset(this, 0, sizeof(gta2dEffectEntry));
}

gtaGeometry2dEffect::gtaGeometry2dEffect()
{
	memset(this, 0, sizeof(gtaGeometry2dEffect));
}

gtaRwBool gtaGeometry2dEffect::StreamWrite(gtaRwStream *Stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, gtaID_2DEFFECT, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(Stream, &effectCount, 4))
			return false;
		for(gtaRwInt32 i = 0; i < effectCount; i++)
		{
			if(!gtaRwStreamWrite(Stream, &effects[i], 20))
				return false;
			if(effects[i].size > 0)
			{
				if(!gtaRwStreamWrite(Stream, &effects[i].light, effects[i].size))
					return false;
			}
		}
	}
	return true;
}

gtaRwBool gtaGeometry2dEffect::StreamRead(gtaRwStream *Stream)
{
	memset(this, 0, sizeof(gtaGeometry2dEffect));
	enabled = true;
	if(gtaRwStreamRead(Stream, &effectCount, 4) != 4)
		return false;
	if(effectCount > 0)
	{
		effects = (gta2dEffectEntry *)malloc(sizeof(gta2dEffectEntry) * effectCount);
		memset(effects, 0, sizeof(gta2dEffectEntry) * effectCount);
		for(gtaRwInt32 i = 0; i < effectCount; i++)
		{
			if(gtaRwStreamRead(Stream, &effects[i], 20) != 20)
			{
				Destroy();
				return false;
			}
			if(effects[i].size > 0)
			{
				if(gtaRwStreamRead(Stream, &effects[i].light, effects[i].size) != effects[i].size)
				{
					Destroy();
					return false;
				}
			}
		}
	}
	return true;
}

gtaRwUInt32 gtaGeometry2dEffect::GetStreamSize()
{
	if(enabled)
	{
		gtaRwUInt32 size = 16;
		for(gtaRwInt32 i = 0; i < effectCount; i++)
			size += 20 + effects[i].size;
		return size;
	}
	return 0;
}

void gtaGeometry2dEffect::Initialise(gtaRwUInt32 EffectCount)
{
	memset(this, 0, sizeof(gtaGeometry2dEffect));
	enabled = true;
	effectCount = EffectCount;
	if(effectCount > 0)
	{
		effects = (gta2dEffectEntry *)malloc(sizeof(gta2dEffectEntry) * EffectCount);
		memset(effects, 0, sizeof(gta2dEffectEntry) * EffectCount);
	}
}

void gtaGeometry2dEffect::Destroy()
{
	if(effects)
	{
		for(gtaRwInt32 i = 0; i < effectCount; i++)
			effects[i].Destroy();
		free(effects);
	}
	memset(this, 0, sizeof(gtaGeometry2dEffect));
}