require 'utils'

premake.modules.compose_files = {}

-- Config variables
local BIN_DIR = "Bin"
local DATA_DIR = "Shared/data/MTA San Andreas"
local OUTPUT_DIR = "InstallFiles"
local WINDOWS = os.host() == "windows"

newaction {
	trigger = "compose_files",
	description = "Composes files that are required for building the installer",
	
	execute = function()
		os.mkdir(OUTPUT_DIR)
		
		-- Copy data files
		if WINDOWS then
			os.copydir(DATA_DIR.."/MTA", OUTPUT_DIR.."/MTA")
			os.copydir(DATA_DIR.."/skins", OUTPUT_DIR.."/skins")
			os.copydir(DATA_DIR.."/server/mods/deathmatch", OUTPUT_DIR.."/server/mods/deathmatch", "*.dll")
			os.copydir(DATA_DIR.."/redist", OUTPUT_DIR.."/redist")
		end

		-- Copy configs
		os.copydir("Server/mods/deathmatch", OUTPUT_DIR.."/server/mods/deathmatch", "*.conf")
		os.copydir("Server/mods/deathmatch", OUTPUT_DIR.."/server/mods/deathmatch", "mtaserver.conf.template")
		os.copydir("Server/mods/deathmatch", OUTPUT_DIR.."/server/mods/deathmatch", "*.xml")
		
		-- Copy compiled binaries
		if WINDOWS then
			os.copydir(BIN_DIR, OUTPUT_DIR, "**.exe")
			os.copydir(BIN_DIR, OUTPUT_DIR, "**.dll")
			os.copydir(BIN_DIR, OUTPUT_DIR, "**.pak")
			os.copydir(BIN_DIR, OUTPUT_DIR, "**.bin")
			os.copydir(BIN_DIR, OUTPUT_DIR, "**.dat")
		else
			os.copydir(BIN_DIR, OUTPUT_DIR, "**.so")
			os.copydir(BIN_DIR, OUTPUT_DIR, "**mta-server*")
		end

		-- Copy publicsdk
		-- os.copydir("Shared/publicsdk", OUTPUT_DIR.."/development/publicsdk")
	end
}

return premake.modules.compose_files
