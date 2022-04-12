require 'utils'

premake.modules.install_bulletphysics = {}

-- Config variables
local BULLETPHYSICS_BASEURL = "https://github.com/bulletphysics/bullet3/archive/refs/tags/"
local BULLETPHYSICS_FILENAME = "3.22b"
local BULLETPHYSICS_PATH = "vendor/bulletphysics3d"
local BULLETPHYSICS_HASH = "b295cc1805f7c9a34269f9d01df5d219e86318ec60fd95afcddfa6c50240448f"

newaction {
	trigger = "install_bulletphysics",
	description = "Downloads and installs bullet physics",

	execute = function()        
		-- Check file hash
		local archive_path = BULLETPHYSICS_PATH.."/"..BULLETPHYSICS_FILENAME..".zip"
		if os.isfile(archive_path) and os.sha256_file(archive_path) == BULLETPHYSICS_HASH then
			print("Bullet physics is up to date.")
			return
		end

		-- Download Bullet physics
		print("Downloading Bullet physics...")
		if not http.download_print_errors(BULLETPHYSICS_BASEURL.."/"..BULLETPHYSICS_FILENAME..".zip", archive_path) then
			os.exit(1)
			return
		end

		-- Check downloaded file hash
		local download_hash = os.sha256_file(archive_path)
		if download_hash ~= BULLETPHYSICS_HASH then
			errormsg("ERROR: Bullet physics hash mismatch!", ("\nExpected %s, got %s"):format(BULLETPHYSICS_HASH, download_hash))
			-- Delete bad file
			-- os.remove(archive_path)
			-- os.exit(1)
			-- return
		end

        if not os.extract_archive(archive_path, BULLETPHYSICS_PATH, true) then
            errormsg("ERROR: Could not extract .zip")
            os.exit(1)
            return
        end

        os.copydir(BULLETPHYSICS_PATH.."/bullet3-"..BULLETPHYSICS_FILENAME.."/src/", BULLETPHYSICS_PATH.."/")
		
        print("Bullet physics updated.")
	end
}

return premake.modules.install_bulletphysics
