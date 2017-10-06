require 'utils'

premake.modules.install_data = {}

-- Config variables
local BIN_DIR = "Bin"
local DATA_DIR = "Shared/data/MTA San Andreas"

local NET_PATH_X86_WIN = "https://mirror.mtasa.com/bdata/net.dll"
local NET_PATH_X64_WIN = "https://mirror.mtasa.com/bdata/net_64.dll"
local NETC_PATH_WIN = "https://mirror.mtasa.com/bdata/netc.dll"

local NET_PATH_X86_LINUX = "https://mirror.mtasa.com/bdata/net.so"
local NET_PATH_X64_LINUX = "https://mirror.mtasa.com/bdata/net_64.so"

newaction {
	trigger = "install_data",
	description = "Installs data files",
	
	execute = function()
		-- Make Bin directory if not exists
		os.mkdir(BIN_DIR)
		
		-- Copy data files
		if os.host() == "windows" then
			os.copydir(DATA_DIR, BIN_DIR)
		end

		-- Copy configs if they don't already exist
		os.copydir("Server/mods/deathmatch", BIN_DIR.."/server/mods/deathmatch", "*.conf", false, true)
		os.copydir("Server/mods/deathmatch", BIN_DIR.."/server/mods/deathmatch", "*.xml", false, true)
		
		-- Make sure server/x64 directory exists
		os.mkdir(BIN_DIR.."/server/x64")
		
		if os.host() == "windows" then
			http.download_print_errors(NET_PATH_X86_WIN, BIN_DIR.."/server/net.dll")
			http.download_print_errors(NET_PATH_X64_WIN, BIN_DIR.."/server/x64/net.dll")
			http.download_print_errors(NETC_PATH_WIN, BIN_DIR.."/MTA/netc.dll")
			
			os.copyfile(BIN_DIR.."/MTA/netc.dll", BIN_DIR.."/MTA/netc_d.dll")
			os.copyfile(BIN_DIR.."/server/net.dll", BIN_DIR.."/server/net_d.dll")
			os.copyfile(BIN_DIR.."/server/x64/net.dll", BIN_DIR.."/server/x64/net_d.dll")
		else
			http.download_print_errors(NET_PATH_X86_LINUX, BIN_DIR.."/server/net.so")
			http.download_print_errors(NET_PATH_X64_LINUX, BIN_DIR.."/server/x64/net.so")
			
			os.copyfile(BIN_DIR.."/server/net.so", BIN_DIR.."/server/net_d.so")
			os.copyfile(BIN_DIR.."/server/x64/net.so", BIN_DIR.."/server/x64/net_d.so")
		end
	end
}

return premake.modules.install_data
