cmake_minimum_required(VERSION 3.19)

project(ghidradec-idaplugin CXX C)

include(cmake/GhidraDecManifest.cmake)
ghidradec_manifest_load_project()
ghidradec_manifest_load_dependencies()

set(RELEASE_VERSION "${GHIDRADEC_RELEASE_VERSION}")
set(GHIDRADEC_IDA_VERSION "${GHIDRADEC_DEFAULT_IDA_SDK}" CACHE STRING "IDA SDK version from ghidradec.targets.json.")
set(GHIDRA_VER "${GHIDRADEC_DEFAULT_GHIDRA_VERSION}" CACHE STRING "Ghidra version used by the plugin.")
set(GHIDRA_INSTALL_DIR "" CACHE PATH "Optional path to a local Ghidra installation.")
set(GHIDRA_DECOMPILER_CPP_DIR "" CACHE PATH "Optional path to Ghidra's decompiler C++ sources.")
option(GHIDRADEC_REFRESH_GHIDRA_SOURCES "Refresh vendored decompiler sources from GHIDRA_DECOMPILER_CPP_DIR during configure." OFF)

set(GHIDRA_DEC_DEPS
	"address.*"
	"context.*"
	"emulate.*"
	"error.hh"
	"filemanage.*"
	"float.*"
	"globalcontext.*"
	"loadimage.*"
	"marshal.*"
	"memstate.*"
	"opbehavior.*"
	"opcodes.*"
	"partmap.hh"
	"pcodecompile.*"
	"pcodeparse.*"
	"pcoderaw.*"
	"rangemap.hh"
	"semantics.*"
	"slaformat.*"
	"sleigh.*"
	"sleighbase.*"
	"slghpatexpress.*"
	"slghpattern.*"
	"slghsymbol.*"
	"space.*"
	"translate.*"
	"types.h"
	"xml.*"
	"compression.*"
	"crc32.*")

if(GHIDRADEC_REFRESH_GHIDRA_SOURCES)
	if(NOT GHIDRA_DECOMPILER_CPP_DIR AND GHIDRA_INSTALL_DIR)
		set(GHIDRA_DECOMPILER_CPP_DIR "${GHIDRA_INSTALL_DIR}/Ghidra/Features/Decompiler/src/decompile/cpp")
	endif()
	if(NOT EXISTS "${GHIDRA_DECOMPILER_CPP_DIR}/address.cc")
		message(FATAL_ERROR "Ghidra decompiler sources were not found under GHIDRA_DECOMPILER_CPP_DIR=${GHIDRA_DECOMPILER_CPP_DIR}")
	endif()

	file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/Ghidra")
	file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/decompile")

	file(GLOB GHIDRA_DEC_ALL_FILES CONFIGURE_DEPENDS "${GHIDRA_DECOMPILER_CPP_DIR}/*")
	foreach(FNAME IN LISTS GHIDRA_DEC_ALL_FILES)
		if(NOT IS_DIRECTORY "${FNAME}")
			file(COPY "${FNAME}" DESTINATION "${CMAKE_SOURCE_DIR}/Ghidra")
		endif()
	endforeach()

	foreach(PATTERN IN LISTS GHIDRA_DEC_DEPS)
		file(GLOB GHIDRA_DEC_MATCHES CONFIGURE_DEPENDS "${GHIDRA_DECOMPILER_CPP_DIR}/${PATTERN}")
		foreach(FNAME IN LISTS GHIDRA_DEC_MATCHES)
			file(COPY "${FNAME}" DESTINATION "${CMAKE_SOURCE_DIR}/decompile")
		endforeach()
	endforeach()

	file(READ "${CMAKE_SOURCE_DIR}/decompile/filemanage.cc" filedata)
	string(REGEX REPLACE "GetFileAttributes\\(" "GetFileAttributesA(" filedata "${filedata}")
	file(WRITE "${CMAKE_SOURCE_DIR}/decompile/filemanage.cc" "${filedata}")

	if(EXISTS "${GHIDRA_DECOMPILER_CPP_DIR}/../zlib/zlib.h")
		file(COPY "${GHIDRA_DECOMPILER_CPP_DIR}/../zlib" DESTINATION "${CMAKE_SOURCE_DIR}/decompile")
	endif()
endif()

