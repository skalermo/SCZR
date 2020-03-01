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

Na koniec należy przerzucić pakiety do maszyny wirtualnej i zainstalować je.

#### Ewentualne błędy kompilacji

Radzę nie dotykać niczego w katalogu cw3_owrt_pkg poza plikami user_tim1.c oraz user_tim2.c.
Także jeśli mimo to występują błędy upewnijcie się, że:
- kod źródłowy (pliki user_tim1.c oraz user_tim2.c) nie zawiera błędów
- poprawnie wpisywaliście polecenia

## Badania wpływu sposobu obsługi oraz obciążenia systemu na opóźnienie obsługi timera

Przy pełnym wykorzystaniu sterownika timer co każdy okres zgłasza przerwanie: "...Odczyt z pliku blokuje proces czytający, dopóki nie upłynie okres timera i nie zostanie zgłoszone przerwanie..."

Natomiast przy minimalnym wsparciu sterownika (realizujemy obsługę ręcznie w programie) wyłączamy przerwania, wykorzystujemy oczekiwanie aktywne.

Jako przykład zbadamy opóźnienia timera na **maszynie jednoprocesorowej**

**Ważne**: przy każdym nowym uruchomieniu maszyny wirtualnej należy należy do jej kernela dodawać moduł naszego sterownika prostym poleceniem:

```console
root@OpenWrt:/# modprobe drv-sysbus-tim1
[   43.520729] Connected IRQ=37
[   43.522871] Connected registers at 900a000
[   43.527942] <1>Successful registration. The major device number is 251.
[   43.530638] Witam serdecznie
```

Robimy to po to by nasze urządzenie `my_tim0` pojawiło w katalogu /dev.

Teraz możemy przystąpić do zadania.

Uruchamiamy programy na maszynie jednoprocesorowej przy zerowym obciążeniu (uruchomionych programów `gen_load1`).

```console
root@OpenWrt:/# user_tim1 100000 1000 > tim1_proc1_load0
root@OpenWrt:/# user_tim2 100000 1000 > tim2_proc1_load0
```

Możemy skorzystać z przygotowanych skryptów:

- znajdziemy średni czas opóźnienia:

```console
$ python calc_avg.py tim1_proc1_load0 tim2_proc1_load0
file: tim1_proc1_load0, lines: 1000, avg: 60573.075
file: tim2_proc1_load0, lines: 1000, avg: 23185.852
```

- oraz zbudujemy wykresy:
```console
$ python hist_nonoverlapping.py tim1_proc1_load0 tim2_proc1_load0
```
![nonoverlapping](/sczr_lab3/img/chart.png)

```console
$ python hist_transparent.py tim1_proc1_load0 tim2_proc1_load0
```
![nonoverlapping](/sczr_lab3/img/chart(1).png)

Wnioski i resztę zadania pozostawiam do samodzielnego wykonania.

Dodam tylko, że przy małym obciążeniu (niż np przy uruchomionym `gen_load1`) procesor przechodzi w tryb oszczędzania energii co skutkuje mniejszą częstotliwością działania, a więc mamy większy czas opóźnienia. Ważne jest uwzględnić to przy napisaniu wniosków.


### Zbadanie wpływu parametrów szeregowania procesów na opóźnienie obsługi timera

Jednym ze sposobów zmiany szeregowania procesów jest zmiana priorytetu procesu. Aby to zrobić możemy skorzystać z polecenia systemowego [`nice`](https://linux.die.net/man/1/nice). Więcej na temat tego jak się ma parametr `nice` do priorytetu procesu możecie przeczytać np [tutaj](https://askubuntu.com/questions/656771/process-niceness-vs-priority).

Przykład:
Uruchomić program user_tim1 przy okresie timera 100,000 nanosekund oraz 200 pętli obiegu ustawiając parametr nice na -20.

```console
$ nice -20 user_tim1 100000 200

```

