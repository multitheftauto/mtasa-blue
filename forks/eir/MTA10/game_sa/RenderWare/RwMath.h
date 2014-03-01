/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwMath.h
*  PURPOSE:     RenderWare Math using logical objects
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_MATH_
#define _RENDERWARE_MATH_

bool __cdecl RwD3D9CameraIsSphereFullyInsideFrustum( RwCamera *camera, const RwSphere& sphere );
bool __cdecl RwD3D9CameraIsBBoxFullyInsideFrustum( RwCamera *camera, const CVector *bbox );

#endif //_RENDERWARE_MATH_