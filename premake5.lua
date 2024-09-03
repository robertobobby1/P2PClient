outputdir = "%{cfg.buildcfg}-%{cfg.system}"

workspace "P2PClient"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	project "P2PClient"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
	
		targetdir ("%{wks.location}/bin/" .. outputdir .. "/")
		objdir ("%{wks.location}/bin/bin-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{wks.location}/P2PClient/Source/**.h",
			"%{wks.location}/P2PClient/Source/**.cpp",
		}  
		includedirs
		{
			"%{wks.location}/P2PClient/Source",
		}
		buildoptions
		{
			"-g"
		}