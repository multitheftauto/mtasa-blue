#pragma once

#include "SkyMipmap.h"
#include "StdInc.h"

gtaRwTextureSkyMipmap::gtaRwTextureSkyMipmap()
{
	memset(this, 0, sizeof(gtaRwTextureSkyMipmap));
}

void gtaRwTextureSkyMipmap::SetK(float k)
{
	kValue = (gtaRwInt32)(k * 16.0f);
	if(kValue >= -2048)
	{
		if(kValue > 2047)
			kValue = 2047;
	}
	else
		kValue = -2048;
}

void gtaRwTextureSkyMipmap::SetL(gtaRwUInt32 l)
{
	if(l > 3)
		l = 3;
	lValue = l;
}

float gtaRwTextureSkyMipmap::GetK()
{
	if(kValue < 0)
		return 0.0f;
	else
		return (float)kValue * 0.0625f;
}

gtaRwUInt32 gtaRwTextureSkyMipmap::GetL()
{
	return lValue;
}

bool gtaRwTextureSkyMipmap::StreamWrite(gtaRwStream *stream)
{
	if(enabled && gtaPlatform == PLATFORM_PS2)
	{
		if(!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_SKYMIPMAP, 4, gtaRwVersion, gtaRwBuild))
			return false;
		if(!gtaRwStreamWrite(stream, &klValues, 4))
			return false;
	}
	return true;
}

bool gtaRwTextureSkyMipmap::StreamRead(gtaRwStream *stream)
{
	memset(this, 0, sizeof(gtaRwTextureSkyMipmap));
	enabled = true;
	if(gtaRwStreamRead(stream, &klValues, 4) != 4)
		return false;
	return true;
}

unsigned int gtaRwTextureSkyMipmap::GetStreamSize()
{
	if(enabled && gtaPlatform == PLATFORM_PS2)
		return 16;
	return 0;
}

void gtaRwTextureSkyMipmap::Initialise(gtaRwInt32 KValue, gtaRwUInt32 LValue)
{
	memset(this, 0, sizeof(gtaRwTextureSkyMipmap));
	enabled = true;
	kValue = KValue;
	lValue = LValue;
}

void gtaRwTextureSkyMipmap::InitialiseWithUncompressedKL(gtaRwReal KValue, gtaRwUInt32 LValue)
{
	memset(this, 0, sizeof(gtaRwTextureSkyMipmap));
	enabled = true;
	SetK(KValue);
	SetL(LValue);
}

void gtaRwTextureSkyMipmap::Destroy()
{
	memset(this, 0, sizeof(gtaRwTextureSkyMipmap));
}