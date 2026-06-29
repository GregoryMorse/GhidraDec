cmake_minimum_required(VERSION 3.6)

project(ghidradec-idaplugin CXX C)
set(RELEASE_VERSION "1.3")

set(GHIDRA_VER "12.1.2" CACHE STRING "Ghidra version used by the plugin.")
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
if(NOT IDA_SDK_DIR AND NOT IDA_SDK_DIR32)
	message(FATAL_ERROR "Path to IDA SDK 7.x or 4-6.x was not specified. Use -DIDA_SDK_DIR=<path> or -DIDA_SDK_DIR32=<path>.")
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

# Create release.
if(GHIDRADEC_IDAPLUGIN_DOC)
	set(RELEASE_DIR_NAME "ghidradec-idaplugin")
	set(RELEASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${RELEASE_DIR_NAME}")
	set(RELEASE_LICENSE_DIR "${RELEASE_DIR}/license")
	if(MSVC) # Windows
		set(RELEASE_OS_NAME "windows")
	elseif(APPLE) # macOS
		set(RELEASE_OS_NAME "macOS")
	else() # Linux
		set(RELEASE_OS_NAME "linux")
	endif()
	add_custom_target(release
		DEPENDS user-guide idaplugin32 idaplugin64
		COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_LICENSE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSE" "${RELEASE_LICENSE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSE-THIRD-PARTY" "${RELEASE_LICENSE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/doc/user_guide/user_guide.pdf" "${RELEASE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:idaplugin32>" "${RELEASE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:idaplugin64>" "${RELEASE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${CMAKE_CURRENT_BINARY_DIR}/${RELEASE_DIR_NAME}-v${RELEASE_VERSION}-${RELEASE_OS_NAME}.zip" --format=zip "${RELEASE_DIR}"
	)
endif()

##
## CMake build script for the IDA Pro plugin.
##

# IDA SDK libs.
if(IDA_SDK_DIR32)
if(WIN32)
	set(idasdk_ea32 "${IDA_SDK_DIR32}/lib/x86_win_vc_32/ida.lib")
	set(idasdk_ea64 "${IDA_SDK_DIR32}/lib/x86_win_vc_64/ida.lib")
elseif(APPLE)
	set(idasdk_ea32 "${IDA_SDK_DIR32}/lib/x86_mac_gcc_32/libida.dylib")
	set(idasdk_ea64 "${IDA_SDK_DIR32}/lib/x86_mac_gcc_64/libida64.dylib")
elseif(UNIX) # APPLE is also UNIX, so it MUST be before this elseif().
	set(idasdk_ea32 "${IDA_SDK_DIR32}/lib/x86_linux_gcc_32/libida.so")
	set(idasdk_ea64 "${IDA_SDK_DIR32}/lib/x86_linux_gcc_64/libida64.so")
else()
	message(FATAL_ERROR "Unsupported system type: ${CMAKE_SYSTEM_NAME}")
endif()
else()
if(WIN32)
	set(idasdk_ea32 "${IDA_SDK_DIR}/lib/x64_win_vc_32/ida.lib")
	set(idasdk_ea64 "${IDA_SDK_DIR}/lib/x64_win_vc_64/ida.lib")
elseif(APPLE)
	set(idasdk_ea32 "${IDA_SDK_DIR}/lib/x64_mac_gcc_32/libida.dylib")
	set(idasdk_ea64 "${IDA_SDK_DIR}/lib/x64_mac_gcc_64/libida64.dylib")
elseif(UNIX) # APPLE is also UNIX, so it MUST be before this elseif().
	set(idasdk_ea32 "${IDA_SDK_DIR}/lib/x64_linux_gcc_32/libida.so")
	set(idasdk_ea64 "${IDA_SDK_DIR}/lib/x64_linux_gcc_64/libida64.so")
else()
	message(FATAL_ERROR "Unsupported system type: ${CMAKE_SYSTEM_NAME}")
endif()
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
add_library(idaplugin32 SHARED ${IDAPLUGIN_SOURCES} ${GHIDRA_SLEIGH_SOURCES} ${GHIDRA_CORE_SOURCES} ${GHIDRA_LIBSLA_NAMES} ${GHIDRA_ZLIB_SOURCES})
add_library(idaplugin64 SHARED ${IDAPLUGIN_SOURCES} ${GHIDRA_SLEIGH_SOURCES} ${GHIDRA_CORE_SOURCES} ${GHIDRA_LIBSLA_NAMES} ${GHIDRA_ZLIB_SOURCES})

target_compile_definitions(idaplugin64 PUBLIC __EA64__)

target_link_libraries(idaplugin32 retdec jsoncpp ${idasdk_ea32})
target_link_libraries(idaplugin64 retdec jsoncpp ${idasdk_ea64})

if(WIN32)
	# shlwapi.dll for PathRemoveFileSpec() used in utilsl/filesystem_path.h
	target_link_libraries(idaplugin32 shlwapi)
	target_link_libraries(idaplugin64 shlwapi)
endif()
if(MSYS)
	target_link_libraries(idaplugin32 ws2_32)
	target_link_libraries(idaplugin64 ws2_32)
endif()

set_target_properties(idaplugin32 PROPERTIES PREFIX "")
set_target_properties(idaplugin64 PROPERTIES PREFIX "")
set_target_properties(idaplugin32 PROPERTIES OUTPUT_NAME "ghidradec")
set_target_properties(idaplugin64 PROPERTIES OUTPUT_NAME "ghidradec64")
if(IDA_SDK_DIR32)
set_target_properties(idaplugin32 PROPERTIES SUFFIX ".plw")
#Note: ghidradec64 should be renamed after build to ghidradec - but lib and exp cannot conflict
set_target_properties(idaplugin64 PROPERTIES SUFFIX ".p64")
endif()

# Installation.
if(IDA_PATH)
	install(TARGETS idaplugin32 idaplugin64
		LIBRARY DESTINATION "${IDA_PATH}/plugins/"
		RUNTIME DESTINATION "${IDA_PATH}/plugins/"
	)
endif()
