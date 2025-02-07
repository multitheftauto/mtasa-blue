require 'utils'

premake.modules.install_cef = {}

-- Config variables
local CEF_PATH = "vendor/cef3/cef/"
local CEF_TEMP_PATH = "vendor/cef3/"
local CEF_URL_PREFIX = "https://cef-builds.spotifycdn.com/cef_binary_"
local CEF_URL_SUFFIX = "_windows32_minimal.tar.bz2"

-- Change here to update CEF version
local CEF_VERSION = "132.3.2+g4997b2f+chromium-132.0.6834.161"
local CEF_HASH = "9910e4180df0e5f400a033aba33c418c8eda57adbaac68aa005b649d5b2ea52b"

function make_cef_download_url()
	return CEF_URL_PREFIX..CEF_VERSION..CEF_URL_SUFFIX
end

function update_install_cef(version, hash)
	local filename = "utils/buildactions/install_cef.lua"
	local f = io.open(filename)
	local text = f:read("*all")
	f:close()

	-- Replace version and hash lines
	local version_line = 'local CEF_VERSION = "' .. version .. '"'
	local hash_line = 'local CEF_HASH = "' .. hash .. '"'
	text = text:gsub('local CEF_VERSION = ".-"', version_line, 1)
	text = text:gsub('local CEF_HASH = ".-"', hash_line, 1)

	local f = io.open(filename, "w")
	f:write(text)
	f:close()
end

local function cef_version_comparator(a, b)
	local a_major, a_minor, a_patch = a.cef_version:match("^(%d+).(%d+).(%d+)%+*")
	local b_major, b_minor, c_patch = b.cef_version:match("^(%d+).(%d+).(%d+)%+*")

	return a_major > b_major and a_minor > b_minor and a_patch > c_patch
end

newaction {
	trigger = "install_cef",
	description = "Downloads and installs CEF",

	execute = function(...)
		local upgrade = _ARGS[1] == "upgrade"
		if upgrade and _ARGS[2] then
			local version = _ARGS[2]

			if version == CEF_VERSION then
				print(("CEF version is already %s"):format(version))
				return
			end

			CEF_VERSION = version
			CEF_HASH = ""
		elseif upgrade then
			print("Checking opensource.spotify.com for an update...")
			local resource, result_str, result_code = http.get("https://cef-builds.spotifycdn.com/index.json")
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

			local builds_by_version = table.filter(meta["windows32"]["versions"], function(build) return build.channel == "stable" end)
			table.sort(builds_by_version, cef_version_comparator)
			local latest_build = builds_by_version[1]

			if latest_build.cef_version == CEF_VERSION then
				print(("CEF is already up to date (%s)"):format(latest_build.cef_version))
				return
			end

			io.write(("Does version '%s' look OK to you? (Y/n) "):format(latest_build.cef_version))
			local input = io.read():lower()
			if not (input == "y" or input == "yes") then
				errormsg("Aborting due to user request.")
				return
			end

			CEF_VERSION = latest_build.cef_version
			CEF_HASH = ""
		end

		-- Only execute on Windows in normal scenarios
		if os.host() ~= "windows" and not upgrade then
			return
		end

		local has_cef_dir = os.isdir(CEF_PATH)

		-- Check file hash
		local archive_path = CEF_TEMP_PATH.."temp.tar.bz2"
		local hash_passed = os.isfile(archive_path) and os.sha256_file(archive_path) == CEF_HASH
		if hash_passed then
			print("CEF consistency checks succeeded")

			if has_cef_dir then
				return
			end
		else
			-- Download CEF
			print("Downloading CEF " .. CEF_VERSION ..  "...")
			if not http.download_print_errors(make_cef_download_url(), archive_path, { progress = http.create_download_progress_handler{update_interval_s = 5} }) then
				os.exit(1)
				return
			end
		end

		local downloaded_hash = os.sha256_file(archive_path)
		if upgrade then
			print("New CEF hash is:", downloaded_hash)
			CEF_HASH = downloaded_hash

			io.write(("Update `install_cef.lua` file? (Y/n) "):format(version))
			local input = io.read():lower()
			if (input == "y" or input == "yes") then
				update_install_cef(CEF_VERSION, downloaded_hash)
			end
		end

		if downloaded_hash == CEF_HASH then
			print("CEF consistency checks succeeded")
		else
			errormsg("CEF consistency checks failed.", ("Expected %s, got %s"):format(CEF_HASH, downloaded_hash))
			os.exit(1)
			return
		end

		-- Seriously abort now if we're not using Windows
		if os.host() ~= "windows" then
			return
		end

		-- Delete old CEF files
		if has_cef_dir then
			if not os.rmdir(CEF_PATH) then
				errormsg("ERROR: Could not delete cef folder")
				os.exit(1)
				return
			end
		end

		if not os.mkdir(CEF_PATH) then
			errormsg("ERROR: Could not create cef folder (2)")
			os.exit(1)
			return
		end

		-- Extract first bz2 and then tar
		if not os.extract_archive(archive_path, CEF_PATH, true) then -- Extract .tar.bz2 to .tar
			errormsg("ERROR: Could not extract .tar.bz2")
			os.exit(1)
			return
		end

		if not os.extract_archive(CEF_PATH.."temp.tar", CEF_PATH, true) then -- Extract .tar
			errormsg("ERROR: Could not extract .tar")
			os.exit(1)
			return
		end

		-- Move all files from cef_binary*/* to ./
		os.expanddir_wildcard(CEF_PATH.."cef_binary*", CEF_PATH)

		-- Delete .tar archive, but keep .tar.bz2 for checksumming
		if not os.remove(CEF_PATH.."temp.tar") then
			errormsg("ERROR: Could not remove temp.tar")
			os.exit(1)
			return
		end
	end
}

return premake.modules.install_cef
