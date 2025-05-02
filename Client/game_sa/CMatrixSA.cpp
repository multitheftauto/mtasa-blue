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
    ((void(__thiscall*)(CMatrixSAInterface*))0x59ACD0)(this);
}

void CMatrixSAInterface::ConvertToEulerAngles(float& x, float& y, float& z, std::int32_t flags)
{
    ((void(__thiscall*)(CMatrixSAInterface*, float&, float&, float&, std::int32_t))0x59A840)(this, x, y, z, flags);
}
void CMatrixSAInterface::ConvertFromEulerAngles(float x, float y, float z, std::int32_t flags)
{
    ((void(__thiscall*)(CMatrixSAInterface*, float, float, float, std::int32_t))0x59AA40)(this, x, y, z, flags);
}

void CMatrixSAInterface::UpdateRW()
{
    ((void(__thiscall*)(CMatrixSAInterface*))0x59BBB0)(this);
}
