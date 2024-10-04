#include "StdInc.h"
#include "CMatrixSA.h"

CMatrixSAInterface::CMatrixSAInterface(CMatrixSAInterface const& matrix)
{
    auto args = PrepareSignature(this, &matrix);
    CallGTAFunction<void, __THISCALL>(CMatrix_Constructor, args);
}

CMatrixSAInterface::CMatrixSAInterface(RwMatrix* matrix, bool temporary)
{
    auto args = PrepareSignature(this, matrix, temporary);
    CallGTAFunction<void, __THISCALL>(CMatrix_Constructor2, args);
}

// destructor detaches matrix if attached
CMatrixSAInterface::~CMatrixSAInterface()
{
    CallGTAFunction<void, __THISCALL>(CMatrix_Destructor, PrepareSignature(this));
}

void CMatrixSAInterface::ConvertToEulerAngles(float& x, float& y, float& z, std::int32_t flags)
{
    auto args = PrepareSignature(this, &x, &y, &z, flags);
    CallGTAFunction<void, __THISCALL>(FUNC_CMatrix_ConvertToEulerAngles, args);
}
void CMatrixSAInterface::ConvertFromEulerAngles(float x, float y, float z, std::int32_t flags)
{
    auto args = PrepareSignature(this, x, y, z, flags);
    CallGTAFunction<void, __THISCALL>(FUNC_CMatrix_ConvertFromEulerAngles, args);
}

void CMatrixSAInterface::UpdateRW()
{
    CallGTAFunction<void, __THISCALL>(FUNC_CMatrix_UpdateRW, PrepareSignature(this));
}
