# Laboratorium 2 - Walkthrough

Pobieramy archiwum z kodem źródłowym oraz SDK OpenWRT. Przyda nam się również skrypt ![owrt_ext4_9p.sh](/sczr_lab2/owrt_ext4_9p.sh) z poprzedniego ćwiczenia. 

Pominę pierwsze dwa zadania, ponieważ jest to dobrze opisane dla pierwszego ćwiczenia.

Wyjaśnię tylko dodawanie katalogu klienta cw2b do ścieżki PATH:

```console
$ ls
cw2a.c  cw2a.h  cw2b.c  Makefile
$ make
cc -c  cw2a.c -o cw2a.o
cc -o cw2a  cw2a.o  -pthread -lrt 
cc -c  cw2b.c -o cw2b.o
cc -o cw2b  cw2b.o -pthread -lrt
$ ./cw2a 3 200 10000 1000000
I couldn't start cw2b client!
I couldn't start cw2b client!
I couldn't start cw2b client!
$ pwd
.../SCZR_lab_cw2/cw2_owrt_pkg/cwicz2mak/src (pełna ścieżka do folderu w którym znajduje się program cw2b)
$ export PATH=".../SCZR_lab_cw2/cw2_owrt_pkg/cwicz2mak/src:$PATH"
$ ./cw2a 3 200 10000 1000000
Client: 0, nsmp=200, del=1000000
Client: 1, nsmp=200, del=1000000
Client: 2, nsmp=200, del=1000000
Sample 0, client 0, delivery time: 56
Sample 0, client 1, delivery time: 66
Sample 0, client 2, delivery time: 73
Sample 1, client 1, delivery time: 54
...
```

