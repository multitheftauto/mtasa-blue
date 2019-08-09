#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

struct gtaRwTextureSkyMipmap : public gtaRwExtension
{
	union{
		gtaRwInt32 klValues;
		struct{
			gtaRwInt32 kValue : 12;
			gtaRwInt32 lValue : 2;
		};
	};

	gtaRwTextureSkyMipmap();

	void SetK(float k);

	void SetL(gtaRwUInt32 l);

	float GetK();

	gtaRwUInt32 GetL();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();

	void Initialise(gtaRwInt32 KValue, gtaRwUInt32 LValue);

	void InitialiseWithUncompressedKL(gtaRwReal KValue, gtaRwUInt32 LValue);

	void Destroy();
};