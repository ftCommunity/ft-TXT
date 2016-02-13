export DRIVE=/dev/sdd

umount ${DRIVE}1

rm -rf /tmp/boot

mkdir /tmp/boot

mount ${DRIVE}1 /tmp/boot
cp ./output/images/am335x-kno_txt.dtb /tmp/boot
cp ./output/images/uImage /tmp/boot
cp ./output/images/rootfs.ubi /tmp/boot
cp ./board/knobloch/TXT/rootfs/etc/ft-logo.bmp /tmp/boot/bootlogo.bmp
cp ./board/knobloch/TXT/board-support/u-boot-2013.10-ti2013.12.01/MLO /tmp/boot
cp ./board/knobloch/TXT/board-support/u-boot-2013.10-ti2013.12.01/u-boot.img /tmp/boot
 
umount ${DRIVE}1 
 
