workspace "utility"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    preferredtoolarchitecture "x86_64"
    staticruntime "on"
    cppdialect "C++23"
    characterset "Unicode"
    flags "MultiProcessorCompile"
    warnings "Off"
    location "build"
    targetdir "build/bin/%{cfg.buildcfg}"
    objdir "build/obj"

    filter "configurations:Debug"
        defines { "MTA_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "system:windows"
        defines { "_WIN32_WINNT=0x601" }
        buildoptions { "/Zc:__cplusplus" }

project "hookcheck"
    kind "ConsoleApp"
    language "C++"
    warnings "Extra"

    files {
        "hookcheck.cpp",
        "pdb-analyzer.cpp",
        "pdb-analyzer.h",
        "dll-analyzer.cpp",
        "dll-analyzer.h",
        "utility.cpp",
        "utility.h",
    }

    -- Call locate-dia-sdk.bat to print out the absolute path to the DIA SDK directory
    diaSdkRoot, errc = os.outputof("locate-dia-sdk.bat")
    if errc ~= 0 then error("DIA SDK not found") end
    print("[+] Using ".. diaSdkRoot)

    -- Copy all *.dll files in <VS>/DIA SDK/bin/amd64 to the build directory
    runtimeDlls = os.matchfiles(path.join(diaSdkRoot, "bin", "amd64", "*.dll"))
    for i = 1, #runtimeDlls do
        runtimeDlls[i] = ("{COPYFILE} %%[%s] %%[%%{cfg.buildtarget.directory}]"):format(runtimeDlls[i])
    end
    if runtimeDlls[1] then
        postbuildcommands(runtimeDlls)
    end

    includedirs { path.join(diaSdkRoot, "include") }
    libdirs { path.join(diaSdkRoot, "lib", "amd64") }
