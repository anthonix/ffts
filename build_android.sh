#!/bin/sh
# Compiles ffts for Android
# Make sure you have NDK_ROOT defined in .bashrc or .bash_profile
# Modify INSTALL_DIR to suit your situation

INSTALL_DIR="`pwd`/jni/ffts"

export PATH="$NDK_ROOT/toolchains/arm-linux-androideabi-4.6/prebuilt/darwin-x86/bin/:$PATH"
export SYS_ROOT="$NDK_ROOT/platforms/android-8/arch-arm/"
export CC="arm-linux-androideabi-gcc --sysroot=$SYS_ROOT"
export LD="arm-linux-androideabi-ld"
export AR="arm-linux-androideabi-ar"
export RANLIB="arm-linux-androideabi-ranlib"
export STRIP="arm-linux-androideabi-strip"
export CFLAGS="-O3"

mkdir -p $INSTALL_DIR
./configure --enable-neon --build=i386-apple-darwin10.8.0 --host=arm-eabi --prefix=$INSTALL_DIR LIBS="-lc -lgcc"

make clean
make
make install

exit 0
