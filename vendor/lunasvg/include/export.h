#pragma once

#if defined _WIN32 && defined LUNASVG_SHARED
    #ifdef LUNASVG_EXPORTS
        #define LUNASVG_API __declspec(dllexport)
    #else
        #define LUNASVG_API __declspec(dllimport)
    #endif
#else
    #define LUNASVG_API
#endif