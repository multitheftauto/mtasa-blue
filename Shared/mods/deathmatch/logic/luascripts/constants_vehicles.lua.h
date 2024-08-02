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
}, __readonly_table_meta__)

constants.VehicleLightState = setmetatable({
    Normal = 0,
    Broken = 1
}, __readonly_table_meta__)

constants.VehicleDoorType = setmetatable({
    Hood = 0,
    Trunk = 1,
    FrontLeft = 2,
    FrontRight = 3,
    RearLeft = 4,
    RearRight = 5
}, __readonly_table_meta__)

constants.VehicleDoorState = setmetatable({
    ClosedUndamaged = 0,
    OpenUndamaged = 1,
    ClosedDamaged = 2,
    OpenDamaged = 3,
    Missing = 4
}, __readonly_table_meta__)

constants.VehicleLightOverride = setmetatable({
    Disable = 0,
    Off = 1,
    On = 2
}, __readonly_table_meta__)

constants.CarPanel = setmetatable({
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3,
    Windscreen = 4,
    FrontBumper = 5,
    RearBumper = 6
}, __readonly_table_meta__)

constants.PlanePanels = setmetatable({
    LeftEngine = 0,
    RightEngine = 1,
    Rudder = 2,
    Elevators = 3,
    Ailerons = 4,
    --Unknown = 5,
    --Unknown = 6
}, __readonly_table_meta__)

constants.VehiclePanelState = setmetatable({
    Undamaged = 0,
    SlightlyDamaged = 1,
    Damaged = 2,
    HeavilyDamaged = 3
}, __readonly_table_meta__)

constants.vehicleWheelState = setmetatable({
    Inflated = 0,
    Flat = 1,
    FallenOff = 2,
    Collisionless = 3
}, __readonly_table_meta__)

constants.VehicleSeat = setmetatable({
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3
}, __readonly_table_meta__)

constants.TrafficLightState = setmetatable({
    Auto = 'auto',
    Disabled = 'disabled',
    Red = 'red',
    Yellow = 'yellow',
    Green = 'green'
}, __readonly_table_meta__)

    )~LUA~";
}