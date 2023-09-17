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

# mount ext2 partition to copy neldr and kernel
echo "Mount ext2 partition"
MNTDIR=$PREFIX/mnt
mkdir -p $MNTDIR
sudo mount -o loop,offset=$offset $DISK $MNTDIR

CopyToExt2() {
    echo "$3"
    stat $1
    sudo cp -p $1 $MNTDIR/$2
}

# neldr: OS boot manager
NELDR=$PREFIX/boot/loader64/neldrbin
CopyToExt2 $NELDR neldr "Copy neldr to neldr2 on ext2 partition"

# kernel
NEOSKNL=$PREFIX/kernel/neosknl
sudo mkdir -p $MNTDIR/neos
CopyToExt2 $NEOSKNL neos/neosknl "Copy kernel to neos/neosknl on ext2 partition"

# hal: hardware abstraction layer
HAL=$PREFIX/hal/x8664/halx8664
CopyToExt2 $HAL neos/hal.sys "Copy x86-64 hal to halx8664 on ext2 partition"

echo "Final Results:"
sudo ls -R $MNTDIR
sudo umount $MNTDIR
sudo rm -rf $MNTDIR
