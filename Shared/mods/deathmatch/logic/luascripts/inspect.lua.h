namespace EmbeddedLuaCode
{
    const char* const inspect = R"~LUA~(

--[[
    SERVER AND CLIENT.

    Provide extra utility scripting functions:
        string inspect ( arg )  --Provides a human description of any argument, i.e. string,bool,table,element,xml etc
        string iprint ( ... ) --Prints any number of arguments in human readable format

    Table output adapted from inspect.lua provided by Kikito with minor modifiactions (License below)
--]]
inspect ={
  _VERSION = 'inspect.lua 3.1.0',
  _URL     = 'https://github.com/kikito/inspect.lua',
  _DESCRIPTION = 'human-readable representations of tables and MTA datatypes',
  _LICENSE = [[
    MIT LICENSE

    Copyright (c) 2013 Enrique Garcia Cota

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ]]
}
local mta_type
local tostring = tostring
local table_concat = table.concat
local select = select

inspect.KEY       = setmetatable({}, {__tostring = function() return 'inspect.KEY' end})
inspect.METATABLE = setmetatable({}, {__tostring = function() return 'inspect.METATABLE' end})

-- Apostrophizes the string if it has quotes, but not aphostrophes
-- Otherwise, it returns a regular quoted string
local function smartQuote(str)
  if str:match('"') and not str:match("'") then
    return "'" .. str .. "'"
  end
  return '"' .. str:gsub('"', '\\"') .. '"'
end

-- \a => '\\a', \0 => '\\0', 31 => '\31'
local shortControlCharEscapes = {
  ["\a"] = "\\a",  ["\b"] = "\\b", ["\f"] = "\\f", ["\n"] = "\\n",
  ["\r"] = "\\r",  ["\t"] = "\\t", ["\v"] = "\\v"
}
local longControlCharEscapes = {} -- \a => nil, \0 => \000, 31 => \031
for i=0, 31 do
  local ch = string.char(i)
  if not shortControlCharEscapes[ch] then
    shortControlCharEscapes[ch] = "\\"..i
    longControlCharEscapes[ch]  = string.format("\\%03d", i)
  end
end

local function escape(str)
  return (str:gsub("\\", "\\\\")
             :gsub("(%c)%f[0-9]", longControlCharEscapes)
             :gsub("%c", shortControlCharEscapes))
end

local function isIdentifier(str)
  return type(str) == 'string' and str:match( "^[_%a][_%a%d]*$" )
end

local function isSequenceKey(k, sequenceLength)
  return type(k) == 'number'
     and 1 <= k
     and k <= sequenceLength
     and math.floor(k) == k
end

local defaultTypeOrders = {
  ['number']   = 1, ['boolean']  = 2, ['string'] = 3, ['table'] = 4,
  ['function'] = 5, ['userdata'] = 6, ['thread'] = 7
}

local function sortKeys(a, b)
  local ta, tb = mta_type(a), mta_type(b)

  -- strings and numbers are sorted numerically/alphabetically
  if ta == tb and (ta == 'string' or ta == 'number') then return a < b end

  local dta, dtb = defaultTypeOrders[ta], defaultTypeOrders[tb]
  -- Two default types are compared according to the defaultTypeOrders table
  if dta and dtb then return defaultTypeOrders[ta] < defaultTypeOrders[tb]
  elseif dta     then return true  -- default types before custom ones
  elseif dtb     then return false -- custom types after default ones
  end

  -- custom types are sorted out alphabetically
  return ta < tb
end

-- For implementation reasons, the behavior of rawlen & # is "undefined" when
-- tables aren't pure sequences. So we implement our own # operator.
local function getSequenceLength(t)
  local len = 1
  local v = rawget(t,len)
  while v ~= nil do
    len = len + 1
    v = rawget(t,len)
  end
  return len - 1
end

local function getNonSequentialKeys(t)
  local keys = {}
  local sequenceLength = getSequenceLength(t)
  for k,_ in pairs(t) do
    if not isSequenceKey(k, sequenceLength) then table.insert(keys, k) end
  end
  table.sort(keys, sortKeys)
  return keys, sequenceLength
end

local function getToStringResultSafely(t, mt)
  local __tostring = type(mt) == 'table' and rawget(mt, '__tostring')
  local str, ok
  if type(__tostring) == 'function' then
    ok, str = pcall(__tostring, t)
    str = ok and str or 'error: ' .. tostring(str)
  end
  if type(str) == 'string' and #str > 0 then return str end
end

local function countTableAppearances(t, tableAppearances)
  tableAppearances = tableAppearances or {}

  if type(t) == 'table' then
    if not tableAppearances[t] then
      tableAppearances[t] = 1
      for k,v in pairs(t) do
        countTableAppearances(k, tableAppearances)
        countTableAppearances(v, tableAppearances)
      end
      countTableAppearances(getmetatable(t), tableAppearances)
    else
      tableAppearances[t] = tableAppearances[t] + 1
    end
  end

  return tableAppearances
end

local copySequence = function(s)
  local copy, len = {}, #s
  for i=1, len do copy[i] = s[i] end
  return copy, len
end

local function makePath(path, ...)
  local keys = {...}
  local newPath, len = copySequence(path)
  for i=1, #keys do
    newPath[len + i] = keys[i]
  end
  return newPath
end

local function processRecursive(process, item, path, visited)

    if item == nil then return nil end
    if visited[item] then return visited[item] end

    local processed = process(item, path)
    if type(processed) == 'table' then
      local processedCopy = {}
      visited[item] = processedCopy
      local processedKey

      for k,v in pairs(processed) do
        processedKey = processRecursive(process, k, makePath(path, k, inspect.KEY), visited)
        if processedKey ~= nil then
          processedCopy[processedKey] = processRecursive(process, v, makePath(path, processedKey), visited)
        end
      end

      local mt  = processRecursive(process, getmetatable(processed), makePath(path, inspect.METATABLE), visited)
      setmetatable(processedCopy, mt)
      processed = processedCopy
    end
    return processed
end



-------------------------------------------------------------------

local Inspector = {}
local Inspector_mt = {__index = Inspector}

function Inspector:puts(...)
  local args   = {...}
  local buffer = self.buffer
  local len    = #buffer
  for i=1, #args do
    len = len + 1
    buffer[len] = args[i]
  end
end

function Inspector:down(f)
  self.level = self.level + 1
  f()
  self.level = self.level - 1
end

function Inspector:tabify()
  self:puts(self.newline, string.rep(self.indent, self.level))
end

function Inspector:alreadyVisited(v)
  return self.ids[v] ~= nil
end

function Inspector:getId(v)
  local id = self.ids[v]
  if not id then
    local tv = type(v)
    id              = (self.maxIds[tv] or 0) + 1
    self.maxIds[tv] = id
    self.ids[v]     = id
  end
  return tostring(id)
end

function Inspector:putKey(k)
  if isIdentifier(k) then return self:puts(k) end
  self:puts("[")
  self:putValue(k)
  self:puts("]")
end

function Inspector:putTable(t)
  if t == inspect.KEY or t == inspect.METATABLE then
    self:puts(tostring(t))
  elseif self:alreadyVisited(t) then
    self:puts('<table ', self:getId(t), '>')
  elseif self.level >= self.depth then
    self:puts('{...}')
  else
    if self.tableAppearances[t] > 1 then self:puts('<', self:getId(t), '>') end

    local nonSequentialKeys, sequenceLength = getNonSequentialKeys(t)
    local mt                = getmetatable(t)
    local toStringResult    = getToStringResultSafely(t, mt)

    self:puts('{')
    self:down(function()
      if toStringResult then
        self:puts(' -- ', escape(toStringResult))
        if sequenceLength >= 1 then self:tabify() end
      end

      local count = 0
      for i=1, sequenceLength do
        if count > 0 then self:puts(',') end
        self:puts(' ')
        self:putValue(t[i])
        count = count + 1
      end

      for _,k in ipairs(nonSequentialKeys) do
        if count > 0 then self:puts(',') end
        self:tabify()
        self:putKey(k)
        self:puts(' = ')
        self:putValue(t[k])
        count = count + 1
      end

      if mt then
        if count > 0 then self:puts(',') end
        self:tabify()
        self:puts('<metatable> = ')
        self:putValue(mt)
      end
    end)

    if #nonSequentialKeys > 0 or mt then -- result is multi-lined. Justify closing }
      self:tabify()
    elseif sequenceLength > 0 then -- array tables have one extra space before closing }
      self:puts(' ')
    end

    self:puts('}')
  end
