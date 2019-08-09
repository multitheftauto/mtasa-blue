#pragma once

#include "MatFX.h"
#include "StdInc.h"

gtaRwMatFXBump::gtaRwMatFXBump()
{
	memset(this, 0, sizeof(gtaRwMatFXBump));
}

bool gtaRwMatFXBump::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWrite(stream, &bumpiness, 8))
		return false;
	if(hasTexture)
	{
		if(!texture.StreamWrite(stream))
			return false;
	}
	if(!gtaRwStreamWrite(stream, &hasBumpedTexture, 4))
		return false;
	if(hasBumpedTexture)
	{
		if(!bumpedTexture.StreamWrite(stream))
			return false;
	}
	return true;
}

bool gtaRwMatFXBump::StreamRead(gtaRwStream *stream)
{
	if(gtaRwStreamRead(stream, &bumpiness, 8) != 8)
		return false;
	if(hasTexture)
	{
		if(!texture.StreamRead(stream))
			return false;
	}
	if(gtaRwStreamRead(stream, &hasBumpedTexture, 4) != 4)
		return false;
	if(hasBumpedTexture)
	{
		if(!bumpedTexture.StreamRead(stream))
			return false;
	}
	return true;
}

unsigned int gtaRwMatFXBump::GetStreamSize()
{
	unsigned int size = 12;
	if(hasTexture)
		size += texture.GetStreamSize();
	if(hasBumpedTexture)
		size += bumpedTexture.GetStreamSize();
	return size;
}

gtaRwMatFXEnv::gtaRwMatFXEnv()
{
	memset(this, 0, sizeof(gtaRwMatFXEnv));
}

bool gtaRwMatFXEnv::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWrite(stream, &coefficient, 12))
		return false;
	if(hasTexture)
	{
		if(!texture.StreamWrite(stream))
			return false;
	}
	return true;
}

bool gtaRwMatFXEnv::StreamRead(gtaRwStream *stream)
{
	if(gtaRwStreamRead(stream, &coefficient, 12) != 12)
		return false;
	if(hasTexture)
	{
		if(!texture.StreamRead(stream))
			return false;
	}
	return true;
}

unsigned int gtaRwMatFXEnv::GetStreamSize()
{
	unsigned int size = 12;
	if(hasTexture)
		size += texture.GetStreamSize();
	return size;
}

gtaRwMatFXDual::gtaRwMatFXDual()
{
	memset(this, 0, sizeof(gtaRwMatFXDual));
}

bool gtaRwMatFXDual::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWrite(stream, &srcBlendMode, 12))
		return false;
	if(hasTexture)
	{
		if(!texture.StreamWrite(stream))
			return false;
	}
	return true;
}

bool gtaRwMatFXDual::StreamRead(gtaRwStream *stream)
{
	if(gtaRwStreamRead(stream, &srcBlendMode, 12) != 12)
		return false;
	if(hasTexture)
	{
		if(!texture.StreamRead(stream))
			return false;
	}
	return true;
}

unsigned int gtaRwMatFXDual::GetStreamSize()
{
	unsigned int size = 12;
	if(hasTexture)
		size += texture.GetStreamSize();
	return size;
}

void gtaRwMatFxEffect::SetupNull()
{
	type = rpMATFXEFFECTNULL;
}

void gtaRwMatFxEffect::SetupBump(gtaRwReal Bumpiness, gtaRwBool HasTexture, gtaRwBool HasBumpedTexture)
{
	type = rpMATFXEFFECTBUMPMAP;
	bump.bumpiness = Bumpiness;
	bump.hasTexture = HasTexture;
	bump.hasBumpedTexture = HasBumpedTexture;
}

void gtaRwMatFxEffect::SetupEnv(gtaRwReal Coefficient, gtaRwBool UseFrameBufferAlpha, gtaRwBool HasTexture)
{
	type = rpMATFXEFFECTENVMAP;
	env.coefficient = Coefficient;
	env.hasTexture = HasTexture;
	env.useFrameBufferAlpha = UseFrameBufferAlpha;
}

void gtaRwMatFxEffect::SetupDual(gtaRwUInt32 SrcBlendMode, gtaRwUInt32 DstBlendMode, gtaRwBool HasTexture)
{
	type = rpMATFXEFFECTDUAL;
	dual.srcBlendMode = SrcBlendMode;
	dual.dstBlendMode = DstBlendMode;
	dual.hasTexture = HasTexture;
}

void gtaRwMatFxEffect::SetupUvTransform()
{
	type = rpMATFXEFFECTUVTRANSFORM;
}

bool gtaRwMatFxEffect::StreamWrite(gtaRwStream *stream)
{
	if(!gtaRwStreamWrite(stream, &type, 4))
		return false;
	switch(type)
	{
	case rpMATFXEFFECTBUMPMAP:
		if(!bump.StreamWrite(stream))
			return false;
		break;
	case rpMATFXEFFECTENVMAP:
		if(!env.StreamWrite(stream))
			return false;
		break;
	case rpMATFXEFFECTDUAL:
		if(!dual.StreamWrite(stream))
			return false;
	}
	return true;
}

