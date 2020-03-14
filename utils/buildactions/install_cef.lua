require 'utils'

premake.modules.install_cef = {}

-- Config variables
local CEF_PATH = "vendor/cef3/"
local CEF_URL_PREFIX = "https://mirror.mtasa.com/bdata/cef/"
local CEF_URL_SUFFIX = "_windows32_minimal.tar.bz2"

local CEF_VERSION = "80.0.4+g74f7b0c+chromium-80.0.3987.122" -- Change here to update CEF version

function make_cef_download_url()
	return CEF_URL_PREFIX..http.escapeUrlParam(CEF_VERSION)..CEF_URL_SUFFIX
end

newaction {
	trigger = "install_cef",
	description = "Downloads and installs CEF",

	execute = function()
		-- Only execute on Windows
		if os.host() ~= "windows" then return end

		-- Download md5
		local correct_checksum, result_string = http.get(make_cef_download_url()..".md5")
		if result_string ~= "OK" and result_string then
			print("Could not check CEF checksum: "..result_string)
			return -- Do nothing and rely on earlier installed files (to allow working offline)

			-- TODO(jusonex): It might make sense to fallback to spotify cef mirror
		end

		-- Trim whitespace
		correct_checksum = correct_checksum:gsub("[%s%c]", "")

		-- Check md5
		local archive_path = CEF_PATH.."temp.tar.bz2"
		if os.isfile(archive_path) and os.md5_file(archive_path) == correct_checksum then
			print("CEF consistency checks succeeded")
			return
		end

		-- Download CEF
		print("Downloading CEF...")
		http.download(make_cef_download_url(), archive_path)

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
