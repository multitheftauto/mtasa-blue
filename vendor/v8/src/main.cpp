#include "StdInc.h"
#include "SharedUtil.Defines.h"
#include <Windows.h>
MTAEXPORT CV8Base* InitV8()
{
    CV8* v8 = new CV8();
    return v8;
}
