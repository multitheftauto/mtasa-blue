/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwRenderStates.h
*  PURPOSE:     RenderWare renderstate management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_RENDERSTATES_
#define _RENDERWARE_RENDERSTATES_

inline DWORD F2DW( float val )  { return *(DWORD*)&val; }

// RenderState functions.
int RwD3D9SetFogColor                       ( unsigned int color );
int RwD3D9ResetFogColor                     ( void );
int RwD3D9SetShadeMode                      ( unsigned int mode );
int RwD3D9ResetShadeMode                    ( void );
int RwD3D9SetZWriteEnable                   ( unsigned int enable );
int RwD3D9ResetZWriteEnable                 ( void );
int RwD3D9SetDepthFunctionEnable            ( unsigned int enable );
int RwD3D9ResetDepthFunctionEnable          ( void );
int RwD3D9SetStencilEnable                  ( unsigned int enable );
int RwD3D9ResetStencilEnable                ( void );
int RwD3D9SetStencilFail                    ( unsigned int opMode );
int RwD3D9ResetStencilFail                  ( void );
int RwD3D9SetStencilZFail                   ( unsigned int opMode );
int RwD3D9ResetStencilZFail                 ( void );
int RwD3D9SetStencilPass                    ( unsigned int opMode );
int RwD3D9ResetStencilPass                  ( void );
int RwD3D9SetStencilFunc                    ( unsigned int cmpOp );
int RwD3D9ResetStencilFunc                  ( void );
int RwD3D9SetStencilRef                     ( unsigned int ref );
int RwD3D9ResetStencilRef                   ( void );
int RwD3D9SetStencilMask                    ( unsigned int mask );
int RwD3D9ResetStencilMask                  ( void );
int RwD3D9SetStencilWriteMask               ( unsigned int mask );
int RwD3D9ResetStencilWriteMask             ( void );

// Raster stage functions.
int RwD3D9RasterStageSetFilterMode          ( unsigned int stageIdx, unsigned int filterMode );
int RwD3D9RasterStageResetFilterMode        ( unsigned int stageIdx );
int RwD3D9RasterStageSetAddressModeU        ( unsigned int stageIdx, unsigned int modeIdx );
int RwD3D9RasterStageResetAddressModeU      ( unsigned int stageIdx );
int RwD3D9RasterStageSetAddressModeV        ( unsigned int stageIdx, unsigned int modeIdx );
int RwD3D9RasterStageResetAddressModeV      ( unsigned int stageIdx );
int RwD3D9RasterStageSetBorderColor         ( unsigned int stageIdx, unsigned int color );
int RwD3D9RasterStageResetBorderColor       ( unsigned int stageIdx );
int RwD3D9RasterStageSetMaxAnisotropy       ( unsigned int stageIdx, unsigned int maxAnisotropy );
int RwD3D9RasterStageResetMaxAnisotropy     ( unsigned int stageIdx );

// More RenderState functions.
int RwD3D9SetSrcBlend                       ( unsigned int blendMode );
int RwD3D9ResetSrcBlend                     ( void );
int RwD3D9SetDstBlend                       ( unsigned int blendMode );
int RwD3D9ResetDstBlend                     ( void );
int RwD3D9SetAlphaFunc                      ( unsigned int cmpOp );
int RwD3D9ResetAlphaFunc                    ( void );
int RwD3D9SetAlphaEnable                    ( unsigned int blendEnable, unsigned int testEnable );
int RwD3D9ResetAlphaEnable                  ( void );
int RwD3D9SetCullMode                       ( unsigned int cullMode );
int RwD3D9ResetCullMode                     ( void );
int RwD3D9SetFogEnable                      ( unsigned int enable );
int RwD3D9ResetFogEnable                    ( void );

// Initializators.
void __cdecl RwD3D9InitializeDeviceStates   ( void );
void __cdecl RwD3D9ResetDeviceStates        ( void );

#endif //_RENDERWARE_RENDERSTATES_