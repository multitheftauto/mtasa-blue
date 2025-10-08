/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.hpp
 *  PURPOSE:     Solution wide utility functions.
 *               Include this file once per dll.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "SharedUtil.AllocTracking.hpp"
#include "SString.hpp"
#include "WString.hpp"
#include "SharedUtil.Misc.hpp"
#include "SharedUtil.File.hpp"
#include "SharedUtil.Time.hpp"
#include "SharedUtil.Game.hpp"
#include "SharedUtil.Hash.hpp"
#if defined(SHARED_UTIL_WITH_SYS_INFO)
    #include "SharedUtil.SysInfo.hpp"
#endif
#include "SharedUtil.Profiling.hpp"
#include "SharedUtil.Logging.hpp"
#include "SharedUtil.AsyncTaskScheduler.hpp"
#include "SharedUtil.Memory.hpp"
#include "UTF8.hpp"
