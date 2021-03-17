/* This file serves as a temporary replacement for StdInc.cpp
    for projects which previously used PCHs to provide sdk/*.hpp
    implementations
*/

#include "SString.hpp"
#include "WString.hpp"
#include "SharedUtil.AllocTracking.hpp"
#include "SharedUtil.Misc.hpp"
#include "SharedUtil.File.hpp"
#include "SharedUtil.Time.hpp"
#include "SharedUtil.Game.hpp"
#include "SharedUtil.Hash.hpp"
#include "SharedUtil.Profiling.hpp"
#include "SharedUtil.Logging.hpp"
#include "SharedUtil.AsyncTaskScheduler.hpp"
#if defined(SHARED_UTIL_WITH_SYS_INFO)
    #include "SharedUtil.SysInfo.hpp"
#endif
