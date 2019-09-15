// Credits http://www.rastertek.com/dx10tut16.html
// and http://www.chadvernon.com/blog/resources/directx9/frustum-culling/

#pragma once
class CFrustum
{
public:
    CFrustum();
    ~CFrustum();

	void ConstructFrustum(float, D3DXMATRIX, D3DXMATRIX);

	bool CheckPoint(CVector& vecPosition);
    bool CheckCube(CVector& vecCube, float fRadius);
    bool CheckSphere(CVector& vecPosition, float radius);
    bool CheckRectangle(CVector& vecCenter, CVector& vecSize);

private:
	D3DXPLANE m_planes[6];
};
