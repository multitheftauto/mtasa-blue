namespace EmbeddedLuaCode
{
    const char* const constantsElements = R"~LUA~(
--[[
    SERVER AND CLIENT.
    Defines a constant variables available for server and client.
--]]

constants.ElementType = setmetatable({
    Player = 'player',
    Ped = 'ped',
    Water = 'water',
    Sound = 'sound',
    Vehicle = 'vehicle',
    Object = 'object',
    Pickup = 'pickup',
    Marker = 'marker',
    Colshape = 'colshape',
    Blip = 'blip',
    RadarArea = 'radararea',
    Team = 'team',
    SpawnPoint = 'spawnpoint',
    Console = 'console',
    Projectile = 'projectile',
    Effect = 'effect',
    Light = 'light',
    Searchlight = 'searchlight',
    Shader = 'shader',
    Texture = 'texture',
}, __readonly_table_meta__)

    )~LUA~";
}