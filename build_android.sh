#!/bin/sh
# Compiles ffts for Android
# Make sure you have NDK_ROOT defined in .bashrc or .bash_profile
# Modify INSTALL_DIR to suit your situation

INSTALL_DIR="`pwd`/jni/ffts"

case $(uname -s) in
  Darwin)
    CONFBUILD=i386-apple-darwin`uname -r`
    HOSTPLAT=darwin-x86
  ;;
  Linux)
    CONFBUILD=i386-unknown-linux
    HOSTPLAT=linux-x86
  ;;
  *) echo $0: Unknown platform; exit
esac

case arm in
  arm)
    TARGPLAT=arm-linux-androideabi
    ARCH=arm
    CONFTARG=arm-eabi
  ;;
  x86)
    TARGPLAT=x86
    ARCH=x86
    CONFTARG=x86
  ;;
  mips)
  ## probably wrong
    TARGPLAT=mipsel-linux-android
    ARCH=mips
    CONFTARG=mips
  ;;
  *) echo $0: Unknown target; exit
esac

: ${NDK_ROOT:?}

export PATH="$NDK_ROOT/toolchains/${TARGPLAT}-4.6/prebuilt/${HOSTPLAT}/bin/:$PATH"
export SYS_ROOT="$NDK_ROOT/platforms/android-8/arch-${ARCH}/"
export CC="${TARGPLAT}-gcc --sysroot=$SYS_ROOT"
export LD="${TARGPLAT}-ld"
export AR="${TARGPLAT}-ar"
export RANLIB="${TARGPLAT}-ranlib"
export STRIP="${TARGPLAT}-strip"
export CFLAGS="-Os"

mkdir -p $INSTALL_DIR
./configure --enable-neon --build=${CONFBUILD} --host=${CONFTARG} --prefix=$INSTALL_DIR LIBS="-lc -lgcc"

make clean
make
make install

exit 0
