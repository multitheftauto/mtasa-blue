namespace EmbeddedLuaCode
{
    const char* const constantsUI = R"~LUA~(
--[[
    SERVER AND CLIENT.
    Defines a constant variables available for server and client.
--]]

constants.HudComponents = setmetatable({
    All = 'all',
    Ammo = 'ammo',
    Area_name = 'area_name',
    Armour = 'armour',
    Breath = 'breath',
    Clock = 'clock',
    Health = 'health',
    Money = 'money',
    Radar = 'radar',
    Vehicle_name = 'vehicle_name',
    Weapon = 'weapon',
    Radio = 'radio',
    Wanted = 'wanted',
    Crosshair = 'crosshair',
}, __readonly_table_meta__)

constants.BlipIcons = setmetatable({
    Marker = 0,
    WhiteSquare = 1,
    Centre = 2,
    MapHere = 3,
    North = 4,
    Airyard = 5,
    Gun = 6,
    Barbers = 7,
    BigSmoke = 8,
    Boatyard = 9,
    Burgershot = 10,
    Bulldozer = 11,
    CatPink = 12,
    Cesar = 13,
    Chicken = 14,
    CJ = 15,
    Crash1 = 16,
    Diner = 17,
    EmmetGun = 18,
    EnemyAttack = 19,
    Fire = 20,
    Girlfriend = 21,
    Hospital = 22,
    Loco = 23,
    MaddDogg = 24,
    Mafia = 25,
    McStrap = 26,
    ModGarage = 27,
    OgLoc = 28,
    Pizza = 29,
    Police = 30,
    PropertyGreen = 31,
    PropertyRed = 32,
    Race = 33,
    Ryder = 34,
    Savehouse = 35,
    School = 36,
    Mystery = 37,
    Sweet = 38,
    Tattoo = 39,
    Truth = 40,
    Waypoint = 41,
    TorenoRanch = 42,
    Triads = 43,
    TriadsCasino = 44,
    TShirt = 45,
    Woozie = 46,
    Zero = 47,
    DateDisco = 48,
    DateDrink = 49,
    DateFood = 50,
    Truck = 51,
    Cash = 52,
    Flag = 53,
    Gym = 54,
    Impound = 55,
    RunwayLight = 56,
    Runway = 57,
    GangLosAztecas = 58,
    GangBallas = 59,
    GangVagos = 60,
    GangRifa = 61,
    GangFamilies = 62,
    Spray = 63,
}, __readonly_table_meta__)

constants.MarkerTypes = setmetatable({
    Checkpoint = 'checkpoint',
    Ring = 'ring',
    Cylinder = 'cylinder',
    Arrow = 'arrow',
    Corona = 'corona',
}, __readonly_table_meta__)

constants.PickupTypes = setmetatable({
    Health = 0,
    Armour = 1,
    Weapon = 2,
    Custom = 3
}, __readonly_table_meta__)

constants.ModelPickupTypes = setmetatable({
    Jetpack = 370,
    Money = 1212,
    InfoIcon = 1239,
    Health = 1240,
    Adrenaline = 1241,
    Armour = 1242,
    Bribe = 1247,
    GTA3Bomb = 1252,
    Photo = 1253,
    Skull = 1254,
    GTA3Sign = 1248,
    HouseBlue = 1272,
    HouseGreen = 1273,
    MoneyIcon = 1274,
    BlueTShirt = 1275,
    TikiStatue = 1276,
    SaveDisk = 1277,
    DrugBundle = 1279,
    Parachute = 1310,
    Skulls = 1313,
    PlayersIcon = 1314,
    DownArrow = 1318,
}, __readonly_table_meta__)

    )~LUA~";
}