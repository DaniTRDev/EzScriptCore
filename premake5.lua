function DeclareCommon()
    staticruntime "Off"
    floatingpoint "Fast"
    vectorextensions "AVX2"

    -- versions
    systemversion "10.0"
    toolset "v142"
    cppdialect "C++20"

    -- build output options
    outputdir = "%{cfg.buildcfg}/%{cfg.platform}" -- "Release/x32" / "Release/x64" / "Debug/x32" / "Debug/x64"
    targetdir   ("bin/" .. outputdir)
    objdir      ("bin/int/" .. outputdir .. "/%{prj.name}")
    libdirs     ("bin/" .. outputdir .. "/%{prj.name}")

    --
    defines {
        "_CRT_SECURE_NO_WARNINGS", -- removes pesky warnings to use Microsoft "safer" variants of certain std:: functions
        "NOMINMAX", -- excludes certain MINMAX macros
        "WIN32_LEAN_AND_MEAN", -- excludes certain includes that we do not require from Windows.h
    }
end

function does_file_exist(file)
    local f = io.open(file,"r")
    if f ~= nil then
        io.close(f)

        return true
    end

    return false
end

workspace "EzScript"
    startproject "EzScriptUpdater"

    configurations { "Debug", "Release" }
    platforms { "x64" }

    project "magic_enum"
        location "vendor/%{prj.name}"
        kind "none"
        language "C++"

        files {
            "vendor/%{prj.name}/include/**.hpp",
            "vendor/%{prj.name}/**.cpp"
        }

        DeclareCommon()

    project "EzScriptCore"
        location "%{prj.name}"
        kind "StaticLib"
        language "C++"

        files {
            "%{prj.name}/**.h",
            "%{prj.name}/**.cpp"
        }

        includedirs {
            "vendor/magic_enum"
        }

        flags { "MultiProcessorCompile" }

        DeclareCommon()

    project "EzGlobalUpdater"
        location "%{prj.name}"
        kind "ConsoleApp"
        language "C++"

        includedirs {
			"EzScriptCore",
            "EzGlobalUpdater",
            "vendor/magic_enum"
        }

        -- PrecompiledHeaderInclude = "pch.h"
        -- PrecompiledHeaderSource = "src/pch.cpp"

        files {
            "%{prj.name}/**.h",
            "%{prj.name}/**.cpp",
            "%{prj.name}/**.asm"
        }

        libdirs {
            "bin/lib"
        }

        links {
			"EzScriptCore",
        }

        -- pchheader "%{PrecompiledHeaderInclude}"
        -- pchsource "%{PrecompiledHeaderSource}"

        DeclareCommon()

        flags {
            "LinkTimeOptimization",
            "Maps",
            "NoImportLib",
            "MultiProcessorCompile"
        }

        filter "configurations:Debug"
            flags {  }
            symbols "On"
            editandcontinue "Off"
            defines { "DEBUG" }

        filter "configurations:Release"
            flags { "NoManifest" }
            defines { "RELEASE", "NDEBUG" }
            optimize "speed"

        filter "configurations:Dist"
            flags { "FatalWarnings", "NoManifest" }
            defines { "DIST", "NDEBUG" }
            optimize "speed"
