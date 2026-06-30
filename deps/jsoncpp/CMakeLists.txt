include(ExternalProject)

ExternalProject_Add(jsoncpp-project
	URL ${GHIDRADEC_JSONCPP_URL}
	URL_HASH SHA256=${GHIDRADEC_JSONCPP_SHA256}
	DOWNLOAD_NAME jsoncpp.zip
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	# We build the three jsoncpp sources as part of the top-level project.
	# jsoncpp 1.8.4's own CMake hard-codes Clang -Werror flags that fail on
	# newer AppleClang versions.
	CONFIGURE_COMMAND ""
	# Disable the update step.
	UPDATE_COMMAND ""
	# Disable the build step.
	BUILD_COMMAND ""
	# Disable the install step.
	INSTALL_COMMAND ""
	LOG_DOWNLOAD ON
)

# Set include directories.
ExternalProject_Get_Property(jsoncpp-project source_dir)
set(JSONCPP_INCLUDE_DIR ${source_dir}/include)

set(JSONCPP_SOURCES
	${source_dir}/src/lib_json/json_reader.cpp
	${source_dir}/src/lib_json/json_value.cpp
	${source_dir}/src/lib_json/json_writer.cpp
)

add_custom_command(OUTPUT ${JSONCPP_SOURCES} DEPENDS jsoncpp-project)

add_library(jsoncpp STATIC ${JSONCPP_SOURCES})
add_dependencies(jsoncpp jsoncpp-project)
set_target_properties(jsoncpp PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(jsoncpp SYSTEM PUBLIC ${JSONCPP_INCLUDE_DIR})
