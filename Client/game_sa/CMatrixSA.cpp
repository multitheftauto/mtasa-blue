/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMatrixSA.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMatrixSA.h"

CMatrixSAInterface::CMatrixSAInterface(CMatrixSAInterface const& matrix)
{
    ((void(__thiscall*)(CMatrixSAInterface*, CMatrixSAInterface const&))0x59BCF0)(this, matrix);
}

CMatrixSAInterface::CMatrixSAInterface(RwMatrix* matrix, bool temporary)
{
    ((void(__thiscall*)(CMatrixSAInterface*, RwMatrix*, bool))0x59C050)(this, matrix, temporary);
}

// destructor detaches matrix if attached
CMatrixSAInterface::~CMatrixSAInterface()
{
    if (m_bOwnsAttachedMatrix && m_pAttachMatrix)
        ((void(__cdecl*)(RwMatrix*))0x7F2A20)(m_pAttachMatrix);

    //((void(__thiscall*)(CMatrixSAInterface*))0x59ACD0)(this);
}

void CMatrixSAInterface::ConvertToEulerAngles(float& x, float& y, float& z, std::int32_t flags)
{
    ((void(__thiscall*)(CMatrixSAInterface*, float&, float&, float&, std::int32_t))0x59A840)(this, x, y, z, flags);
}
void CMatrixSAInterface::ConvertFromEulerAngles(float x, float y, float z, std::int32_t flags)
{
    ((void(__thiscall*)(CMatrixSAInterface*, float, float, float, std::int32_t))0x59AA40)(this, x, y, z, flags);
}

void CMatrixSAInterface::UpdateRwMatrix(RwMatrix* rwMatrix)
{
    rwMatrix->right.x = m_right.fX;
    rwMatrix->right.y = m_right.fY;
    rwMatrix->right.z = m_right.fZ;

    rwMatrix->up.x = m_forward.fX;
    rwMatrix->up.y = m_forward.fY;
    rwMatrix->up.z = m_forward.fZ;

    rwMatrix->at.x = m_up.fX;
    rwMatrix->at.y = m_up.fY;
    rwMatrix->at.z = m_up.fZ;

    rwMatrix->pos.x = m_pos.fX;
    rwMatrix->pos.y = m_pos.fY;
    rwMatrix->pos.z = m_pos.fZ;

    ((void(__cdecl*)(RwMatrix*))0x7F18A0)(rwMatrix);
    //RwMatrixUpdate(rwMatrix);
}

void CMatrixSAInterface::UpdateRW()
{
    if (!m_pAttachMatrix)
        return;

    UpdateRwMatrix(m_pAttachMatrix);
}

void CMatrixSAInterface::SetTranslate(const CVector& position)
{
    m_right.fX = 1.0f;
    m_right.fY = 0;
    m_right.fZ = 0;

    m_forward.fX = 0;
    m_forward.fY = 1.0f;
    m_forward.fZ = 0;

    m_up.fX = 0;
    m_up.fY = 0;
    m_up.fZ = 1.0f;

    m_pos = position;
}

void CMatrixSAInterface::RotateZ(float angle)
{
    float cos = std::cos(angle);
    float sin = std::sin(angle);

    m_right.fX = cos * m_right.fX - sin * m_right.fY;
    m_right.fY = cos * m_right.fY + sin * m_right.fX;

    m_forward.fX = cos * m_forward.fX - sin * m_forward.fY;
    m_forward.fY = cos * m_forward.fY + sin * m_forward.fX;

    m_up.fX = cos * m_up.fX - sin * m_up.fY;
    m_up.fY = cos * m_up.fY + sin * m_up.fX;

    m_pos.fX = cos * m_pos.fX - sin * m_pos.fY;
    m_pos.fY = cos * m_pos.fY + sin * m_pos.fX;
}