end

function Inspector:putValue(v)
  local tv = type(v)

  if tv == 'string' then
    self:puts(smartQuote(escape(v)))
  elseif tv == 'number' or tv == 'boolean' or tv == 'nil' then
    self:puts(tostring(v))
  elseif tv == 'table' then
    self:putTable(v)
  elseif tv == 'userdata' then
    self:putUserdata(v)
  else
    self:puts('<',tv,' ',self:getId(v),'>')
  end
end

-------------------------------------------------------------------
-- MTA EXTENSIONS

-- Prevent VM modifications from affecting us
local isElement = isElement
local getElementType = getElementType
local getPlayerName = getPlayerName
local getVehicleName = getVehicleName
local getElementModel = getElementModel
local getPickupType = getPickupType
local getWeaponNameFromID = getWeaponNameFromID
local getMarkerType = getMarkerType
local getTeamName = getTeamName
local xmlNodeGetName = xmlNodeGetName
local getResourceName = getResourceName
local getAccountName = getAccountName
local aclGetName = aclGetName
local aclGroupGetName = aclGroupGetName
local outputDebugString = outputDebugString

function mta_type (v)
    local tv = type(v)
    if tv == 'userdata' then
        if isElement(v) then
            return "elem:"..getElementType(v) --
        else
            return getUserdataType(v)
        end
    end
    return tv
