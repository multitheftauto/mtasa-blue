require 'utils'

premake.modules.compose_files = {}

-- Config variables
local BIN_DIR = "Bin"
local DATA_DIR = "Shared/data/MTA San Andreas"
local OUTPUT_DIR = "InstallFiles"

newaction {
	trigger = "compose_files",
	description = "Composes files that are required for building the installer",
	
	execute = function()
		os.mkdir(OUTPUT_DIR)
		
		-- Copy data files
		os.copydir(DATA_DIR.."/MTA", OUTPUT_DIR.."/MTA")
		os.copydir(DATA_DIR.."/skins/Classic", OUTPUT_DIR.."/skins/Default", "*.xml")
		os.copydir(DATA_DIR.."/server/mods/deathmatch", OUTPUT_DIR.."/server/mods/deathmatch", "*.dll")
		os.copydir(DATA_DIR.."/redist", OUTPUT_DIR.."/redist")
		
		-- Copy configs
		os.copydir("Server/mods/deathmatch", OUTPUT_DIR.."/server/mods/deathmatch", "*.conf")
		os.copydir("Server/mods/deathmatch", OUTPUT_DIR.."/server/mods/deathmatch", "*.xml")
		
		-- Copy compiled binaries
		os.copydir(BIN_DIR, OUTPUT_DIR, "**.exe")
		os.copydir(BIN_DIR, OUTPUT_DIR, "**.dll")
	end
}

return premake.modules.compose_files
