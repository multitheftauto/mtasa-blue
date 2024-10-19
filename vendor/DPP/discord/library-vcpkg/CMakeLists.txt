file(GLOB THE_SOURCES "${DPP_ROOT_PATH}/src/dpp/events/*.cpp" "${DPP_ROOT_PATH}/src/dpp/cluster/*.cpp" "${DPP_ROOT_PATH}/src/dpp/*.cpp" "${DPP_ROOT_PATH}/src/dpp/*.rc")

set(LIB_NAME "${PROJECT_NAME}")

if(WIN32)
	add_library("${LIB_NAME}" SHARED "${THE_SOURCES}")
else()
	add_library("${LIB_NAME}" STATIC "${THE_SOURCES}")
	find_package(Threads REQUIRED)
endif()

add_library("${PROJECT_NAME}::${LIB_NAME}" ALIAS "${LIB_NAME}")

if(${AVX_TYPE} STREQUAL "OFF")
	include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/DetectArchitecture.cmake")
else()
	message("-- AVX type overridden by configuration: ${AVX_TYPE}")
endif()
STRING(REPLACE "AVX" "" AVX_TYPE ${AVX_TYPE})
add_compile_definitions(AVX_TYPE=${AVX_TYPE})
add_compile_options(${AVX_FLAG})

target_compile_definitions(
	"${LIB_NAME}" PUBLIC
	"DPP_BUILD"
)

if(WIN32)
	add_compile_definitions(OPENSSL_SYS_WIN32)
	add_compile_definitions(_WINSOCK_DEPRECATED_NO_WARNINGS)
	add_compile_definitions(WIN32_LEAN_AND_MEAN)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	add_compile_definitions(_CRT_NONSTDC_NO_DEPRECATE)
endif()

target_compile_options(
	"${LIB_NAME}"
	PUBLIC
		"$<$<PLATFORM_ID:Windows>:/bigobj;/Zc:preprocessor>"
	PRIVATE
		"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:/sdl;/Od;/DEBUG;/MP;/DFD_SETSIZE=1024>>"
		"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release>:/O2;/Oi;/Oy;/GL;/Gy;/sdl;/MP;/DFD_SETSIZE=1024>>"
		"$<$<PLATFORM_ID:Linux>:$<$<CONFIG:Debug>:-Wall;-Wempty-body;-Wno-psabi;-Wunknown-pragmas;-Wignored-qualifiers;-Wimplicit-fallthrough;-Wmissing-field-initializers;-Wsign-compare;-Wtype-limits;-Wuninitialized;-Wshift-negative-value;-pthread;-g;-Og;-fPIC>>"
		"$<$<PLATFORM_ID:Linux>:$<$<CONFIG:Release>:-Wall;-Wempty-body;-Wno-psabi;-Wunknown-pragmas;-Wignored-qualifiers;-Wimplicit-fallthrough;-Wmissing-field-initializers;-Wsign-compare;-Wtype-limits;-Wuninitialized;-Wshift-negative-value;-pthread;-O3;-fPIC>>"
		"${AVX_FLAG}"
)

target_compile_features(
	"${LIB_NAME}" PUBLIC
	"cxx_std_17" "cxx_constexpr" "cxx_auto_type"
	"cxx_defaulted_functions" "cxx_deleted_functions"
	"cxx_final" "cxx_lambdas" "cxx_override" "cxx_thread_local"
	"cxx_variadic_templates" "cxx_attribute_deprecated"
	"cxx_enum_forward_declarations"
)

target_include_directories(
	"${LIB_NAME}" PUBLIC
	"$<BUILD_INTERFACE:${DPP_ROOT_PATH}/include>"
	"$<INSTALL_INTERFACE:include>"
)

set_target_properties(
	"${LIB_NAME}" PROPERTIES
	OUTPUT_NAME "dpp"
	CXX_STANDARD_REQUIRED ON
)

target_link_options(
	"${LIB_NAME}" PUBLIC
	"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:/DEBUG>>"
)

add_compile_definitions(HAVE_VOICE)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Opus CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(
	"${LIB_NAME}" PUBLIC
	$<$<TARGET_EXISTS:nlohmann_json::nlohmann_json>:nlohmann_json::nlohmann_json>
	$<$<TARGET_EXISTS:OpenSSL::SSL>:OpenSSL::SSL>
	$<$<TARGET_EXISTS:OpenSSL::Crypto>:OpenSSL::Crypto>
	$<$<TARGET_EXISTS:Opus::opus>:Opus::opus>
	$<$<TARGET_EXISTS:unofficial-sodium::sodium>:unofficial-sodium::sodium>
	$<$<TARGET_EXISTS:ZLIB::ZLIB>:ZLIB::ZLIB>
	$<$<TARGET_EXISTS:Threads::Threads>:Threads::Threads>
)

set(CONFIG_FILE_NAME "${PROJECT_NAME}Config.cmake")
set(EXPORTED_TARGETS_NAME "${PROJECT_NAME}Targets")
set(EXPORTED_TARGETS_FILE_NAME "${EXPORTED_TARGETS_NAME}.cmake")
set(EXPORTED_TARGETS_FILE_PATH "share/dpp/${EXPORTED_TARGETS_FILE_NAME}")
set(RELEASE_PDB_FILE_PATH "bin/dpp.pdb")
set(DEBUG_PDB_FILE_PATH "debug/bin/dpp.pdb")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${DPP_ROOT_PATH}/cmake/${CONFIG_FILE_NAME}.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
	INSTALL_DESTINATION "share/dpp"
	PATH_VARS
	EXPORTED_TARGETS_FILE_PATH
	RELEASE_PDB_FILE_PATH
	DEBUG_PDB_FILE_PATH
)

set(VERSION_FILE_NAME "${PROJECT_NAME}ConfigVersion.cmake")

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
	VERSION "${PRODUCT_VERSION}"
	COMPATIBILITY AnyNewerVersion
)

install(
	DIRECTORY "${DPP_ROOT_PATH}/include/"
	DESTINATION "include"
)

install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
	"${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
	DESTINATION "share/dpp"
)

install(
	TARGETS "${LIB_NAME}"
	EXPORT "${EXPORTED_TARGETS_NAME}"
	RUNTIME DESTINATION "$<$<NOT:$<PLATFORM_ID:Linux>>:$<IF:$<CONFIG:Debug>,${DEBUG_PREFIX}bin,bin>>"
	ARCHIVE DESTINATION "$<IF:$<CONFIG:Debug>,${DEBUG_PREFIX}lib,lib>"
)

install(
	EXPORT "${EXPORTED_TARGETS_NAME}"
	FILE "${EXPORTED_TARGETS_FILE_NAME}"
	NAMESPACE "${PROJECT_NAME}::"
	DESTINATION "share/dpp"
)

if (WIN32)
	install(
		FILES
		"$<TARGET_PDB_FILE:${LIB_NAME}>"
		DESTINATION "$<IF:$<CONFIG:Debug>,${DEBUG_PREFIX}bin,bin>"
		OPTIONAL
	)
endif()
