#if defined(_XBOX) || defined(X360)
#include "XBOX360Includes.h"
#elif defined(_WIN32)
#include "WindowsIncludes.h" // Sleep
#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
#include "PS3Includes.h"
#include <sys/timer.h>
#else
#include <unistd.h> // usleep
#endif


#include "RakSleep.h"

void RakSleep(unsigned int ms)
{
#ifdef _WIN32
	Sleep(ms);
#elif defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
	// Use the version of usleep on the console here, this is a macro
	_PS3_usleep(ms * 1000);
#else
	usleep(ms * 1000);
#endif
}
