-- Shared regression checks for deep native Lua-table traversal.
-- Start pr5051_target and pr5051_review, then run depthaudit_server in the server
-- console and depthaudit in client F8. Add "deep" for exports, events and timers.
local clientSide = localPlayer ~= nil
local function log(text)
    text = "[PR5051 " .. (clientSide and "client" or "server") .. "] " .. text
    outputDebugString(text)
    if clientSide then outputConsole(text) else outputServerLog(text) end
end

local function chain(total)
    local root = {}
    local current = root
    for i = 2, total do
        current.nested = {}
        current = current.nested
    end
    current.leaf = true
    return root
end

local function check(name, condition, actual)
    log((condition and "PASS " or "FAIL ") .. name .. " actual=" .. tostring(actual))
end

local function run(mode)
    local version = getVersion()
    log("START mode=" .. tostring(mode) .. " version=" .. tostring(version.sortable))
    if mode == "deep" then
        local root = {}
        local current = root
        for i = 1, 8000 do
            local nextTable = {level = i}
            current.nested = nextTable
            current = nextTable
        end
        local started = getTickCount()
        local json = toJSON(root)
        check("reviewer 8000 links", type(json) == "string" and #json == 230899,
            type(json) .. " bytes=" .. (type(json) == "string" and #json or 0) .. " ms=" .. (getTickCount() - started))
        check("usable after deep JSON", toJSON({ok = true}, true) == '[{"ok":true}]', "follow-up")
        local function intact(value)
            if type(value) ~= "table" then return false end
            for level = 1, 8000 do
                value = value.nested
                if type(value) ~= "table" or value.level ~= level then return false end
            end
            return true
        end
        local target = getResourceFromName("pr5051_target")
        local before, copied, after = call(target, "echo", "before", root, "after")
        check("deep export roundtrip", before == "before" and intact(copied) and after == "after", "8000 links")

        local eventName = "pr5051:deep"
        addEvent(eventName, false)
        local function onDeep(value)
            check("deep local event", intact(value), "8000 links")
        end
        addEventHandler(eventName, resourceRoot, onDeep, false)
        triggerEvent(eventName, resourceRoot, root)
        removeEventHandler(eventName, resourceRoot, onDeep)

        local allSerialized = true
        for iteration = 1, 5 do
            local repeated = toJSON(root)
            allSerialized = allSerialized and type(repeated) == "string" and #repeated == 230899
        end
        check("repeated deep conversion and cleanup", allSerialized, "5 repetitions")

        setTimer(function(first, value, last)
            check("deep timer copy", first == "before" and intact(value) and last == "after", "8000 links")
            log("END deep")
        end, 100, 1, "before", root, "after")
        return
    end

    -- PASS means the original supported value survives, not merely that a cutoff rejects it.
    for _, depth in ipairs({1, 16, 64, 65, 4096}) do
        local expected = "[" .. string.rep('{"nested":', depth - 1) .. '{"leaf":true}' .. string.rep("}", depth - 1) .. "]"
        local actual = toJSON(chain(depth), true)
        check("JSON total tables=" .. depth, actual == expected, type(actual) .. " bytes=" .. (type(actual) == "string" and #actual or 0))
    end
    local shared = {value = 7}
    local reference = {a = shared, b = shared}
    reference.self = reference
    local cyclic = toJSON(reference, true)
    check("cyclic JSON reference marker", type(cyclic) == "string" and cyclic:find("^T^", 1, true) ~= nil, cyclic)
    local target = getResourceFromName("pr5051_target")
    if not target or getResourceState(target) ~= "running" then
        log("FAIL helper resource must be running")
        return
    end
    local sameRoot, sameChild = call(target, "checkReferences", reference)
    check("export preserves cycle and alias", sameRoot == true and sameChild == true, tostring(sameRoot) .. "," .. tostring(sameChild))
    local count = call(target, "countArgs", "before", chain(65), "after")
    check("export argument count", count == 3, count)
    local function capture(...)
        return select("#", ...), ...
    end
    local returnCount, before, middle, after = capture(call(target, "deepReturn"))
    check("export return positions", returnCount == 3 and before == "before" and type(middle) == "table" and after == "after",
        tostring(returnCount) .. ":" .. tostring(before) .. "," .. type(middle) .. "," .. tostring(after))

    if clientSide then
        local grid = guiCreateGridList(0, 0, 400, 160, false)
        for i = 1, 3 do guiGridListAddColumn(grid, "Column " .. i, 0.3) end
        guiSetVisible(grid, false)
        local row = guiGridListAddRow(grid, 11, 22, "tail")
        local function checkRow(name, index, first, second, third)
            if type(index) ~= "number" then check(name, false, index); return end
            local a, b, c = guiGridListGetItemText(grid, index, 1), guiGridListGetItemText(grid, index, 2), guiGridListGetItemText(grid, index, 3)
            check(name, a == first and b == second and c == third, a .. "," .. b .. "," .. c)
        end
        checkRow("grid add numeric cells", row, "11", "22", "tail")
        local inserted = guiGridListInsertRowAfter(grid, row, 33, 44, "end")
        checkRow("grid insert numeric cells", inserted, "33", "44", "end")
        destroyElement(grid)
    end
    log("END standard")
end

if clientSide then
    addCommandHandler("depthaudit", function(_, mode) run(mode) end)
else
    addCommandHandler("depthaudit_server", function(_, _, mode) run(mode) end)
end

log("READY iterative-v1: use " .. (clientSide and "depthaudit [deep]" or "depthaudit_server [deep]"))
