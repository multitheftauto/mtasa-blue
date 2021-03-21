require 'utils'

premake.modules.install_unifont = {}

-- Config variables
local UNIFONT_BASEURL = "https://github.com/multitheftauto/unifont/releases/download/"
local UNIFONT_DOWNLOAD_FILENAME = "unifont-13.0.04.ttf"
local UNIFONT_FILENAME = "unifont.ttf"
local UNIFONT_PATH = "Shared/data/MTA San Andreas/MTA/cgui"

-- Change these to update the version
local UNIFONT_TAG = "v13.0.04"
local UNIFONT_HASH = "a74c124f25c25ee588c3e8436781d93f181d049ea838ea8eb97e85cd9266c2b1"

newaction {
	trigger = "install_unifont",
	description = "Downloads and installs Unifont",

	execute = function()
		-- Check file hash
		local archive_path = UNIFONT_PATH.."/"..UNIFONT_FILENAME
		if os.isfile(archive_path) and os.sha256_file(archive_path) == UNIFONT_HASH then
			print("Unifont is up to date.")
			return
		end

		-- Download Unifont
		print("Downloading Unifont...")
		if not http.download_print_errors(UNIFONT_BASEURL..UNIFONT_TAG.."/"..UNIFONT_DOWNLOAD_FILENAME, archive_path) then
			return
		end

		-- Check downloaded file hash
		if os.sha256_file(archive_path) ~= UNIFONT_HASH then
			print("Unifont hash mismatch!")
			-- Delete bad file
			os.remove(archive_path)
			return
		end

		print("Unifont updated.")
	end
}

return premake.modules.install_unifont
