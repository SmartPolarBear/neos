qemu-system-x86_64 \
    -vga std \
    -d int,cpu_reset \
    -no-shutdown \
    -no-reboot \
    -m 8G \
    -smp 4 \
    -machine pc \
    -drive file=scripts/disk.img,if=ide \


# qemu-system-x86_64 \
#     -vga std \
#     -d int,cpu_reset \
#     -no-shutdown \
#     -no-reboot \
#     -m 8G \
#     -smp 4 \
#     -machine pc \
#     -drive file=scripts/disk.img,index=0,media=disk,format=raw,id=disk0,if=none \
#                         -device ahci,id=ahci \
#                         -device ide-hd,drive=disk0,bus=ahci.0 \