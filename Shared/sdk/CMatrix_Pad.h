/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CMatrix_Pad.h
 *  PURPOSE:     4x3 GTA padded matrix
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVector.h"
#include "CMatrix.h"

/**
 * CMatrix Structure used internally by GTA.
 */
class CMatrix_Padded
{
public:
    CVector       vRight; // right
    std::uint32_t pad_0;  // flags?
    CVector       vFront; // forward
    std::uint32_t pad_1;
    CVector       vUp;    // up
    std::uint32_t pad_2;
    CVector       vPos;   // translate
    std::uint32_t pad_3;

    void* attachedMatrix; // RwMatrix*
    bool  ownsAttachedMatrix;

public:
    CMatrix_Padded() { memset(this, 0, sizeof(CMatrix_Padded)); }

    CMatrix_Padded(const CMatrix& Matrix) { SetFromMatrix(Matrix); }

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
        pad_3 = 0;

        vFront = Matrix.vFront;
        pad_1 = 0;

        vUp = Matrix.vUp;
        pad_2 = 0;

        vRight = Matrix.vRight;
        pad_0 = 0;
    }

    void SetFromMatrixSkipPadding(const CMatrix& Matrix)
    {
        vPos = Matrix.vPos;
        vFront = Matrix.vFront;
        vUp = Matrix.vUp;
        vRight = Matrix.vRight;
    }
};
