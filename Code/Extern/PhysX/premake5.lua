project "PhysX"
    kind "StaticLib"

    includedirs 
    {
        "include",
    
        "src/physx/src",
        "src/physx/src/buffering",
        "src/physx/src/device",
        "src/physx/src/gpu",
        "src/physx/src/windows",
        
        "src/foundation/include",

        "src/simulationcontroller/src",
        "src/simulationcontroller/include",
        
        "src/common/src",
        "src/common/include",
        
        "src/fastxml/include",
        "src/lowleveldynamics/include",

        "src/geomutils/include",
        "src/geomutils/src",
        "src/geomutils/src/ccd",
        "src/geomutils/src/common",
        "src/geomutils/src/contact",
        "src/geomutils/src/convex",
        "src/geomutils/src/distance",
        "src/geomutils/src/gjk",
        "src/geomutils/src/hf",
        "src/geomutils/src/intersection",
        "src/geomutils/src/mesh",
        "src/geomutils/src/pcm",
        "src/geomutils/src/sweep",

        "src/filebuf/include",

        "src/physxcooking/src",
        "src/physxcooking/src/convex",
        "src/physxcooking/src/mesh",

        "src/lowlevel/api/include",
        "src/lowlevel/common/include",
        "src/lowlevel/common/include/collision",
        "src/lowlevel/common/include/pipeline",
        "src/lowlevel/common/include/utils",
        "src/lowlevel/software/include",
        "src/lowlevelaabb/include",
        "src/lowleveldynamics/include",

        "src/pvd/include",
        "src/scenequery/include",
        
        "src/physxextensions/src",
        "src/physxextensions/src/serialization",
        "src/physxextensions/src/serialization/Binary",
        "src/physxextensions/src/serialization/File",
        "src/physxextensions/src/serialization/Xml",

        "src/physxgpu/include",
        "src/physxmetadata/core/include",
        "src/physxmetadata/extensions/include",

        "src/physxvehicle/src",
        "src/physxvehicle/src/physxmetadata/include"
    }

    files 
    {
        "src/**.cpp",
        "src/**.h"
    }

    defines { "PX_SUPPORT_PVD=0", "PX_PHYSX_STATIC_LIB", "_CRT_SECURE_NO_WARNINGS", "PX_COOKING" }

    filter "configurations:Debug"
        defines { "_DEBUG" }

    filter "configurations:Release"
        defines { "NDEBUG" }

    filter "configurations:Final"
        defines { "NDEBUG" }