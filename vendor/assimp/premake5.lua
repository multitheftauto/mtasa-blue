project "assimp"
	targetname "assimp"
	language "C++"
	kind "StaticLib"
	pic "On"
	
	includedirs { ".", "include", "include/assimp", "contrib/irrXML",
	"code/BoostWorkaround", "contrib/zlib", "contrib/unzip", "contrib/openddlparser/include",
	"code/MD5", "code/CApi", "code/FBX", "code", "contrib/rapidjson/include",
	}
	defines { "Z_PREFIX", "_SCL_SECURE_NO_WARNINGS", "ASSIMP_BUILD_NO_EXPORT",
		"ASSIMP_BUILD_NO_C4D_IMPORTER", "ASSIMP_BUILD_NO_OPENGEX_IMPORTER", "ASSIMP_BUILD_NO_3DS_IMPORTER", "ASSIMP_BUILD_NO_AC_IMPORTER", "ASSIMP_BUILD_NO_ASE_IMPORTER",
		"ASSIMP_BUILD_NO_ASSBIN_IMPORTER", "ASSIMP_BUILD_NO_B3D_IMPORTER", "ASSIMP_BUILD_NO_BVH_IMPORTER", "ASSIMP_BUILD_NO_COB_IMPORTER", "ASSIMP_BUILD_NO_CSM_IMPORTER",
		"ASSIMP_BUILD_NO_DXF_IMPORTER", "ASSIMP_BUILD_NO_HMP_IMPORTER", "ASSIMP_BUILD_NO_IFC_IMPORTER", "ASSIMP_BUILD_NO_IRR_IMPORTER", "ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
		"ASSIMP_BUILD_NO_LWO_IMPORTER", "ASSIMP_BUILD_NO_LWS_IMPORTER", "ASSIMP_BUILD_NO_MDL_IMPORTER", "ASSIMP_BUILD_NO_MS3D_IMPORTER", "ASSIMP_BUILD_NO_NDO_IMPORTER",
		"ASSIMP_BUILD_NO_NFF_IMPORTER", "ASSIMP_BUILD_NO_OFF_IMPORTER", "ASSIMP_BUILD_NO_Q3BSP_IMPORTER", "ASSIMP_BUILD_NO_Q3D_IMPORTER", "ASSIMP_BUILD_NO_STL_IMPORTER" }
	disablewarnings { "4267", "4996" }
	
	vpaths { 
		["Headers/**"] = "**.h",
		["Sources/**"] = "**.c*",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"include/**.h",
		"code/**.h",
		"code/**.cpp",

		"contrib/ConvertUTF/*.c",
		"contrib/irrXML/*.cpp",
		"contrib/clipper/*.cpp",
		--"contrib/openddlparser/code/*.cpp",
		"contrib/zlib/*.c",
		"contrib/unzip/*.c",
		"contrib/poly2tri/**.cc"
	}