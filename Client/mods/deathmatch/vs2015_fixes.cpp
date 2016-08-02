#include "StdInc.h"

#if _MSC_VER == 1900
    /**
     * VS2015 defines std io in another way which is incompatible to the way VS2013 used
     * We can fix this however by defining the old way manually
     * It's only required for libspeex, so once we replace libspeex, we can remove this fix
     */
    FILE _iob[] = { *stdin, *stdout, *stderr };

    extern "C" FILE * __cdecl __iob_func(void)
    {
        return _iob;
    }
#endif
