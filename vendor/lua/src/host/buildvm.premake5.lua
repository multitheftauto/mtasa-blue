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

-- Similar to table.concat, but adds a prefix instead of a suffix
-- used to make an arg list from multiple args
-- Eg.: t = {'a', 'b'}, prefix = '-D' => -D a -D b
function table.prefixconcat(t, prefix) 
    local s = ''
    for k, v in ipairs(t) do
        s = ("%s %s %s"):format(s, prefix, v)
    end
    return s:sub(2) -- Remove unnecessary whitespace at the beginning 
end

-- Produces a command to generate `buildvm_arch.h`
function generate_buildvm_arch_h(cfg) 
    -- All of these are guaranteed on x64/x86. 
    -- We have an FPU in hard mode, and we are little endian.
    local defs = {'FPU', 'HFABI', 'ENDIAN_LE'}
    
    if os.host() == "windows" then
        table.insert(defs, 'WIN')
    end
    if cfg.platform == 'x64' then
        table.insert(defs, 'P64')
    end
    if JIT then
        table.insert(defs, 'JIT')
    end
    if FFI then
        table.insert(defs, 'FFI')
    end

    -- Produce command
    local defs_str  = table.prefixconcat(defs, '-D')
    local dasc_path = get_dasc_filename(cfg)
    print(('minilua "%s" -LN %s -o host/buildvm_arch.h %s'):format(DASM, defs_str, dasc_path))
    return ('minilua "%s" -LN %s -o host/buildvm_arch.h %s'):format(DASM, defs_str, dasc_path)
end

project "BuildVM"
    kind "ConsoleApp"
    targetname "buildvm"
    targetsuffix ""
    targetdir ".." -- spit out stuff directly into src/ folder
    compileas "C"

    dependson "MiniLua"

    defines "_CRT_SECURE_NO_WARNINGS"
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

    filter "platforms:x64"
        if not GC64 then
            defines "LUAJIT_DISABLE_GC64"
        end

    filter {}

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
