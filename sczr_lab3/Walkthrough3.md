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


## Stworzenie i uruchomienie programów współpracujących z timerem

Tak jak w poprzednich ćwiczeniach pobieramy:
- SDK dla OpenWRT
- Obraz systemu
- Wirtualny dysk systemu
- Skrypt dla uruchamiania qemu
oraz
- archiwum SCZR_lab_cw3.tar.bz2

W skrypcie należy podać poprawną ścieżkę do pliku wykonywalnego qemu, który wcześniej skompilowaliśmy:

sklonowane repo qemu/aarch64-softmmu/qemu-system-aarch64

Także w razie potrzeby należy poprawić ścieżki do obrazu systemu, wirtualnego dysku, oraz współdzielonego katalogu.


Po rozpakowaniu arhiwum w folderze cw3_owrt_pkg znajdziemy źródła pakietów OpenWRT. Pakiet drv-sysbus-tim1 zawiera sterownik badanego urządzenia (timera). Pakiet user-sysbus-tim1 zawiera programy gen_load1 oraz user_tim1 i user_tim2. W plikach [user_tim1.c](/sczr_lab3/user_tim1.c) i [user_tim2.c](/sczr_lab3/user_tim2.c) znajdziecie kod źródłowy wystarczający do realizacji ćwiczenia.

### Kompilacja pakietów

Dużo osób miało problemy w tym miejscu więc postaram się opisać ścieżkę postępowania:

1. W katalogu SDK dla OpenWRT na koniec pliku feeds.conf.default dopisujemy jak w poprzednich ćwiczeniach ścieżkę do katalogu cw3_owrt_pkg. 

2. Znajdując się w katalogu SDK odpalamy następujące polecenia:
```console
$ export LANG=C
$ scripts/feeds update sczr
$ scripts/feeds install -p sczr -a
```

3.0 (Opcjonalne) Możemy upewnić się czy nasze pakiety są widzialne przez SDK:
```console
$ make menuconfig
```
Po kliknięciu na Examples powinniśmy zobaczyć nasz pakiet user-sysbus-tim1. 

3.1 Kompilujemy pakiety:
```console
$ make package/drv-sysbus-tim1/compile
$ make package/user-sysbus-tim1/compile
```
Gdzie znaleźć skompilowane pakiety:

**drv-sysbus-tim1**:
.../bin/targets/armvirt/64/packages/kmod-drv-sysbus-tim1_4.14.131-1_aarch64_generic.ipk

**user-sysbus-tim1**:
.../bin/packages/aarch64_generic/sczr/user-sysbus-tim1_1_aarch64_generic.ipk

#### Ewentualne błędy kompilacji

Radzę nie dotykać niczego w katalogu cw3_owrt_pkg poza plikami user_tim1.c oraz user_tim2.c.
Także jeśli mimo to występują błędy upewnijcie się, że:
- kod źródłowy (pliki user_tim1.c oraz user_tim2.c) nie zawiera błędów składniowych
- poprawnie wpisywaliście polecenia


