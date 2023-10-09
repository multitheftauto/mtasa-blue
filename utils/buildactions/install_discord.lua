require 'utils'

premake.modules.install_discord = {}

-- Config variables
local DISCORD_PATH = "vendor/discord-rpc/discord/"
local DISCORD_TEMP = "vendor/discord-rpc/discord-rpc.zip"
local DISCORD_UPDATE = "https://api.github.com/repos/multitheftauto/discord-rpc/releases/latest"
local DISCORD_URL = "https://github.com/multitheftauto/discord-rpc/archive/refs/tags/"
local DISCORD_EXT = ".zip"
local RAPID_PATH = "vendor/discord-rpc/discord/thirdparty/rapidjson/"
local RAPID_TEMP = "vendor/discord-rpc/rapidjson.zip"
local RAPID_UPDATE = "https://api.github.com/repos/multitheftauto/rapidjson/releases/latest"
local RAPID_URL = "https://github.com/multitheftauto/rapidjson/archive/refs/tags/"
local RAPID_EXT = ".zip"

-- Auto-update variables
local DISCORD_VERSION = "v3.4.3"
local DISCORD_HASH = "dacfcf9ac6f005923eef55b4e41f0e46dc64f7a25da88e00faeef86e8553f32f"
local RAPID_VERSION = "v1.1.1"
local RAPID_HASH = "3d638ad2549645a4831e8e98cf7b919f191de0d60816e63e1d5aa08cd56e6db8"

function make_download_url(url, version, ext)
	return url..http.escapeUrlParam(version)..ext
end

function update_install_discord(variable, version, hash)
	local filename = "utils/buildactions/install_discord.lua"
	local f = io.open(filename)
	local text = f:read("*all")
	f:close()

	-- Replace version and hash lines
	local version_line = 'local '.. variable ..'_VERSION = "' .. version .. '"'
	local hash_line = 'local '.. variable ..'_HASH = "' .. hash .. '"'
	text = text:gsub('local '.. variable ..'_VERSION = ".-"', version_line, 1)
	text = text:gsub('local '.. variable ..'_HASH = ".-"', hash_line, 1)

	local f = io.open(filename, "w")
	f:write(text)
	f:close()
end

local function check_github_update(name, url, version)
	print("Checking github for ".. name .. " update...")
	local resource, result_str, result_code = http.get(url)
	if result_str ~= "OK" or result_code ~= 200 then
		errormsg(("Could not get page with status code %s: "):format(response_code), result_str)
		os.exit(1)
		return
	end

	local meta, err = json.decode(resource)
	if err then
		errormsg("Could not parse json meta data:", err)
		os.exit(1)
		return
	end

	if meta["tag_name"] == version then
		print((name .. " is already up to date (%s)"):format(meta["tag_name"]))
		return false
	end

	io.write(("Does version '%s' look OK to you? (Y/n) "):format(meta["tag_name"]))
	local input = io.read():lower()
	if not (input == "y" or input == "yes") then
		errormsg("Aborting due to user request.")
		return false
	end
	return meta["tag_name"]
end

