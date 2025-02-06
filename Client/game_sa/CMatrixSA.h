/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMatrixSA.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CVector.h"
#include "CRenderWareSA.h"

class CMatrixSAInterface
{
private:
    // RwV3d-like:
    CVector      m_right;
    unsigned int flags;
    CVector      m_forward;
    unsigned int pad1;
    CVector      m_up;
    unsigned int pad2;
    CVector      m_pos;
    unsigned int pad3;

public:
    RwMatrix* m_pAttachMatrix = nullptr;
    bool      m_bOwnsAttachedMatrix = false;            // do we need to delete attaching matrix at detaching

    CMatrixSAInterface() : m_right{}, flags{}, m_forward{}, pad1{}, m_up{}, pad2{}, m_pos{}, pad3{} {};
    CMatrixSAInterface(CMatrixSAInterface const& matrix);
    CMatrixSAInterface(RwMatrix* matrix, bool temporary);            // like previous + attach
    ~CMatrixSAInterface();                                           // destructor detaches matrix if attached

    void ConvertToEulerAngles(float& x, float& y, float& z, std::int32_t flags);
    void ConvertFromEulerAngles(float x, float y, float z, std::int32_t flags);
    void UpdateRW();
    void SetTranslateOnly(CVector position) { m_pos = position; }
    void SetMatrix(const CVector& right, const CVector& forward, const CVector& up, const CVector& pos)
    {
        m_right = right;
        m_forward = forward;
        m_up = up;
        m_pos = pos;
    }
};
