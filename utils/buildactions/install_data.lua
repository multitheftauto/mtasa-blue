require 'utils'

premake.modules.install_data = {}

-- Config variables
local BIN_DIR = "Bin"
local DATA_DIR = "Shared/data/MTA San Andreas"

-- TODO
local NET_PATH_X86_WIN = "https://"
--local NET_PATH_X64_WIN = "https://"
local NET_PATH_X86_LINUX = "https://"
--local NET_PATH_X64_LINUX = "https://"
local NETC_PATH_WIN = "https://"

newaction {
	trigger = "install_data",
	description = "Installs data files",
	
	execute = function()
		-- Make Bin directory if not exists
		os.mkdir(BIN_DIR)
		
		-- Copy data files
		os.copydir(DATA_DIR, BIN_DIR)

		if false then -- TODO
			if os.get() == "windows" then
				http.download(NET_PATH_X86_WIN, BIN_DIR.."/server/net.dll")
				--http.download(NET_PATH_X64_WIN, BIN_DIR.."/server/x64/mods/net.dll")
				http.download(NETC_PATH_WIN, BIN_DIR.."/MTA/netc.dll")
				
				os.copyfile("/MTA/netc.dll", "/MTA/netc_d.dll")
				os.copyfile("/server/net.dll", "/server/net_d.dll")
				--os.copyfile("/server/x64/net.dll", "/server/x64/net_d.dll")
			else
				http.download(NET_PATH_X86_LINUX, BIN_DIR.."/server/net.so")
				--http.download(NET_PATH_X64_LINUX, BIN_DIR.."/server/x64/mods/net.so")
				
				os.copyfile("/server/net.so", "/server/net_d.so")
				--os.copyfile("/server/x64/net.so", "/server/x64/net_d.so")
			end
		end
	end
}

return premake.modules.install_data
