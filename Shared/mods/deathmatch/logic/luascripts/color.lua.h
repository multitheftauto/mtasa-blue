namespace EmbeddedLuaCode
{
    const char* const tocolor = R"~LUA~(

--[[
    SERVER AND CLIENT.

    Provide a lua implementation of old tocolor function
--]]

local _errMsg = "Bad argument @ 'tocolor' [Expected %s at argument %d, got %s]"

tocolor = function(r,g,b,a)
    assert(type(r) == 'number', _errMsg:format(1, type(r)))
    assert(type(g) == 'number', _errMsg:format(2, type(g)))
    assert(type(b) == 'number', _errMsg:format(3, type(b)))

    if type(a) ~= 'number' then a = 255 end

    return b + g * 256 + r * 256 * 256 + a * 256 * 256 * 256
end

    )~LUA~";
}
