-- Used in BuildVM
GC64 = true -- Use 64 bit GC. Only available in x64 
FFI  = true -- Enable FFI
JIT  = true -- Use JIT?

function getrootpath()
    return "%{wks.location}/../vendor/lua/"
end

function srcpath(p)
    return "%{getrootpath()}/src/" .. (p or "")
end

--group "LuaYEET"
staticruntime "On" -- /MD switch
include "src/host/buildvm.premake5.lua"
include "src/host/minilua.premake5.lua"
include "luajit.premake5.lua"
