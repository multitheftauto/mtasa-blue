#include "StdInc.h"
#include "CFrustum.h"

CFrustum::CFrustum()
{
}

CFrustum::~CFrustum()
{
}

void CFrustum::ConstructFrustum(float screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix)
{
    D3DXMATRIX viewProjection;
    D3DXMatrixMultiply(&viewProjection, &viewMatrix, &projectionMatrix);
    // D3DXMatrixMultiply( &viewProjection, &m_mView, &m_mProjection );
    // Left plane
    m_planes[0].a = viewProjection._14 + viewProjection._11;
    m_planes[0].b = viewProjection._24 + viewProjection._21;
    m_planes[0].c = viewProjection._34 + viewProjection._31;
    m_planes[0].d = viewProjection._44 + viewProjection._41;

    // Right plane
    m_planes[1].a = viewProjection._14 - viewProjection._11;
    m_planes[1].b = viewProjection._24 - viewProjection._21;
    m_planes[1].c = viewProjection._34 - viewProjection._31;
    m_planes[1].d = viewProjection._44 - viewProjection._41;

    // Top plane
    m_planes[2].a = viewProjection._14 - viewProjection._12;
    m_planes[2].b = viewProjection._24 - viewProjection._22;
    m_planes[2].c = viewProjection._34 - viewProjection._32;
    m_planes[2].d = viewProjection._44 - viewProjection._42;

    // Bottom plane
    m_planes[3].a = viewProjection._14 + viewProjection._12;
    m_planes[3].b = viewProjection._24 + viewProjection._22;
    m_planes[3].c = viewProjection._34 + viewProjection._32;
    m_planes[3].d = viewProjection._44 + viewProjection._42;

    // Near plane
    m_planes[4].a = viewProjection._13;
    m_planes[4].b = viewProjection._23;
    m_planes[4].c = viewProjection._33;
    m_planes[4].d = viewProjection._43;

    // Far plane
    m_planes[5].a = viewProjection._14 - viewProjection._13;
    m_planes[5].b = viewProjection._24 - viewProjection._23;
    m_planes[5].c = viewProjection._34 - viewProjection._33;
    m_planes[5].d = viewProjection._44 - viewProjection._43;
}

bool CFrustum::CheckPoint(CVector& vecPosition)
{
	// Check if the point is inside all six planes of the view frustum.
	for(int i=0; i<6; i++) 
	{
        if(D3DXPlaneDotCoord(&m_planes[i], (D3DXVECTOR3*)&vecPosition) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool CFrustum::CheckCube(CVector& vecCube, float fRadius)
{
	// Check if any one point of the cube is in the view frustum.
	for(int i=0; i<6; i++) 
	{
        if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCube.fX - fRadius), (vecCube.fY - fRadius), (vecCube.fZ - fRadius))) >= 0.0f)
		{
			continue;
		}
		
		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCube.fX + fRadius), (vecCube.fY - fRadius), (vecCube.fZ - fRadius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCube.fX - fRadius), (vecCube.fY + fRadius), (vecCube.fZ - fRadius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCube.fX + fRadius), (vecCube.fY + fRadius), (vecCube.fZ - fRadius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCube.fX - fRadius), (vecCube.fY - fRadius), (vecCube.fZ + fRadius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCube.fX + fRadius), (vecCube.fY - fRadius), (vecCube.fZ + fRadius))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCube.fX - fRadius), (vecCube.fY + fRadius), (vecCube.fZ + fRadius))) >= 0.0f)
		{
			continue;
		}
		
		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCube.fX + fRadius), (vecCube.fY + fRadius), (vecCube.fZ + fRadius))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool CFrustum::CheckSphere(CVector& vecPosition, float radius)
{
	// Check if the radius of the sphere is inside the view frustum.
	for(int i=0; i<6; i++) 
	{
        if(D3DXPlaneDotCoord(&m_planes[i], (D3DXVECTOR3*)(&vecPosition)) < -radius)
		{
			return false;
		}
	}

	return true;
}

bool CFrustum::CheckRectangle(CVector& vecCenter, CVector& vecSize)
{
	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for(int i=0; i<6; i++)
	{
        if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCenter.fX - vecSize.fX), (vecCenter.fY - vecSize.fY), (vecCenter.fZ - vecSize.fZ))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCenter.fX + vecSize.fX), (vecCenter.fY - vecSize.fY), (vecCenter.fZ - vecSize.fZ))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCenter.fX - vecSize.fX), (vecCenter.fY + vecSize.fY), (vecCenter.fZ - vecSize.fZ))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCenter.fX - vecSize.fX), (vecCenter.fY - vecSize.fY), (vecCenter.fZ + vecSize.fZ))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCenter.fX + vecSize.fX), (vecCenter.fY + vecSize.fY), (vecCenter.fZ - vecSize.fZ))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCenter.fX + vecSize.fX), (vecCenter.fY - vecSize.fY), (vecCenter.fZ + vecSize.fZ))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCenter.fX - vecSize.fX), (vecCenter.fY + vecSize.fY), (vecCenter.fZ + vecSize.fZ))) >= 0.0f)
		{
			continue;
		}

		if (D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((vecCenter.fX + vecSize.fX), (vecCenter.fY + vecSize.fY), (vecCenter.fZ + vecSize.fZ))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}
