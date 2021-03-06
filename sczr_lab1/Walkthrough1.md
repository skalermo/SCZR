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

[Tutaj](/sczr_lab1/cw1m.c) można znaleźć moją realizację zadania. 

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
    
    
4. 9P *(nie wymaga rekompilacji)*. Nie udało mi się zmusić to działać, dostawałem błąd
    `mount: mounting h on /mnt failed: No such device`
    
Rekompilację przeprowadzamy poleceniem `make`. W porównaniu do pełnej kompilacji trwa niedługo.

**Uruchomienie programu**

Odpalamy na gospodarzu netcat w trybie nasłuchiwania na porcie 10000. Netcat powinien być wersji OpenBSD (bo z wersją GNU były jakieś kłopoty).

Na maszynie wirtualnej odpalamy nasz program z dwoma parametrami:
- hostname - czyli odbiorca
- timeout - symulowane opóźnienie


- gospodarz: `nc -lv 10000`
- maszyna wirtualna: `./cw1m 10.0.2.2 your_timeout`

## OpenWRT

```console
$ cd OWRT
$ ls
cw1_owrt_pkg  owrt_ext4_9p.sh  owrt_ext4.sh  owrt_irfs_9p.sh  owrt_irfs.sh
$ wget https://downloads.openwrt.org/releases/18.06.4/targets/armvirt/64/openwrt-18.06.4-armvirt-64-Image
$ wget https://downloads.openwrt.org/releases/18.06.4/targets/armvirt/64/openwrt-18.06.4-armvirt-64-root.ext4.gz
$ gzip -d openwrt-18.06.4-armvirt-64-root.ext4.gz
$ ls
cw1_owrt_pkg                          owrt_ext4_9p.sh  owrt_irfs.sh
openwrt-18.06.4-armvirt-64-Image      owrt_ext4.sh
openwrt-18.06.4-armvirt-64-root.ext4  owrt_irfs_9p.sh
```
Pobrany obraz OpenWRT będzie wymagał konfiguracji interfejsu sieciowego (zmienić kilka plików). By nie robić tego za każdym razem po uruchomieniu w przypadku initramfs radziłbym skorzystać z wersji z wirtualnym dyskiem.

Aby odpalić obraz należy skorzystać z udostępnionych skryptów (np `owrt_ext4_9p.sh`)
Wersja ściągniętego obrazu nie zgadza się z wersją wpisaną w skryptach (trzeba zmienić 18.06.2 na 18.06.4)
![OWRT_different_versions](/sczr_lab1/screenshots/owrt_different_versions.png)

Po odpaleniu skryptu system owrt powinien się uruchomić. Jeśli terminal się nie pojawia należy aktywować go wciskając Enter.

Domyślnie interfejs w OpenWRT nie jest skonfigurowany. Można go skonfigurować w taki sposób:
```console
root@OpenWrt:/# ping google.com
ping: bad address 'google.com'
root@OpenWrt:/# cat /etc/config/network 
...
config interface 'lan'
	option type 'bridge'
	option ifname 'eth0'
	option proto 'static'
	option ipaddr '192.168.1.1'
	option netmask '255.255.255.0'
	option ip6assign '60'

root@OpenWrt:/# vim /etc/config/network
# (w tym kroku modyfikujemy plik, mamy dostać to co poniżej)
root@OpenWrt:/# cat /etc/config/network 
...
config interface 'lan'
	option ifname 'eth0'
	option proto 'dhcp'

root@OpenWrt:/# /etc/init.d/network reload
root@OpenWrt:/# rm /etc/rc.d/S19dnsmasq 
root@OpenWrt:/# ping google.com
PING google.com (216.58.215.110): 56 data bytes
64 bytes from 216.58.215.110: seq=0 ttl=255 time=31.702 ms
64 bytes from 216.58.215.110: seq=1 ttl=255 time=40.040 ms
^C
...
```

Jeśli był to obraz systemu z wirtualnym dyskiem wszystkie te zmiany zostaną zapisane czyli przy następnym uruchomieniu powinniśmy mieć już skonfigurowany interfejs. (Ewentualnie trzeba będzie trochę poczekać na usługi systemowe owrt, które nie zdążyły się odpalić)

## Kompilacja pakietu i uruchomienie

