project "tracy"
    language "C++"
    kind "SharedLib"
    targetname "tracy"
    warnings "Off"

    vpaths {
        ["Headers/*"] = {"**.h*",},
        ["Sources/*"] = {"**.cpp",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    includedirs {
        "./public/",
    }

    files {
        "public/TracyClient.cpp",
        
        -- tracy_includes
        "public/tracy/TracyC.h",
        "public/tracy/Tracy.hpp",
        "public/tracy/TracyD3D11.hpp",
        "public/tracy/TracyD3D12.hpp",
        "public/tracy/TracyLua.hpp",
        "public/tracy/TracyOpenCL.hpp",
        "public/tracy/TracyOpenGL.hpp",
        "public/tracy/TracyVulkan.hpp",

        -- client_includes
        "public/client/tracy_concurrentqueue.h",
        "public/client/tracy_rpmalloc.hpp",
        "public/client/tracy_SPSCQueue.h",
        "public/client/TracyArmCpuTable.hpp",
        "public/client/TracyCallstack.h",
        "public/client/TracyCallstack.hpp",
        "public/client/TracyCpuid.hpp",
        "public/client/TracyDebug.hpp",
        "public/client/TracyDxt1.hpp",
        "public/client/TracyFastVector.hpp",
        "public/client/TracyLock.hpp",
        "public/client/TracyProfiler.hpp",
        "public/client/TracyRingBuffer.hpp",
        "public/client/TracyScoped.hpp",
        "public/client/TracyStringHelpers.hpp",
        "public/client/TracySysPower.hpp",
        "public/client/TracySysTime.hpp",
        "public/client/TracySysTrace.hpp",
        "public/client/TracyThread.hpp",

        -- common_includes
        "public/common/tracy_lz4.hpp",
        "public/common/tracy_lz4hc.hpp",
        "public/common/TracyAlign.hpp",
        "public/common/TracyAlloc.hpp",
        "public/common/TracyApi.h",
        "public/common/TracyColor.hpp",
        "public/common/TracyForceInline.hpp",
        "public/common/TracyMutex.hpp",
        "public/common/TracyProtocol.hpp",
        "public/common/TracyQueue.hpp",
        "public/common/TracySocket.hpp",
        "public/common/TracyStackFrames.hpp",
        "public/common/TracySystem.hpp",
        "public/common/TracyUwp.hpp",
        "public/common/TracyYield.hpp",
    }

    defines {
        "TRACY_ENABLE",
        "TRACY_CALLSTACK",
        "TRACY_ON_DEMAND",
        "TRACY_NO_CODE_TRANSFER",
        "TRACY_IMPORTS"
    }
    