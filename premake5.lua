-- Add buildactions to path
premake.path = premake.path..";utils/buildactions"
require "compose_files"
require "install_data"
require "install_resources"
require "install_cef"

-- Set CI Build global
local ci = os.getenv("CI")
if ci and ci:lower() == "true" then 
	CI_BUILD = true 
else 
	CI_BUILD = false
end 

workspace "MTASA"
	configurations {"Debug", "Release", "Nightly"}
	platforms { "x86", "x64"}
	targetprefix ("")
	
	location "Build"
	startproject "Client Launcher"
	
	flags { "C++11", "Symbols" }
	characterset "MBCS"
	pic "On"
	
	dxdir = os.getenv("DXSDK_DIR") or ""
	includedirs { 
		"vendor",
		"Shared/sdk", 
	}

	defines { 
		"_CRT_SECURE_NO_WARNINGS",
		"_SCL_SECURE_NO_WARNINGS",
		"_CRT_NONSTDC_NO_DEPRECATE"
	}
		
	-- Helper function for output path 
	buildpath = function(p) return "%{wks.location}../Bin/"..p.."/" end
	copy = function(p) return "{COPY} %{cfg.buildtarget.abspath} %{wks.location}../Bin/"..p.."/" end 
	
	filter "platforms:x86"
		architecture "x86"
	filter "platforms:x64"
		architecture "x86_64"
	
	filter "configurations:Debug"
		defines { "MTA_DEBUG" }
		targetsuffix "_d"
	
	filter "configurations:Release or configurations:Nightly"
		flags { "Optimize" }
	
	if CI_BUILD then
		filter {}
			defines { "CI_BUILD=1" }
		
		filter { "system:linux" }
			linkoptions { "-s" }
	end 
	
	filter {"system:windows", "configurations:Nightly", "kind:not StaticLib"}
		os.mkdir("Build/Symbols")
		linkoptions "/PDB:\"Symbols\\$(ProjectName).pdb\""
		
	filter {"system:windows", "toolset:*140*"}
		defines { "_TIMESPEC_DEFINED" } -- fix pthread redefinition error, TODO: Remove when we fully moved to vs2015
	
	filter "system:windows"
		toolset "v140"
		defines { "WIN32", "_WIN32" }
		includedirs { 
			dxdir.."Include"
		}
		libdirs {
			dxdir.."Lib/x86"
		}
	
	-- Only build the client on Windows
	if os.get() == "windows" then
		group "Client"
		include "Client/ceflauncher"
		include "Client/ceflauncher_DLL"
		include "Client/core"
		include "Client/game_sa"
		include "Client/sdk"
		include "Client/gui"
		include "Client/launch"
		include "Client/loader"
		include "Client/multiplayer_sa"
		include "Client/mods/deathmatch"
		
		group "Client/CEGUI"
		include "vendor/cegui-0.4.0-custom/src/renderers/directx9GUIRenderer"
		include "vendor/cegui-0.4.0-custom/WidgetSets/Falagard"
		include "vendor/cegui-0.4.0-custom"
		
		group "Vendor"
		include "vendor/portaudio"
		include "vendor/cef3"
		include "vendor/jpeg-9b"
		include "vendor/libpng"
		include "vendor/tinygettext"
		include "vendor/pthreads"
		include "vendor/libspeex"
		include "vendor/curl/lib"
	end
	
	filter {}
		group "Server"
		include "Server/core"
		include "Server/dbconmy"
		include "Server/launcher"
		include "Server/mods/deathmatch"
		
		group "Shared"
		include "Shared/XML"
		
		group "Vendor"
		include "vendor/cryptopp"
		include "vendor/ehs"
		include "vendor/google-breakpad"
		include "vendor/json-c"
		include "vendor/lua"
		include "vendor/pcre"
		include "vendor/pme"
		include "vendor/sqlite"
		include "vendor/tinyxml"
		include "vendor/unrar"
		include "vendor/zip"
		include "vendor/zlib"
