require 'utils'

premake.modules.install_resources = {}

-- Config variables
local RESOURCES_URL = "https://mirror-cdn.multitheftauto.com/mtasa/resources/mtasa-resources-latest.zip"
local EXTRACT_DIR = "Bin/server/mods/deathmatch/resources/"

newaction {
	trigger = "install_resources",
	description = "Installs the resources to the bin directory",
	
	execute = function()
		-- Download resources
		if not http.download_print_errors(RESOURCES_URL, "temp_resources.zip") then
			os.exit(1)
			return
		end
		
		-- Extract resources
		if not os.extract_archive("temp_resources.zip", EXTRACT_DIR, true) then
			errormsg("ERROR: Couldn't unzip resources")
			os.exit(1)
			return
		end

		-- Cleanup
		if not os.remove("temp_resources.zip") then
			errormsg("ERROR: Couldn't delete downloaded resources zip file")
			os.exit(1)
			return
		end
	end
}

return premake.modules.install_resources
