# Laboratorium 1 - Walkthrough

## Buildroot
**Ściągamy Buildroot'a i rozpakowujemy go:**

```console
wget https://buildroot.org/downloads/buildroot-2019.08.1.tar.bz2
tar -xjf buildroot-2019.08.1.tar.bz2

$ ls
buildroot-2019.08.1  buildroot-2019.08.1.tar.bz2
$ cd buildroot-2019.08.1/
/home/skalermo/SCZR/sczr_lab1/buildroot-2019.08.1
```

**Wybieramy poniższa konfigurację:**

(inne nie rozpatrywane na laboratoriach)

```console
make qemu_aarch64_virt_defconfig
```
**Pakiety które chcemy zainstalować oraz interesujące nas opcje wybieramy w pojawiającym się menu:**
```console
make menuconfig
```
**Upewnijcie się, że okienko terminalu jest wystarczająco duże:**
![Small_display](/sczr_lab1/screenshots/small_display.png)

**Musimy odnaleźć i zaznaczyć następujące opcję:**
- Toolchain -> Toolchain type -> External toolchain
- Target packages -> Show packages that are also provided by busybox
- System configuration -> System hostname *(nazwa systemu)*
- Target packages -> Networking applications -> ntp
- Target packages -> Networking applications -> ntpd
- Target packages -> Networking applications -> ntpdate *(na wszelki wypadek)*
- Target packages -> Networking applications -> netcat-openbsd *(nie jest potrzebne jeśli używac sokietów do komunikacji wątków)*
- Filesystem images -> cpio the root filesystem
- Filesystem images -> ext2/3/4 root filesystem *(odznaczyć)*
- Filesystem images -> initial RAM filesystem linked into linux kernel

Na koniec klikamy 'Exit' na dole, zaznaczamy 'Yes' by zachować nową konfigurację.

**System gotowy do kompilacji:**

kompilacja potrwa długo: zajęło mi to około godziny na moim laptopie.

```console
$ make
/bin/make -j1 O=/home/skalermo/SCZR/sczr_lab1/buildroot-2019.08.1/output HOSTCC="/bin/gcc" HOSTCXX="/bin/g++" syncconfig
make[1]: Entering directory '/home/skalermo/SCZR/sczr_lab1/buildroot-2019.08.1'
make[1]: Leaving directory '/home/skalermo/SCZR/sczr_lab1/buildroot-2019.08.1'
which: no patch in (/bin:/usr/bin:/usr/local/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/usr/lib/jvm/default/bin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
You must install 'patch' on your build machine
which: no cpio in (/bin:/usr/bin:/usr/local/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/usr/lib/jvm/default/bin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
You must install 'cpio' on your build machine
which: no rsync in (/bin:/usr/bin:/usr/local/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/usr/lib/jvm/default/bin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
You must install 'rsync' on your build machine
which: no bc in (/bin:/usr/bin:/usr/local/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/usr/lib/jvm/default/bin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl)
You must install 'bc' on your build machine
make: *** [support/dependencies/dependencies.mk:27: dependencies] Error 1
```
Jak widać brakuje pakietów:
- patch
- cpio
- rsync
- bc

Po zainstalowaniu tych pakietów kompilacja powinna przejść bez błędów. 

**Ściągamy archiwum ze strony przedmiotu:**

https://studia3.elka.pw.edu.pl/f-pl/19Z/103D-INSID-ISP-SCZR/priv/cw1%5Fprzyklady.tar.gz

```console
$ tar -xzf cw1_przyklady.tar.gz
$ ls
BR  buildroot-2019.08.1  cw1_przyklady.tar.gz  OWRT
```

Następnie przenosimy skrypty do uruchamiania systemu do folderu buildroot aby ścieżka w nich się zgadzała (albo zmieniamy ścieżkę wewnątrz skryptu)

**Potrzebujemy zainstalować *qemu* i wszystkie niezbędne pakiety dla pracy *qemu* z *aarch64***

Dla archlinuxa to są:
- qemu
- qemu-arch-extra

Po uruchomieniu skryptu (np ./rv_irfs) system powinien dać się uruchomić.
![Configured_buildroot](/sczr_lab1/screenshots/configured_buildroot.png)

Polecenia `date` i `ping` potwierdzą że pierwsze zadanie jest wykonane (zdarza się, że usługi systemowe odpowiadające za czas systemowy oraz za połączenie z siecią nie zdążają się włączyć, w tym przypadku należy trochę poczekać).

## Program

[Tutaj](sczr_lab1/cw1m.c) można znaleźć moją realizację zadania. 

Komunikaty przechowywane w liście dwukierunkowej, komunikacja odbywa się za pomocą sokietów.
Synchronizacja jest *poprawna* (sprawdzający nie znalazł do czego się przyczepić).

Nie jest to jedyne możliwe rozwiązanie, koledzy robili na różne sposoby (nie polecam kolejek komunikatów, OpenWRT ich nie wspiera).

## Kompilacja programu i uruchomienie w Buildroot'cie

Skorzystamy ze skryptu *buduj*, wymaga to odpowiedniego dostosowania zmiennej `BRPATH`. Najwygodniej jest odpalić komendę `pwd` w folderze buildroot'a, po czym wynik podstawić do zmiennej `BRPATH`.

Skrypt należy odpalać po kompilacji buildroot'a.

**Przeniesienie pliku do maszyny emulowanej**

Istnieje kilka sposobów:
1. Przenieść plik do buildroot/output/target *(wymaga rekompilacji systemu)*
2. Skorzystać z nakładek na system plików *(wymaga rekompilacji systemu)*
    - make menuconfig -> System configuration -> Root filesystem overlay directories -> *Pełna ścieżka do folderu który chcemy udostępnić*
    
    
3. Uruchomić na "gospodarzu" serwer http, ściągnąć z systemu buildroot plik poleceniem wget *(nie wymaga rekompilacji)*:
    - gospodarz: `python3 -m http.server 1234` (albo `python2 -m SimpleHTTPServer 1234`)
    - maszyna wirtualna: `wget 10.0.2.2:1234/*jakiś_plik*`
    
    
4. 9P (nie wymaga rekompilacji). Nie udało mi się zmusić to działać, dostawałem błąd
    `mount: mounting h on /mnt failed: No such device`
    
Rekompilację przeprowadzamy poleceniem `make`. W porównaniu do pełnej kompilacji trwa niedługo.

**Uruchomienie programu**

Odpalamy na gospodarzu netcat w trybie nasłuchiwania na porcie 10000. Netcat powinien być wersji OpenBSD (bo z wersją GNU były jakieś kłopoty).

Na maszynie wirtualnej odpalamy nasz program z dwoma parametrami:
- hostname - czyli odbiorca
- timeout - symulowane opóźnienie


- gosodarz: `nc -lv 10000`
- maszyna wirtualna: `./cw1m 10.0.2.2 your_timeout`

jeśli z jakiegoś powodu nie uda się połączyć z 10.0.2.2 należy spróbować 10.0.2.3.