# Set the default build type to 'Release'
if(NOT CMAKE_BUILD_TYPE)
	set(default_build_type "Release")
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Check that obligatory parameters were defined.
ghidradec_manifest_resolve_ida_sdk("${GHIDRADEC_IDA_VERSION}")
if(NOT IDA_SDK_DIR AND NOT IDA_SDK_DIR32)
	message(FATAL_ERROR "Path to IDA SDK ${GHIDRADEC_IDA_VERSION} was not found. Run `python tools/ida_sdk.py ensure --version ${GHIDRADEC_IDA_VERSION}` or pass -DIDA_SDK_DIR=<path> / -DIDA_SDK_DIR32=<path>.")
endif()
if(NOT EXISTS "${IDA_SDK_DIR}" AND NOT EXISTS "${IDA_SDK_DIR32}")
	message(FATAL_ERROR "Specified IDA SDK path does not exist.")
endif()

# Build parameters.
if(MSVC) # Windows
	# Disable warnings (there are too many of them, including warnings from
	# third-party libraries, which cannot be selectively disabled when using MSVC).
	string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")

	# Disable the min() and max() macros to prevent errors when using e.g.
	# std::numeric_limits<...>::max()
	# (http://stackoverflow.com/questions/1904635/warning-c4003-and-errors-c2589-and-c2059-on-x-stdnumeric-limitsintmax).
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
elseif(UNIX) # Linux or macOS
	# Common options.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")

	# Ignore the following warnings (they are not fixable).
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-virtual-dtor")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
else()
	message(FATAL_ERROR "Unsupported system type: ${CMAKE_SYSTEM_NAME}")
endif()

# Global defines.
add_definitions(-D__IDP__ -D__PLUGIN__ -DNO_OBSOLETE_FUNCS -DNO_GZIP)
if(NOT IDA_SDK_DIR32)
add_definitions(-D__X64__)
endif()
add_definitions(-DRELEASE_VERSION="${RELEASE_VERSION}")
if(WIN32)
	add_definitions(-D__NT__)
elseif(APPLE)
	add_definitions(-D__MAC__)
elseif(UNIX) # APPLE is also UNIX, so it MUST be before this elseif().
	add_definitions(-D__LINUX__)
else()
	message(FATAL_ERROR "Unsupported system type: ${CMAKE_SYSTEM_NAME}")
endif()

# Subdirectories.
add_subdirectory(deps)
if(GHIDRADEC_IDAPLUGIN_DOC)
	add_subdirectory(doc)
endif()
## add_subdirectory(scripts)
## add_subdirectory(src)

##
## CMake build script for the IDA Pro plugin.
##

# IDA SDK libs.
if(IDA_SDK_DIR32)
	set(GHIDRADEC_IDA_SDK_ROOT "${IDA_SDK_DIR32}")
else()
	set(GHIDRADEC_IDA_SDK_ROOT "${IDA_SDK_DIR}")
endif()

function(ghidradec_find_ida_library OUT_VAR EA_MODE)
	if(WIN32)
		file(GLOB _preferred_candidates LIST_DIRECTORIES false "${GHIDRADEC_IDA_SDK_ROOT}/lib/x64_win_vc_${EA_MODE}/ida.lib")
		file(GLOB _candidates LIST_DIRECTORIES false "${GHIDRADEC_IDA_SDK_ROOT}/lib/*_win_vc_${EA_MODE}/ida.lib")
	elseif(APPLE)
		if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
			file(GLOB _preferred_candidates LIST_DIRECTORIES false "${GHIDRADEC_IDA_SDK_ROOT}/lib/arm64_mac_*_${EA_MODE}/libida*.dylib")
		else()
			file(GLOB _preferred_candidates LIST_DIRECTORIES false "${GHIDRADEC_IDA_SDK_ROOT}/lib/x64_mac_*_${EA_MODE}/libida*.dylib")
		endif()
		file(GLOB _candidates LIST_DIRECTORIES false "${GHIDRADEC_IDA_SDK_ROOT}/lib/*_mac_*_${EA_MODE}/libida*.dylib")
	elseif(UNIX)
		file(GLOB _preferred_candidates LIST_DIRECTORIES false "${GHIDRADEC_IDA_SDK_ROOT}/lib/x64_linux_*_${EA_MODE}/libida*.so")
		file(GLOB _candidates LIST_DIRECTORIES false "${GHIDRADEC_IDA_SDK_ROOT}/lib/*_linux_*_${EA_MODE}/libida*.so")
	else()
		message(FATAL_ERROR "Unsupported system type: ${CMAKE_SYSTEM_NAME}")
	endif()

	if(_preferred_candidates)
		list(SORT _preferred_candidates)
		list(GET _preferred_candidates 0 _library)
		set(${OUT_VAR} "${_library}" PARENT_SCOPE)
		return()
	endif()

	if(_candidates)
		list(SORT _candidates)
		list(GET _candidates 0 _library)
		set(${OUT_VAR} "${_library}" PARENT_SCOPE)
	else()
		set(${OUT_VAR} "" PARENT_SCOPE)
	endif()
