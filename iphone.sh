#/bin/sh

export SDKVER="5.1"
export DEVROOT="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer"
export SDKROOT="$DEVROOT/SDKs/iPhoneOS$SDKVER.sdk"
export CFLAGS="-O3 -Wreturn-type -Wparentheses -Wswitch -Wno-unused-parameter -Wno-unused-variable -Wunused-value -Wno-shorten-64-to-32 -Wno-trigraphs -fpascal-strings -miphoneos-version-min=5.0 -mcpu=cortex-a9 -arch armv7 -mfpu=neon -pipe -isysroot $SDKROOT -isystem $SDKROOT/usr/include -isystem $DEVROOT/usr/include -std=c11 -mno-thumb"
export AR="$DEVROOT/usr/bin/ar"
#export CC="$DEVROOT/usr/bin/clang"
