solution "Solar Odyssey"
	location "../Solution/x64"
	startproject "Solar Odyssey"
	
    kind "WindowedApp"
	language "C++"
	cppdialect "C++14"
	architecture "x64"
	filter "system:windows"
		systemversion "latest"
	
	debugdir "../"
	
	configurations { "Debug", "Release", "Final" }
	objdir ("../Temp")
	filter "configurations:Debug"
		symbols "on"
		targetdir ("../Bin/Debug")
	
	filter "configurations:Release"
		symbols "on"
		optimize "on"
		targetdir ("../Bin/Release")

	filter "configurations:Final"
		optimize "on"	
		targetdir ("../Bin/Final")

project "Solar Odyssey"
	files 
	{ 
		"../Code/GrumpyEngine/**.h", "../Code/GrumpyEngine/**.cpp",
		"../Code/SolarOdyssey/**.h", "../Code/SolarOdyssey/**.cpp"
	}
	
	includedirs  
	{		
		"../Code/GrumpyEngine",
		"../Code/GrumpyEngine/Audio",
		"../Code/GrumpyEngine/Common",
		"../Code/GrumpyEngine/Graphics",
		"../Code/GrumpyEngine/Physics",
		"../Code/GrumpyEngine/Input",
		"../Code/GrumpyEngine/Networking",
		"../Code/GrumpyEngine/Editor",

		"../Code/SolarOdyssey",
		
		"../Code/Extern/ImGui",
		"../Code/Extern/GLM",
		"../Code/Extern/stb",
		"../Code/Extern/PhysX/include",
		"../Code/Extern/GLFW/include",
		"../Code/Extern/GLAD/include",
		"../Code/Extern/SoLoud/include",
		"../Code/Extern/yaml-cpp/include",
		"../Code/Extern/json/include",
        "../Code/Extern/tinygltf/include"
	}

	links 
	{
		"ImGui",
		"Stb",
		"GLFW",
		"GLAD",
		"SoLoud",
        "PhysX",
		"yaml-cpp"
	}

	defines
	{
		"GE_PLATFORM_PC=1",
		
		"GE_ENABLE_PHYSICS=1",
		"GE_ENABLE_NETWORKING=0",

		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"GLM_ENABLE_EXPERIMENTAL",
		"NOMINMAX",
		"WIN32_LEAN_AND_MEAN",
		"GLM_ENABLE_EXPERIMENTAL",
        "PX_PHYSX_STATIC_LIB"
	}

	flags { "MultiProcessorCompile" }

	filter "configurations:Debug"
		defines { "GE_DEBUG=1", "GE_RELEASE=0", "GE_FINAL=0", "_DEBUG" }

	filter "configurations:Release"
		defines { "GE_DEBUG=0", "GE_RELEASE=1", "GE_FINAL=0", "NDEBUG" }

	filter "configurations:Final"
		defines { "GE_DEBUG=0", "GE_RELEASE=0", "GE_FINAL=1", "NDEBUG" }

    filter ""

	pchheader "pch.h"
	pchsource "../Code/Game/pch.cpp"

group "Dependencies"
include "../Code/Extern/GLFW"
include "../Code/Extern/GLAD"
include "../Code/Extern/GLM"
include "../Code/Extern/ImGui"
include "../Code/Extern/json"
include "../Code/Extern/stb"
include "../Code/Extern/SoLoud"
include "../Code/Extern/yaml-cpp"
include "../Code/Extern/tinygltf"
include "../Code/Extern/Physx/"