-- Add buildactions to path
premake.path = premake.path..";utils/buildactions"
require "compose_files"
require "install_data"
require "install_resources"
require "install_cef"
require "install_discord"
require "install_unifont"

-- Set CI Build global
local ci = os.getenv("CI")
if ci and ci:lower() == "true" then
	CI_BUILD = true
else
	CI_BUILD = false
end
GLIBC_COMPAT = os.getenv("GLIBC_COMPAT") == "true"

workspace "MTASA"
	configurations {"Debug", "Release", "Nightly"}

	platforms { "x86", "x64"}
	if os.host() == "macosx" then
		removeplatforms { "x86" }
	end

	targetprefix ""

	location "Build"
	startproject "Client Launcher"

	cppdialect "C++17"
	characterset "MBCS"
	pic "On"
	symbols "On"

	dxdir = os.getenv("DXSDK_DIR") or ""
	includedirs {
		"vendor",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"_SCL_SECURE_NO_WARNINGS",
		"_CRT_NONSTDC_NO_DEPRECATE",
		"NOMINMAX",
		"_TIMESPEC_DEFINED"
	}

	-- Helper function for output path
	buildpath = function(p) return "%{wks.location}/../Bin/"..p.."/" end
	copy = function(p) return "{COPY} %{cfg.buildtarget.abspath} \"%{wks.location}../Bin/"..p.."/\"" end

	if GLIBC_COMPAT then
		filter { "system:linux" }
			includedirs "/compat"
			linkoptions "-static-libstdc++ -static-libgcc"
			forceincludes  { "glibc_version.h" }
		filter { "system:linux", "platforms:x86" }
			libdirs { "/compat/x86" }
		filter { "system:linux", "platforms:x64" }
			libdirs { "/compat/x64" }
	end

	filter "platforms:x86"
		architecture "x86"
	filter "platforms:x64"
		architecture "x86_64"

	filter "configurations:Debug"
		defines { "MTA_DEBUG" }
		targetsuffix "_d"

	filter "configurations:Release or configurations:Nightly"
		optimize "Speed"	-- "On"=MS:/Ox GCC:/O2  "Speed"=MS:/O2 GCC:/O3  "Full"=MS:/Ox GCC:/O3

	if CI_BUILD then
		filter {}
			defines { "CI_BUILD=1" }

		filter { "system:linux" }
			linkoptions { "-s" }
	end

	filter {"system:windows", "configurations:Nightly", "kind:not StaticLib"}
		os.mkdir("Build/Symbols")
		linkoptions "/PDB:\"Symbols\\$(ProjectName).pdb\""

	filter "system:windows"
		toolset "v142"
		staticruntime "On"
		defines { "WIN32", "_WIN32", "_WIN32_WINNT=0x601", "_MSC_PLATFORM_TOOLSET=$(PlatformToolsetVersion)" }
		buildoptions { "/Zc:__cplusplus" }
		includedirs {
			path.join(dxdir, "Include")
		}
		libdirs {
			path.join(dxdir, "Lib/x86")
		}

	filter {"system:windows", "configurations:Debug"}
		runtime "Release" -- Always use Release runtime
		defines { "DEBUG" } -- Using DEBUG as _DEBUG is not available with /MT

	filter "system:linux"
		vectorextensions "SSE2"
		buildoptions { "-fvisibility=hidden" }

	-- Only build the client on Windows
	if os.target() == "windows" then
		group "Client"
		include "Client/ceflauncher"
		include "Client/ceflauncher_DLL"
		include "Client/cefweb"
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
		include "vendor/freetype"
		include "vendor/jpeg-9d"
		include "vendor/ksignals"
		include "vendor/libpng"
		include "vendor/tinygettext"
		include "vendor/pthreads"
		include "vendor/libspeex"
		include "vendor/detours"
		include "vendor/lunasvg"
	end

	filter {}
		group "Server"
		include "Server/core"
		include "Server/dbconmy"
		include "Server/launcher"
		include "Server/mods/deathmatch"
		include "Server/sdk"

		group "Shared"
		include "Shared"
		include "Shared/XML"

		group "Vendor"
		include "vendor/bcrypt"
		include "vendor/cryptopp"
		include "vendor/curl"
		include "vendor/ehs"
		include "vendor/google-breakpad"
		include "vendor/json-c"
		include "vendor/lua"
		include "vendor/mbedtls"
		include "vendor/pcre"
		include "vendor/pme"
		include "vendor/sqlite"
		include "vendor/tinyxml"
		include "vendor/unrar"
		include "vendor/zip"
		include "vendor/zlib"
