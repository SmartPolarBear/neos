qemu-system-x86_64 -drive file=scripts/disk.img,if=ide \
    -vga std \
    -d int,cpu_reset \
    -no-shutdown \
    -no-reboot \
    -m 8G \
