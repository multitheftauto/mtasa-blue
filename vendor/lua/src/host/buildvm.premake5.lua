-- Mostly based on host/buildvm/CMakeLists.txt and `msvcbuild.bat`

local DASMDIR = "%{getrootpath()}/dynasm"
local DASM    = path.join(DASMDIR, "dynasm.lua")

-- Okay, so:
-- X64 + GC64 => vm_x64
-- X64 no GC64 => vm_x86
-- X86 + GC64 => vm_x86
-- X86 no GC64 => vm_x86
local function get_dasc_filename(cfg)
    return cfg.platform == 'x64' and GC64 and 'vm_x64.dasc' or 'vm_x86.dasc'
end

-- Produces a command to generate `buildvm_arch.h`
function generate_buildvm_arch_h(cfg) 
    --local cmd = cfg.buildtarget.abspath
    local cmd = ''
    cmd = cmd .. 'minilua "' .. DASM .. '" -LN -D WIN -D JIT '
    if cfg.platform == 'x64' then
        cmd = cmd .. '-D P64 '
    end
    if FFI then
        cmd = cmd .. '-D FFI '
    end
    cmd = cmd .. '-o host/buildvm_arch.h ' .. get_dasc_filename(cfg)
    return cmd
end

project "BuildVM"
    kind "ConsoleApp"
    targetname "buildvm"
    targetsuffix ""
    targetdir ".." -- spit out stuff directly into src/ folder
    compileas "C"

    dependson "MiniLua"

    defines {"_CRT_SECURE_NO_WARNINGS"}
    linkoptions { "/SAFESEH:NO" }

    includedirs {
        "../",  -- src folder
        ".",    -- src/host (this folder)
    }

    vpaths {
        ["Headers/*"] = "*.h",
        ["Soruces/*"] = "*.c",
        ["*"] = "buildvm.premake5.lua"
    }

    files {
        "buildvm.c",
        "buildvm_asm.c",
        "buildvm_fold.c",
        "buildvm_lib.c",
        "buildvm_peobj.c"
    }

    if not GC64 then
        filter "platforms:x64"
            defines { "LUAJIT_DISABLE_GC64" }
    end

    -- Generate buildvm_arch.h
    prebuildcommands {'{CHDIR} "%{srcpath()}"', "%{generate_buildvm_arch_h(cfg)}"}

    -- Run buildvm
    
    -- Change directory to `src/` to make our life easier
    -- This code is mostly based on `msvcbuild.bat`
    postbuildcommands {"{CHDIR} %{srcpath()}"}

    -- Spits out command for running buildvm in the given `mode`, with -o as `out`, and args `args
    local function buildvm_cmd(mode, out, args)
        return ('buildvm -m %s -o %s %s'):format(mode, out, args)
    end
    
    -- Modes to run buildvm with
    local allibs = " lib_base.c lib_math.c lib_bit.c lib_string.c lib_table.c lib_io.c lib_os.c lib_package.c lib_debug.c lib_jit.c lib_ffi.c lib_buffer.c"
    local modes = {
        -- mode,       output path,       additional args
        {"peobj",      "lj_vm.obj",          ""         },
        {"bcdef",      "lj_bcdef.h",     allibs         },
        {"ffdef",      "lj_ffdef.h",     allibs         },
        {"libdef",     "lj_libdef.h",    allibs         },
        {"recdef",     "lj_recdef.h",    allibs         },
        {"vmdef",      "jit/vmdef.lua",  allibs         },
        {"folddef",    "lj_folddef.h",   "lj_opt_fold.c"}
    }

    -- Finally, add buildvm commands to postbuild
    for _, v in pairs(modes) do
        postbuildcommands(buildvm_cmd(table.unpack(v)))
    end
