require 'utils'

premake.modules.install_discord = {}

-- Config variables
local DISCORD_BASEURL = "https://github.com/qaisjp/discord_game_sdk/releases/download/"
local DISCORD_FILENAME = "cpp_x86.zip"
local DISCORD_VENDOR = "vendor/discordgsdk"

-- Change these to update the version
local DISCORD_TAG = "v2020-11-02_21-48-56"
local DISCORD_MD5 = "3d7b86f7fee560d85de3d6e9bac1efbb"

newaction {
	trigger = "install_discord",
	description = "Downloads and installs Discord SDK",

	execute = function()
		-- Only execute on Windows
		if os.host() ~= "windows" then return end

		-- Check md5
		local download_discord = true
		local archive_path = DISCORD_VENDOR.."/"..DISCORD_FILENAME
		if os.isfile(archive_path) and os.md5_file(archive_path) == DISCORD_MD5 then
			print("Discord Game SDK is up to date.")
			download_discord = false
		end

		-- Download Discord
		if download_discord then
			print("Downloading Discord Game SDK...")
			if not http.download_print_errors(DISCORD_BASEURL..DISCORD_TAG.."/"..DISCORD_FILENAME, archive_path) then
				return
			end
		end

		-- Delete old stuff
		os.rmdir(DISCORD_VENDOR.."/cpp")
		os.rmdir(DISCORD_VENDOR.."/lib")
		os.remove("Shared/data/MTA San Andreas/MTA/discord_game_sdk.dll")

		-- We cannot use zip.extract here because it ends up being read only
		-- see https://github.com/premake/premake-core/blob/master/src/host/zip_extract.c#L206-L210
		os.executef("powershell -Command Expand-Archive \"%s\" \"%s\"", archive_path, DISCORD_VENDOR)

		-- Move DLL to shared data folder
		os.copyfile(DISCORD_VENDOR.."/lib/x86/discord_game_sdk.dll", "Shared/data/MTA San Andreas/MTA/discord_game_sdk.dll")
		os.remove(DISCORD_VENDOR.."lib/x86/discord_game_sdk.dll")

		print("Discord Game SDK updated. Don't forget to run win-install-data.bat.")
	end
}

return premake.modules.install_discord
