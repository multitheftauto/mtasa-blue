#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"
#include "StdInc.h"

enum gtaNativeOGlVertexElementType
{
	OGL_VERTEX_POSITION = 0,
	OGL_VERTEX_TEXCOORD0 = 1,
	OGL_VERTEX_NORMAL = 2,
	OGL_VERTEX_GLOBALCOLOR = 3,
	OGL_VERTEX_BLENDWEIGHT = 4,
	OGL_VERTEX_BLENDINDICES = 5,
	OGL_VERTEX_COLOR2 = 6
};

enum gtaNativeOGlVertexElementDataFormat
{
	OGL_VERTEX_DATA_FLOAT3 = 0,
	OGL_VERTEX_DATA_BYTE3 = 1, // ??
	OGL_VERTEX_DATA_BYTE4 = 2,
	OGL_VERTEX_DATA_WORD2 = 3
};

enum gtaNativePS2VIFCodeCommand
{
	PS2_CMD_NOP,
	PS2_CMD_STCYCL,
	PS2_CMD_OFFSET,
	PS2_CMD_BASE,
	PS2_CMD_ITOP,
	PS2_CMD_STMOD,
	PS2_CMD_MSKPATH3,
	PS2_CMD_MARK,
	PS2_CMD_FLUSHE = 0x10,
	PS2_CMD_FLUSH,
	PS2_CMD_FLUSHA = 0x13,
	PS2_CMD_MSCAL,
	PS2_CMD_MSCALF,
	PS2_CMD_MSCNT = 0x17
};

struct gtaNativeOGlVertexElement
{
	gtaRwUInt32 paramType; // gtaNativeOGlVertexElementType
	gtaRwUInt32 dataType; // gtaNativeOGlVertexElementDataFormat
	gtaRwUInt32 unknown; // 1 for color and boneweight
	gtaRwUInt32 size; // num of elements in this element
	gtaRwUInt32 stride; // total size of vertex in bytes
	gtaRwUInt32 offset; // offset to this element in vertex
};

struct gtaNativeOGlCompressedNormal
{
	gtaRwInt8 x;
	gtaRwInt8 y;
	gtaRwInt8 z;
	gtaRwInt8 unused;
};

struct gtaNativeOGlCompressedTexCoords
{
	gtaRwInt16 u;
	gtaRwInt16 v;
};

union gtaNativePS2DMATag
{
	gtaRwUInt16 qwc;
	gtaRwUInt16 unused : 10;
	gtaRwUInt16 pce : 2;
	gtaRwUInt16 id : 3;
	gtaRwUInt16 irq : 1;
	gtaRwUInt32 addr;
};

struct gtaNativePS2VIFCodeNop
{
	gtaRwUInt16 unused_immediate;
	gtaRwUInt8 unused_num;
	gtaRwUInt8 cmd;
};

struct gtaNativePS2VIFCodeStcycl
{
	gtaRwUInt8 cl;
	gtaRwUInt8 wl;
	gtaRwUInt8 unused_num;
	gtaRwUInt8 cmd;
};

struct gtaNativePS2VIFCodeOffset
{
	gtaRwUInt8 offset;
	gtaRwUInt8 unused;
	gtaRwUInt8 num;
	gtaRwUInt8 cmd;
};

union gtaNativePS2VIFCode
{
	gtaNativePS2VIFCodeNop _nop;
	gtaNativePS2VIFCodeStcycl _stcycl;
	gtaNativePS2VIFCodeOffset _offset;
	struct{
		gtaRwUInt16 immediate;
		gtaRwUInt8 num;
		gtaRwUInt8 cmd;
	};
};

union gtaNativePS2VIFPacket
{
	gtaNativePS2VIFCode code;
	void *unpackData;
};

union gtaNativePS2DMAPacket
{
	gtaNativePS2DMATag tag;
	gtaNativePS2VIFPacket *packets;
};

struct gtaNativePS2Mesh
{
	gtaRwUInt32 dataSize; // data next - data previous
	gtaRwBool notSkinned; // if element with id #3 not presented
	void *data; // 16-byte aligned mesh data 
};

struct gtaRwGeometryNative : public gtaRwExtension
{
	gtaPlatformId dataFormat;

	struct gtaNativeD3D8
	{

	} d3d8;

	struct gtaNativeD3D9
	{

	} d3d9;

	struct gtaNativePS2
	{
		gtaRwUInt32 platformId;
		gtaNativePS2Mesh *meshes;
	} ps2;

	struct gtaNativeXBOX
	{

	} xbox;

	struct gtaNativeOGl
	{
		gtaRwUInt32 numVertexElements;
		gtaNativeOGlVertexElement *vertexElements;
		void *vertexBuffer;
		gtaRwBool generated;

		gtaNativeOGl();

		void Initialise(gtaRwUInt32 NumVertexElements, gtaRwUInt32 NumVertices, gtaRwUInt32 VertexStride);

		void Destroy();

		gtaRwUInt32 GetStreamSize(gtaRwUInt32 NumVertices);

		gtaRwBool StreamWrite(gtaRwStream *Stream, gtaRwUInt32 NumVertices);

		gtaRwBool StreamRead(gtaRwStream *Stream, gtaRwUInt32 NumVertices);

	} ogl;

	gtaRwGeometryNative();

	gtaRwBool StreamWrite(gtaRwStream *Stream, gtaRwUInt32 NumVertices);

	gtaRwBool StreamRead(gtaRwStream *Stream, gtaRwUInt32 NumVertices);
	
	gtaRwUInt32 GetStreamSize(gtaRwUInt32 NumVertices);

	void Initialise();

	void Destroy();
};