end

function Inspector:putUserdata(v)
    local human = mta_type (v)
    local ptr = tostring(v):sub(11,-1)
    local o = ""
    if isElement(v) then
        etype = getElementType(v)
        if etype == "player" then
            o = "elem:"..etype.."["..getPlayerName(v).."]"
        elseif etype == "vehicle" then
            o = "elem:"..etype.."["..getVehicleName(v).."]"..ptr
        elseif etype == "object" then
            o = "elem:"..etype.."["..getElementModel(v).."]"..ptr
        elseif etype == "ped" then
            o = "elem:"..etype.."["..getElementModel(v).."]"..ptr
        elseif etype == "pickup" then
            local pType
            if getPickupType(v) == 0 then pType = "health"
            elseif getPickupType(v) == 1 then pType = "armor"
            elseif getPickupType(v) == 2 then pType = "weapon["..getWeaponNameFromID(getPickupWeapon(v)).."]"
            else pType = "custom["..getElementModel(v).."]"    end
            o = "elem:"..etype.."["..pType.."]"..ptr
        elseif etype == "marker" then
            o = "elem:"..etype.."["..getMarkerType (v).."]"..ptr
        elseif etype == "team" then
            o = "elem:"..etype.."["..getTeamName (v).."]"..ptr
        else
            o = "elem:"..etype..ptr
        end
    elseif human == "xml-node" then
        o = "xml-node["..xmlNodeGetName(v).."]"..ptr
    elseif human == "resource-data" then
        o = "resource["..getResourceName(v).."]"
    elseif human == "account" then
        o = "account["..getAccountName(v).."]"
    elseif human == "acl" then
        o = "acl["..aclGetName(v).."]"
    elseif human == "acl-group" then
        o = "acl-group["..aclGroupGetName(v).."]"
    elseif human == "vector4" or human == "vector3" or human == "vector2" or human == "matrix" then
        o = tostring(v)
    else
        o = human..ptr
    end

    self:puts(o)
end

function iprint(...)
    local arg = {...}
    for i = 1, select("#",...) do
        arg[i] = inspect(arg[i])
    end
    outputDebugString(table_concat(arg,"    "),3,180,180,180)
end

-------------------------------------------------------------------

function inspect.inspect(root, options)
  options       = options or {}

  local depth   = options.depth   or math.huge
  local newline = options.newline or '\n'
  local indent  = options.indent  or '  '
  local process = options.process

  if process then
    root = processRecursive(process, root, {}, {})
  end

  local inspector = setmetatable({
    depth            = depth,
    level            = 0,
    buffer           = {},
    ids              = {},
    maxIds           = {},
    newline          = newline,
    indent           = indent,
    tableAppearances = countTableAppearances(root)
  }, Inspector_mt)

  inspector:putValue(root)

  return table.concat(inspector.buffer)
end

setmetatable(inspect, { __call = function(_, ...) return inspect.inspect(...) end })


    )~LUA~";
}