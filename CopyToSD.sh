export DRIVE=/dev/sdb

umount ${DRIVE}1
umount ${DRIVE}2
umount ${DRIVE}3

rm -rf /tmp/boot
rm -rf /tmp/rootfs

mkdir /tmp/boot
mkdir /tmp/rootfs

cat << EOM
################################################################################

                Now Copying Data

################################################################################

EOM

cat << EOM

################################################################################

                Copying Boot

################################################################################
EOM
	dd if=/dev/zero of=${DRIVE}1 bs=1024 count=1024
        mkfs.vfat -F 32 -n "boot" ${DRIVE}${P}1
	mount ${DRIVE}1 /tmp/boot
	cp ./output/images/am335x-kno_txt.dtb /tmp/boot
	cp ./output/images/uImage /tmp/boot
	cp ./output/images/rootfs.ubi /tmp/boot
	cp ./board/knobloch/TXT/rootfs/etc/ft-logo.bmp /tmp/boot/bootlogo.bmp
	cp ./board/knobloch/TXT/board-support/u-boot-2013.10-ti2013.12.01/MLO /tmp/boot
	cp ./board/knobloch/TXT/board-support/u-boot-2013.10-ti2013.12.01/u-boot.img /tmp/boot
cat << EOM

################################################################################

                Copying Rootfs

################################################################################
EOM
	dd if=/dev/zero of=${DRIVE}2 bs=1024 count=1024
        mkfs.ext3 -L "rootfs" ${DRIVE}${P}2
	mount ${DRIVE}2 /tmp/rootfs
	tar -C /tmp/rootfs -xzvf ./output/images/rootfs.tar.gz

        sync
        sync
	umount ${DRIVE}1 
	umount ${DRIVE}2 
