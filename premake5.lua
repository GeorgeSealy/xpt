
xpt_version = "0.0.1-WiP"

if os.is "windows" then
    -- debug_libs = { "sodium-debug", "mbedtls-debug", "mbedx509-debug", "mbedcrypto-debug" }
    debug_libs = {  }
    release_libs = {  }
else
    debug_libs = { "SDL2.framework", "OpenGL.framework" }
    release_libs = debug_libs
    defines { "APPLE" }
end

language "C++"
kind "ConsoleApp"
configuration "Debug"
    defines { "DEBUG" }
    links { debug_libs }
configuration "Release"
    links { release_libs }

solution "xpt"
    platforms { "x64" }
    configurations { "Debug", "Release" }

    if os.is "windows" then
        includedirs { "src", "windows" }
        libdirs { "windows" }
    else
        includedirs { "src", "./macos" }   
        libdirs { ".", "./macos" }  
        buildoptions {"-F ../macos"} -- added 
        linkoptions {"-F ../macos"} -- note the ../ here, as it's used verbatim in the build settings
    end

    targetdir "bin/%{cfg.buildcfg}"
    location "build"

    -- if not os.is "windows" then
    --     targetdir "bin/"  
    -- end
    rtti "Off"
    flags { "ExtraWarnings", "StaticRuntime", "FloatFast", "EnableSSE2" }
    configuration "Debug"
        flags { "Symbols" }
        defines { "DEBUG" }
    configuration "Release"
        optimize "Speed"
        defines { "NDEBUG" }
        
project "xtp"
    files { "src/*.h",  "src/*.cpp" }

    if os.is "windows" then
    else
        links { "SDL2.framework" }
    end

    -- links { "yojimbo" }


if os.is "windows" then

    -- Windows

    newaction
    {
        trigger     = "vs",
        description = "Open xpt.sln",
        execute = function ()
            os.execute "premake5 vs2015"
            os.execute "start xpt.sln"
        end
    }

else

    -- MacOSX and Linux.
    
    newaction
    {
        trigger     = "xc",
        description = "Open xpt.xcworkspace",
        execute = function ()
            os.execute "premake5 xcode4"
            os.execute "open build/xpt.xcworkspace"
        end
    }

    -- newaction
    -- {
    --     trigger     = "test",
    --     description = "Build and run all unit tests",
    --     execute = function ()
    --         os.execute "test ! -e Makefile && premake5 gmake"
    --         if os.execute "make -j32 test" == 0 then
    --             os.execute "./bin/test"
    --         end
    --     end
    -- }

    -- newaction
    -- {
    --     trigger     = "release",
    --     description = "Create a release of this project",
    --     execute = function ()
    --         _ACTION = "clean"
    --         premake.action.call( "clean" )
    --         files_to_zip = "README.md BUILDING.md CHANGES.md ROADMAP.md *.cpp *.h premake5.lua docker rapidjson windows"
    --         os.execute( "rm -rf *.zip *.tar.gz *.7z" );
    --         os.execute( "rm -rf docker/libyojimbo" );
    --         os.execute( "zip -9r libyojimbo-" .. libyojimbo_version .. ".zip " .. files_to_zip )
    --         os.execute( "7z a -y -mx=9 -p\"panther moderns\" libyojimbo-" .. libyojimbo_version .. ".7z " .. files_to_zip )
    --         os.execute( "unzip libyojimbo-" .. libyojimbo_version .. ".zip -d libyojimbo-" .. libyojimbo_version );
    --         os.execute( "tar -zcvf libyojimbo-" .. libyojimbo_version .. ".tar.gz libyojimbo-" .. libyojimbo_version );
    --         os.execute( "rm -rf libyojimbo-" .. libyojimbo_version );
    --         os.execute( "mkdir -p release" );
    --         os.execute( "mv libyojimbo-" .. libyojimbo_version .. ".7z release" );
    --         os.execute( "mv libyojimbo-" .. libyojimbo_version .. ".zip release" );
    --         os.execute( "mv libyojimbo-" .. libyojimbo_version .. ".tar.gz release" );
    --         os.execute( "echo" );
    --         os.execute( "echo \"*** SUCCESSFULLY CREATED RELEASE - libyojimbo-" .. libyojimbo_version .. " *** \"" );
    --         os.execute( "echo" );
    --     end
    -- }



	-- newaction
	-- {
	-- 	trigger     = "docker",
	-- 	description = "Build and run a yojimbo server inside a docker container",
	-- 	execute = function ()
	-- 		os.execute "cd docker && copyFiles.bat && buildServer.bat && runServer.bat"
	-- 	end
	-- }

 --    newaction
 --    {
 --        trigger     = "matcher",
 --        description = "Build and run the matchmaker web service inside a docker container",
 --        execute = function ()
 --            os.execute "cd docker\\matcher && docker build -t networkprotocol:yojimbo-matcher . && docker run -ti -p 8080:8080 networkprotocol:yojimbo-matcher"
 --        end
 --    }

 --    newaction
 --    {
 --        trigger     = "stress",
 --        description = "Launch 64 connect instances to stress the matcher and server",
 --        execute = function ()
 --            for i = 0, 63 do
 --                os.execute "if exist bin\\x64\\Debug\\connect.exe ( start /B bin\\x64\\Debug\\connect ) else ( echo could not find bin\\x64\\Debug\\connect.exe )"
 --            end
 --        end
 --    }

end

-- newaction
-- {
--     trigger     = "clean",

--     description = "Clean all build files and output",

--     execute = function ()

--         files_to_delete = 
--         {
--             "Makefile",
--             "*.make",
--             "*.txt",
--             "*.7z",
--             "*.zip",
--             "*.tar.gz",
--             "*.db",
--             "*.opendb",
--             "*.vcproj",
--             "*.vcxproj",
--             "*.vcxproj.user",
--             "*.sln"
--         }

--         directories_to_delete = 
--         {
--             "obj",
--             "ipch",
--             "bin",
--             ".vs",
--             "Debug",
--             "Release",
--             "release",
--             "cov-int",
--             "docker/libyojimbo"
--         }

--         for i,v in ipairs( directories_to_delete ) do
--           os.rmdir( v )
--         end

--         if not os.is "windows" then
--             os.execute "find . -name .DS_Store -delete"
--             for i,v in ipairs( files_to_delete ) do
--               os.execute( "rm -f " .. v )
--             end
--         else
--             for i,v in ipairs( files_to_delete ) do
--               os.execute( "del /F /Q  " .. v )
--             end
--         end

--     end
-- }
