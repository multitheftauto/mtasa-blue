require 'utils'

premake.modules.install_simdjson = {}

-- Config variables
local SIMDJSON_PATH = "vendor/simdjson/"
local SIMDJSON_TEMP = "vendor/simdjson/tmp.zip"
local SIMDJSON_UPDATE = "https://api.github.com/repos/simdjson/simdjson/releases/latest"
local SIMDJSON_URL = "https://github.com/simdjson/simdjson/archive/refs/tags/"
local SIMDJSON_EXT = ".zip"

-- Auto-update variables
local SIMDJSON_VERSION = "v3.6.3"
local SIMDJSON_HASH = "0b9f866c577f78c1f422e59c7ecf446cef6bfe95e4e1448350c9cbb4c7e6fe3e"

function make_download_url(url, version, ext)
	return url..http.escapeUrlParam(version)..ext
end

function update_install_SIMDJSON_json(variable, version, hash)
	local filename = "utils/buildactions/install_simdjson.lua"
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

local function check_simdjson(should_upgrade)
	local has_SIMDJSON_dir = os.isdir(SIMDJSON_PATH)

	-- Check file hash
	local archive_path = SIMDJSON_TEMP
	local hash_passed = os.isfile(archive_path) and os.sha256_file(archive_path) == SIMDJSON_HASH
	if hash_passed then
		print("simdjson consistency checks succeeded")
		if has_SIMDJSON_dir then
			return
		end
	else
		-- Download simdjson
		print("Downloading simdjson " .. SIMDJSON_VERSION ..  "...")
		if not http.download_print_errors(make_download_url(SIMDJSON_URL, SIMDJSON_VERSION, SIMDJSON_EXT), archive_path) then
			os.exit(1)
			return
		end
	end

	local downloaded_hash = os.sha256_file(archive_path)
	if should_upgrade then
		print("New simdjson hash is:", downloaded_hash)
		SIMDJSON_HASH = downloaded_hash

		io.write(("Update `install_simdjson.lua` file? (Y/n) "):format(version))
		local input = io.read():lower()
		if (input == "y" or input == "yes") then
			update_install_SIMDJSON_json("simdjson", SIMDJSON_VERSION, downloaded_hash)
		end
	end

	if downloaded_hash == SIMDJSON_HASH then
		print("simdjson consistency checks succeeded")
	else
		errormsg("simdjson consistency checks failed.", ("Expected %s, got %s"):format(SIMDJSON_HASH, downloaded_hash))
		os.exit(1)
		return
	end

	-- Seriously abort now if we're not using Windows
	if os.host() ~= "windows" then
		return
	end

	-- Delete old simdjson files
	if has_discord_dir then
		if not os.rmdir(SIMDJSON_PATH) then
			errormsg("ERROR: Could not delete simdjson folder")
			os.exit(1)
			return
		end
	end

	if not os.mkdir(SIMDJSON_PATH) then
		errormsg("ERROR: Could not create simdjson folder (2)")
		os.exit(1)
		return
	end

	-- Extract zip
	if not os.extract_archive(archive_path, SIMDJSON_PATH, true) then
		errormsg("ERROR: Could not extract .zip")
		os.exit(1)
		return
	end

	-- Move all files from simdjson*/* to ./
	os.expanddir_wildcard(SIMDJSON_PATH.."simdjson*", SIMDJSON_PATH)
end

newaction {
	trigger = "install_simdjson",
	description = "Downloads and installs simdjson",

	execute = function(...)
		local should_upgrade = _ARGS[1] == "upgrade"
		if should_upgrade then
			-- simdjson
			local str_version = check_github_update("simdjson", SIMDJSON_UPDATE, SIMDJSON_VERSION)
			if str_version then
				SIMDJSON_VERSION = str_version
				SIMDJSON_HASH = ""
			end
		end

		-- Only execute on Windows in normal scenarios
		if os.host() ~= "windows" and not should_upgrade then
			return
		end
		check_simdjson(should_upgrade)
	end
}

return premake.modules.install_simdjson