cmake_minimum_required(VERSION 2.8)

project(ffts C ASM)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# default build type is Debug which means no optimization
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

# common options
option(ENABLE_SSE
  "Enables the use of SSE instructions." ON
)

option(ENABLE_NEON
  "Enables the use of NEON instructions." OFF
)

option(ENABLE_VFP
  "Enables the use of VFP instructions." OFF
)

option(DISABLE_DYNAMIC_CODE
  "Disables the use of dynamic machine code generation." OFF
)

option(ENABLE_SHARED
  "Enable building a shared library." OFF
)

add_definitions(-DFFTS_CMAKE_GENERATED)

if(MSVC)
  # enable all warnings but also disable some..
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /wd4127")

  add_definitions(-D_USE_MATH_DEFINES)
endif(MSVC)

# GCC specific options
if(CMAKE_COMPILER_IS_GNUCC)
  include(CheckIncludeFile)
  include(CheckLibraryExists)
  
  # enable all warnings
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
  
  # some systems need libm for some of the math functions to work
  check_library_exists(m pow "" HAVE_LIBM)
  if(HAVE_LIBM)
    list(APPEND CMAKE_REQUIRED_LIBRARIES m)
    list(APPEND FFTS_EXTRA_LIBRARIES m)
  endif(HAVE_LIBM)
endif(CMAKE_COMPILER_IS_GNUCC)

include_directories(src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(FFTS_SOURCES
  src/ffts_attributes.h
  src/ffts.c
  src/ffts.h
  src/ffts_nd.c
  src/ffts_nd.h
  src/ffts_real.h
  src/ffts_real.c  
  src/ffts_real_nd.c
  src/ffts_real_nd.h
  src/ffts_small.c
  src/macros.h
  src/patterns.c
  src/patterns.h
  src/types.h
)

if(ENABLE_SSE)
  add_definitions(-DHAVE_SSE)
  add_definitions(-D__x86_64__)

  list(APPEND FFTS_SOURCES
    src/macros-sse.h
  )

  if(MSVC)
    set(CMAKE_ASM-ATT_COMPILER yasm)
    enable_language(ASM-ATT)

    add_custom_command(
      OUTPUT sse_win64.obj
      COMMAND ${CMAKE_ASM-ATT_COMPILER} -f win64 -m amd64
      -o ${CMAKE_CURRENT_BINARY_DIR}/sse_win64.obj -p gas
      ${CMAKE_CURRENT_SOURCE_DIR}/src/sse_win64.s
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/sse_win64.s
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Generating sse_win64.obj"
    )

    list(APPEND FFTS_SOURCES
      ${CMAKE_CURRENT_BINARY_DIR}/sse_win64.obj
      src/sse_win64.s
    )
  else()
    list(APPEND FFTS_SOURCES
      src/sse.s
    )
  else()
    add_definitions(-msse2)
  endif(MSVC)
endif()

if(ENABLE_NEON)
  if(DISABLE_DYNAMIC_CODE)
    list(APPEND FFTS_SOURCES
      source/neon_static_f.s
      source/neon_static_i.s
    )
  else()
    list(APPEND FFTS_SOURCES
      source/neon.s
      source/arch/neon.c
    )
  endif()

  add_definitions(-DHAVE_NEON)
endif()

if(ENABLE_VFP)
  list(APPEND FFTS_SOURCES
    source/vfp.s
    source/arch/vfp.c
  )

  add_definitions(-DHAVE_VFP)
endif()

if(ENABLE_SINGLE)
  add_definitions(-DHAVE_SINGLE)
endif()

if(DISABLE_DYNAMIC_CODE)
  list(APPEND FFTS_SOURCES
    src/ffts_static.c
  )

  add_definitions(-DDYNAMIC_DISABLED)
else()
  list(APPEND FFTS_SOURCES
    src/codegen.c
    src/codegen.h
  )
  
  if(ENABLE_SSE)
    list(APPEND FFTS_SOURCES
      src/codegen_sse.h
    )
  endif(ENABLE_SSE)
endif()

add_library(ffts_static
  ${FFTS_SOURCES}
)

add_executable(ffts_test
  tests/test.c
)

target_link_libraries(ffts_test
  ffts_static
  ${FFTS_EXTRA_LIBRARIES}
)