Pobieramy i rozpakowujemy środowisko OpenSDK:
```console
$ wget https://downloads.openwrt.org/releases/18.06.4/targets/armvirt/64/openwrt-sdk-18.06.4-armvirt-64_gcc-7.3.0_musl.Linux-x86_64.tar.xz
$ tar -xJf openwrt-sdk-18.06.4-armvirt-64_gcc-7.3.0_musl.Linux-x86_64.tar.xz
```
**Przygotujmy folder z kodem źródłowym do kompilacji:**
```console
$ cd cw1_owrt_pkg
$ ls
cwicz1mak  cwiczenie1
# Folder cwicz1mak możemy usunąć, wystarczy nam tylko jednego
$ rm -r cwicz1mak
$ cd cwiczenie1
$ ls
Makefile  src
# Jeśli chcemy zmienić nazwę pakietu (i tak naprawdę jeszcze dużo czego) to można pogrzebać w Makefile (choć lepiej nie)
$ cd src
$ ls
cw1m.c
# W folderze src leży plik cw1m.c, zamieniamy go naszym kodem
# Skopiujemy ścieżkę do foldery pakietu
$ cd ../..
$ pwd
/home/skalermo/SCZR/sczr_lab1/OWRT/cw1_owrt_pkg <-- skopiować to (w terminalu Ctrl+Shift+C)
```
Wracamy do katalogu SDK.
Na końcu pliku 'feeds.conf.default' dopisujemy linię

`src-link sczr` *`skopiowana_ścieżka`*

W tym samym katalogu wykonujemy polecenia
```console
$ export LANG=C
$ scripts/feeds update sczr
$ scripts/feeds install -p sczr -a
$ make menuconfig
```

Jeśli postępowaliście zgodnie z tym jak powyżej, to po uruchomieniu `make menuconfig` powinniście zobaczyć coś takiego:
![OWRT_menuconfig_packages](/sczr_lab1/screenshots/owrt_menuconfig_packages.png)

Klikamy 'Exit' potwierdzając zachowanie zmian (choć tak naprawdę tylko sprawdziliśmy czy jest nasz pakiet).

Jeśli nie ma opcji 'Examples' albo pakietu 'cwiczenie1' oznacza to że gdzieś po drodze coś źle zrobiliście. W takim przypadku sugerowałbym sprawdzić poprawność wklejonej wcześniej ścieżki i poprawność wpisywanych komend.

Następnym krokiem jest skompilowanie naszego pakietu:
`$ make package/cwiczenie1/compile`
Chcemy zobaczyć te 4 pomarańczowe linii, oznacza to, że kompilacja zakończyła się powodzeniem.
![Success_compilation](/sczr_lab1/screenshots/successful_compilation.png)

W przypadku takim:
![Failed_compilation](/sczr_lab1/screenshots/failed_compilation.png)

sugerowałbym najpierw odpalić komendę `make package/cwiczenie1/clean`, po czym szukać przyczynę błędu (tutaj doprowadziłem do błędu zmieniając nazwę folderu `/cw1_owrt_pkg/cwiczenie1`, mi się wydaje że **nazwa folderu powinna być taka sama co nazwa pakietu**, co w sumie jest logiczne). 

Po naprawieniu błedów ponownie kompilujemy pakiet: `make package/cwiczenie1/compile`

Teraz mając skompilowany pakiet chcemy go przerzucić do naszej maszyny wirtualnej.
Opiszę sposób z 9p.

Pakiet skompilowany leży w `.../openwrt-sdk-18.06.4-armvirt-64_gcc-7.3.0_musl.Linux-x86_64/bin/packages/aarch64_generic/sczr`.
W skrypcie owrt_ext4_9p.sh zmieniamy zmienną *`path`* odpowiednio do lokalizacji naszego pakietu. 

Po tej operacji odpalamy skrypt.
Znajdujemy nasz pakiet i instalujemy go.

```console
root@OpenWrt:~# mount -t 9p h /mnt
root@OpenWrt:~# cd /mnt
root@OpenWrt:/mnt# ls
cwiczenie1_1.0-1_aarch64_generic.ipk
root@OpenWrt:/mnt# opkg install cwiczenie1_1.0-1_aarch64_generic.ipk 
Installing cwiczenie1 (1.0-1) to root...
Configuring cwiczenie1.
root@OpenWrt:/mnt# cw1m 10.0.2.2 1
...
```
Jeśli wcześniej ten pakiet już był zainstalowany używamy opcji `--force-reinstall` :

`opkg install --force-reinstall cwiczenie1_1.0-1_aarch64_generic.ipk`

Dalej postępujemy analogicznie jak w przypadku Buildroot'a.
