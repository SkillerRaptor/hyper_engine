#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
include("${CMAKE_SOURCE_DIR}/Utils/HyperUtils.cmake")

#-------------------------------------------------------------------------------------------
# assimp
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(ASSIMP_HUNTER_ENABLED OFF)
hyperengine_set_cache_value(BUILD_SHARED_LIBS OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_FRAMEWORK OFF)
hyperengine_set_cache_value(ASSIMP_DOUBLE_PRECISION OFF)
hyperengine_set_cache_value(ASSIMP_OPT_BUILD_PACKAGES OFF)
hyperengine_set_cache_value(ASSIMP_ANDROID_JNIIOSYSTEM OFF)
hyperengine_set_cache_value(ASSIMP_NO_EXPORT OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_ZLIB OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_ASSIMP_TOOLS ON)
hyperengine_set_cache_value(ASSIMP_BUILD_SAMPLES OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_TESTS OFF)
hyperengine_set_cache_value(ASSIMP_COVERALLS OFF)
hyperengine_set_cache_value(ASSIMP_INSTALL OFF)
hyperengine_set_cache_value(ASSIMP_ERROR_MAX OFF)
hyperengine_set_cache_value(ASSIMP_ASAN OFF)
hyperengine_set_cache_value(ASSIMP_UBSAN OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_DOCS OFF)
hyperengine_set_cache_value(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
hyperengine_set_cache_value(ASSIMP_IGNORE_GIT_HASH OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/assimp)
set_target_properties(assimp PROPERTIES FOLDER vendor/assimp)
hyperengine_set_output_directories(assimp)

#-------------------------------------------------------------------------------------------
# SPIRV-Headers
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Headers)
foreach(TARGET
        "install-headers" "SPIRV-Headers-example")
    set_target_properties(${TARGET} PROPERTIES FOLDER vendor/${TARGET})
    hyperengine_set_output_directories(${TARGET})
endforeach()

#-------------------------------------------------------------------------------------------
# SPIRV-Cross
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS ON)
hyperengine_set_cache_value(SPIRV_CROSS_SHARED OFF)
hyperengine_set_cache_value(SPIRV_CROSS_STATIC ON)
hyperengine_set_cache_value(SPIRV_CROSS_CLI OFF)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_TESTS OFF)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_GLSL ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_HLSL ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_MSL ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_CPP ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_REFLECT ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_C_API ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_UTIL ON)
hyperengine_set_cache_value(SPIRV_CROSS_SANITIZE_ADDRESS OFF)
hyperengine_set_cache_value(SPIRV_CROSS_SANITIZE_MEMORY OFF)
hyperengine_set_cache_value(SPIRV_CROSS_SANITIZE_THREADS OFF)
hyperengine_set_cache_value(SPIRV_CROSS_SANITIZE_UNDEFINED OFF)
hyperengine_set_cache_value(SPIRV_CROSS_NAMESPACE_OVERRIDE ON)
hyperengine_set_cache_value(SPIRV_CROSS_FORCE_STL_TYPES OFF)
hyperengine_set_cache_value(SPIRV_CROSS_SKIP_INSTALL ON)
hyperengine_set_cache_value(SPIRV_CROSS_WERROR OFF)
hyperengine_set_cache_value(SPIRV_CROSS_MISC_WARNINGS OFF)
hyperengine_set_cache_value(SPIRV_CROSS_FORCE_PIC OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross)
foreach(TARGET
        "spirv-cross-core" "spirv-cross-cpp"
        "spirv-cross-hlsl" "spirv-cross-glsl"
        "spirv-cross-reflect")
    set_target_properties(${TARGET} PROPERTIES FOLDER vendor/${TARGET})
    hyperengine_set_output_directories(${TARGET})
endforeach()

#-------------------------------------------------------------------------------------------
# SPIRV-Tools
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(SPIRV_SKIP_EXECUTABLES ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Tools)
foreach(TARGET
        "core_tables" "enum_string_mapping" "extinst_tables"
        "spirv-tools-pkg-config" "spirv-tools-shared-pkg-config"
        "spirv-tools-build-version" "spirv-tools-header-DebugInfo"
        "SPIRV-Tools-link" "SPIRV-Tools-shared"
        "spirv-tools-header-OpenCLDebugInfo100" "spirv-tools-vimsyntax" "spv-tools-cldi100" "spv-tools-clspvreflection" "spv-tools-debuginfo" "spv-tools-spv-amd-gs"
        "spv-tools-spv-amd-sb" "spv-tools-spv-amd-sevp" "spv-tools-spv-amd-stm")
    set_target_properties(${TARGET} PROPERTIES FOLDER vendor/${TARGET})
    hyperengine_set_output_directories(${TARGET})
endforeach()

