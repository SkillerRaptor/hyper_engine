#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# doxygen
#-------------------------------------------------------------------------------------------
function(hyper_enable_doxygen)
		option(HYPER_ENABLE_DOXYGEN "Enable doxygen documentation builds of source" OFF)

		if (HYPER_ENABLE_DOXYGEN)
				set(DOXYGEN_ALPHABETICAL_INDEX NO)
				set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
				set(DOXYGEN_CALLER_GRAPH YES)
				set(DOXYGEN_CALL_GRAPH YES)
				set(DOXYGEN_CLASS_DIAGRAMS YES)
				set(DOXYGEN_CLASS_GRAPH YES)
				set(DOXYGEN_COLLABORATION_GRAPH YES)
				set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)
				set(DOXYGEN_DOT_TRANSPARENT YES)
				set(DOXYGEN_ENABLE_PREPROCESSING YES)
				set(DOXYGEN_EXCLUDE_PATTERNS "*/third_party/*")
				set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
				set(DOXYGEN_EXTRACT_ALL YES)
				set(DOXYGEN_GENERATE_TREEVIEW YES)
				set(DOXYGEN_HAVE_DOT YES)
				set(DOXYGEN_HIDE_UNDOC_RELATIONS NO)
				set(DOXYGEN_MACRO_EXPANSION YES)
				set(DOXYGEN_RECURSIVE YES)
				set(DOXYGEN_REFERENCED_BY_RELATION YES)
				set(DOXYGEN_REFERENCES_RELATION YES)
				set(DOXYGEN_SORT_BY_SCOPE_NAME YES)
				set(DOXYGEN_SOURCE_BROWSER YES)
				set(DOXYGEN_TEMPLATE_RELATIONS YES)
				
				if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
						set(DOXYGEN_PREDEFINED "HYPER_DEBUG=1")
				else()
						set(DOXYGEN_PREDEFINED "HYPER_DEBUG=0")
				endif()

				find_package(Doxygen REQUIRED dot)
				doxygen_add_docs(hyper_docs ${PROJECT_SOURCE_DIR})
		endif ()
endfunction()
