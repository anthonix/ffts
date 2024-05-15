# Please see more recently updated forks:

https://github.com/linkotec/ffts

https://github.com/ValveSoftware/ffts 

Sorry, but I haven't been able to touch this for many years!

# FFTS -- The Fastest Fourier Transform in the South

[![Build Status](https://travis-ci.org/linkotec/ffts.svg?branch=master)](https://travis-ci.org/linkotec/ffts)

To build for Android, edit and run build_android.sh

To build for iOS, edit and run build_iphone.sh 

To build for Linux or OS X on x86, run 
  ./configure --enable-sse --enable-single --prefix=/usr/local
  make
  make install

Optionally build for Windows and Linux with CMake, run
  mkdir build
  cd build
  cmake ..
  
FFTS dynamically generates code at runtime. This can be disabled with 
--disable-dynamic-code

Note that 32 bit x86 dynamic machine code generation is not supported at the moment.

For JNI targets: --enable-jni will build the jni stuff automatically for
the host target, and --enable-shared must also be added manually for it to
work.

