#!/bin/sh
BUILD=`cat board/knobloch/TXT/BUILD`
IMAGEFILE=../ft-TXT_Build_$BUILD.img

echo "Image: $IMAGEFILE"

sudo dd if=$IMAGEFILE of=/dev/sdd


