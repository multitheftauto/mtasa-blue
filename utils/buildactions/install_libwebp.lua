require 'utils'

premake.modules.install_libwebp = {}

-- Config variables
local LIBWEBP_PATH = "vendor/libwebp/"
local LIBWEBP_TEMP = "vendor/libwebp/libwebp.zip"
local LIBWEBP_UPDATE = "https://api.github.com/repos/webmproject/libwebp/releases/latest"
local LIBWEBP_URL = "https://github.com/webmproject/libwebp/archive/refs/tags/"
local LIBWEBP_EXT = ".zip"

-- Auto-update variables
-- NOTE: On first run the downloaded hash will differ; the script prints it and
-- offers to write it back into this file when invoked with the "upgrade" arg.
local LIBWEBP_VERSION = "v1.6.0"
local LIBWEBP_HASH = "7a89bb7a6a0c30b5c0f521c2673635381b4e7c916585f4a1f702d3b47f15d5e5"

-- Sentinel file used to detect a completed install
local LIBWEBP_SENTINEL = "src/webp/decode.h"

function make_download_url(url, version, ext)
	return url..http.escapeUrlParam(version)..ext
end

function update_install_libwebp(variable, version, hash)
	local filename = "utils/buildactions/install_libwebp.lua"
	local f = io.open(filename)
	local text = f:read("*all")
	f:close()

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
		errormsg(("Could not get page with status code %s: "):format(result_code), result_str)
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

local function check_libwebp(should_upgrade)
	local has_libwebp_sources = os.isfile(LIBWEBP_PATH .. LIBWEBP_SENTINEL)

	-- Check file hash
	local archive_path = LIBWEBP_TEMP
	local hash_passed = os.isfile(archive_path) and os.sha256_file(archive_path) == LIBWEBP_HASH
	if hash_passed then
		print("libwebp consistency checks succeeded")

		if has_libwebp_sources then
			return
		end
	else
		-- Download libwebp
		print("Downloading libwebp " .. LIBWEBP_VERSION ..  "...")
		if not http.download_print_errors(make_download_url(LIBWEBP_URL, LIBWEBP_VERSION, LIBWEBP_EXT), archive_path) then
			os.exit(1)
			return
		end
	end

	local downloaded_hash = os.sha256_file(archive_path)
	if should_upgrade then
		print("New libwebp hash is:", downloaded_hash)
		LIBWEBP_HASH = downloaded_hash

		io.write("Update `install_libwebp.lua` file? (Y/n) ")
		local input = io.read():lower()
		if (input == "y" or input == "yes") then
			update_install_libwebp("LIBWEBP", LIBWEBP_VERSION, downloaded_hash)
		end
	end

	if downloaded_hash == LIBWEBP_HASH then
		print("libwebp consistency checks succeeded")
	else
		errormsg("libwebp consistency checks failed.", ("Expected %s, got %s"):format(LIBWEBP_HASH, downloaded_hash))
		os.exit(1)
		return
	end

	-- Seriously abort now if we're not using Windows
	if os.host() ~= "windows" then
		return
	end

	-- Extract zip into a scratch subfolder then move contents up.
	-- We do NOT rmdir vendor/libwebp/ because that folder also contains our
	-- own premake5.lua / README.md.
	local extract_dir = LIBWEBP_PATH .. "_extract/"
	if os.isdir(extract_dir) then
		if not os.rmdir(extract_dir) then
			errormsg("ERROR: Could not delete libwebp extract folder")
			os.exit(1)
			return
		end
	end

	if not os.mkdir(extract_dir) then
		errormsg("ERROR: Could not create libwebp extract folder")
		os.exit(1)
		return
	end

	if not os.extract_archive(archive_path, extract_dir, true) then
		errormsg("ERROR: Could not extract libwebp .zip")
		os.exit(1)
		return
	end

	-- Move all files from _extract/libwebp*/* to vendor/libwebp/
	os.expanddir_wildcard(extract_dir .. "libwebp*", LIBWEBP_PATH)

	-- Clean up scratch folder
	os.rmdir(extract_dir)
end

newaction {
	trigger = "install_libwebp",
	description = "Downloads and installs libwebp",

	execute = function(...)
		local should_upgrade = _ARGS[1] == "upgrade"
		if should_upgrade then
			local libwebp = check_github_update("libwebp", LIBWEBP_UPDATE, LIBWEBP_VERSION)
			if libwebp then
				LIBWEBP_VERSION = libwebp
				LIBWEBP_HASH = ""
			end
		end

		-- Only execute on Windows in normal scenarios
		if os.host() ~= "windows" and not should_upgrade then
			return
		end

		check_libwebp(should_upgrade)
	end
}
