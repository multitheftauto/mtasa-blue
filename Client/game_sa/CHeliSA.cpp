#include "StdInc.h"
#include "CHeliSA.h"

CHeliSA::CHeliSA(CHeliSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}
