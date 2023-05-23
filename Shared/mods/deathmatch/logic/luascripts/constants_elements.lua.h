namespace EmbeddedLuaCode
{
    const char* const constantsElements = R"~LUA~(
--[[
    SERVER AND CLIENT.

    Defines a constant variables available for server and client.
--]]

local _elementType = {
    Player = 'Player',
    Ped = 'Ped',
    Vehicle = 'Vehicle',
    Object = 'Object',
    Pickup = 'Pickup',
    Marker = 'Marker',
    CollisionShape = 'Collision shape',
    Blip = 'Blip',
    RadarArea = 'Radar area',
    Projectile = 'Projectile',
    Team = 'Team',
    ServerConsole = 'Server console',
    Button = 'Button',
    Checkbox = 'Checkbox',
    Combobox = 'Combobox',
    EditField = 'Edit field',
    Gridlist = 'Gridlist',
    Memo = 'Memo',
    ProgressBar = 'Progress bar',
    RadioButton = 'Radio button',
    Scrollbar = 'Scrollbar',
    Scrollpane = 'Scrollpane',
    StaticImage = 'Static image',
    TabPanel = 'Tab panel',
    Tab = 'Tab',
    TextLabel = 'Text label',
    Window = 'Window',
    TXD = 'TXD',
    DFF = 'DFF',
    COL = 'COL',
    Sound = 'Sound',
    Texture = 'Texture',
    Shader = 'Shader',
    DXFont = 'DX font',
    GUIFont = 'GUI font',
    Weapon = 'Weapon',
    Camera = 'Camera',
    Effect = 'Effect',
    Browser = 'Browser',
    Light = 'Light',
    Searchlight = 'Searchlight',
    Water = 'Water',
}

constants.ElementType = _elementType
    )~LUA~";
}
