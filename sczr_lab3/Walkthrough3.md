# Laboratorium 3 - Walkthrough

## Kompilacja emulatora QEMU z modelem timera

Oczywiście nie trzeba mieć zainstalowanego qemu na komputerze.

Odpalamy polecenia tak jak opisano w skrypcie.
Klonowanie zajmuje trochę czasu, do ściągnięcia ~254 MiB.

```console
$ git clone https://github.com/wzab/qemu
$ cd qemu
$ git checkout sczr1
$ pwd
/home/skalermo/Downloads/qemu
$ ./configure --target-list=arm-softmmu,aarch64-softmmu --enable-virtfs --prefix=/home/skalermo/Downloads/qemu

# Jeśli odpalicie to na swoim komputerze może pojawić się ten błąd:
ERROR: configure test passed without -Werror but failed with -Werror.
       This is probably a bug in the configure script. The failing command
       will be at the bottom of config.log.
       You can run configure with --disable-werror to bypass this check.
       
# Dodajemy do polecenia `--disable-werror` i uruchamiamy ponownie
$ ./configure --target-list=arm-softmmu,aarch64-softmmu --enable-virtfs --prefix=/home/skalermo/Downloads/qemu --disable-werror
Install prefix    /home/skalermo/Downloads/qemu
BIOS directory    /home/skalermo/Downloads/qemu/share/qemu
firmware path     /home/skalermo/Downloads/qemu/share/qemu-firmware
binary directory  /home/skalermo/Downloads/qemu/bin
...
...
avx2 optimization yes
replication support yes
VxHS block device no
capstone          git

$ scripts/hxtool -h < hmp-commands-info.hx > hmp-commands-info.h
$ scripts/hxtool -h < hmp-commands.hx > hmp-commands.h
$ make -j 8 install 
```

Na moim laptopie kompilacja trwała około 5 minut.

