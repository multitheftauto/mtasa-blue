require 'utils'

premake.modules.install_cef = {}

-- Config variables
local CEF_PATH = "vendor/cef3/"
local CEF_URL_PREFIX = "http://opensource.spotify.com/cefbuilds/cef_binary_"
local CEF_URL_SUFFIX = "_windows32_minimal.tar.bz2"

-- Change here to update CEF version
local CEF_VERSION = "85.3.9+gb045a6e+chromium-85.0.4183.102"
local CEF_HASH = "e7c1ef41b09c8de496ad7d0b3ef9220fad0ff10d0714b2251851a0061bd72a59"

function make_cef_download_url()
	return CEF_URL_PREFIX..http.escapeUrlParam(CEF_VERSION)..CEF_URL_SUFFIX
end

function errormsg(title, message)
	term.pushColor(term.red)
	io.write(title)
	if message then
		term.setTextColor(term.purple)
		print(" " .. message)
	else
		print()
	end
	term.popColor()
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
			resource, result_str, result_code = http.get("http://opensource.spotify.com/cefbuilds/index.html")
			if result_str ~= "OK" or result_code ~= 200 then
				errormsg(("Could not get page with status code %s: "):format(response_code), result_str)
				return
			end

			local _, index = resource:find('Windows 32%-bit Builds.-data%-version="')
			if not index then
				errormsg("Could not find version string index.")
				return
			end

			local version = resource:match("(.-)\">", index+1)
			if not version then
				errormsg("Could not get version string from index.")
			end

			if version == CEF_VERSION then
				print(("CEF is already up to date (%s)"):format(version))
				return
			end

			io.write(("Does version '%s' look OK to you? (Y/n) "):format(version))
			local input = io.read():lower()
			if not (input == "y" or input == "yes") then
				errormsg("Aborting due to user request.")
				return
			end

			CEF_VERSION = version
			CEF_HASH = ""
		end

		-- Only execute on Windows in normal scenarios
		if os.host() ~= "windows" and not upgrade then
			return
		end

		-- Check file hash
		local archive_path = CEF_PATH.."temp.tar.bz2"
		if os.isfile(archive_path) and os.sha256_file(archive_path) == CEF_HASH then
			print("CEF consistency checks succeeded")
			return
		end

		-- Download CEF
		print("Downloading CEF " .. CEF_VERSION ..  "...")
		local result_str, response_code = http.download(make_cef_download_url(), archive_path)
		if result_str ~= "OK" or response_code ~= 200 then
			errormsg(("Could not download CEF with status code %s: "):format(response_code), result_str)
			return
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
		-- TODO: It might be better to download the files into a new folder and delete this folder at once
		os.rmdir(CEF_PATH.."cmake")
		os.rmdir(CEF_PATH.."include")
		os.rmdir(CEF_PATH.."libcef_dll")
		os.rmdir(CEF_PATH.."Release")
		os.rmdir(CEF_PATH.."Resources")
		os.remove_wildcard(CEF_PATH.."*.txt")

		-- Extract first bz2 and then tar
		os.extract_archive(archive_path, CEF_PATH, true) -- Extract .tar.bz2 to .tar
		os.extract_archive(CEF_PATH.."temp.tar", CEF_PATH, true) -- Extract .tar

		-- Move all files from cef_binary*/* to ./
		os.expanddir_wildcard(CEF_PATH.."cef_binary*", CEF_PATH)

		-- Delete .tar archive, but keep .tar.bz2 for checksumming
		os.remove(CEF_PATH.."temp.tar")
	end
}

return premake.modules.install_cef
