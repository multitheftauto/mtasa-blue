//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

static gtaRwChar errText[1024];

gtaRwBool gtaRwError(gtaRwChar *message, ...) {
    va_list ap;
    va_start(ap, message);
    vsnprintf(errText, 1024, message, ap);
    va_end(ap);
    MessageBoxA(rwNULL, errText, "gta-rw Error", 0);
    return rwFALSE;
}
