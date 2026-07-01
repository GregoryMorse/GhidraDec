if(NOT DEFINED SOURCE OR SOURCE STREQUAL "")
	message(WARNING "GhidraDec deploy skipped: SOURCE was not provided.")
	return()
endif()

if(NOT DEFINED DESTINATION OR DESTINATION STREQUAL "")
	message(WARNING "GhidraDec deploy skipped: DESTINATION was not provided.")
	return()
endif()

if(NOT EXISTS "${SOURCE}")
	message(WARNING "GhidraDec deploy skipped: built plugin does not exist: ${SOURCE}")
	return()
endif()

get_filename_component(_plugin_name "${SOURCE}" NAME)
set(_destination_file "${DESTINATION}/${_plugin_name}")

execute_process(
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${DESTINATION}"
	RESULT_VARIABLE _mkdir_result
	OUTPUT_VARIABLE _mkdir_output
	ERROR_VARIABLE _mkdir_error
)
if(NOT _mkdir_result STREQUAL "0")
	message(WARNING "GhidraDec deploy could not create ${DESTINATION}: ${_mkdir_error}")
	return()
endif()

execute_process(
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${SOURCE}" "${_destination_file}"
	RESULT_VARIABLE _copy_result
	OUTPUT_VARIABLE _copy_output
	ERROR_VARIABLE _copy_error
)
if(_copy_result STREQUAL "0")
	message(STATUS "GhidraDec deployed ${_plugin_name} to ${DESTINATION}")
else()
	message(WARNING "GhidraDec deploy could not copy ${_plugin_name} to ${DESTINATION}: ${_copy_error}")
endif()
