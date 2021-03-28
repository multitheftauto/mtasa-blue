require 'utils'

premake.modules.install_v8 = {}

local unzip = function(zip_path, target_path)
	zip_path = path.translate(zip_path)
	target_path = path.translate(target_path)

	if os.host() == "windows" then
		os.executef("call \"utils\\7z\\7za.exe\" x \"%s\" -aoa -o\"%s\"", zip_path, target_path)
	else
		os.executef("unzip \"%s\" -d \"%s\"", zip_path, target_path)
	end
end

-- Config variables
local V8_BASEURL = "https://chromium.googlesource.com/v8/v8.git/+archive/refs/tags"

-- Check what recent stable version is at: https://omahaproxy.appspot.com/
local V8_DOWNLOAD_FILENAME = "8.9.255.20" -- without extension

local V8_PATH = "vendor/v8/sources"
local V8_PATH_LIBRARY = "vendor/v8/library"
local V8_PATH_DLLS = "bin/server/v8"

local selectedVersion = "win32bit"

-- Change these to update the version
local V8_HASH = "0a090f8689f97152685a6101c49250fa99c1e47a582801c3f1d285554fd14ff5"

newaction {
	trigger = "install_v8",
	description = "Downloads and installs v8",

	execute = function()
		-- make sure pathes exists, otherwise download fails
		os.mkdir(V8_PATH)

		-- sources
		local archive_path = V8_PATH.."/"..V8_DOWNLOAD_FILENAME..".tar.gz"
		if not os.isfile(archive_path) or not os.sha256_file(archive_path) == V8_HASH or true then
			-- Download V8 sources
			print("Downloading v8 sources...")
			if not http.download_print_errors(V8_BASEURL.."/"..V8_DOWNLOAD_FILENAME..".tar.gz", archive_path) then
				print("Failed to download v8 sources :(")
				return 
			else
				print("Unpacking v8...", V8_PATH.."/"..V8_DOWNLOAD_FILENAME..".tar.gz")
				--unzip(archive_path, V8_PATH)
				os.executef('powershell robocopy "%s" "%s" /E /XJ  /NFL /NDL /nc /ns /np', V8_PATH.."/"..V8_DOWNLOAD_FILENAME..".tar.gz", V8_PATH)
				print("v8 sources has been updated.")
			end
		else
			print("v8 sources are up to date.")
		end

		-- -- Check downloaded file hash
		-- if os.sha256_file(archive_path) ~= UNIFONT_HASH then
		-- 	print("Unifont hash mismatch!")
		-- 	-- Delete bad file
		-- 	os.remove(archive_path)
		-- 	return
		-- end


		print("V8 updated.")
	end
}

return premake.modules.install_v8
