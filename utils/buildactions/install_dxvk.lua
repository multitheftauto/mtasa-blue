require 'utils'

premake.modules.install_dxvk = {}

-- Downloads the official DXVK release and installs its 32-bit d3d9.dll, which the loader
-- uses to run GTA's D3D9 renderer through Vulkan when "Enable Vulkan" is ticked.
--
-- Only d3d9.dll is needed: it doesn't import dxgi.dll, and GTA SA never uses D3D10/D3D11.
-- Using the official binary (pinned by hash) instead of vendoring DXVK's source keeps the
-- repository small and means every build gets the same, known file.

-- Change these to update the version
local DXVK_VERSION = "1.10.3"
local DXVK_ARCHIVE_HASH = "8d1a3c912761b450c879f98478ae64f6f6639e40ce6848170a0f6b8596fd53c6"
local DXVK_D3D9_HASH = "b6cfa2cd62af73b80d461085d126004b0e22dd3944c9246c58e3a68e747b56b6"

local DXVK_URL = "https://github.com/doitsujin/dxvk/releases/download/v"..DXVK_VERSION.."/dxvk-"..DXVK_VERSION..".tar.gz"
local DXVK_TEMP_PATH = "vendor/dxvk/"
local DXVK_DATA_PATH = "Shared/data/MTA San Andreas/MTA/dxvk/"
local DXVK_BIN_PATH = "Bin/MTA/dxvk/"

-- Copies d3d9.dll into Bin too, so a dev build that doesn't rerun install_data still gets it
local function copy_to_bin(source)
	if not os.isdir("Bin") then
		return true
	end

	if not os.mkdir(DXVK_BIN_PATH) or not os.copyfile(source, DXVK_BIN_PATH.."d3d9.dll") then
		errormsg("ERROR: Could not copy DXVK d3d9.dll to "..DXVK_BIN_PATH)
		return false
	end
	return true
end

newaction {
	trigger = "install_dxvk",
	description = "Downloads and installs DXVK (D3D9 to Vulkan)",

	execute = function()
		-- DXVK is only used by the Windows client
		if os.host() ~= "windows" then
			return
		end

		local d3d9_path = DXVK_DATA_PATH.."d3d9.dll"
		if os.isfile(d3d9_path) and os.sha256_file(d3d9_path) == DXVK_D3D9_HASH then
			print("DXVK is up to date.")
			if not copy_to_bin(d3d9_path) then
				os.exit(1)
			end
			return
		end

		if not os.mkdir(DXVK_TEMP_PATH) or not os.mkdir(DXVK_DATA_PATH) then
			errormsg("ERROR: Could not create DXVK folders")
			os.exit(1)
			return
		end

		-- Download DXVK, unless the archive from an earlier run is still good
		local archive_path = DXVK_TEMP_PATH.."dxvk.tar.gz"
		if not (os.isfile(archive_path) and os.sha256_file(archive_path) == DXVK_ARCHIVE_HASH) then
			print("Downloading DXVK "..DXVK_VERSION.."...")
			if not http.download_print_errors(DXVK_URL, archive_path) then
				os.exit(1)
				return
			end

			local archive_hash = os.sha256_file(archive_path)
			if archive_hash ~= DXVK_ARCHIVE_HASH then
				errormsg("ERROR: DXVK hash mismatch!", ("\nExpected %s, got %s"):format(DXVK_ARCHIVE_HASH, archive_hash))
				os.remove(archive_path)
				os.exit(1)
				return
			end
		end

		-- Extract .tar.gz to .tar, then the .tar
		if not os.extract_archive(archive_path, DXVK_TEMP_PATH, true) or not os.extract_archive(DXVK_TEMP_PATH.."dxvk.tar", DXVK_TEMP_PATH, true) then
			errormsg("ERROR: Could not extract DXVK")
			os.exit(1)
			return
		end

		local extracted_d3d9 = DXVK_TEMP_PATH.."dxvk-"..DXVK_VERSION.."/x32/d3d9.dll"
		local d3d9_hash = os.sha256_file(extracted_d3d9)
		if d3d9_hash ~= DXVK_D3D9_HASH then
			errormsg("ERROR: DXVK d3d9.dll hash mismatch!", ("\nExpected %s, got %s"):format(DXVK_D3D9_HASH, d3d9_hash))
			os.exit(1)
			return
		end

		if not os.copyfile(extracted_d3d9, d3d9_path) then
			errormsg("ERROR: Could not copy DXVK d3d9.dll to "..DXVK_DATA_PATH)
			os.exit(1)
			return
		end

		-- Keep only the archive, for the hash check on the next run
		os.remove(DXVK_TEMP_PATH.."dxvk.tar")
		os.rmdir(DXVK_TEMP_PATH.."dxvk-"..DXVK_VERSION)

		if not copy_to_bin(d3d9_path) then
			os.exit(1)
			return
		end

		print("DXVK updated.")
	end
}

return premake.modules.install_dxvk
