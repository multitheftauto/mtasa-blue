#pragma once
struct gtaRwStream;
#include "Stream.h"
#include "RwTypes.h"

struct gtaRwString
{
	gtaRwChar *string;
	gtaRwBool isUnicode;

	gtaRwString();

	void Initialise(gtaRwChar *String, gtaRwBool IsUnicode = false);

	void Destroy();

	bool StreamRead(gtaRwStream *stream);

	bool StreamWrite(gtaRwStream *stream);

	unsigned int GetStreamSize();
};