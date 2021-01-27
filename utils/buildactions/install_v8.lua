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

local librariesVersions = {
	["win32bit"] = {"https://content-na.drive.amazonaws.com/v2/download/presigned/unFmgb7VSItjZa4JU3rOmhmbB-4MzsUSAV2meiI6t9QeJxFPc?download=true&ownerId=A3QQ2Y77MB2XV9", "3ba3bd640597376451eaa6c5f524adc937baab3d14767c642b7c8e1c31f28bde" }
}
-- Change these to update the version
local V8_HASH = "0a090f8689f97152685a6101c49250fa99c1e47a582801c3f1d285554fd14ff5"

newaction {
	trigger = "install_v8",
	description = "Downloads and installs v8",

	execute = function()
		-- make sure pathes exists, otherwise download fails
		os.mkdir(V8_PATH)

		for versionName,v in pairs(librariesVersions) do
			local libraryPath = V8_PATH_LIBRARY.."/"..versionName;
		    os.mkdir(libraryPath)
			local archive_path_library = libraryPath.."/library_"..versionName..".zip"

			if not os.isfile(archive_path_library) or os.sha256_file(archive_path_library) == v[1] then
				print("v8 updating libraries: "..versionName);
				if not http.download_print_errors(v[1], archive_path_library) then
					print("error?");
					return
				end
				unzip(archive_path_library, libraryPath.."/")
			end
        end

		-- Check file hash
		local archive_path = V8_PATH.."/"..V8_DOWNLOAD_FILENAME..".zip"
		if os.isfile(archive_path) and os.sha256_file(archive_path) == V8_HASH then
			print("v8 is up to date.")
			return
		end

		-- Download V8
		print("Downloading V8...")
		if not http.download_print_errors(V8_BASEURL.."/"..V8_DOWNLOAD_FILENAME..".zip", archive_path) then
			return
		end

		-- -- Check downloaded file hash
		-- if os.sha256_file(archive_path) ~= UNIFONT_HASH then
		-- 	print("Unifont hash mismatch!")
		-- 	-- Delete bad file
		-- 	os.remove(archive_path)
		-- 	return
		-- end

		print("Unpacking V8...")
		unzip(archive_path, V8_PATH)

		os.executef("powershell robocopy \"%s\" \"%s\" /E /XJ", V8_PATH.."/v8-"..V8_DOWNLOAD_FILENAME, V8_PATH)
		print("V8 updated.")
	end
}

return premake.modules.install_v8