#-------------------------------------------------------------------------------------------
# DirectXShaderCompiler
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(ENABLE_SPIRV_CODEGEN ON)
hyperengine_set_cache_value(CLANG_ENABLE_ARCMT OFF)
hyperengine_set_cache_value(CLANG_ENABLE_STATIC_ANALYZER OFF)
hyperengine_set_cache_value(CLANG_INCLUDE_TESTS OFF)
hyperengine_set_cache_value(LLVM_INCLUDE_TESTS OFF)
hyperengine_set_cache_value(HLSL_INCLUDE_TESTS OFF)
hyperengine_set_cache_value(HLSL_BUILD_DXILCONV OFF)
hyperengine_set_cache_value(HLSL_SUPPORT_QUERY_GIT_COMMIT_INFO OFF)
hyperengine_set_cache_value(HLSL_ENABLE_DEBUG_ITERATORS ON)
hyperengine_set_cache_value(LLVM_TARGETS_TO_BUILD "None" CACHE STRING "" FORCE)
hyperengine_set_cache_value(LLVM_INCLUDE_DOCS OFF)
hyperengine_set_cache_value(LLVM_INCLUDE_EXAMPLES OFF)
hyperengine_set_cache_value(LIBCLANG_BUILD_STATIC ON)
hyperengine_set_cache_value(LLVM_OPTIMIZED_TABLEGEN OFF)
hyperengine_set_cache_value(LLVM_REQUIRES_EH ON)
hyperengine_set_cache_value(LLVM_APPEND_VC_REV ON)
hyperengine_set_cache_value(LLVM_ENABLE_RTTI ON)
hyperengine_set_cache_value(LLVM_ENABLE_EH ON)
hyperengine_set_cache_value(LLVM_ENABLE_TERMINFO OFF)
hyperengine_set_cache_value(LLVM_DEFAULT_TARGET_TRIPLE "dxil-ms-dx" CACHE STRING "" FORCE)
hyperengine_set_cache_value(CLANG_BUILD_EXAMPLES OFF)
hyperengine_set_cache_value(LLVM_REQUIRES_RTTI ON)
hyperengine_set_cache_value(CLANG_CL OFF)
hyperengine_set_cache_value(DXC_BUILD_ARCH "x64" CACHE STRING "" FORCE)
hyperengine_set_cache_value(SPIRV_BUILD_TESTS OFF)
hyperengine_set_cache_value(SPIRV_SKIP_EXECUTABLES ON)
hyperengine_set_cache_value(SPIRV_SKIP_TESTS ON)

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/DirectXShaderCompiler)
set(CMAKE_CXX_STANDARD 17)
foreach(TARGET
        "clang" "dxc"
        "clangAnalysis" "clangAST" "clangASTMatchers" "clangBasic" "clangCodeGen" "clangDriver" "clangEdit" "clangFormat" "clangFrontend"
        "clangFrontendTool" "clangIndex" "clangLex" "clangParse" "clangRewrite" "clangRewriteFrontend" "clangSema" "clangSPIRV" "clangTooling"
        "clangToolingCore" "dxcompiler" "libclang"
        "ClangAttrClasses" "ClangAttrDump" "ClangAttrHasAttributeImpl" "ClangAttrImpl" "ClangAttrList" "ClangAttrParsedAttrImpl"
        "ClangAttrParsedAttrKinds" "ClangAttrParsedAttrList" "ClangAttrParserStringSwitches" "ClangAttrPCHRead" "ClangAttrPCHWrite"
        "ClangAttrSpellingListIndex" "ClangAttrTemplateInstantiate" "ClangAttrVisitor" "ClangCommentCommandInfo" "ClangCommentCommandList"
        "ClangCommentHTMLNamedCharacterReferences" "ClangCommentHTMLTags" "ClangCommentHTMLTagsProperties" "ClangCommentNodes" "ClangDeclNodes"
        "ClangDiagnosticAnalysis" "ClangDiagnosticAST" "ClangDiagnosticComment" "ClangDiagnosticCommon" "ClangDiagnosticDriver"
        "ClangDiagnosticFrontend" "ClangDiagnosticGroups" "ClangDiagnosticIndexName" "ClangDiagnosticLex" "ClangDiagnosticParse"
        "ClangDiagnosticSema" "ClangDiagnosticSerialization" "ClangStmtNodes"
        "LLVMAnalysis" "LLVMAsmParser" "LLVMBitReader" "LLVMBitWriter" "LLVMCore" "LLVMDxcSupport" "LLVMDXIL" "LLVMDxilContainer"
        "LLVMDxilPIXPasses" "LLVMDxilRootSignature" "LLVMDxrFallback" "LLVMHLSL" "LLVMInstCombine" "LLVMipa" "LLVMipo" "LLVMIRReader"
        "LLVMLinker" "LLVMLTO" "LLVMMSSupport" "LLVMOption" "LLVMPasses" "LLVMPassPrinters" "LLVMProfileData" "LLVMScalarOpts" "LLVMSupport"
        "LLVMTableGen" "LLVMTarget" "LLVMTransformUtils" "LLVMVectorize"
        "ClangDriverOptions" "DxcEtw" "intrinsics_gen" "TablegenHLSLOptions"
        "clang-tblgen" "llvm-tblgen" "hlsl_dxcversion_autogen" "hlsl_version_autogen")
    set_target_properties(${TARGET} PROPERTIES FOLDER vendor/${TARGET})
    hyperengine_set_output_directories(${TARGET})