local function check_discord(should_upgrade)
	local has_discord_dir = os.isdir(DISCORD_PATH)

	-- Check file hash
	local archive_path = DISCORD_TEMP
	local hash_passed = os.isfile(archive_path) and os.sha256_file(archive_path) == DISCORD_HASH
	if hash_passed then
		print("discord-rpc consistency checks succeeded")

		if has_discord_dir then
			return
		end
	else
		-- Download discord-rpc
		print("Downloading discord-rpc " .. DISCORD_VERSION ..  "...")
		if not http.download_print_errors(make_download_url(DISCORD_URL, DISCORD_VERSION, DISCORD_EXT), archive_path) then
			os.exit(1)
			return
		end
	end

	local downloaded_hash = os.sha256_file(archive_path)
	if should_upgrade then
		print("New discord-rpc hash is:", downloaded_hash)
		DISCORD_HASH = downloaded_hash

		io.write(("Update `install_discord.lua` file? (Y/n) "):format(version))
		local input = io.read():lower()
		if (input == "y" or input == "yes") then
			update_install_discord("DISCORD", DISCORD_VERSION, downloaded_hash)
		end
	end

	if downloaded_hash == DISCORD_HASH then
		print("discord-rpc consistency checks succeeded")
	else
		errormsg("discord-rpc consistency checks failed.", ("Expected %s, got %s"):format(DISCORD_HASH, downloaded_hash))
		os.exit(1)
		return
	end

	-- Seriously abort now if we're not using Windows
	if os.host() ~= "windows" then
		return
	end

	-- Delete old discord-rpc files
	if has_discord_dir then
		if not os.rmdir(DISCORD_PATH) then
			errormsg("ERROR: Could not delete discord folder")
			os.exit(1)
			return
		end
	end

	if not os.mkdir(DISCORD_PATH) then
		errormsg("ERROR: Could not create discord folder (2)")
		os.exit(1)
		return
	end

	-- Extract zip
	if not os.extract_archive(archive_path, DISCORD_PATH, true) then
		errormsg("ERROR: Could not extract .zip")
		os.exit(1)
		return
	end

	-- Move all files from discord-rpc*/* to ./
	os.expanddir_wildcard(DISCORD_PATH.."discord-rpc*", DISCORD_PATH)
end

local function check_rapid(should_upgrade)
	local has_rapid_dir = os.isdir(RAPID_PATH)

	-- Check file hash
	local archive_path = RAPID_TEMP
	local hash_passed = os.isfile(archive_path) and os.sha256_file(archive_path) == RAPID_HASH
	if hash_passed then
		print("rapidjson consistency checks succeeded")

		if has_rapid_dir then
			return
		end
	else
		-- Download rapidjson
		print("Downloading rapidjson " .. RAPID_VERSION ..  "...")
		if not http.download_print_errors(make_download_url(RAPID_URL, RAPID_VERSION, RAPID_EXT), archive_path) then
			os.exit(1)
			return
		end
	end

	local downloaded_hash = os.sha256_file(archive_path)
	if should_upgrade then
		print("New rapidjson hash is:", downloaded_hash)
		RAPID_HASH = downloaded_hash

		io.write(("Update `install_discord.lua` file? (Y/n) "):format(version))
		local input = io.read():lower()
		if (input == "y" or input == "yes") then
			update_install_discord("RAPID", RAPID_VERSION, downloaded_hash)
		end
	end

	if downloaded_hash == RAPID_HASH then
		print("rapidjson consistency checks succeeded")
	else
		errormsg("rapidjson consistency checks failed.", ("Expected %s, got %s"):format(RAPID_HASH, downloaded_hash))
		os.exit(1)
		return
	end

	-- Seriously abort now if we're not using Windows
	if os.host() ~= "windows" then
		return
	end

	-- Delete old rapidjson files
	if has_discord_dir then
		if not os.rmdir(RAPID_PATH) then
			errormsg("ERROR: Could not delete rapidjson folder")
			os.exit(1)
			return
		end
	end

	if not os.mkdir(RAPID_PATH) then
		errormsg("ERROR: Could not create rapidjson folder (2)")
		os.exit(1)
		return
	end

	-- Extract zip
	if not os.extract_archive(archive_path, RAPID_PATH, true) then
		errormsg("ERROR: Could not extract .zip")
		os.exit(1)
		return
	end

	-- Move all files from rapidjson*/* to ./
	os.expanddir_wildcard(RAPID_PATH.."rapidjson*", RAPID_PATH)
end

newaction {
	trigger = "install_discord",
	description = "Downloads and installs discord-rpc",

	execute = function(...)
		local should_upgrade = _ARGS[1] == "upgrade"
		if should_upgrade then
			-- discord-rpc
			local discord = check_github_update("discord-rpc", DISCORD_UPDATE, DISCORD_VERSION)
			if discord then
				DISCORD_VERSION = discord
				DISCORD_HASH = ""
			end

			-- rapidjson
			local rapidjson = check_github_update("rapidjson", RAPID_UPDATE, RAPID_VERSION)
			if rapidjson then
				RAPID_VERSION = rapidjson
				RAPID_HASH = ""
			end
		end

		-- Only execute on Windows in normal scenarios
		if os.host() ~= "windows" and not should_upgrade then
			return
		end

		check_discord(should_upgrade)
		check_rapid(should_upgrade)
	end
}

return premake.modules.install_discord