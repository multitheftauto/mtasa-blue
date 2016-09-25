require 'utils'

premake.modules.install_data = {}

-- Config variables
local BIN_DIR = "Bin"
local DATA_DIR = "Shared/data/MTA San Andreas"

-- TODO
local NET_PATH_X86_WIN = "https://mirror.mtasa.com/bdata/net.dll"
--local NET_PATH_X64_WIN = "https://"
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
		if os.get() == "windows" then
			os.copydir(DATA_DIR, BIN_DIR)
		end

		-- Copy configs
		os.copydir("Server/mods/deathmatch", BIN_DIR.."/server/mods/deathmatch", "*.conf")
		os.copydir("Server/mods/deathmatch", BIN_DIR.."/server/mods/deathmatch", "*.xml")

		if os.get() == "windows" then
			http.download(NET_PATH_X86_WIN, BIN_DIR.."/server/net.dll")
			--http.download(NET_PATH_X64_WIN, BIN_DIR.."/server/x64/mods/net.dll")
			http.download(NETC_PATH_WIN, BIN_DIR.."/MTA/netc.dll")
			
			os.copyfile(BIN_DIR.."/MTA/netc.dll", BIN_DIR.."/MTA/netc_d.dll")
			os.copyfile(BIN_DIR.."/server/net.dll", BIN_DIR.."/server/net_d.dll")
			--os.copyfile("/server/x64/net.dll", "/server/x64/net_d.dll")
		else
			http.download(NET_PATH_X86_LINUX, BIN_DIR.."/server/net.so")
			http.download(NET_PATH_X64_LINUX, BIN_DIR.."/server/x64/mods/net.so")
			
			os.copyfile(BIN_DIR.."/server/net.so", BIN_DIR.."/server/net_d.so")
			os.copyfile(BIN_DIR.."/server/x64/net.so", BIN_DIR.."/server/x64/net_d.so")
		end
	end
}

return premake.modules.install_data