bool gtaRwMatFxEffect::StreamRead(gtaRwStream *stream)
{
	if(gtaRwStreamRead(stream, &type, 4) != 4)
		return false;
	switch(type)
	{
	case rpMATFXEFFECTBUMPMAP:
		if(!bump.StreamRead(stream))
			return false;
		break;
	case rpMATFXEFFECTENVMAP:
		if(!env.StreamRead(stream))
			return false;
		break;
	case rpMATFXEFFECTDUAL:
		if(!dual.StreamRead(stream))
			return false;
	}
	return true;
}

unsigned int gtaRwMatFxEffect::GetStreamSize()
{
	unsigned int size = 4;
	switch(type)
	{
	case rpMATFXEFFECTBUMPMAP:
		size += bump.GetStreamSize();
		break;
	case rpMATFXEFFECTENVMAP:
		size += env.GetStreamSize();
		break;
	case rpMATFXEFFECTDUAL:
		size += dual.GetStreamSize();
	}
	return size;
}

gtaRwMaterialMatFX::gtaRwMaterialMatFX()
{
	memset(this, 0, sizeof(gtaRwMaterialMatFX));
}

void gtaRwMaterialMatFX::Initialise(RpMatFXMaterialFlags EffectType)
{
	memset(this, 0, sizeof(gtaRwMaterialMatFX));
	enabled = true;
	effectType = EffectType;
	switch(EffectType)
	{
	case rpMATFXEFFECTBUMPMAP:
		effect1.type = rpMATFXEFFECTBUMPMAP;
		effect2.type = rpMATFXEFFECTNULL;
		break;
	case rpMATFXEFFECTENVMAP:
		effect1.type = rpMATFXEFFECTENVMAP;
		effect2.type = rpMATFXEFFECTNULL;
		break;
	case rpMATFXEFFECTBUMPENVMAP:
		effect1.type = rpMATFXEFFECTBUMPMAP;
		effect2.type = rpMATFXEFFECTENVMAP;
		break;
	case rpMATFXEFFECTDUAL:
		effect1.type = rpMATFXEFFECTDUAL;
		effect2.type = rpMATFXEFFECTNULL;
		break;
	case rpMATFXEFFECTUVTRANSFORM:
		effect1.type = rpMATFXEFFECTUVTRANSFORM;
		effect2.type = rpMATFXEFFECTNULL;
		break;
	case rpMATFXEFFECTDUALUVTRANSFORM:
		effect1.type = rpMATFXEFFECTDUAL;
		effect2.type = rpMATFXEFFECTUVTRANSFORM;
		break;
	default:
		effect1.type = rpMATFXEFFECTNULL;
		effect2.type = rpMATFXEFFECTNULL;
	}
}

void gtaRwMaterialMatFX::Destroy()
{
	memset(this, 0, sizeof(gtaRwMaterialMatFX));
}

bool gtaRwMaterialMatFX::StreamWrite(gtaRwStream *stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_MATFX, GetStreamSize() - 12, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &effectType, 4))
			return false;
		if(!effect1.StreamWrite(stream))
			return false;
		if(!effect2.StreamWrite(stream))
			return false;
	}
	return true;
}

bool gtaRwMaterialMatFX::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwMaterialMatFX));
	enabled = true;
	if(gtaRwStreamRead(stream, &effectType, 4) != 4)
		return false;
	if(!effect1.StreamRead(stream))
			return false;
	if(!effect2.StreamRead(stream))
		return false;
	return true;
}

unsigned int gtaRwMaterialMatFX::GetStreamSize()
{
	if(enabled)
		return 16 + effect1.GetStreamSize() + effect2.GetStreamSize();
	return 0;
}

gtaRwAtomicMatFx::gtaRwAtomicMatFx()
{
	memset(this, 0, sizeof(gtaRwAtomicMatFx));
}

gtaRwBool gtaRwAtomicMatFx::StreamWrite(gtaRwStream *Stream)
{
	if(enabled)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(Stream, rwID_MATFX, 4, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(Stream, &matFxEnabled, 4))
			return false;
	}
	return true;
}

gtaRwBool gtaRwAtomicMatFx::StreamRead(gtaRwStream *Stream)
{
	memset(this, 0, sizeof(gtaRwAtomicMatFx));
	enabled = true;
	if(gtaRwStreamRead(Stream, &matFxEnabled, 4) != 4)
		return false;
	return true;
}

gtaRwUInt32 gtaRwAtomicMatFx::GetStreamSize()
{
	if(enabled)
		return 16;
	return 0;
}

void gtaRwAtomicMatFx::Initialise(gtaRwUInt32 MatFxEnabled)
{
	memset(this, 0, sizeof(gtaRwAtomicMatFx));
	enabled = true;
	matFxEnabled = MatFxEnabled;
}

void gtaRwAtomicMatFx::Destroy()
{
	memset(this, 0, sizeof(gtaRwAtomicMatFx));
}