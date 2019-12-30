qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic -smp $1 \
 -kernel openwrt-18.06.4-armvirt-64-Image -append "root=/dev/vda console=ttyAMA0" \
 -netdev user,id=eth0 -device  virtio-net-device,netdev=eth0 \
 -drive file=openwrt-18.06.4-armvirt-64-root.ext4,if=none,format=raw,id=hd0 -device virtio-blk-device,drive=hd0 \
 -fsdev local,id=h9,path=/tmp/wspolny,security_model=none -device virtio-9p-pci,fsdev=h9,mount_tag=h 

