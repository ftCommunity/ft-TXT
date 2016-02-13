#!/bin/bash

BUILD=`cat board/knobloch/TXT/BUILD`
UPDATE_BINARY="./output/images/rootfs.tar.gz"
UPDATE_FILENAME="../UpdateRootfs_Build_$BUILD.sh"
UPDATE_IN_SCRIPT="UpdateRootfs.sh.in"

# Check for payload format option (default is binary).
binary=1
uuencode=0

if [[ "$1" == '--binary' ]]; then
	binary=1
	uuencode=0
	shift
fi
if [[ "$1" == '--uuencode' ]]; then
	binary=0
	uuencode=1
	shift
fi

if [[ ! -f $UPDATE_BINARY ]]; then
	echo "       UPDATE_BINARY $UPDATE_BINARY doesn't exist!"
	echo "Usage: $0 [--binary | --uuencode]"
	exit 1
fi


if [[ $binary -ne 0 ]]; then
	# Append binary data.
	sed \
		-e 's/uuencode=./uuencode=0/' \
		-e 's/binary=./binary=1/' \
			 $UPDATE_IN_SCRIPT > $UPDATE_FILENAME
	echo "PAYLOAD:" >> $UPDATE_FILENAME

	cat $UPDATE_BINARY >> $UPDATE_FILENAME
fi
if [[ $uuencode -ne 0 ]]; then
	# Append uuencoded data.
	sed \
		-e 's/uuencode=./uuencode=1/' \
		-e 's/binary=./binary=0/' \
			 $UPDATE_IN_SCRIPT > $UPDATE_FILENAME
	echo "PAYLOAD:" >> $UPDATE_FILENAME

	cat $UPDATE_BINARY | uuencode - >> $UPDATE_FILENAME
fi