endfunction()

ghidradec_find_ida_library(idasdk_ea32 32)
ghidradec_find_ida_library(idasdk_ea64 64)

if(NOT idasdk_ea32 AND NOT idasdk_ea64)
	message(FATAL_ERROR "No linkable IDA SDK libraries were found under ${GHIDRADEC_IDA_SDK_ROOT}/lib for ${CMAKE_SYSTEM_NAME}.")
endif()
# Includes.
include_directories("..") # Make our includes work.
include_directories(".") # Make our includes work.
include_directories("src") # Make our includes work.
include_directories("decompile") # Make our includes work.
include_directories("decompile/zlib") # Make Ghidra 12 .sla compression support work.
include_directories("whereami") # Make our includes work.
if(IDA_SDK_DIR32)
include_directories(SYSTEM
	"${IDA_SDK_DIR32}/include" # Make IDA SDK includes work.
)
else()
include_directories(SYSTEM
	"${IDA_SDK_DIR}/include" # Make IDA SDK includes work.
)
endif()

# GhidraDec idaplugin sources.
set(IDAPLUGIN_SOURCES
	src/code_viewer.cpp
	src/config_generator.cpp
	src/decompiler.cpp
	src/defs.cpp
	src/idaplugin.cpp
	src/plugin_config.cpp
	src/sleighinterface.cpp
)

##config and utils
#set(RETDEC_CONFIG_SOURCES
#	architecture.cpp
#	base.cpp
#	calling_convention.cpp
#	classes.cpp
#	config.cpp
#	file_format.cpp
#	file_type.cpp
#	functions.cpp
#	language.cpp
#	objects.cpp
#	parameters.cpp
#	patterns.cpp
#	segments.cpp
#	storage.cpp
#	tool_info.cpp
#	types.cpp
#	vtables.cpp
#)

#set(RETDEC_UTILS_SOURCES
#	address.cpp
#	alignment.cpp
#	binary_path.cpp
#	byte_value_storage.cpp
#	conversion.cpp
#	dynamic_buffer.cpp
#	file_io.cpp
#	filesystem_path.cpp
#	math.cpp
#	memory.cpp
#	string.cpp 
#	system.cpp
#	time.cpp
#)

#set(JSON_CPP_SOURCES
#	json_reader.cpp
#	json_value.cpp
#	json_writer.cpp
#)

#set(WHEREAMI_SOURCES
#	whereami/whereami.c
#)

set(GHIDRA_SLEIGH_SOURCES
	decompile/sleigh.cc
	decompile/pcodeparse.cc
	decompile/pcodecompile.cc
	decompile/sleighbase.cc
	decompile/slghsymbol.cc
	decompile/slghpatexpress.cc
	decompile/slghpattern.cc
	decompile/semantics.cc
	decompile/context.cc
	decompile/slaformat.cc
	decompile/compression.cc
	decompile/filemanage.cc
)

set(GHIDRA_CORE_SOURCES
	decompile/xml.cc
	decompile/marshal.cc
	decompile/space.cc
	decompile/float.cc
	decompile/address.cc
	decompile/pcoderaw.cc
	decompile/translate.cc
	decompile/opcodes.cc
	decompile/globalcontext.cc
	decompile/crc32.cc
)

set(GHIDRA_LIBSLA_NAMES
	decompile/loadimage.cc
	decompile/memstate.cc
	decompile/emulate.cc
	decompile/opbehavior.cc
)

set(GHIDRA_ZLIB_SOURCES
	decompile/zlib/adler32.c
	decompile/zlib/deflate.c
	decompile/zlib/inffast.c
	decompile/zlib/inflate.c
	decompile/zlib/inftrees.c
	decompile/zlib/trees.c
	decompile/zlib/zutil.c
)

# GhidraDec idaplugin libs.
set(GHIDRADEC_PLUGIN_TARGETS)

