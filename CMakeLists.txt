cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(ffts C ASM)

# TODO: to support AutoConfigure building, this should came from "template" file
set(FFTS_MAJOR 0)
set(FFTS_MINOR 9)
set(FFTS_MICRO 0)

set(FFTS_VERSION "ffts-${FFTS_MAJOR}.${FFTS_MINOR}.${FFTS_MICRO}")

include(GNUInstallDirs)
include(FindPkgConfig)
include(CheckLibraryExists)
include(CheckIncludeFile)
include(CMakeDetermineCCompiler)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# default build type is Debug which means no optimization
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

# installation parameters
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# options
option(GENERATE_POSITION_INDEPENDENT_CODE "Generate PIC/PIE code." ON)
option(ENABLE_ARM_NEON_ASSEMBLER_CODE "Enables the use of ARM NEON assembler code." OFF)
option(ENABLE_ARM_VFP_ASSEMBLER_CODE "Enables the use of ARM VFP assembler code." OFF)
option(ENABLE_SSE_MACROS "Enable the use of SSE macros." ON)
option(ENABLE_DYNAMIC_CODE "Enables the use of dynamic machine code generation." ON)
option(ENABLE_SHARED "Enable building a shared library." ON)
option(ENABLE_STATIC "Enable building a static library." ON)
option(ENABLE_TESTS "Enable building tests." ON)

# headers
set(FFTS_HEADERS
  include/ffts.h
)

# sources
set(FFTS_SOURCES
  src/ffts_attributes.h
  src/ffts.c
  src/ffts_chirp_z.c
  src/ffts_chirp_z.h
  src/ffts_internal.h
  src/ffts_nd.c
  src/ffts_nd.h
  src/ffts_real.h
  src/ffts_real.c
  src/ffts_real_nd.c
  src/ffts_real_nd.h
  src/ffts_transpose.c
  src/ffts_transpose.h
  src/ffts_trig.c
  src/ffts_trig.h
  src/ffts_static.c
  src/ffts_static.h
  src/macros.h
  src/patterns.h
  src/types.h
)

# special instruction sources
if(ENABLE_ARM_NEON_ASSEMBLER_CODE)
	list(APPEND FFTS_SOURCES src/neon.s src/neon_static.s)
endif()

if(ENABLE_ARM_VFP_ASSEMBLER_CODE)
	list(APPEND FFTS_SOURCES src/vfp.s)
endif()

if(ENABLE_SSE_MACROS)
	list(APPEND FFTS_SOURCES src/macros-sse.h)
endif()

if(ENABLE_DYNAMIC_CODE)
	list(APPEND FFTS_SOURCES src/codegen_sse.h src/codegen.c src/codegen.h)
endif()

if(GENERATE_POSITION_INDEPENDENT_CODE)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif(GENERATE_POSITION_INDEPENDENT_CODE)

# find math lib
check_library_exists(m sincosl "" Math_FOUND REQUIRED)
if(Math_FOUND)
list(APPEND CMAKE_C_STANDARD_LIBRARIES -lm)
endif()

# add shared library
if(ENABLE_SHARED)
add_library(${PROJECT_NAME}_shared SHARED ${FFTS_HEADERS} ${FFTS_SOURCES})
set_target_properties(${PROJECT_NAME}_shared PROPERTIES
	DEFINE_SYMBOL FFTS_SHARED
	OUTPUT_NAME ${PROJECT_NAME}
	VERSION ${FFTS_MAJOR}.${FFTS_MINOR}.${FFTS_MICRO}
)
install(TARGETS ${PROJECT_NAME}_shared DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()

# add static library
if(ENABLE_STATIC)
add_library(${PROJECT_NAME}_static STATIC ${FFTS_HEADERS} ${FFTS_SOURCES})
set_target_properties(${PROJECT_NAME}_static PROPERTIES
	DEFINE_SYMBOL FFTS_STATIC
	OUTPUT_NAME ${PROJECT_NAME}
	VERSION ${FFTS_MAJOR}.${FFTS_MINOR}.${FFTS_MICRO}
)
install(TARGETS ${PROJECT_NAME}_static DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()

# install headers
install(FILES ${FFTS_HEADERS} DESTINATION
	"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")

# add tests
if(ENABLE_TESTS)
add_executable(ffts_test tests/test.c)
target_link_libraries(ffts_test ffts)
endif()

# cmake config
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FFTSConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/FFTSConfig.cmake
                              INSTALL_DESTINATION "${CMAKE_INSTALL_CMAKEDIR}")

		      write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/cmake/FFTSConfigVersion.cmake
                                 VERSION ${FFTS_VERSION}
                                 COMPATIBILITY AnyNewerVersion)

			 install(FILES
				 "${CMAKE_CURRENT_BINARY_DIR}/cmake/FFTSConfig.cmake"
				 "${CMAKE_CURRENT_BINARY_DIR}/cmake/FFTSConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_CMAKEDIR}")

# pkg-config
include(FindPkgConfig QUIET)
if(PKG_CONFIG_FOUND)
	# convert lists of link libraries into -lstdc++ -lm etc..
	foreach(LIB ${CMAKE_C_IMPLICIT_LINK_LIBRARIES} ${PLATFORM_LIBS})
		set(PRIVATE_LIBS "${PRIVATE_LIBS} -l${LIB}")
	endforeach()
	# Produce a pkg-config file for linking against the shared lib
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}.pc" @ONLY)
	install(FILES       "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}.pc"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()
