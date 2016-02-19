#!/bin/sh

TARGETDIR=$1
BUILD=`cat board/knobloch/TXT/BUILD`
BUILDDATE=`date +"%Y-%m-%d %H:%M"`
BUILD=$((BUILD+1))
echo $BUILD > board/knobloch/TXT/BUILD

cp -af board/knobloch/TXT/rootfs/* $TARGETDIR/
mv $TARGETDIR/etc/init.d/S01logging $TARGETDIR/etc/init.d/M01logging||echo "logging alread turned off !"
mv $TARGETDIR/etc/init.d/S50lighttpd $TARGETDIR/etc/init.d/M50lighttpd||echo "lighthttpd alread turned off !"
chmod 0600 $TARGETDIR/etc/issue
echo "ft-TXT Rel 1.0 Build $BUILD ($BUILDDATE)" > $TARGETDIR/etc/BUILD
