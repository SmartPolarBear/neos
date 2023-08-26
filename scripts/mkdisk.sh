DISK=disk.img

# create disk
dd if=/dev/zero of=$DISK bs=1M count=128

# create partitions
parted $DISK mklabel msdos
parted $DISK mkpart primary ext2 1MiB 100%
offset=$((1 * 1024 * 1024))
mkfs.ext2 -F -L MYEXT2 -O ^has_journal -b 1024 -I 128 -E offset=$offset $DISK

# stage 1 bootloader
echo "Copy stage 1 bootloader"
STAGE1=../cmake-build-debug-wsl-clang/boot/boot/bootsector
dd if=$STAGE1 of=$DISK conv=notrunc bs=446 count=1
dd if=$STAGE1 of=$DISK conv=notrunc bs=1 count=2 skip=510 seek=510 # 55 AA mark

# stage 2 bootloader
echo "Copy stage 2 bootloader"
STAGE2=../cmake-build-debug-wsl-clang/boot/loader/osldrbin
dd if=$STAGE2 of=$DISK conv=notrunc bs=512 count=5 skip=0 seek=1