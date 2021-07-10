#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# SPIRV-Headers
#-------------------------------------------------------------------------------------------
set(SPIRV_HEADERS_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SPIRV_HEADERS_SKIP_INSTALL ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Headers)
set_target_properties(install-headers PROPERTIES FOLDER ThirdParty)

#-------------------------------------------------------------------------------------------
# SPIRV-Tools
#-------------------------------------------------------------------------------------------
set(SPIRV_ALLOW_TIMERS ON CACHE STRING "" FORCE)
set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE STRING "" FORCE)
set(SPIRV_BUILD_COMPRESSION OFF CACHE STRING "" FORCE)
set(SPIRV_BUILD_FUZZER OFF CACHE STRING "" FORCE)
set(SPIRV_WERROR OFF CACHE STRING "" FORCE)
set(SPIRV_COLOR_TERMINAL ON CACHE STRING "" FORCE)
set(SPIRV_LOG_DEBUG OFF CACHE STRING "" FORCE)
set(SPIRV_TOOLS_BUILD_STATIC ON CACHE STRING "" FORCE)
set(SPIRV_SKIP_EXECUTABLES ON CACHE STRING "" FORCE)
set(SPIRV_SKIP_TESTS ON CACHE STRING "" FORCE)
set(SPIRV_CHECK_CONTEXT ON CACHE STRING "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Tools)
foreach (TARGET
        "core_tables" "enum_string_mapping" "extinst_tables"
        "spirv-tools-pkg-config" "spirv-tools-shared-pkg-config"
        "spirv-tools-build-version" "spirv-tools-header-DebugInfo"
        "SPIRV-Tools-link" "SPIRV-Tools-shared" "SPIRV-Tools-reduce"
        "spirv-tools-header-OpenCLDebugInfo100" "spirv-tools-vimsyntax" "spv-tools-cldi100" "spv-tools-clspvreflection" "spv-tools-debuginfo" "spv-tools-spv-amd-gs"
        "spv-tools-spv-amd-sb" "spv-tools-spv-amd-sevp" "spv-tools-spv-amd-stm")
    set_target_properties(${TARGET} PROPERTIES FOLDER ThirdParty)
endforeach ()

#-------------------------------------------------------------------------------------------
# SPIRV-Cross
#-------------------------------------------------------------------------------------------
set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_SHARED OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_STATIC ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_CLI OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_ENABLE_GLSL ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_ENABLE_HLSL ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_ENABLE_MSL OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_ENABLE_CPP ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_ENABLE_REFLECT ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_ENABLE_C_API ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_ENABLE_UTIL ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_SANITIZE_ADDRESS OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_SANITIZE_MEMORY OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_SANITIZE_THREADS OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_SANITIZE_UNDEFINED OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_NAMESPACE_OVERRIDE ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_FORCE_STL_TYPES OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_SKIP_INSTALL ON CACHE STRING "" FORCE)
set(SPIRV_CROSS_WERROR OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_MISC_WARNINGS OFF CACHE STRING "" FORCE)
set(SPIRV_CROSS_FORCE_PIC OFF CACHE STRING "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross)
foreach (TARGET
        "spirv-cross-c" "spirv-cross-core" "spirv-cross-cpp"
        "spirv-cross-glsl" "spirv-cross-hlsl"
        "spirv-cross-reflect" "spirv-cross-util")
    set_target_properties(${TARGET} PROPERTIES FOLDER ThirdParty)
endforeach ()

#-------------------------------------------------------------------------------------------
# SPIRV-Reflect
#-------------------------------------------------------------------------------------------
add_library(spirv-reflect STATIC ${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Reflect/spirv_reflect.c ${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Reflect/spirv_reflect.h)
set_target_properties(spirv-reflect PROPERTIES FOLDER ThirdParty)

#-------------------------------------------------------------------------------------------
# glslang
#-------------------------------------------------------------------------------------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
set(ENABLE_SPVREMAPPER ON CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_JS OFF CACHE BOOL "" FORCE)
set(ENABLE_RTTI OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(ENABLE_PCH OFF CACHE BOOL "" FORCE)
set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)
set(USE_CCACHE OFF CACHE BOOL "" FORCE)
add_subdirectory(glslang)

#-------------------------------------------------------------------------------------------
# assimp
#-------------------------------------------------------------------------------------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FRAMEWORK OFF CACHE BOOL "" FORCE)
set(ASSIMP_DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
set(ASSIMP_OPT_BUILD_PACKAGES OFF CACHE BOOL "" FORCE)
set(ASSIMP_ANDROID_JNIIOSYSTEM OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_COVERALLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_ERROR_MAX OFF CACHE BOOL "" FORCE)
set(ASSIMP_ASAN OFF CACHE BOOL "" FORCE)
set(ASSIMP_UBSAN OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX ON CACHE BOOL "" FORCE)
set(ASSIMP_IGNORE_GIT_HASH ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/assimp)
set_target_properties(assimp PROPERTIES FOLDER ThirdParty)

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
set(FMT_PEDANTIC ON CACHE BOOL "" FORCE)
set(FMT_WERROR OFF CACHE BOOL "" FORCE)
set(FMT_DOC OFF CACHE BOOL "" FORCE)
set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_FUZZ OFF CACHE BOOL "" FORCE)
set(FMT_CUDA_TEST OFF CACHE BOOL "" FORCE)
set(FMT_OS ON CACHE BOOL "" FORCE)
set(FMT_MODULE OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fmt)
set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(fmt PROPERTIES FOLDER ThirdParty)

#-------------------------------------------------------------------------------------------
# meshoptimizer
#-------------------------------------------------------------------------------------------
set(MESHOPT_BUILD_DEMO OFF CACHE BOOL "" FORCE)
set(MESHOPT_BUILD_GLTFPACK OFF CACHE BOOL "" FORCE)
set(MESHOPT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/meshoptimizer)
set_target_properties(meshoptimizer PROPERTIES FOLDER ThirdParty)

#-------------------------------------------------------------------------------------------
# stb_image
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stb_image)
set_target_properties(stb_image PROPERTIES FOLDER ThirdParty)
