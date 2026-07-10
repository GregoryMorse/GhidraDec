include(ExternalProject)

ExternalProject_Add(retdec-project
	URL ${GHIDRADEC_RETDEC_URL}
	URL_HASH SHA256=${GHIDRADEC_RETDEC_SHA256}
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	# Disable the configure step.
        CONFIGURE_COMMAND ""
	# Disable the update step.
	UPDATE_COMMAND ""
	# Disable the build step.
	BUILD_COMMAND ""
	# Disable the install step.
	INSTALL_COMMAND ""
	LOG_DOWNLOAD ON
	LOG_CONFIGURE ON
	LOG_BUILD ON
)

# Set include directories.
ExternalProject_Get_Property(retdec-project source_dir)
ExternalProject_Get_Property(retdec-project binary_dir)

set(RETDEC_UTILS_SOURCES
	${source_dir}/src/utils/alignment.cpp
	${source_dir}/src/utils/byte_value_storage.cpp
	${source_dir}/src/utils/conversion.cpp
	${source_dir}/src/utils/file_io.cpp
	${source_dir}/src/utils/math.cpp
	${source_dir}/src/utils/string.cpp
	${source_dir}/src/utils/system.cpp
	${source_dir}/src/utils/time.cpp
)
set(RETDEC_COMMON_SOURCES
	${source_dir}/src/common/address.cpp
	${source_dir}/src/common/architecture.cpp
	${source_dir}/src/common/basic_block.cpp
	${source_dir}/src/common/calling_convention.cpp
	${source_dir}/src/common/class.cpp
	${source_dir}/src/common/file_format.cpp
	${source_dir}/src/common/file_type.cpp
	${source_dir}/src/common/function.cpp
	${source_dir}/src/common/language.cpp
	${source_dir}/src/common/object.cpp
	${source_dir}/src/common/pattern.cpp
	${source_dir}/src/common/storage.cpp
	${source_dir}/src/common/tool_info.cpp
	${source_dir}/src/common/type.cpp
	${source_dir}/src/common/vtable.cpp
)
set(RETDEC_SERDES_SOURCES
	${source_dir}/src/serdes/address.cpp
	${source_dir}/src/serdes/architecture.cpp
	${source_dir}/src/serdes/basic_block.cpp
	${source_dir}/src/serdes/calling_convention.cpp
	${source_dir}/src/serdes/class.cpp
	${source_dir}/src/serdes/file_format.cpp
	${source_dir}/src/serdes/file_type.cpp
	${source_dir}/src/serdes/function.cpp
	${source_dir}/src/serdes/language.cpp
	${source_dir}/src/serdes/object.cpp
	${source_dir}/src/serdes/pattern.cpp
	${source_dir}/src/serdes/std.cpp
	${source_dir}/src/serdes/storage.cpp
	${source_dir}/src/serdes/tool_info.cpp
	${source_dir}/src/serdes/type.cpp
	${source_dir}/src/serdes/vtable.cpp
)
set(RETDEC_CONFIG_SOURCES
	${source_dir}/src/config/config.cpp
	${source_dir}/src/config/parameters.cpp
)

add_custom_command(OUTPUT ${RETDEC_UTILS_SOURCES} ${RETDEC_COMMON_SOURCES} ${RETDEC_SERDES_SOURCES} ${RETDEC_CONFIG_SOURCES} DEPENDS retdec-project)

# Add libraries.
add_library(retdec ${RETDEC_UTILS_SOURCES} ${RETDEC_COMMON_SOURCES} ${RETDEC_SERDES_SOURCES} ${RETDEC_CONFIG_SOURCES})
add_dependencies(retdec retdec-project)
target_include_directories(retdec PUBLIC ${source_dir}/include)
target_include_directories(retdec SYSTEM PUBLIC ${source_dir}/deps/rapidjson/include)
target_compile_features(retdec PUBLIC cxx_std_17)
target_compile_options(retdec PRIVATE
	$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/FI${CMAKE_CURRENT_LIST_DIR}/retdec_prefix.h>
	$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:SHELL:-include ${CMAKE_CURRENT_LIST_DIR}/retdec_prefix.h>
)
target_compile_definitions(retdec PUBLIC RAPIDJSON_HAS_STDSTRING=1)
target_compile_definitions(retdec PRIVATE
	RETDEC_BUILD_DATE=""
	RETDEC_GIT_COMMIT_HASH=""
	RETDEC_GIT_VERSION_TAG="v5.0"
)
target_link_libraries(retdec jsoncpp)