endforeach()

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(FMT_PEDANTIC OFF)
hyperengine_set_cache_value(FMT_WERROR OFF)
hyperengine_set_cache_value(FMT_DOC OFF)
hyperengine_set_cache_value(FMT_INSTALL OFF)
hyperengine_set_cache_value(FMT_TEST OFF)
hyperengine_set_cache_value(FMT_FUZZ OFF)
hyperengine_set_cache_value(FMT_CUDA_TEST OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fmt)
set_target_properties(fmt PROPERTIES FOLDER vendor/fmt)
hyperengine_set_output_directories(fmt)

#-------------------------------------------------------------------------------------------
# glad
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)
set_target_properties(glad PROPERTIES FOLDER vendor/glad)
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")
hyperengine_set_output_directories(glad)

#-------------------------------------------------------------------------------------------
# glfw
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(GLFW_BUILD_EXAMPLES OFF)
hyperengine_set_cache_value(GLFW_BUILD_TESTS OFF)
hyperengine_set_cache_value(GLFW_BUILD_DOCS OFF)
hyperengine_set_cache_value(GLFW_INSTALL OFF)
hyperengine_set_cache_value(GLFW_VULKAN_STATIC OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set_target_properties(glfw PROPERTIES FOLDER vendor/glfw)
hyperengine_set_output_directories(glfw)

#-------------------------------------------------------------------------------------------
# glm
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glm)
set_target_properties(glm PROPERTIES FOLDER vendor/glm)
hyperengine_set_output_directories(glm)

#-------------------------------------------------------------------------------------------
# Dear ImGui
#-------------------------------------------------------------------------------------------
file(GLOB "IMGUI_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp")
file(GLOB "IMGUI_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h")

add_library(imgui ${IMGUI_SOURCES} ${IMGUI_HEADERS})
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(imgui PROPERTIES FOLDER vendor/imgui)
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
hyperengine_set_output_directories(imgui)

#-------------------------------------------------------------------------------------------
# ImGuizmo
#-------------------------------------------------------------------------------------------
file(GLOB "IMGUIZMO_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.cpp")
file(GLOB "IMGUIZMO_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.h")

add_library(ImGuizmo ${IMGUIZMO_SOURCES} ${IMGUIZMO_HEADERS})
set_target_properties(ImGuizmo PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ImGuizmo PROPERTIES FOLDER vendor/ImGuizmo)
target_include_directories(ImGuizmo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
target_include_directories(ImGuizmo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo")
hyperengine_set_output_directories(ImGuizmo)

#-------------------------------------------------------------------------------------------
# json
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(JSON_BuildTests OFF)
hyperengine_set_cache_value(JSON_Install OFF)
hyperengine_set_cache_value(JSON_MultipleHeaders OFF)
hyperengine_set_cache_value(JSON_ImplicitConversions ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/json)
set_target_properties(nlohmann_json PROPERTIES FOLDER vendor/nlohmann_json)
hyperengine_set_output_directories(nlohmann_json)

#-------------------------------------------------------------------------------------------
# meshoptimizer
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(MESHOPT_BUILD_DEMO OFF)
hyperengine_set_cache_value(MESHOPT_BUILD_GLTFPACK OFF)
hyperengine_set_cache_value(MESHOPT_BUILD_SHARED_LIBS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/meshoptimizer)
set_target_properties(meshoptimizer PROPERTIES FOLDER vendor/meshoptimizer)
hyperengine_set_output_directories(meshoptimizer)

#-------------------------------------------------------------------------------------------
# rttr
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(BUILD_RTTR_DYNAMIC OFF)
hyperengine_set_cache_value(BUILD_UNIT_TESTS OFF)
hyperengine_set_cache_value(BUILD_STATIC ON)
hyperengine_set_cache_value(BUILD_WITH_RTTI OFF)
hyperengine_set_cache_value(BUILD_BENCHMARKS OFF)
hyperengine_set_cache_value(BUILD_EXAMPLES OFF)
hyperengine_set_cache_value(BUILD_DOCUMENTATION OFF)
hyperengine_set_cache_value(BUILD_INSTALLER OFF)
hyperengine_set_cache_value(BUILD_PACKAGE OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rttr)
set_target_properties(rttr_core_lib PROPERTIES FOLDER vendor/rttr_core_lib)
hyperengine_set_output_directories(rttr_core_lib)

#-------------------------------------------------------------------------------------------
# stb_image
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stb_image)
set_target_properties(stb_image PROPERTIES FOLDER vendor/stb_image)
target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/stb_image/include")
hyperengine_set_output_directories(stb_image)
