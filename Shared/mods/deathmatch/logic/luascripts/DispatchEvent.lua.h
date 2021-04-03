namespace EmbeddedLuaCode
{
    // Special function for calling event handlers
    // We use a script because doing it in C++ is slower (and way more complex)
    constexpr auto DispatchEvent = R"~LUA~(
function DispatchEvent(handlerfn, _source, _this, _client, _sourceResource, _sourceResourceRoot, _eventName, ...)
    --[[
    print("DispatchEvent with args:", inspect({
        handlerfn = handlerfn, 
        source = _source or "null", 
        this = _this or "null", 
        client = _client or "null", 
        sourceResource = _sourceResource or "null", 
        sourceResourceRoot = _sourceResourceRoot or "null", 
        eventName = _eventName or "null", 
    }))
    ]]

    -- save old globals
    local oldSource = source
    local oldThis = this
    local oldClient = client
    local oldSourceResource = sourceResource
    local oldSourceResourceRoot = sourceResourceRoot
    local oldEventName = eventName

    -- set globals of this event
    source = _source
    this = _this
    client = _client
    sourceResource = _sourceResource
    sourceResourceRoot = _sourceResourceRoot
    eventName = _eventName

    handlerfn(...)

    -- restore globals
    source = oldSource
    this = oldThis
    client = oldClient
    sourceResource = oldSourceResource
    sourceResourceRoot = oldSourceResourceRoot
    eventName = oldEventName
end
    )~LUA~";
};
