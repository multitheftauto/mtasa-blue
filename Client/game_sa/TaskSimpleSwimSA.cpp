/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "TaskSimpleSwimSA.h"
#include "CPedSA.h"

CTaskSimpleSwimSA::CTaskSimpleSwimSA(CPed* ped, CVector* pos)
{
    CreateTaskInterface(sizeof(CTaskSimpleSAInterface));
    if (!IsValid())
        return;

    // Call the constructor
    ((void(__thiscall*)(CTaskSimpleSwimSAInterface*, CVector*, CPedSAInterface*))0x688930)(static_cast<CTaskSimpleSwimSAInterface*>(GetInterface()), pos, ped ? ped->GetPedInterface() : nullptr);
}
