project "SoLoud"	
	kind "staticlib"
	warnings "off"
	includedirs "include/"
	
	files 
	{
		"**.cpp",
		"**.h"
	}
	
	defines "WITH_MINIAUDIO"