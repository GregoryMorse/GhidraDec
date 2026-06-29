set(GHIDRADEC_MANIFEST_PATH "${CMAKE_SOURCE_DIR}/ghidradec.targets.json" CACHE FILEPATH "GhidraDec target/dependency manifest.")

if(NOT EXISTS "${GHIDRADEC_MANIFEST_PATH}")
	message(FATAL_ERROR "Target manifest not found: ${GHIDRADEC_MANIFEST_PATH}")
endif()

file(READ "${GHIDRADEC_MANIFEST_PATH}" GHIDRADEC_MANIFEST_JSON)

function(ghidradec_json_string OUT_VAR)
	string(JSON _value ERROR_VARIABLE _error GET "${GHIDRADEC_MANIFEST_JSON}" ${ARGN})
	if(_error)
		message(FATAL_ERROR "Could not read ${ARGN} from ${GHIDRADEC_MANIFEST_PATH}: ${_error}")
	endif()
	set(${OUT_VAR} "${_value}" PARENT_SCOPE)
endfunction()

function(ghidradec_manifest_load_project)
	ghidradec_json_string(_release_version project releaseVersion)
	ghidradec_json_string(_default_sdk project defaultIdaSdk)
	ghidradec_json_string(_default_ghidra project defaultGhidraVersion)

	set(GHIDRADEC_RELEASE_VERSION "${_release_version}" PARENT_SCOPE)
	set(GHIDRADEC_DEFAULT_IDA_SDK "${_default_sdk}" PARENT_SCOPE)
	set(GHIDRADEC_DEFAULT_GHIDRA_VERSION "${_default_ghidra}" PARENT_SCOPE)
endfunction()

function(ghidradec_manifest_load_dependencies)
	ghidradec_json_string(_jsoncpp_url dependencies jsoncpp url)
	ghidradec_json_string(_jsoncpp_sha256 dependencies jsoncpp sha256)
	ghidradec_json_string(_retdec_url dependencies retdec url)
	ghidradec_json_string(_retdec_sha256 dependencies retdec sha256)

	set(GHIDRADEC_JSONCPP_URL "${_jsoncpp_url}" PARENT_SCOPE)
	set(GHIDRADEC_JSONCPP_SHA256 "${_jsoncpp_sha256}" PARENT_SCOPE)
	set(GHIDRADEC_RETDEC_URL "${_retdec_url}" PARENT_SCOPE)
	set(GHIDRADEC_RETDEC_SHA256 "${_retdec_sha256}" PARENT_SCOPE)
endfunction()

function(ghidradec_manifest_find_ida_sdk VERSION)
	string(JSON _sdk_count LENGTH "${GHIDRADEC_MANIFEST_JSON}" idaSdks)
	math(EXPR _last_sdk "${_sdk_count} - 1")
	foreach(_index RANGE 0 ${_last_sdk})
		string(JSON _candidate GET "${GHIDRADEC_MANIFEST_JSON}" idaSdks ${_index} version)
		if(_candidate STREQUAL "${VERSION}")
			string(JSON _directory GET "${GHIDRADEC_MANIFEST_JSON}" idaSdks ${_index} directory)
			string(JSON _archive GET "${GHIDRADEC_MANIFEST_JSON}" idaSdks ${_index} archive)
			string(JSON _source GET "${GHIDRADEC_MANIFEST_JSON}" idaSdks ${_index} source)
			string(JSON _cmake_variable GET "${GHIDRADEC_MANIFEST_JSON}" idaSdks ${_index} cmakeVariable)

			set(GHIDRADEC_IDA_SDK_DIRECTORY "${_directory}" PARENT_SCOPE)
			set(GHIDRADEC_IDA_SDK_ARCHIVE "${_archive}" PARENT_SCOPE)
			set(GHIDRADEC_IDA_SDK_SOURCE "${_source}" PARENT_SCOPE)
			set(GHIDRADEC_IDA_SDK_CMAKE_VARIABLE "${_cmake_variable}" PARENT_SCOPE)
			set(GHIDRADEC_IDA_SDK_FOUND TRUE PARENT_SCOPE)
			return()
		endif()
	endforeach()

	set(GHIDRADEC_IDA_SDK_FOUND FALSE PARENT_SCOPE)
endfunction()

function(ghidradec_manifest_resolve_ida_sdk VERSION)
	ghidradec_manifest_find_ida_sdk("${VERSION}")
	if(NOT GHIDRADEC_IDA_SDK_FOUND)
		message(FATAL_ERROR "IDA SDK ${VERSION} is not defined in ${GHIDRADEC_MANIFEST_PATH}")
	endif()

	if(IDA_SDK_DIR OR IDA_SDK_DIR32)
		return()
	endif()

	set(_candidate_roots
		"${CMAKE_SOURCE_DIR}/.idasdks"
		"${CMAKE_SOURCE_DIR}/idasdks"
		"${CMAKE_BINARY_DIR}/idasdks"
	)

	foreach(_root IN LISTS _candidate_roots)
		if(EXISTS "${_root}/${GHIDRADEC_IDA_SDK_DIRECTORY}/include/ida.hpp")
			if(GHIDRADEC_IDA_SDK_CMAKE_VARIABLE STREQUAL "IDA_SDK_DIR32")
				set(IDA_SDK_DIR32 "${_root}/${GHIDRADEC_IDA_SDK_DIRECTORY}" CACHE PATH "IDA SDK directory for legacy 32-bit IDA SDKs." FORCE)
			else()
				set(IDA_SDK_DIR "${_root}/${GHIDRADEC_IDA_SDK_DIRECTORY}" CACHE PATH "IDA SDK directory." FORCE)
			endif()
			message(STATUS "Using IDA SDK ${VERSION} from ${_root}/${GHIDRADEC_IDA_SDK_DIRECTORY}")
			return()
		endif()
	endforeach()
endfunction()
