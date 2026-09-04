/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luascripts/async_coroutine.lua.h
 *  PURPOSE:     Embedded Lua primitive helpers for async/await and non-blocking sleep
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

namespace EmbeddedLuaCode
{
    const char* const async_coroutine = R"~LUA~(

--[[
    SERVER AND CLIENT.

    Native Async/Await and non-blocking sleep primitives using Lua coroutines.
    Eliminates callback hell when chaining asynchronous MTA functions (fetchRemote,
    dbQuery, fileGetContentsAsync, passwordHash, etc.).
--]]

local type = type
local tostring = tostring
local tonumber = tonumber
local unpack = unpack
local pcall = pcall
local error = error
local table_insert = table.insert
local outputDebugString = outputDebugString
local setTimer = setTimer
local debug_traceback = debug and debug.traceback

-- Internal coroutine runner
local _coroutine_create = coroutine.create
local _coroutine_running = coroutine.running
local _coroutine_yield = coroutine.yield
local _coroutine_resume = coroutine.resume

-- Known MTA async functions with fixed callback argument positions
local mtaCallbackPositions = {
    dbQuery = 1,
    dbExec = 1,
    fileGetContentsAsync = 2,
    fileReadAsync = 3,
    fromJSONAsync = 2,
    passwordHash = 4,
    passwordVerify = 4,
}

function Async(fn, ...)
    if type(fn) ~= "function" then
        outputDebugString("[Async] First argument must be a function.", 1)
        return nil
    end

    local co = _coroutine_create(fn)
    local results = { _coroutine_resume(co, ...) }
    if not results[1] then
        local err = results[2]
        if debug_traceback then
            err = debug_traceback(co, tostring(err))
        end
        outputDebugString("[Async Error] " .. tostring(err), 1)
    end
    return co
end

function await(asyncFunc, ...)
    local co = _coroutine_running()
    if not co then
        error("await() must be called inside an Async() coroutine context.", 2)
    end

    local funcName = nil
    if type(asyncFunc) == "string" then
        funcName = asyncFunc
        asyncFunc = _G[funcName]
    end

    if type(asyncFunc) ~= "function" then
        error("await() expected a function or valid function name as first argument.", 2)
    end

    local args = { ... }
    local hasResumed = false

    local function asyncCallback(...)
        if not hasResumed then
            hasResumed = true
            local cbArgs = { ... }
            local results = { _coroutine_resume(co, unpack(cbArgs)) }
            if not results[1] then
                local err = results[2]
                if debug_traceback then
                    err = debug_traceback(co, tostring(err))
                end
                outputDebugString("[Async Await Callback Error] " .. tostring(err), 1)
            end
        end
    end

    -- Determine callback parameter position
    local pos = nil
    if funcName and mtaCallbackPositions[funcName] then
        pos = mtaCallbackPositions[funcName]
    elseif funcName == "fetchRemote" then
        -- fetchRemote(url, callback) or fetchRemote(url, options, callback)
        pos = (type(args[2]) == "table" or type(args[2]) == "string") and 3 or 2
    end

    if pos and pos <= (#args + 1) then
        table_insert(args, pos, asyncCallback)
    else
        table_insert(args, asyncCallback)
    end

    local ok, callResult = pcall(asyncFunc, unpack(args))
    if not ok then
        error("await() failed to execute async function: " .. tostring(callResult), 2)
    elseif callResult == false and not hasResumed then
        -- If the function returned false immediately and did not schedule callback
        hasResumed = true
        return nil, "Async operation failed to start."
    end

    return _coroutine_yield()
end

function sleep(ms)
    local co = _coroutine_running()
    if not co then
        error("sleep() must be called inside an Async() coroutine context.", 2)
    end

    ms = tonumber(ms) or 0
    if ms < 0 then ms = 0 end

    setTimer(function()
        local results = { _coroutine_resume(co) }
        if not results[1] then
            local err = results[2]
            if debug_traceback then
                err = debug_traceback(co, tostring(err))
            end
            outputDebugString("[Async Sleep Error] " .. tostring(err), 1)
        end
    end, ms, 1)

    return _coroutine_yield()
end

    )~LUA~";
}
