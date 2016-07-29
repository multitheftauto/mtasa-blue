--Luaify;SCRIPT_STACK=R"~LUACODE~(
--[[
    SERVER AND CLIENT.

    Output errors that occur inside coroutines
--]]

-- Protect some functions from modifications by resources
local tostring = tostring
local outputDebugString = outputDebugString

coroutine._resume = coroutine.resume -- For access to the original function
local _coroutine_resume = coroutine.resume
function coroutine.resume(...)
    local state,result = _coroutine_resume(...)
    if not state then
        outputDebugString( tostring(result), 1 )
    end
    return state,result
end

--)~LUACODE~";