#pragma once

#include "texture_helper.h"
#include <stdio.h>

#define D3DFMT_ATI1 MAKEFOURCC('A', 'T', 'I', '1')
#define D3DFMT_ATI2 MAKEFOURCC('A', 'T', 'I', '2')

char unknownformat[32];
 
const char* GetD3DFormatName(int nD3DFormat)
{
    switch(nD3DFormat)
    {
        D3DVal(UNKNOWN0,0)
 
        D3DVal(R8G8B8,20)
        D3DVal(A8R8G8B8,21)
        D3DVal(X8R8G8B8,22)
        D3DVal(R5G6B5,23)
        D3DVal(X1R5G5B5,24)
        D3DVal(A1R5G5B5,25)
        D3DVal(A4R4G4B4,26)
        D3DVal(R3G3B2,27)
        D3DVal(A8,28)
        D3DVal(A8R3G3B2,29)
        D3DVal(X4R4G4B4,30)
        D3DVal(A2B10G10R10,31)
        D3DVal(A8B8G8R8,32)
        D3DVal(X8B8G8R8,33)
        D3DVal(G16R16,34)
        D3DVal(A2R10G10B10,35)
        D3DVal(A16B16G16R16,36)
 
        D3DVal(A8P8,40)
        D3DVal(P8,41)
 
        D3DVal(L8,50)
        D3DVal(A8L8,51)
        D3DVal(A4L4,52)
 
        D3DVal(V8U8,60)
        D3DVal(L6V5U5,61)
        D3DVal(X8L8V8U8,62)
        D3DVal(Q8W8V8U8,63)
        D3DVal(V16U16,64)
        D3DVal(A2W10V10U10,67)
 
        D3DVal(UYVY,MAKEFOURCC('U', 'Y', 'V', 'Y'))
        D3DVal(R8G8_B8G8,MAKEFOURCC('R', 'G', 'B', 'G'))
        D3DVal(YUY2,MAKEFOURCC('Y', 'U', 'Y', '2'))
        D3DVal(G8R8_G8B8,MAKEFOURCC('G', 'R', 'G', 'B'))
        D3DVal(DXT1,MAKEFOURCC('D', 'X', 'T', '1'))
        D3DVal(DXT2,MAKEFOURCC('D', 'X', 'T', '2'))
        D3DVal(DXT3,MAKEFOURCC('D', 'X', 'T', '3'))
        D3DVal(DXT4,MAKEFOURCC('D', 'X', 'T', '4'))
        D3DVal(DXT5,MAKEFOURCC('D', 'X', 'T', '5'))
 
        D3DVal(D16_LOCKABLE,70)
        D3DVal(D32,71)
        D3DVal(D15S1,73)
        D3DVal(D24S8,75)
        D3DVal(D24X8,77)
        D3DVal(D24X4S4,79)
        D3DVal(D16,80)
 
        D3DVal(D32F_LOCKABLE,82)
        D3DVal(D24FS8,83)
 
        D3DVal(L16,81)
 
        D3DVal(VERTEXDATA,100)
        D3DVal(INDEX16,101)
        D3DVal(INDEX32,102)
 
        D3DVal(Q16W16V16U16,110)
 
        D3DVal(MULTI2_ARGB8,MAKEFOURCC('M','E','T','1'))
 
        D3DVal(R16F,111)
        D3DVal(G16R16F,112)
        D3DVal(A16B16G16R16F,113)
 
        D3DVal(R32F,114)
        D3DVal(G32R32F,115)
        D3DVal(A32B32G32R32F,116)
 
        D3DVal(CxV8U8,117)
    }
	sprintf(unknownformat, "UNKNOWN%d", nD3DFormat);
	return unknownformat;
}

const char *GetRasterFormat(int format)
{
	int f = format & 0xF;
	switch(f)
	{
		D3DVal(FORMATDEFAULT,0)
    	D3DVal(FORMAT1555,1)
    	D3DVal(FORMAT565,2)
    	D3DVal(FORMAT4444,3)
    	D3DVal(FORMATLUM8,4)
    	D3DVal(FORMAT8888,5)
    	D3DVal(FORMAT888,6)
    	D3DVal(FORMAT16,7)
    	D3DVal(FORMAT24,8)
    	D3DVal(FORMAT32,9)
    	D3DVal(FORMAT555,10)
	}
	return "INCORRECT_RASTER_FORMAT";
}

