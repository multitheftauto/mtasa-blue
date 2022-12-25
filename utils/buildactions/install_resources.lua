premake.modules.install_resources = {}

local unzip = function(zip_path, target_path)
	zip_path = path.translate(zip_path)
	target_path = path.translate(target_path)

	if os.host() == "windows" then
		os.executef("call \"utils\\7z\\7za.exe\" x \"%s\" -aoa -o\"%s\"", zip_path, target_path)
	else
		os.executef("unzip \"%s\" -d \"%s\"", zip_path, target_path)
	end
end

newaction {
	trigger = "install_resources",
	description = "Installs the resources to the bin directory",
	
	execute = function()
		-- Download resources
		http.download("http://mirror.mtasa.com/mtasa/resources/mtasa-resources-latest.zip", "temp_resources.zip")
		
		-- Extract resources
		unzip("temp_resources.zip", "Bin/server/mods/deathmatch/resources/")
		
		-- Cleanup
		os.remove("temp_resources.zip")
	end
}

return premake.modules.install_resources
