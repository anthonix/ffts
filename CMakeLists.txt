cmake_minimum_required(VERSION 2.8)

project(ffts C ASM)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# default build type is Debug which means no optimization
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

# common options
option(ENABLE_NEON
  "Enables the use of NEON instructions." OFF
)

option(ENABLE_VFP
  "Enables the use of VFP instructions." OFF
)

option(DISABLE_DYNAMIC_CODE
  "Disables the use of dynamic machine code generation." OFF
)

option(ENABLE_RUNTIME_DYNAMIC_CODE
  "Enables the runtime generation of dynamic machine code." ON
)

option(ENABLE_SHARED
  "Enable building a shared library." OFF
)

include(CheckIncludeFile)

add_definitions(-DFFTS_CMAKE_GENERATED)

# check if the platform has support for SSE SIMD extension
check_include_file(xmmintrin.h HAVE_XMMINTRIN_H)
if(HAVE_XMMINTRIN_H)
  add_definitions(-DHAVE_SSE)
else()
  # check if the platform has support NEON SIMD extension
  check_include_file(arm_neon.h HAVE_ARM_NEON_H)
  if(HAVE_ARM_NEON_H)
  endif(HAVE_ARM_NEON_H)
endif(HAVE_XMMINTRIN_H)

# compiler settings
if(MSVC)
  # enable all warnings but also disable some..
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /wd4127")

  add_definitions(-D_USE_MATH_DEFINES)
elseif(CMAKE_COMPILER_IS_GNUCC)
  include(CheckLibraryExists)

  # enable all warnings
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

  # some systems need libm for some of the math functions to work
  check_library_exists(m pow "" HAVE_LIBM)
  if(HAVE_LIBM)
    list(APPEND CMAKE_REQUIRED_LIBRARIES m)
    list(APPEND FFTS_EXTRA_LIBRARIES m)
  endif(HAVE_LIBM)

  if(HAVE_XMMINTRIN_H)
    add_definitions(-msse)
  endif(HAVE_XMMINTRIN_H)
endif(MSVC)

include_directories(include)
include_directories(src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(FFTS_HEADERS
  include/ffts.h
)

set(FFTS_SOURCES
  src/ffts_attributes.h
  src/ffts.c
  src/ffts_internal.h
  src/ffts_nd.c
  src/ffts_nd.h
  src/ffts_real.h
  src/ffts_real.c
  src/ffts_real_nd.c
  src/ffts_real_nd.h
  src/ffts_small.c
  src/ffts_small.h
  src/macros.h
  src/patterns.c
  src/patterns.h
  src/types.h
)

if(ENABLE_NEON)
  if(DISABLE_DYNAMIC_CODE)
    list(APPEND FFTS_SOURCES
      src/neon_static_f.s
      src/neon_static_i.s
    )
  else()
    list(APPEND FFTS_SOURCES
      src/neon.s
      src/arch/neon.c
    )
  endif(DISABLE_DYNAMIC_CODE)

  add_definitions(-DHAVE_NEON)
elseif(ENABLE_VFP)
  list(APPEND FFTS_SOURCES
    src/vfp.s
    src/arch/vfp.c
  )

  add_definitions(-DHAVE_VFP)
elseif(HAVE_XMMINTRIN_H)
  add_definitions(-DHAVE_SSE)

  list(APPEND FFTS_SOURCES
    src/macros-sse.h
  )

  if(NOT DISABLE_DYNAMIC_CODE)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      list(APPEND FFTS_SOURCES
        src/codegen_sse.h
      )

      if(MSVC)
        if(NOT ENABLE_RUNTIME_DYNAMIC_CODE)
          # YASM supports x86 GAS syntax
          set(CMAKE_ASM-ATT_COMPILER yasm)
          enable_language(ASM-ATT)

          if(CMAKE_ASM-ATT_COMPILER_WORKS)
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
            message(WARNING "YASM is required, enabling runtime dynamic code.")
            set(ENABLE_RUNTIME_DYNAMIC_CODE ON)
          endif(CMAKE_ASM-ATT_COMPILER_WORKS)
        endif(NOT ENABLE_RUNTIME_DYNAMIC_CODE)
      
        if(ENABLE_RUNTIME_DYNAMIC_CODE)
          add_definitions(-DSSE_DEFINE_CONSTANTS)
        endif(ENABLE_RUNTIME_DYNAMIC_CODE)
      else()
        list(APPEND FFTS_SOURCES
          src/sse.s
        )
      endif(MSVC)
    else()
      message(WARNING "Dynamic code is only supported with x64, disabling dynamic code.")
      set(DISABLE_DYNAMIC_CODE ON)
    endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  endif(NOT DISABLE_DYNAMIC_CODE)
endif(ENABLE_NEON)

if(DISABLE_DYNAMIC_CODE)
  list(APPEND FFTS_SOURCES
    src/ffts_static.c
    src/ffts_static.h
  )

  add_definitions(-DDYNAMIC_DISABLED)
else()
  list(APPEND FFTS_SOURCES
    src/codegen.c
    src/codegen.h
  )
endif(DISABLE_DYNAMIC_CODE)

add_library(ffts_static
  ${FFTS_HEADERS}
  ${FFTS_SOURCES}
)

add_executable(ffts_test
  tests/test.c
)

target_link_libraries(ffts_test
  ffts_static
  ${FFTS_EXTRA_LIBRARIES}
)