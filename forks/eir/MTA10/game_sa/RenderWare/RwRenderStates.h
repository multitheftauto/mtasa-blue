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
inline int RwD3D9SetFogColor                    ( unsigned int color );
inline int RwD3D9ResetFogColor                  ( void );
inline int RwD3D9SetShadeMode                   ( unsigned int mode );
inline int RwD3D9ResetShadeMode                 ( void );
inline int RwD3D9SetZWriteEnable                ( unsigned int enable );
inline int RwD3D9ResetZWriteEnable              ( void );
inline int RwD3D9SetDepthFunctionEnable         ( unsigned int enable );
inline int RwD3D9ResetDepthFunctionEnable       ( void );
inline int RwD3D9SetStencilEnable               ( unsigned int enable );
inline int RwD3D9ResetStencilEnable             ( void );
inline int RwD3D9SetStencilFail                 ( unsigned int opMode );
inline int RwD3D9ResetStencilFail               ( void );
inline int RwD3D9SetStencilZFail                ( unsigned int opMode );
inline int RwD3D9ResetStencilZFail              ( void );
inline int RwD3D9SetStencilPass                 ( unsigned int opMode );
inline int RwD3D9ResetStencilPass               ( void );
inline int RwD3D9SetStencilFunc                 ( unsigned int cmpOp );
inline int RwD3D9ResetStencilFunc               ( void );
inline int RwD3D9SetStencilRef                  ( unsigned int ref );
inline int RwD3D9ResetStencilRef                ( void );
inline int RwD3D9SetStencilMask                 ( unsigned int mask );
inline int RwD3D9ResetStencilMask               ( void );
inline int RwD3D9SetStencilWriteMask            ( unsigned int mask );
inline int RwD3D9ResetStencilWriteMask          ( void );

// Raster stage functions.
inline int RwD3D9RasterStageSetFilterMode       ( unsigned int stageIdx, unsigned int filterMode );
inline int RwD3D9RasterStageResetFilterMode     ( unsigned int stageIdx );
inline int RwD3D9RasterStageSetAddressModeU     ( unsigned int stageIdx, unsigned int modeIdx );
inline int RwD3D9RasterStageResetAddressModeU   ( unsigned int stageIdx );
inline int RwD3D9RasterStageSetAddressModeV     ( unsigned int stageIdx, unsigned int modeIdx );
inline int RwD3D9RasterStageResetAddressModeV   ( unsigned int stageIdx );
inline int RwD3D9RasterStageSetBorderColor      ( unsigned int stageIdx, unsigned int color );
inline int RwD3D9RasterStageResetBorderColor    ( unsigned int stageIdx );
inline int RwD3D9RasterStageSetMaxAnisotropy    ( unsigned int stageIdx, unsigned int maxAnisotropy );
inline int RwD3D9RasterStageResetMaxAnisotropy  ( unsigned int stageIdx );

// More RenderState functions.
inline int RwD3D9SetSrcBlend                    ( unsigned int blendMode );
inline int RwD3D9ResetSrcBlend                  ( void );
inline int RwD3D9SetDstBlend                    ( unsigned int blendMode );
inline int RwD3D9ResetDstBlend                  ( void );
inline int RwD3D9SetAlphaFunc                   ( unsigned int cmpOp );
inline int RwD3D9ResetAlphaFunc                 ( void );
inline int RwD3D9SetAlphaEnable                 ( unsigned int blendEnable, unsigned int testEnable );
inline int RwD3D9ResetAlphaEnable               ( void );
inline int RwD3D9SetCullMode                    ( unsigned int cullMode );
inline int RwD3D9ResetCullMode                  ( void );
inline int RwD3D9SetFogEnable                   ( unsigned int enable );
inline int RwD3D9ResetFogEnable                 ( void );

// Initializators.
void __cdecl RwD3D9InitializeDeviceStates       ( void );
void __cdecl RwD3D9ResetDeviceStates            ( void );

#endif //_RENDERWARE_RENDERSTATES_