project "wasm-micro-runtime"
    language "C++"
    kind "SharedLib"
    toolset "clang"
    
	targetdir(buildpath("server/mods/deathmatch"))

	filter "system:windows"
		links { "ws2_32", "pthread" }

	filter "system:not windows"
		buildoptions { "-Wno-narrowing" } -- We should fix the warnings at some point
		buildoptions { "-pthread" }
		linkoptions { "-pthread" }

    filter {}

    defines {
        "WASM_ENABLE_INTERP=1","WASM_ENABLE_AOT=1","WASM_ENABLE_LIBC_BUILTIN=1",
        "BUILD_TARGET_X86_32","WASM_ENABLE_FAST_INTERP=1",
        "WASM_ENABLE_MULTI_MODULE=0","WASM_ENABLE_BULK_MEMORY=1","WASM_ENABLE_SHARED_MEMORY=0",
        "WASM_ENABLE_MINI_LOADER=0","WASM_DISABLE_HW_BOUND_CHECK=0","WASM_DISABLE_STACK_HW_BOUND_CHECK=0",
        "WASM_ENABLE_SIMD=1","WASM_GLOBAL_HEAP_SIZE=10485760","BH_PLATFORM_WINDOWS",
        "HAVE_STRUCT_TIMESPEC","BH_MALLOC=wasm_runtime_malloc","BH_FREE=wasm_runtime_free",
        --"WASM_ENABLE_UVWASI=1",
        "WASM_ENABLE_LIBC_WASI=1",
        --[["WASM_RUNTIME_API_EXTERN=",
        "WASM_API_EXTERN=",
        "__cplusplus=201500L"]]
    }
    
    files {
        "core/iwasm/aot/*.h",
        "core/iwasm/aot/*.c",
        "core/iwasm/include/*.h",
        "core/shared/utils/*.c",
        "core/shared/utils/*.h",
        "core/shared/mem-alloc/*.c",
        "core/shared/mem-alloc/*.h",
        "core/shared/mem-alloc/ems/*.c",
        "core/shared/mem-alloc/ems/*.h",
        "core/iwasm/libraries/libc-builtin/*.c",
        "core/iwasm/libraries/libc-builtin/*.h",
        --"core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/*.c",
        "core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/*.h",
        "core/shared/libc-wasi/sandboxed-system-primitives/src/*.h",
        "core/shared/libc-wasi/sandboxed-system-primitives/src/*.c",
        "core/iwasm/common/*.h",
        "core/iwasm/common/*.c",
        "core/iwasm/interpret/*.c",
        "core/iwasm/interpret/*.h",
        "core/shared/platform/windows/*.h",
        "core/shared/platform/windows/*.c",
        "core/iwasm/interpreter/*.h",
        "core/iwasm/interpreter/*.c",
        "core/iwasm/aot/arch/aot_reloc_x86_32.c",
        "core/iwasm/common/arch/invokeNative_general.c",
    }
    removefiles {
        "core/iwasm/interpreter/wasm_interp_classic.c",
        "core/iwasm/interpreter/wasm_mini_loader.c",
    }

	includedirs {
        "core/iwasm/interpreter",
        "core/iwasm/aot",
        "core/iwasm/libraries/libc-builtin",
        "core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/include",
        "core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src",
        "core/iwasm/include",
        "core/shared/platform/windows",
        "core/shared/platform/windows/../include",
        "core/shared/mem-alloc",
        "core/iwasm/common",
        "core/shared/utils",
        "core/shared/utils/uncommon",
	}

    vpaths {
        ["Headers/*"] = "**.h",
        ["Sources/*"] = { "**.c" },
        ["*"] = "premake5.lua"
    }


    --warnings 'Off'
    --buildoptions { "/Wall" }

