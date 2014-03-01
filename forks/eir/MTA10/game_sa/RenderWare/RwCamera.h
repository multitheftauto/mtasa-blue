/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwCamera.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_CAMERA_
#define _RENDERWARE_CAMERA_

struct RwCameraFrustum
{
    RwPlane       plane;
    unsigned char x,y,z;    // can be zero or one, decides whether to use maximum or minimum of bbox
    unsigned char unknown1;
};
struct RwCamera : public RwObjectFrame   //size: 428
{
    RwCameraType            camType;            // 20
    RwCameraPreCallback     preCallback;        // 24
    RwCameraPostCallback    postCallback;       // 28
    RwMatrix                matrix;             // 32
    RwRaster*               rendertarget;       // 96
    RwRaster*               bufferDepth;        // 100
    RwV2d                   screen;             // 104
    RwV2d                   screenInverse;      // 112
    RwV2d                   screenOffset;       // 120
    float                   nearplane;          // 128
    float                   farplane;           // 132
    float                   fog;                // 136
    float                   unknown1;           // 140
    float                   unknown2;           // 144
    RwCameraFrustum         frustum4D[6];       // 148
    RwBBox                  viewBBox;           // 268
    RwV3d                   frustum3D[8];       // 292
    BYTE                    unk[28];            // 388
    RpClump*                clump;              // 416
    RwListEntry <RwCamera>  clumpCameras;       // 420

    // Methods.
    void                    BeginUpdate         ( void );
    void                    EndUpdate           ( void );

    void                    AddToClump          ( RpClump *clump );
    void                    RemoveFromClump     ( void );
};

// Camera API.
RwCamera*           RwCameraCreate( void );     // US exe: 0x007EE4F0

#endif //_RENDERWARE_CAMERA_