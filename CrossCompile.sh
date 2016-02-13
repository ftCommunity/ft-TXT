#! /bin/sh
#
PWD=`pwd`
export SYSROOT=`pwd`/output/host/usr/bin
export SYSROOTARM=`pwd`/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot
export TARGET_ROOTFS=`pwd`/board/knobloch/TXT/rootfs
export SDL_CFLAGS="`$PWD/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot/usr/bin/sdl-config --cflags`"
export SDL_LIBS="`$PWD/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot/usr/bin/sdl-config --libs`"
export CROSS_COMPILE=arm-linux-gnueabihf-
##export SDL_CFLAGS="`$PWD/output/host/usr/arm-buildroot-linux-gnueabi/sysroot/usr/bin/sdl-config --cflags`"
##export SDL_LIBS="`$PWD/output/host/usr/arm-buildroot-linux-gnueabi/sysroot/usr/bin/sdl-config --libs`"
##export CROSS_COMPILE=arm-none-linux-gnueabi-
export LPATH=`pwd`/output/host/usr/lib
export PATH=$SYSROOT:$PATH
if [ $# -ge 1 ]
	then
	cd $1
fi	
PROMPT_COMMAND='PS1="\[\033[0;31m\]CROSS CC:\[\033[0;30m\]$PS1";unset PROMPT_COMMAND' bash