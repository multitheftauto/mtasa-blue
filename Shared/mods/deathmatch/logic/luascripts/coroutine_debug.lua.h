namespace EmbeddedLuaCode
{
    const char* const coroutine_debug = R"~LUA~(

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
    local result = {_coroutine_resume(...)}
    if not result[1] then
        outputDebugString( tostring(result[2]), 1 )
    end
    return unpack(result)
end

    )~LUA~";
}