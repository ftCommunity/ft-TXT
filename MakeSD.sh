DRIVE=/dev/sdd

umount ${DRIVE}1
umount ${DRIVE}2
umount ${DRIVE}3

cat << EOM
################################################################################

                Now making 3 partitions

################################################################################
EOM

dd if=/dev/zero of=$DRIVE bs=1024 count=1024

SIZE=`fdisk -l $DRIVE | grep -E "Disk|Platte" | awk '{print $5}'`
echo DISK SIZE - $SIZE bytes
CYLINDERS=`echo $SIZE/255/63/512 | bc`
echo CYLINDERS $CYLINDERS

sfdisk -D -H 255 -S 63 -C $CYLINDERS $DRIVE << EOF
,150,0x0C,*
,200,,-
,,,-
EOF

sync
cat << EOM
################################################################################

                Partitioning Boot

################################################################################
EOM
	dd if=/dev/zero of=${DRIVE}1 bs=1024 count=1024
        mkfs.vfat -F 32 -n "boot" ${DRIVE}1

cat << EOM
################################################################################

                Partitioning Rootfs

################################################################################
EOM
	dd if=/dev/zero of=${DRIVE}2 bs=1024 count=1024
        mkfs.ext3 -L "rootfs" ${DRIVE}2

cat << EOM
################################################################################

                Partitioning START_HERE

################################################################################
EOM

	dd if=/dev/zero of=${DRIVE}3 bs=1024 count=1024
        mkfs.ext3 -L "START_HERE" ${DRIVE}3
        sync
        sync
