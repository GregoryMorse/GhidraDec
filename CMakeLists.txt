cmake_minimum_required(VERSION 3.6)

project(ghidradec-idaplugin CXX C)
set(RELEASE_VERSION "1.3")

set(GHIDRA_VER "10.1.2")
set(GHIDRA_SRC_ZIP "Ghidra_${GHIDRA_VER}_build.zip")
set(GHIDRA_SRC_ZIP_HASH "c8614fa0145254c3540b50df808645290282adc797f1cce948ee3c3dfe9faa1d")

if(NOT EXISTS ${GHIDRA_SRC_ZIP})
	file(DOWNLOAD "https://github.com/NationalSecurityAgency/ghidra/archive/refs/tags/${GHIDRA_SRC_ZIP}" ${GHIDRA_SRC_ZIP} TIMEOUT 60 EXPECTED_HASH SHA256=${GHIDRA_SRC_ZIP_HASH})
endif()
set(GHIDRA_DEC_DEPS
	"address.*"
	"context.*"
	"emulate.*"
	"error.hh"
	"filemanage.*"
	"float.*"
	"globalcontext.*"
	"loadimage.*"
	"memstate.*"
	"opbehavior.*"
	"opcodes.*"
	"partmap.hh"
	"pcodecompile.*"
	"pcodeparse.*"
	"pcoderaw.*"
	"semantics.*"
	"sleigh.*"
	"sleighbase.*"
	"slghpatexpress.*"
	"slghpattern.*"
	"slghsymbol.*"
	"space.*"
	"translate.*"
	"types.h"
	"xml.*")

set(GHIDRA_DEC_PATH "ghidra-Ghidra_${GHIDRA_VER}_build/Ghidra/Features/Decompiler/src/decompile/cpp/")
list(TRANSFORM GHIDRA_DEC_DEPS PREPEND "${GHIDRA_DEC_PATH}")
file(ARCHIVE_EXTRACT INPUT ${GHIDRA_SRC_ZIP} DESTINATION ${CMAKE_SOURCE_DIR}/Ghidra PATTERNS "${GHIDRA_DEC_PATH}*")
file(ARCHIVE_EXTRACT INPUT ${GHIDRA_SRC_ZIP} DESTINATION ${CMAKE_SOURCE_DIR}/decompile PATTERNS ${GHIDRA_DEC_DEPS})
file(GLOB FILES_EXTRACTED RELATIVE "${CMAKE_SOURCE_DIR}/Ghidra/${GHIDRA_DEC_PATH}" "${CMAKE_SOURCE_DIR}/Ghidra/${GHIDRA_DEC_PATH}*")
foreach(FNAME IN LISTS FILES_EXTRACTED)
	file(RENAME "${CMAKE_SOURCE_DIR}/Ghidra/${GHIDRA_DEC_PATH}${FNAME}" "${CMAKE_SOURCE_DIR}/Ghidra/${FNAME}")
endforeach()
file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/Ghidra/ghidra-Ghidra_${GHIDRA_VER}_build")
file(GLOB FILES_EXTRACTED RELATIVE "${CMAKE_SOURCE_DIR}/decompile/${GHIDRA_DEC_PATH}" "${CMAKE_SOURCE_DIR}/decompile/${GHIDRA_DEC_PATH}*")
foreach(FNAME IN LISTS FILES_EXTRACTED)
	file(RENAME "${CMAKE_SOURCE_DIR}/decompile/${GHIDRA_DEC_PATH}${FNAME}" "${CMAKE_SOURCE_DIR}/decompile/${FNAME}")
endforeach()
file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/decompile/ghidra-Ghidra_${GHIDRA_VER}_build")
file(READ "${CMAKE_SOURCE_DIR}/decompile/filemanage.cc" filedata)
string(REGEX REPLACE "GetFileAttributes\\(" "GetFileAttributesA(" filedata "${filedata}")
file(WRITE "${CMAKE_SOURCE_DIR}/decompile/filemanage.cc" "${filedata}")

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
add_definitions(-D__IDP__ -D__PLUGIN__ -DNO_OBSOLETE_FUNCS)
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
include_directories("decompile") # Make our includes work.
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
	code_viewer.cpp
	config_generator.cpp
	decompiler.cpp
	defs.cpp
	idaplugin.cpp
	plugin_config.cpp
	sleighinterface.cpp
)

find_program(YACC_EXECUTABLE NAMES bison win_bison byacc yacc)
if(YACC_EXECUTABLE STREQUAL "YACC_EXECUTABLE-NOTFOUND")
	message(FATAL_ERROR "Neither bison nor win_bison nor byacc nor yacc was found.")
endif()
message(STATUS "Parser generator: ${YACC_EXECUTABLE}")

## bison: pcodeparse.y xml.y
##if(WIN32)
##	set(YACC_EXECUTABLE "win_bison.exe")
##	$(YACC_EXECUTABLE) -p pcode -o
##else()
##	set(YACC_EXECUTABLE "bison")
##endif()

add_custom_command(
	OUTPUT ${CMAKE_SOURCE_DIR}/decompile/xml.tab.cpp ${CMAKE_SOURCE_DIR}/decompile/xml.tab.h
	SOURCE ${CMAKE_SOURCE_DIR}/decompile/xml.y
	COMMAND ${YACC_EXECUTABLE} -p xml -o ${CMAKE_SOURCE_DIR}/decompile/xml.tab.cpp -d ${CMAKE_SOURCE_DIR}/decompile/xml.y
	DEPENDS ${CMAKE_SOURCE_DIR}/decompile/xml.y
)
add_custom_command(
	OUTPUT ${CMAKE_SOURCE_DIR}/decompile/pcodeparse.tab.cpp ${CMAKE_SOURCE_DIR}/decompile/pcodeparse.tab.h
	SOURCE ${pcap_SOURCE_DIR}/decompile/pcodeparse.y
	COMMAND ${YACC_EXECUTABLE} -p pcode -o ${CMAKE_SOURCE_DIR}/decompile/pcodeparse.tab.cpp -d ${CMAKE_SOURCE_DIR}/decompile/pcodeparse.y
	DEPENDS ${CMAKE_SOURCE_DIR}/decompile/pcodeparse.y
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
	decompile/pcodeparse.tab.cpp
	decompile/pcodecompile.cc
	decompile/sleighbase.cc
	decompile/slghsymbol.cc
	decompile/slghpatexpress.cc
	decompile/slghpattern.cc
	decompile/semantics.cc
	decompile/context.cc
	decompile/filemanage.cc
)

set(GHIDRA_CORE_SOURCES
	decompile/xml.tab.cpp
	decompile/space.cc
	decompile/float.cc
	decompile/address.cc
	decompile/pcoderaw.cc
	decompile/translate.cc
	decompile/opcodes.cc
	decompile/globalcontext.cc
)

set(GHIDRA_LIBSLA_NAMES
	decompile/loadimage.cc
	decompile/memstate.cc
	decompile/emulate.cc
	decompile/opbehavior.cc
)

# GhidraDec idaplugin libs.
add_library(idaplugin32 SHARED ${IDAPLUGIN_SOURCES} ${GHIDRA_SLEIGH_SOURCES} ${GHIDRA_CORE_SOURCES} ${GHIDRA_LIBSLA_NAMES})
add_library(idaplugin64 SHARED ${IDAPLUGIN_SOURCES} ${GHIDRA_SLEIGH_SOURCES} ${GHIDRA_CORE_SOURCES} ${GHIDRA_LIBSLA_NAMES})

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