D3DFORMAT GetTextureFormat(gtaRwTextureNative *texture)
{
	D3DFORMAT fmt = D3DFMT_UNKNOWN;
	switch(texture->platformId)
	{
	case 8:
		switch(texture->compression)
		{
		case 1:
			fmt = D3DFMT_DXT1;
			break;
		case 2:
			fmt = D3DFMT_DXT2;
			break;
		case 3:
			fmt = D3DFMT_DXT3;
			break;
		case 4:
			fmt = D3DFMT_DXT4;
			break;
		case 5:
			fmt = D3DFMT_DXT5;
			break;
		default:
			switch(texture->rasterFormat & rwRASTERFORMATPIXELFORMATMASK)
			{
			case rwRASTERFORMAT8888:
				fmt = D3DFMT_A8R8G8B8;
				break;
			case rwRASTERFORMAT1555:
				fmt = D3DFMT_A1R5G5B5;
				break;
			case rwRASTERFORMAT565:
				fmt = D3DFMT_R5G6B5;
				break;
			case rwRASTERFORMAT4444:
				fmt = D3DFMT_A4R4G4B4;
				break;
			case rwRASTERFORMATLUM8:
				fmt = D3DFMT_L8;
				break;
			case rwRASTERFORMAT888:
				fmt = D3DFMT_X8R8G8B8;
				break;
			case rwRASTERFORMAT555:
				fmt = D3DFMT_X1R5G5B5;
				break;
			}
		}
		break;
	case 9:
		fmt = (D3DFORMAT)texture->d3dFormat;
		break;
	}
	return fmt;
}

unsigned char GetFormatDepth(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_A1:
		return 1;
	case D3DFMT_A4L4:
	case D3DFMT_A8:
	case D3DFMT_R3G3B2:
	case D3DFMT_P8:
	case D3DFMT_L8:
		return 8;
	case D3DFMT_A8L8:
	case D3DFMT_A8P8:
	case D3DFMT_R16F:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_L16:
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
	case D3DFMT_ATI1:
	case D3DFMT_ATI2:
		return 16;
	case D3DFMT_R8G8B8:
		return 24;
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_R32F:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_X8L8V8U8:
	case D3DFMT_V16U16:
	case D3DFMT_Q8W8V8U8:
	case D3DFMT_G16R16:
	case D3DFMT_G16R16F:
		return 32;
	case D3DFMT_A16B16G16R16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_G32R32F:
	case D3DFMT_Q16W16V16U16:
		return 64;
	case D3DFMT_A32B32G32R32F:
		return 128;
	default:
		return 0;
	}
};

bool GetFormatHasAlpha(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_A1:
	case D3DFMT_A4L4:
	case D3DFMT_A8:
	case D3DFMT_A8L8:
	case D3DFMT_A8P8:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_A16B16G16R16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_A32B32G32R32F:
		return true;
	default:
		return false;
	}
};

unsigned int GetFormatRwFormat(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
		return rwRASTERFORMAT8888;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_R8G8B8:
		return rwRASTERFORMAT888;
	case D3DFMT_R5G6B5:
	case D3DFMT_DXT1:
		return rwRASTERFORMAT565;
	case D3DFMT_X1R5G5B5:
		return rwRASTERFORMAT555;
	case D3DFMT_A1R5G5B5:
		return rwRASTERFORMAT1555;
	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
	case D3DFMT_ATI1:
	case D3DFMT_ATI2:
		return rwRASTERFORMAT4444;
	case D3DFMT_P8:
		return rwRASTERFORMATPAL8;
	case D3DFMT_L8:
		return rwRASTERFORMATLUM8;
	case D3DFMT_D16:
	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_D15S1:
		return rwRASTERFORMAT16;
	case D3DFMT_D32:
	case D3DFMT_D32_LOCKABLE:
	case D3DFMT_D24S8:
	case D3DFMT_D24X8:
	case D3DFMT_D24X4S4:
	case D3DFMT_D32F_LOCKABLE:
		return rwRASTERFORMAT32;
	default:
		return 0;
	}
};

bool GetFormatIsCompressed(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
	case D3DFMT_ATI1:
	case D3DFMT_ATI2:
		return true;
	default:
		return false;
	}
};

unsigned char GetFormatCompression(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_DXT1:
		return 1;
	case D3DFMT_DXT2:
		return 2;
	case D3DFMT_DXT3:
		return 3;
	case D3DFMT_DXT4:
		return 4;
	case D3DFMT_DXT5:
		return 5;
	default:
		return 0;
	}
};

// we will stick to this one instead of DXT1HasAlphaPixels() currently.
bool DXT1HasAlphaFormat(_dxt1block *blocks, unsigned int numBlocks)
{
	for(int i = 0; i < numBlocks; i++)
	{
		// pixel-block uses alpha format (1555) if color_0 is less than color_1
		if(blocks[i].color_0 <= blocks[i].color_1)
			return true;
	}
	return false;
}

bool DXT1HasAlphaPixels(_dxt1block *blocks, unsigned int numBlocks)
{
	for(int i = 0; i < numBlocks; i++)
	{
		// pixel-block uses alpha format (1555) if color_0 is less than color_1
		if(blocks[i].color_0 <= blocks[i].color_1)
		{
			// check if any pixel is transparent
			for(int j = 0; j < 16; j++)
			{
				if(((blocks[i].indices >> (j * 2)) & 3) == 3)
					return true;
			}
		}
	}
	return false;
}