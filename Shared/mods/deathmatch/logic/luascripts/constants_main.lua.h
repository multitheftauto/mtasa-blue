namespace EmbeddedLuaCode
{
    const char* const constantsMain = R"~LUA~(
--[[
    SERVER AND CLIENT.
    Defines a constant variables available for server and client.
--]]

__readonly_table_meta__ = {
    __index = function(tbl, key)
        local val = rawget(tbl, key)
        if val then
            return val
        end
        for k,v in pairs(tbl) do
            if tostring(k):lower() == tostring(key):lower() then
                return v
            end
        end
    end,
    __newindex = function(tbl, key)
        return error('Table is read only!')
    end,
    __call = function(tbl, key)
        if not key then return end
        for k,v in pairs(tbl) do
            if tostring(v):lower() == tostring(key):lower() then
                return k
            end
        end
    end
}

constants = {}

    )~LUA~";
}