namespace EmbeddedLuaCode
{
    const char* const exports = R"~LUA~(

--[[
    SERVER AND CLIENT.

    Code for allowing this syntax:   exports.resourceName:exportedFunctionName (...)
                                     exports["resourceName"]:exportedFunctionName (...)
                                     exports[resourcePointer]:exportedFunctionName (...)
    Aswell as the old:               call ( getResourceFromName ( "resourceName" ), "exportedFunctionName", ... )
--]]

-- Protect some functions from modifications by resources
local type = type
local setmetatable = setmetatable
local getResourceRootElement = getResourceRootElement
local call = call
local getResourceFromName = getResourceFromName
local tostring = tostring
local outputDebugString = outputDebugString

local rescallMT = {}
function rescallMT:__index(k)
    if type(k) ~= 'string' then k = tostring(k) end
        self[k] = function(resExportTable, ...)
        if type(self.res) == 'userdata' and getResourceRootElement(self.res) then
                return call(self.res, k, ...)
        else
                return nil
        end
    end
    return self[k]
end

local exportsMT = {}
function exportsMT:__index(k)
    if type(k) == 'userdata' and getResourceRootElement(k) then
        return setmetatable({ res = k }, rescallMT)
    elseif type(k) ~= 'string' then
        k = tostring(k)
    end

    local res = getResourceFromName(k)
    if res and getResourceRootElement(res) then
        return setmetatable({ res = res }, rescallMT)
    else
        outputDebugString('exports: Call to non-running server resource (' .. k .. ')', 1)
        return setmetatable({}, rescallMT)
    end
end
exports = setmetatable({}, exportsMT)

    )~LUA~";
}
