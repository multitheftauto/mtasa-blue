require 'utils'

premake.modules.install_cef = {}

-- Config variables
local CEF_PATH = "vendor/cef3/"
local CEF_URL_PREFIX = "http://opensource.spotify.com/cefbuilds/cef_binary_"
local CEF_URL_SUFFIX = "_windows32_minimal.tar.bz2"

-- Change here to update CEF version
local CEF_VERSION = "80.0.4+g74f7b0c+chromium-80.0.3987.122"
local CEF_HASH = "8FD8E24AF196F00FEAAA1553496BAE99D8196BA023D0DD0FE44EFEEE93B04DFC"

function make_cef_download_url()
	return CEF_URL_PREFIX..http.escapeUrlParam(CEF_VERSION)..CEF_URL_SUFFIX
end

newaction {
	trigger = "install_cef",
	description = "Downloads and installs CEF",

	execute = function()
		-- Only execute on Windows
		if os.host() ~= "windows" then return end

		-- Check file hash
		local archive_path = CEF_PATH.."temp.tar.bz2"
		if os.isfile(archive_path) and os.sha256_file(archive_path) == CEF_HASH then
			print("CEF consistency checks succeeded")
			return
		end

		-- Download CEF
		print("Downloading CEF...")
		local result_str, response_code = http.download(make_cef_download_url(), archive_path)
		if result_str ~= "OK" and response_code ~= 200 then
			term.pushColor(term.red)
			io.write(("Could not download CEF with status code %d: "):format(response_code))
			term.setTextColor(term.purple)
			print(result_str)
			term.popColor()
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
