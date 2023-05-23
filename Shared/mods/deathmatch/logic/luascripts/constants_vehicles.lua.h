namespace EmbeddedLuaCode
{
    const char* const constantsVehicles = R"~LUA~(
--[[
    SERVER AND CLIENT.

    Defines a constant variables available for server and client.
--]]

local _vehicleLights = {
    FrontLeft = 0,
    FrontRight = 1,
    RearRight = 2,
    RearLeft = 3
}
local _vehicleLightState = {
    Normal = 0,
    Broken = 1
}
local _vehicleDoorType = {
    Hood = 0,
    Trunk = 1,
    FrontLeft = 2,
    FrontRight = 3,
    RearLeft = 4,
    RearRight = 5
}
local _vehicleDoors = {
    ClosedUndamaged = 0,
    OpenUndamaged = 1,
    ClosedDamaged = 2,
    OpenDamaged = 3,
    Missing = 4
}
local _vehicleLightOverride = {
    Disable = 0,
    Off = 1,
    On = 2
}
local _carPanels = {
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3,
    Windscreen = 4,
    FrontBumper = 5,
    RearBumper = 6
}
local _planePanels = {
    LeftEngine = 0,
    RightEngine = 1,
    Rudder = 2,
    Elevators = 3,
    Ailerons = 4,
    --Unknown = 5,
    --Unknown = 6
}
local _vehiclePanelState = {
    Undamaged = 0,
    SlightlyDamaged = 1,
    Damaged = 2,
    HeavilyDamaged = 3
}
local _vehicleWheelState = {
    Inflated = 0,
    Flat = 1,
    Fallen off = 2,
    Collisionless = 3
}
local _vehicleSeat = {
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3
}

constants.VehicleLights = _vehicleLights
constants.VehicleLightState = _vehicleLightState
constants.VehicleLightOverride = _vehicleLightOverride
constants.VehicleDoors = _vehicleDoors
constants.VehicleDoorState = _vehicleDoorState
constants.VehiclePanelState = _vehiclePanelState
constants.CarPanels = _carPanels
constants.PlanePanels = _planePanels
constants.VehicleSeat = _vehicleSeat
    )~LUA~";
}
