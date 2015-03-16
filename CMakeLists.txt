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
include(CheckSymbolExists)
include(CMakePushCheckState)

add_definitions(-DFFTS_CMAKE_GENERATED)

# check existence of various headers
check_include_file(stdint.h   HAVE_STDINT_H)
check_include_file(stdlib.h   HAVE_STDLIB_H)
check_include_file(string.h   HAVE_STRING_H)
check_include_file(sys/mman.h HAVE_SYS_MMAN_H)
check_include_file(unistd.h   HAVE_UNISTD_H)

if(HAVE_STDINT_H)
  add_definitions(-DHAVE_STDINT_H)
endif(HAVE_STDINT_H)

if(HAVE_STDLIB_H)
  add_definitions(-DHAVE_STDLIB_H)
endif(HAVE_STDLIB_H)

if(HAVE_STRING_H)
  add_definitions(-DHAVE_STRING_H)
endif(HAVE_STRING_H)

if(HAVE_SYS_MMAN_H)
  add_definitions(-DHAVE_SYS_MMAN_H)
endif(HAVE_SYS_MMAN_H)

if(HAVE_UNISTD_H)
  add_definitions(-DHAVE_UNISTD_H)
endif(HAVE_UNISTD_H)

# Determinate if we are cross-compiling
if(NOT CMAKE_CROSSCOMPILING)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    # Determinate what floating-point hardware
    # (or hardware emulation) is available
    #
    cmake_push_check_state()

    # Try to execute quietly without messages
    set(CMAKE_REQUIRED_QUIET 1)

    # Test compilation with -mfpu=neon
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mfpu=neon")
    check_symbol_exists(exit stdlib.h NEON_AVAILABLE)
    if(NOT NEON_AVAILABLE)
      cmake_reset_check_state()

      # Test compilation with -mfpu=vfp
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mfpu=vfp")
      check_symbol_exists(exit stdlib.h VFP_AVAILABLE)
      if(NOT VFP_AVAILABLE)
        message(WARNING "FFTS is using 'soft' FPU")
      else()
        message("FFTS is using 'vfp' FPU")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=vfp")
        
        set(ENABLE_NEON 0)
        set(ENABLE_VFP 1)
      endif(NOT SOFTFP_AVAILABLE)
    else()
      message("FFTS is using 'neon' FPU")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
      
      set(ENABLE_NEON 1)
      set(ENABLE_VFP 0)
    endif(NOT NEON_AVAILABLE)

    # Determinate float ABI if NEON or VFP is used
    if(NEON_AVAILABLE OR VFP_AVAILABLE)
      cmake_push_check_state()

      # Test compilation with -mfloat-abi=hard
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mfloat-abi=hardfp")
      check_symbol_exists(exit stdlib.h HARDFP_AVAILABLE)
      if(NOT HARDFP_AVAILABLE)
        cmake_reset_check_state()

        # Test compilation with -mfloat-abi=hard
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mfloat-abi=softfp")
        check_symbol_exists(exit stdlib.h SOFTFP_AVAILABLE)
        if(NOT SOFTFP_AVAILABLE)
          # Most likely development libraries are missing
          message(WARNING "FFTS is using 'soft' float ABI")
        else()
          message("FFTS is using 'softfp' float ABI")
          set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp")
        endif(NOT SOFTFP_AVAILABLE)
      else()
        message(WARNING "FFTS is using 'hard' float ABI")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard")
      endif(NOT HARDFP_AVAILABLE)

      cmake_pop_check_state()
    endif(NEON_AVAILABLE OR VFP_AVAILABLE)

    cmake_pop_check_state()
  else()
    # check if the platform has support for SSE SIMD extension
    check_include_file(xmmintrin.h HAVE_XMMINTRIN_H)
    if(HAVE_XMMINTRIN_H)
      add_definitions(-DHAVE_SSE)
    endif(HAVE_XMMINTRIN_H)
  endif(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
else()
  # Check if we can always use detection code above?
endif(NOT CMAKE_CROSSCOMPILING)

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
  src/ffts_static.c
  src/ffts_static.h
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
    )
  endif(DISABLE_DYNAMIC_CODE)

  add_definitions(-DHAVE_NEON)
elseif(ENABLE_VFP)
  if(NOT DISABLE_DYNAMIC_CODE)
    list(APPEND FFTS_SOURCES
      src/vfp.s
    )
  endif(NOT DISABLE_DYNAMIC_CODE)

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