#-------------------------------------------------------------------------------------------
# Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
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
set(SPIRV_HEADERS_SKIP_EXAMPLES ON)
set(SPIRV_HEADERS_SKIP_INSTALL ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Headers EXCLUDE_FROM_ALL)
set_target_properties(install-headers PROPERTIES FOLDER ThirdParty)

#-------------------------------------------------------------------------------------------
# SPIRV-Tools
#-------------------------------------------------------------------------------------------
set(SPIRV_ALLOW_TIMERS ON)
set(SKIP_SPIRV_TOOLS_INSTALL ON)
set(SPIRV_BUILD_COMPRESSION OFF)
set(SPIRV_BUILD_FUZZER OFF)
set(SPIRV_WERROR OFF)
set(SPIRV_COLOR_TERMINAL ON)
set(SPIRV_LOG_DEBUG OFF)
set(SPIRV_TOOLS_BUILD_STATIC ON)
set(SPIRV_SKIP_EXECUTABLES ON)
set(SPIRV_SKIP_TESTS ON)
set(SPIRV_CHECK_CONTEXT ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Tools EXCLUDE_FROM_ALL)
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
# DirectXShaderCompiler
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    if (NOT CMAKE_GENERATOR MATCHES "Visual Studio 15 2017.*")
        if (NOT CMAKE_GENERATOR MATCHES "Visual Studio 16 2019")
            set(PROGRAMFILES_X86 "ProgramFiles(x86)")
            execute_process(
                    COMMAND "$ENV{${PROGRAMFILES_X86}}/Microsoft Visual Studio/Installer/vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
                    OUTPUT_VARIABLE VSWHERE_LATEST
                    ERROR_QUIET
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

            find_path(
                    DIASDK_INCLUDE_DIR dia2.h
                    HINTS "${VS_DIA_INC_PATH}"
                    HINTS "${VSWHERE_LATEST}/DIA SDK/include"
                    DOC "path to DIA SDK header files")

            find_library(DIASDK_GUIDS_LIBRARY NAMES diaguids.lib HINTS ${DIASDK_INCLUDE_DIR}/../lib/amd64)
        endif ()
    endif ()
endif ()

set(ENABLE_SPIRV_CODEGEN ON)
set(CLANG_ENABLE_ARCMT OFF)
set(CLANG_ENABLE_STATIC_ANALYZER OFF)
set(CLANG_INCLUDE_TESTS OFF)
set(LLVM_INCLUDE_TESTS OFF)
set(HLSL_INCLUDE_TESTS OFF)
set(HLSL_BUILD_DXILCONV OFF)
set(HLSL_SUPPORT_QUERY_GIT_COMMIT_INFO ON)
set(HLSL_ENABLE_DEBUG_ITERATORS ON)
set(LLVM_TARGETS_TO_BUILD "None")
set(LLVM_INCLUDE_DOCS OFF)
set(LLVM_INCLUDE_EXAMPLES OFF)
set(LIBCLANG_BUILD_STATIC ON)
set(LLVM_OPTIMIZED_TABLEGEN OFF)
set(LLVM_REQUIRES_EH ON)
set(LLVM_APPEND_VC_REV ON)
set(LLVM_ENABLE_RTTI ON)
set(LLVM_ENABLE_EH ON)
set(LLVM_ENABLE_TERMINFO OFF)
set(LLVM_DEFAULT_TARGET_TRIPLE "dxil-ms-dx")
set(CLANG_BUILD_EXAMPLES OFF)
set(LLVM_REQUIRES_RTTI ON)
set(CLANG_CL OFF)
set(DXC_BUILD_ARCH "x64")
set(SPIRV_BUILD_TESTS OFF)
set(CMAKE_CXX_STANDARD 14)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/DirectXShaderCompiler EXCLUDE_FROM_ALL)
set(CMAKE_CXX_STANDARD 17)
foreach (TARGET
        "clang"
        "clangAnalysis" "clangAST" "clangASTMatchers" "clangBasic" "clangCodeGen" "clangDriver" "clangEdit" "clangFormat" "clangFrontend"
        "clangFrontendTool" "clangIndex" "clangLex" "clangParse" "clangRewrite" "clangRewriteFrontend" "clangSema" "clangSPIRV" "clangTooling"
        "clangToolingCore" "libclang"
        "dxc" "dxcompiler"
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
        "LLVMTableGen" "LLVMTarget" "LLVMTransformUtils" "LLVMVectorize" "LLVMMiniz"
        "ClangDriverOptions" "DxcEtw" "intrinsics_gen" "TablegenHLSLOptions"
        "clang-tblgen" "llvm-tblgen" "hlsl_dxcversion_autogen" "hlsl_version_autogen" "SPIRV-Tools-static" "SPIRV-Tools-opt"
        "GIT_COMMIT_INFO_ALWAYS_REBUILD")
    set_target_properties(${TARGET} PROPERTIES FOLDER ThirdParty)
endforeach ()

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    foreach (TARGET
            "dxexp" "dxclib" "dxa" "dxl" "dxopt" "dxr" "dxv" "dndxc" "dxlib_sample" "dxrfallbackcompiler"
            "d3dcompiler_dxc_bridge" "LLVMDxilDia")
        set_target_properties(${TARGET} PROPERTIES FOLDER ThirdParty)
    endforeach ()
endif ()

#-------------------------------------------------------------------------------------------
# SPIRV-Cross
#-------------------------------------------------------------------------------------------
set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS ON)
set(SPIRV_CROSS_SHARED OFF)
set(SPIRV_CROSS_STATIC ON)
set(SPIRV_CROSS_CLI OFF)
set(SPIRV_CROSS_ENABLE_TESTS OFF)
set(SPIRV_CROSS_ENABLE_GLSL ON)
set(SPIRV_CROSS_ENABLE_HLSL ON)
set(SPIRV_CROSS_ENABLE_MSL OFF)
set(SPIRV_CROSS_ENABLE_CPP ON)
set(SPIRV_CROSS_ENABLE_REFLECT ON)
set(SPIRV_CROSS_ENABLE_C_API ON)
set(SPIRV_CROSS_ENABLE_UTIL ON)
set(SPIRV_CROSS_SANITIZE_ADDRESS OFF)
set(SPIRV_CROSS_SANITIZE_MEMORY OFF)
set(SPIRV_CROSS_SANITIZE_THREADS OFF)
set(SPIRV_CROSS_SANITIZE_UNDEFINED OFF)
set(SPIRV_CROSS_NAMESPACE_OVERRIDE ON)
set(SPIRV_CROSS_FORCE_STL_TYPES OFF)
set(SPIRV_CROSS_SKIP_INSTALL ON)
set(SPIRV_CROSS_WERROR OFF)
set(SPIRV_CROSS_MISC_WARNINGS OFF)
set(SPIRV_CROSS_FORCE_PIC OFF)
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
# fmt
#-------------------------------------------------------------------------------------------
set(FMT_PEDANTIC ON)
set(FMT_WERROR OFF)
set(FMT_DOC OFF)
set(FMT_INSTALL OFF)
set(FMT_TEST OFF)
set(FMT_FUZZ OFF)
set(FMT_CUDA_TEST OFF)
set(FMT_OS ON)
set(FMT_MODULE OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fmt EXCLUDE_FROM_ALL)
set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(fmt PROPERTIES FOLDER ThirdParty)
