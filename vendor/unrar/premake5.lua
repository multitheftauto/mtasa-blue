project "unrar"
	language "C++"
	kind "StaticLib"
	targetname "unrar"

	defines { "RARDLL" }

	vpaths {
		["Headers/*"] = "**.hpp",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",

		"*.hpp",

		"archive.cpp",
		"arcread.cpp",
		"cmddata.cpp",
		"consio.cpp",
		"crc.cpp",
		"crypt.cpp",
		"dll.cpp",
		"encname.cpp",
		"errhnd.cpp",
		"extinfo.cpp",
		"extract.cpp",
		"filcreat.cpp",
		"file.cpp",
		"filefn.cpp",
		"filestr.cpp",
		"find.cpp",
		"getbits.cpp",
		"global.cpp",
		"list.cpp",
		"match.cpp",
		"options.cpp",
		"pathfn.cpp",
		"rar.cpp",
		"rarpch.cpp",
		"rarvm.cpp",
		"rawread.cpp",
		"rdwrfn.cpp",
		"recvol.cpp",
		"rijndael.cpp",
		"rs.cpp",
		"scantree.cpp",
		"secpassword.cpp",
		"sha1.cpp",
		"smallfn.cpp",
		"strfn.cpp",
		"strlist.cpp",
		"system.cpp",
		"timefn.cpp",
		"unicode.cpp",
		"unpack.cpp",
		"volume.cpp",
		"blake2s.cpp",
		"hash.cpp",
		"headers.cpp",
		"qopen.cpp",
		"rs16.cpp",
		"sha256.cpp",
		"threadpool.cpp",
		"ui.cpp"
	}

	filter "system:Windows*"
		files { "isnt.cpp" }