namespace EmbeddedLuaCode
{
    const char* const constantsVehicles = R"~LUA~(
--[[
    SERVER AND CLIENT.
    Defines a constant variables available for server and client.
--]]

constants.VehicleLights = setmetatable({
    FrontLeft = 0,
    FrontRight = 1,
    RearRight = 2,
    RearLeft = 3
}, metatable)

constants.VehicleLightState = setmetatable({
    Normal = 0,
    Broken = 1
}, metatable)

constants.VehicleDoorType = setmetatable({
    Hood = 0,
    Trunk = 1,
    FrontLeft = 2,
    FrontRight = 3,
    RearLeft = 4,
    RearRight = 5
}, metatable)

constants.VehicleDoorState = setmetatable({
    ClosedUndamaged = 0,
    OpenUndamaged = 1,
    ClosedDamaged = 2,
    OpenDamaged = 3,
    Missing = 4
}, metatable)

constants.VehicleLightOverride = setmetatable({
    Disable = 0,
    Off = 1,
    On = 2
}, metatable)

constants.CarPanel = setmetatable({
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3,
    Windscreen = 4,
    FrontBumper = 5,
    RearBumper = 6
}, metatable)

constants.PlanePanels = setmetatable({
    LeftEngine = 0,
    RightEngine = 1,
    Rudder = 2,
    Elevators = 3,
    Ailerons = 4,
    --Unknown = 5,
    --Unknown = 6
}, metatable)

constants.VehiclePanelState = setmetatable({
    Undamaged = 0,
    SlightlyDamaged = 1,
    Damaged = 2,
    HeavilyDamaged = 3
}, metatable)

constants.vehicleWheelState = setmetatable({
    Inflated = 0,
    Flat = 1,
    FallenOff = 2,
    Collisionless = 3
}, metatable)

constants.VehicleSeat = setmetatable({
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3
}, metatable)

constants.TrafficLightState = setmetatable({
    Auto = 'auto',
    Disabled = 'disabled',
    Red = 'red',
    Yellow = 'yellow',
    Green = 'green'
}, metatable)

    )~LUA~";
}