function(ghidradec_add_idaplugin TARGET_NAME EA_MODE IDA_LIBRARY OUTPUT_NAME)
	add_library(${TARGET_NAME} SHARED ${IDAPLUGIN_SOURCES} ${GHIDRA_SLEIGH_SOURCES} ${GHIDRA_CORE_SOURCES} ${GHIDRA_LIBSLA_NAMES} ${GHIDRA_ZLIB_SOURCES})
	if(EA_MODE EQUAL 64)
		target_compile_definitions(${TARGET_NAME} PUBLIC __EA64__)
	endif()

	target_link_libraries(${TARGET_NAME} retdec jsoncpp "${IDA_LIBRARY}")
	if(WIN32)
		# shlwapi.dll for PathRemoveFileSpec() used in utilsl/filesystem_path.h
		target_link_libraries(${TARGET_NAME} shlwapi)
	endif()
	if(MSYS)
		target_link_libraries(${TARGET_NAME} ws2_32)
	endif()

	set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" OUTPUT_NAME "${OUTPUT_NAME}")
	if(IDA_SDK_DIR32)
		if(EA_MODE EQUAL 32)
			set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ".plw")
		else()
			# ghidradec64 should be renamed after build to ghidradec, but lib and exp cannot conflict.
			set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ".p64")
		endif()
	endif()

	set(GHIDRADEC_PLUGIN_TARGETS ${GHIDRADEC_PLUGIN_TARGETS} ${TARGET_NAME} PARENT_SCOPE)
endfunction()

if(idasdk_ea32)
	message(STATUS "Building IDA EA32 plugin against ${idasdk_ea32}")
	ghidradec_add_idaplugin(idaplugin32 32 "${idasdk_ea32}" "ghidradec")
else()
	message(STATUS "Skipping IDA EA32 plugin; no EA32 SDK library exists for ${CMAKE_SYSTEM_NAME}.")
endif()

if(idasdk_ea64)
	message(STATUS "Building IDA EA64 plugin against ${idasdk_ea64}")
	ghidradec_add_idaplugin(idaplugin64 64 "${idasdk_ea64}" "ghidradec64")
else()
	message(STATUS "Skipping IDA EA64 plugin; no EA64 SDK library exists for ${CMAKE_SYSTEM_NAME}.")
endif()

# Installation.
if(IDA_PATH)
	install(TARGETS ${GHIDRADEC_PLUGIN_TARGETS}
		LIBRARY DESTINATION "${IDA_PATH}/plugins/"
		RUNTIME DESTINATION "${IDA_PATH}/plugins/"
	)
endif()

if(WIN32)
	set(GHIDRADEC_PACKAGE_OS "windows")
elseif(APPLE)
	set(GHIDRADEC_PACKAGE_OS "macos")
elseif(UNIX)
	set(GHIDRADEC_PACKAGE_OS "linux")
endif()

set(GHIDRADEC_PACKAGE_ROOT "${CMAKE_BINARY_DIR}/artifacts")
set(GHIDRADEC_PACKAGE_NAME "ghidradec-ida${GHIDRADEC_IDA_VERSION}-${GHIDRADEC_PACKAGE_OS}-${CMAKE_SYSTEM_PROCESSOR}")
set(GHIDRADEC_PACKAGE_DIR "${GHIDRADEC_PACKAGE_ROOT}/${GHIDRADEC_PACKAGE_NAME}")
file(MAKE_DIRECTORY "${GHIDRADEC_PACKAGE_ROOT}")
set(GHIDRADEC_PACKAGE_COPY_COMMANDS)
foreach(_plugin_target IN LISTS GHIDRADEC_PLUGIN_TARGETS)
	list(APPEND GHIDRADEC_PACKAGE_COPY_COMMANDS
		COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${_plugin_target}>" "${GHIDRADEC_PACKAGE_DIR}/plugins"
	)
endforeach()

add_custom_target(package_plugins
	DEPENDS ${GHIDRADEC_PLUGIN_TARGETS}
	COMMAND ${CMAKE_COMMAND} -E rm -rf "${GHIDRADEC_PACKAGE_DIR}"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GHIDRADEC_PACKAGE_DIR}/plugins"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSE" "${GHIDRADEC_PACKAGE_DIR}"
	${GHIDRADEC_PACKAGE_COPY_COMMANDS}
	COMMAND ${CMAKE_COMMAND} -E tar "cf" "${GHIDRADEC_PACKAGE_ROOT}/${GHIDRADEC_PACKAGE_NAME}.zip" --format=zip "${GHIDRADEC_PACKAGE_NAME}"
	WORKING_DIRECTORY "${GHIDRADEC_PACKAGE_ROOT}"
	COMMENT "Packaging ${GHIDRADEC_PACKAGE_NAME}.zip"
)
