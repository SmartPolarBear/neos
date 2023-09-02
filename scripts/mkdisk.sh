DISK=disk.img
#PREFIX=../cmake-build-debug-wsl-clang
PREFIX=../cmake-build-debug-wsl

# create disk
dd if=/dev/zero of=$DISK bs=1M count=128

# create partitions
parted $DISK mklabel msdos
parted $DISK mkpart primary ext2 1MiB 100%
parted $DISK set 1 boot on
offset=$((1 * 1024 * 1024))
mkfs.ext2 -F -L MYEXT2 -O ^has_journal -b 1024 -I 128 -E offset=$offset $DISK

# stage 1 bootloader
echo "Copy stage 1 bootloader"
STAGE1=$PREFIX/boot/boot/bootsector
stat $STAGE1
dd if=$STAGE1 of=$DISK conv=notrunc bs=446 count=1
dd if=$STAGE1 of=$DISK conv=notrunc bs=1 count=2 skip=510 seek=510 # 55 AA mark

# stage 2 bootloader
echo "Copy stage 2 bootloader"
STAGE2=$PREFIX/boot/loader/osldrbin
stat $STAGE2
dd if=$STAGE2 of=$DISK conv=notrunc bs=512 count=16 skip=0 seek=1