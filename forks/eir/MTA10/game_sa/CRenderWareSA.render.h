/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.render.h
*  PURPOSE:     RenderWare rendering API routines
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_RENDERING_
#define _RENDERWARE_RENDERING_

RwList <RwFrame>& RwFrameGetDirtyList_MTA( void );

void RenderWareRender_Init( void );
void RenderWareRender_Shutdown( void );

#endif //_RENDERWARE_RENDERING_