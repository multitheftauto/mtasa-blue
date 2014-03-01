/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwMath.cpp
*  PURPOSE:     RenderWare Math using logical objects
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

bool __cdecl RwD3D9CameraIsSphereFullyInsideFrustum( RwCamera *camera, const RwSphere& sphere )
{
    float negSphereRadius = -sphere.radius;

    for ( unsigned int n = 0; n < 6; n++ )
    {
        RwCameraFrustum& curFrustum = camera->frustum4D[n];

        if ( sphere.pos.DotProduct( (const CVector&)curFrustum.plane.normal ) < negSphereRadius )
            return false;
    }

    return true;
}

bool __cdecl RwD3D9CameraIsBBoxFullyInsideFrustum( RwCamera *camera, const CVector *bbox )
{
    for ( unsigned int n = 0; n < 6; n++ )
    {
        RwCameraFrustum& curFrustum = camera->frustum4D[n];

        CVector boxVector = CVector(
            ( bbox - curFrustum.x )->fX,
            ( bbox - curFrustum.y )->fY,
            ( bbox - curFrustum.z )->fZ
        );

        if ( boxVector.DotProduct( (const CVector&)curFrustum.plane.normal ) < curFrustum.plane.length )
            return false;
    }

    return true;
}