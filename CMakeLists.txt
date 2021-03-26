cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Set HyperEngine Project
project(HyperEngine LANGUAGES C CXX)

# Set Compiler Options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS Off)

option(BUILD_SAMPLES "Build sample projects" ON)

# Set Definitions
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    add_definitions(-DHYPERENGINE_RELEASE)
else ()
    add_definitions(-DHYPERENGINE_DEBUG)
endif ()

# Add Third-Party Dependencies
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# Set CXX Flags
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g3 -O0 -Wall -pedantic -Wextra -m64 -mavx2 -mfma -ffast-math")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -m64 -mavx2 -mfma -ffast-math")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /W4 /arch:AVX2 /fp:fast /MP /DNOMINMAX")
    endif ()
endif ()

# Adding Projects
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fno-rtti -fno-exceptions)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fno-rtti -fno-exceptions)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    string(REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(/EHsc)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperEngine)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperResource)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperEditor)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    string(REGEX REPLACE "-fno-rtti fno-exceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    string(REGEX REPLACE "-fno-rtti fno-exceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    string(REGEX REPLACE "/GR-" "/GR" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if(BUILD_SAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Samples)
endif()

# Set Visual Studio Property
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT HyperEditor)