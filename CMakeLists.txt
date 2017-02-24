cmake_minimum_required(VERSION 3.6)
project(padPokopom)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -D_UNICODE -DUNICODE")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(SOURCE_FILES
        Pokopom/Chankast.cpp
        Pokopom/Chankast.h
        Pokopom/ConfigDialog.cpp
        Pokopom/ConfigDialog.h
        Pokopom/DebugStuff.cpp
        Pokopom/DebugStuff.h
        Pokopom/demul.cpp
        Pokopom/demul.h
        Pokopom/FileIO.cpp
        Pokopom/FileIO.h
        Pokopom/General.h
        Pokopom/Input.h
        Pokopom/Input_Linux.cpp
        Pokopom/Input_Shared.cpp
        Pokopom/Input_Shared.h
        Pokopom/Input_XInput.cpp
        Pokopom/playstation_codes.h
        Pokopom/playstation_devices.h
        Pokopom/playstation_dualshock.cpp
        Pokopom/playstation_dualshock2.cpp
        Pokopom/playstation_guitar.cpp
        Pokopom/playstation_mtap.cpp
        Pokopom/Pokopom.rc
        Pokopom/psemupro.cpp
        Pokopom/psemupro.h
        Pokopom/PSemuPro_Interface.h
        Pokopom/regini.cpp
        Pokopom/regini.h
        Pokopom/resource.h
        Pokopom/Settings.cpp
        Pokopom/Settings.h
        Pokopom/Stuff.h
        Pokopom/Stuff_Linux.cpp
        Pokopom/Stuff_Shared.cpp
        Pokopom/Stuff_Windows.cpp
        Pokopom/TypeDefs.h
        Pokopom/Zilmar.cpp
        Pokopom/Zilmar_Controller_Interface.h
        Pokopom/Zilmar_Devices.cpp
        Pokopom/Zilmar_Devices.h)

add_library(padPokopom SHARED ${SOURCE_FILES})
set(CMAKE_EXE_LINKER_FLAGS " -static")
if(WIN32)
target_link_libraries(padPokopom UxTheme.lib)
endif(WIN32)