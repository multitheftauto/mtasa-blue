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
local V8_BASEURL = "https://github.com/v8/v8/archive"
local V8_DOWNLOAD_FILENAME = "8.9.276" -- without extension

local V8_PATH = "vendor/v8/sources"
local V8_PATH_LIBRARY = "vendor/v8/library"
local V8_PATH_DLLS = "bin/server/v8"

local selectedVersion = "win32bit"

local librariesVersions = {
	["win32bit"] = {"https://download1512.mediafire.com/blbwbsm7xygg/e755ws5hl52j8m0/library_win32bit.zip", "3ba3bd640597376451eaa6c5f524adc937baab3d14767c642b7c8e1c31f28bde" }
}

local dllsVersions = {
	["win32bit"] = {"http://zweryfikowaneserwery.mtasa.eu/mtasa/v8_win32bit.zip", "d0723d6f40bb60c04583278b425fdd7237f239611dbdac4be8c3b07d28a81022" }
}
-- Change these to update the version
local V8_HASH = "0a090f8689f97152685a6101c49250fa99c1e47a582801c3f1d285554fd14ff5"

newaction {
	trigger = "install_v8",
	description = "Downloads and installs v8",

	execute = function()
		-- make sure pathes exists, otherwise download fails
		os.mkdir(V8_PATH)
		os.mkdir(V8_PATH_DLLS)

		local sources = librariesVersions[selectedVersion]
		local libraryPath = V8_PATH_LIBRARY.."/"..selectedVersion;
		local dllsPath = V8_PATH_DLLS.."/"..selectedVersion;
		os.mkdir(libraryPath)
		local archive_path_library = libraryPath.."/library_"..selectedVersion..".zip"

		-- libraries
		if not os.isfile(archive_path_library) or os.sha256_file(archive_path_library) == sources[1] then
			print("v8 updating libraries: "..selectedVersion);
			if not http.download_print_errors(sources[1], archive_path_library) then
				print("error?");
				return
			end
			unzip(archive_path_library, libraryPath.."/")
		end

		-- sources
		local archive_path = V8_PATH.."/"..V8_DOWNLOAD_FILENAME..".zip"
		if not os.isfile(archive_path) or not os.sha256_file(archive_path) == V8_HASH then
			-- Download V8 sources
			print("Downloading v8 sources...")
			if --[[not http.download_print_errors(V8_BASEURL.."/"..V8_DOWNLOAD_FILENAME..".zip", archive_path) ]] false then
				print("Failed to download v8 sources :(")
				return 
			else
				print("Unpacking v8...")
				--unzip(archive_path, V8_PATH)
				os.executef("powershell robocopy \"%s\" \"%s\" /E /XJ", V8_PATH.."/v8-"..V8_DOWNLOAD_FILENAME, V8_PATH)
				print("v8 sources has been updated.")
			end
		else
			print("v8 sources are up to date.")
		end

		-- dlls
		local dlls = dllsVersions[selectedVersion]
		local archive_path = V8_PATH_DLLS.."/dlls.zip"
		if not os.isfile(archive_path) or not os.sha256_file(archive_path) == dllsVersions[2] then
			print("Downloading v8 dlls...")
			if not http.download_print_errors(dlls[1], archive_path) then
				print("Failed to download v8 dlls :(")
				return 
			else
				print("Unpacking v8 dlls...")
				unzip(archive_path, V8_PATH_DLLS)
				--os.executef("powershell robocopy \"%s\" \"%s\" /E /XJ", V8_PATH.."/v8-"..V8_DOWNLOAD_FILENAME, V8_PATH)
				print("v8 sources has been updated.")
			end
		else
			print("v8 dlls are up to date.")
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
