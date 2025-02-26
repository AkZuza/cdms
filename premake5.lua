workspace "cdms"
	configurations { "Debug", "Release" }

	project "cdms"
		kind "ConsoleApp"
		language "C"

		files {
			"src/**.c",
			"src/**.h"
		}


