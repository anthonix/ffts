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

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckIncludeFile)

add_definitions(-DFFTS_CMAKE_GENERATED)

# check existence of various headers
check_include_file(malloc.h   HAVE_MALLOC_H)
check_include_file(stdint.h   HAVE_STDINT_H)
check_include_file(stdlib.h   HAVE_STDLIB_H)
check_include_file(string.h   HAVE_STRING_H)
check_include_file(sys/mman.h HAVE_SYS_MMAN_H)
check_include_file(unistd.h   HAVE_UNISTD_H)

if(HAVE_MALLOC_H)
  add_definitions(-DHAVE_MALLOC_H)
endif(HAVE_MALLOC_H)

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
    # Determinate ARM architecture
    set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})

    # Try to execute quietly without messages
    set(CMAKE_REQUIRED_QUIET 1)

    # The test for ARM architecture
    set(TEST_SOURCE_CODE "int main() { return 0; }")

    # GCC documentation says "native" is only supported on Linux, but let's try
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -march=native")
    check_c_source_runs("${TEST_SOURCE_CODE}" GCC_MARCH_NATIVE_FLAG_SUPPORTED)

    if(NOT GCC_MARCH_NATIVE_FLAG_SUPPORTED)
      # Fallback trying generic ARMv7
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -march=armv7-a")
      check_c_source_runs("${TEST_SOURCE_CODE}" GCC_MARCH_ARMV7A_FLAG_SUPPORTED)

      if(NOT GCC_MARCH_ARMV7A_FLAG_SUPPORTED)
        # Fallback trying generic ARMv6
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -march=armv6")
        check_c_source_runs("${TEST_SOURCE_CODE}" GCC_MARCH_ARMV6_FLAG_SUPPORTED)

        if(NOT GCC_MARCH_ARMV6_FLAG_SUPPORTED)
          message(WARNING "FFTS failed to determinate ARM architecture")
          set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})
        else()
          message("FFTS is build using 'march=armv6'")
          set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=armv6")
          set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv6")
        endif(NOT GCC_MARCH_ARMV6_FLAG_SUPPORTED)
      else()
        message("FFTS is build using 'march=armv7-a'")
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=armv7-a")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a")
      endif(NOT GCC_MARCH_ARMV7A_FLAG_SUPPORTED)
    else()
       message("FFTS is build using 'march=native'")
       set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=native")
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    endif(NOT GCC_MARCH_NATIVE_FLAG_SUPPORTED)

    # Determinate what floating-point hardware (or hardware emulation) is available
    set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})

    # The test for ARM NEON support
    set(TEST_SOURCE_CODE "
      #include <arm_neon.h>
      int main()
      {
       float32x4_t v;
       float zeros[4] = {0.0f, 0.0f, 0.0f, 0.0f};
       v = vld1q_f32(zeros);
       return 0;
      }"
    )

    # Test running with -mfpu=neon
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -mfpu=neon -mfloat-abi=softfp")
    check_c_source_runs("${TEST_SOURCE_CODE}" NEON_SUPPORTED)

    if(NOT NEON_SUPPORTED)
      # Fallback using VFP if NEON is not supported
      if(ENABLE_NEON)
        message(FATAL_ERROR "FFTS cannot enable NEON on this platform")
      endif(ENABLE_NEON)

      # Test for ARM VFP support
      set(TEST_SOURCE_CODE "
        double sum(double a, double b)
        {
         return a + b;
        }
        int main()
        {
         double s1, s2, v1 = 1.0, v2 = 2.0, v3 = 1.0e-322;
         s1 = sum(v1, v2);
         s2 = sum(v3, v3);
         return 0;
        }"
      )

      # Test running with -mfpu=vfp
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -mfpu=vfp")
      check_c_source_runs("${TEST_SOURCE_CODE}" VFP_SUPPORTED)

      if(NOT VFP_SUPPORTED)
        # Fallback using emulation if VFP is not supported
        if(ENABLE_VFP)
          message(FATAL_ERROR "FFTS cannot enable VFP on this platform")
        endif(ENABLE_VFP)

        message(WARNING "FFTS is using 'soft' FPU")
      else()
        message("FFTS is using 'vfp' FPU")
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfpu=vfp")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=vfp")
        set(ENABLE_VFP ON)
      endif(NOT VFP_SUPPORTED)
    else()
      message("FFTS is using 'neon' FPU")
      set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfpu=neon")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -mfpu=neon")
      set(ENABLE_NEON ON)
    endif(NOT NEON_SUPPORTED)

    # Determinate float ABI if NEON or VFP is used
    if(NEON_SUPPORTED OR VFP_SUPPORTED)
      set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})

      # Test running with -mfloat-abi=hard
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -mfloat-abi=hard")

      # Use the same test as before
      check_c_source_runs("${TEST_SOURCE_CODE}" HARDFP_SUPPORTED)

      if(NOT HARDFP_SUPPORTED)
        # Test running with -mfloat-abi=softfp
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -mfloat-abi=softfp")
        check_c_source_runs("${TEST_SOURCE_CODE}" SOFTFP_SUPPORTED)
  
        if(NOT SOFTFP_SUPPORTED)
          # Most likely development libraries are missing
          message(WARNING "FFTS is using 'soft' float ABI")
        else()
          message("FFTS is using 'softfp' float ABI")
          set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfloat-abi=softfp")
          set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp")
        endif(NOT SOFTFP_SUPPORTED)
      else()
        message("FFTS is using 'hard' float ABI")
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfloat-abi=hard")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard")
      endif(NOT HARDFP_SUPPORTED)
    endif(NEON_SUPPORTED OR VFP_SUPPORTED)
  else()
    # check if the platform has support for SSE intrinsics
    check_include_file(xmmintrin.h HAVE_XMMINTRIN_H)
    if(HAVE_XMMINTRIN_H)
      add_definitions(-DHAVE_SSE)
    endif(HAVE_XMMINTRIN_H)

    # check if the platform has support for SSE2 intrinsics
    check_include_file(emmintrin.h HAVE_EMMINTRIN_H)
    if(HAVE_EMMINTRIN_H)
      add_definitions(-DHAVE_SSE2)
    endif(HAVE_EMMINTRIN_H)

    # check if the platform has support for SSE3 intrinsics
    check_include_file(pmmintrin.h HAVE_PMMINTRIN_H)
    if(HAVE_PMMINTRIN_H)
      add_definitions(-DHAVE_PMMINTRIN_H)
      add_definitions(-DHAVE_SSE3)
    else()
      # check if the platform has specific intrinsics
      check_include_file(intrin.h HAVE_INTRIN_H)
      if(HAVE_INTRIN_H)
        check_c_source_compiles("
          #include<intrin.h>
          int main(int argc, char** argv)
          {
           (void) argv;
           (void) argc;
           return _mm_movemask_ps(_mm_moveldup_ps(_mm_set_ss(1.0f)));
          }" HAVE__MM_MOVELDUP_PS
        )
        if(HAVE__MM_MOVELDUP_PS)
          # assume that we have all SSE3 intrinsics
          add_definitions(-DHAVE_INTRIN_H)
          add_definitions(-DHAVE_SSE3)
        endif(HAVE__MM_MOVELDUP_PS)
      endif(HAVE_INTRIN_H)
    endif(HAVE_PMMINTRIN_H)
  endif(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
else()
  # TODO: Add detections for compiler support and headers
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

  if(HAVE_EMMINTRIN_H)
    add_definitions(-msse2)
  elseif(HAVE_XMMINTRIN_H)
    add_definitions(-msse)
  endif(HAVE_EMMINTRIN_H)
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
  src/ffts_trig.c
  src/ffts_trig.h
  src/ffts_static.c
  src/ffts_static.h
  src/macros.h
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