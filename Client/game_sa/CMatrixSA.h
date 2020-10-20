#pragma once
#include "CVector.h"
#include "CRenderWareSA.h"

class CMatrixSAInterface
{
public:
    // RwV3d-like:
    CVector      vRight;
    unsigned int flags = 0;
    CVector      vFront;
    unsigned int pad1 = 0;
    CVector      vUp;
    unsigned int pad2 = 0;
    CVector      vPos;
    unsigned int pad3 = 0;

    RwMatrixTag* m_pAttachMatrix = nullptr;
    bool         m_bOwnsAttachedMatrix = false;            // do we need to delete attaching matrix at detaching

    CMatrixSAInterface(const CMatrix& Matrix) { SetFromMatrix(Matrix); }

    void ConvertToMatrix(CMatrix& Matrix) const
    {
        Matrix.vPos = vPos;
        Matrix.vFront = vFront;
        Matrix.vUp = vUp;
        Matrix.vRight = vRight;
    }

    void SetFromMatrix(const CMatrix& Matrix)
    {
        vPos = Matrix.vPos;
        vFront = Matrix.vFront;
        vUp = Matrix.vUp;
        vRight = Matrix.vRight;
        pad3 = 0;
        pad2 = 0;
        pad1 = 0;
        flags = 0;
    }

    void SetFromMatrixSkipPadding(const CMatrix& Matrix)
    {
        vPos = Matrix.vPos;
        vFront = Matrix.vFront;
        vUp = Matrix.vUp;
        vRight = Matrix.vRight;
    }

    CMatrixSAInterface() {}
    CMatrixSAInterface(CMatrixSAInterface const& matrix);
    CMatrixSAInterface(RwMatrixTag* matrix, bool temporary);            // like previous + attach
    ~CMatrixSAInterface();                                              // destructor detaches matrix if attached

    inline CVector& GetRight() { return vRight; }
    inline CVector& GetForward() { return vFront; }
    inline CVector& GetUp() { return vUp; }
    inline CVector& GetPosition() { return vPos; }

    void ConvertToEulerAngles(float& x, float& y, float& z, std::int32_t flags);
    void ConvertFromEulerAngles(float x, float y, float z, std::int32_t flags);
    void UpdateRW();
    void SetTranslateOnly(CVector position) { vPos = position; }
    void SetMatrix(const CVector& right, const CVector& forward, const CVector& up, const CVector& pos)
    {
        vRight = right;
        vFront = forward;
        vUp = up;
        vPos = pos;
    }
    void operator=(CMatrixSAInterface const& right);
    void operator+=(CMatrixSAInterface const& right);
    void operator*=(CMatrixSAInterface const& right);
};
static_assert(sizeof(CMatrixSAInterface) == 0x48, "Invalid size for CMatrixSAInterface");

CMatrixSAInterface operator*(CMatrixSAInterface const& a, CMatrixSAInterface const& b);
CVector            operator*(CMatrixSAInterface const& a, CVector const& b);
CMatrixSAInterface operator+(CMatrixSAInterface const& a, CMatrixSAInterface const& b);
