require 'utils'

premake.modules.install_rapidjson = {}

-- Config variables
local RAPIDJSON_PATH = "vendor/rapidjson/"
local RAPIDJSON_TEMP = "vendor/rapidjson/tmp.zip"
local RAPIDJSON_UPDATE = "https://api.github.com/repos/multitheftauto/rapidjson/releases/latest"
local RAPIDJSON_URL = "https://github.com/multitheftauto/rapidjson/archive/refs/tags/"
local RAPIDJSON_EXT = ".zip"

-- Auto-update variables
local RAPIDJSON_VERSION = "v1.1.1"
local RAPIDJSON_HASH = "3d638ad2549645a4831e8e98cf7b919f191de0d60816e63e1d5aa08cd56e6db8"

function make_download_url(url, version, ext)
	return url..http.escapeUrlParam(version)..ext
end

function update_install_RAPIDJSON_json(variable, version, hash)
	local filename = "utils/buildactions/install_rapidjson.lua"
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

local function check_rapidjson(should_upgrade)
	local has_RAPIDJSON_dir = os.isdir(RAPIDJSON_PATH)

	-- Check file hash
	local archive_path = RAPIDJSON_TEMP
	local hash_passed = os.isfile(archive_path) and os.sha256_file(archive_path) == RAPIDJSON_HASH
	if hash_passed then
		print("rapidjson consistency checks succeeded")
		if has_RAPIDJSON_dir then
			return
		end
	else
		-- Download rapidjson
		print("Downloading rapidjson " .. RAPIDJSON_VERSION ..  "...")
		if not http.download_print_errors(make_download_url(RAPIDJSON_URL, RAPIDJSON_VERSION, RAPIDJSON_EXT), archive_path) then
			os.exit(1)
			return
		end
	end

	local downloaded_hash = os.sha256_file(archive_path)
	if should_upgrade then
		print("New rapidjson hash is:", downloaded_hash)
		RAPIDJSON_HASH = downloaded_hash

		io.write(("Update `install_rapidjson.lua` file? (Y/n) "):format(version))
		local input = io.read():lower()
		if (input == "y" or input == "yes") then
			update_install_RAPIDJSON_json("rapidjson", RAPIDJSON_VERSION, downloaded_hash)
		end
	end

	if downloaded_hash == RAPIDJSON_HASH then
		print("rapidjson consistency checks succeeded")
	else
		errormsg("rapidjson consistency checks failed.", ("Expected %s, got %s"):format(RAPIDJSON_HASH, downloaded_hash))
		os.exit(1)
		return
	end

	-- Seriously abort now if we're not using Windows
	if os.host() ~= "windows" then
		return
	end

	-- Delete old rapidjson files
	if has_discord_dir then
		if not os.rmdir(RAPIDJSON_PATH) then
			errormsg("ERROR: Could not delete rapidjson folder")
			os.exit(1)
			return
		end
	end

	if not os.mkdir(RAPIDJSON_PATH) then
		errormsg("ERROR: Could not create rapidjson folder (2)")
		os.exit(1)
		return
	end

	-- Extract zip
	if not os.extract_archive(archive_path, RAPIDJSON_PATH, true) then
		errormsg("ERROR: Could not extract .zip")
		os.exit(1)
		return
	end

	-- Move all files from rapidjson*/* to ./
	os.expanddir_wildcard(RAPIDJSON_PATH.."rapidjson*", RAPIDJSON_PATH)
end

newaction {
	trigger = "install_rapidjson",
	description = "Downloads and installs rapidjson",

	execute = function(...)
		local should_upgrade = _ARGS[1] == "upgrade"
		if should_upgrade then
			-- rapidjson
			local str_version = check_github_update("rapidjson", RAPIDJSON_UPDATE, RAPIDJSON_VERSION)
			if str_version then
				RAPIDJSON_VERSION = str_version
				RAPIDJSON_HASH = ""
			end
		end

		-- Only execute on Windows in normal scenarios
		if os.host() ~= "windows" and not should_upgrade then
			return
		end
		check_rapidjson(should_upgrade)
	end
}

return premake.modules.install_rapidjson