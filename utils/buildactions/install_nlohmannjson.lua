require 'utils'

premake.modules.install_nlohmannjson = {}

-- Config variables
local NLOHMANN_PATH = "vendor/nlohmann-json/"
local NLOHMANN_TEMP = "vendor/nlohmann-json/tmp.zip"
local NLOHMANN_UPDATE = "https://api.github.com/repos/znvjder/nlohmann-json/releases/latest"
local NLOHMANN_URL = "https://github.com/znvjder/nlohmann-json/archive/refs/tags/"
local NLOHMANN_EXT = ".zip"

-- Auto-update variables
local NLOHMANN_VERSION = "v3.11.3"
local NLOHMANN_HASH = "28e4fcbdff3b07e4e3b32ba5a954af5c20f2563c818ad62f9294b9da5fdd6603"

function make_download_url(url, version, ext)
	return url..http.escapeUrlParam(version)..ext
end

function update_install_nlohmann_json(variable, version, hash)
	local filename = "utils/buildactions/install_nlohmannjson.lua"
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

local function check_nlohmann(should_upgrade)
	local has_NLOHMANN_dir = os.isdir(NLOHMANN_PATH)

	-- Check file hash
	local archive_path = NLOHMANN_TEMP
	local hash_passed = os.isfile(archive_path) and os.sha256_file(archive_path) == NLOHMANN_HASH
	if hash_passed then
		print("nlohmann-json consistency checks succeeded")
		if has_NLOHMANN_dir then
			return
		end
	else
		-- Download nlohmannjson
		print("Downloading nlohmann-json " .. NLOHMANN_VERSION ..  "...")
		if not http.download_print_errors(make_download_url(NLOHMANN_URL, NLOHMANN_VERSION, NLOHMANN_EXT), archive_path) then
			os.exit(1)
			return
		end
	end

	local downloaded_hash = os.sha256_file(archive_path)
	if should_upgrade then
		print("New nlohmann-json hash is:", downloaded_hash)
		NLOHMANN_HASH = downloaded_hash

		io.write(("Update `install_nlohmannjson.lua` file? (Y/n) "):format(version))
		local input = io.read():lower()
		if (input == "y" or input == "yes") then
			update_install_nlohmann_json("NLOHMANNJSON", NLOHMANN_VERSION, downloaded_hash)
		end
	end

	if downloaded_hash == NLOHMANN_HASH then
		print("nlohmann-json consistency checks succeeded")
	else
		errormsg("nlohmann-json consistency checks failed.", ("Expected %s, got %s"):format(NLOHMANN_HASH, downloaded_hash))
		os.exit(1)
		return
	end

	-- Seriously abort now if we're not using Windows
	if os.host() ~= "windows" then
		return
	end

	-- Delete old nlohmann-json files
	if has_discord_dir then
		if not os.rmdir(NLOHMANN_PATH) then
			errormsg("ERROR: Could not delete nlohmann-json folder")
			os.exit(1)
			return
		end
	end

	if not os.mkdir(NLOHMANN_PATH) then
		errormsg("ERROR: Could not create nlohmann-json folder (2)")
		os.exit(1)
		return
	end

	-- Extract zip
	if not os.extract_archive(archive_path, NLOHMANN_PATH, true) then
		errormsg("ERROR: Could not extract .zip")
		os.exit(1)
		return
	end

	-- Move all files from nlohmann-json*/* to ./
	os.expanddir_wildcard(NLOHMANN_PATH.."nlohmann-json*", NLOHMANN_PATH)
end

newaction {
	trigger = "install_nlohmannjson",
	description = "Downloads and installs nlohmann-json",

	execute = function(...)
		local should_upgrade = _ARGS[1] == "upgrade"
		if should_upgrade then
			-- nlohmann-json
			local str_version = check_github_update("nlohmann-json", NLOHMANN_UPDATE, NLOHMANN_VERSION)
			if str_version then
				NLOHMANN_VERSION = str_version
				NLOHMANN_HASH = ""
			end
		end

		-- Only execute on Windows in normal scenarios
		if os.host() ~= "windows" and not should_upgrade then
			return
		end
		check_nlohmann(should_upgrade)
	end
}

return premake.modules.install_nlohmannjson