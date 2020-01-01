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

## Zadania 3-4

**Jak wyznaczyć kiedy system przestaje nadążać przetwarzać dane w czasie rzeczywistym?**

Zadając odpowiedni argument czasu przetwarzania (czwarty argument) możemy wymusić ciągły wzrost czasu opóźnienia. Postarałem się zilustrować to w poniższej tabeli:

|| Nadąża | Nie nadąża |
| --- | --- | --- |
| Polecenie | `./cw2a 1 200 10000` **`50000`** | `./cw2a 1 200 10000` **`1500000`** |
| Dane wyjściowe | ![keepingup](/sczr_lab2/screenshots/keepingup.png) | ![notkeepingup](/sczr_lab2/screenshots/notkeepingup.png) |
| Wykres | ![keepingup_plot](/sczr_lab2/screenshots/keepingup_plot.png) | ![notkeepingup_plot](/sczr_lab2/screenshots/notkeepingup_plot.png) |

Zadanie polega na tym żeby znaleść czas przetwarzania jak najmniejszy przy którym system przestaje nadążać.

W zadaniu czwartym czeka na was co najmniej 6 wykresów (kombinacji dla 1 i 3 klientów oraz dla 1, 2 i 4 rdzeni cpu). Przygotowane skrypty ![owrt_ext4_9p.sh](/sczr_lab2/owrt_ext4_9p.sh) i ![plot_histogram.py](/sczr_lab2/plot_histogram.py) powinne trochę uprościć sprawę.

Reszta zadania jest raczej trywialna.

## Zadanie 5

Oczekiwanie aktywne możemy osiągnąć używając `pthread_mutex_trylock()` zamiast `pthread_mutex_lock` i usuwając `pthread_cond_wait()`. W przypadku oczekiwania aktywnego na jednym kliencie wystarczy dodać warunek na sprawdzenie numeru klienta (interesuje nas zerowy).

| Oczekiwanie w uśpieniu | 1 aktywny | wszystkie aktywne |
| --- | --- | --- |
| ![beforechange](/sczr_lab2/screenshots/client_beforechange.png) | ![change1](/sczr_lab2/screenshots/client_change1.png) | ![changeall](/sczr_lab2/screenshots/client_changeall.png) |

## Zadanie 6

![badserver](/sczr_lab2/screenshots/badserver.png)

Ten rysunek pokazuje stan serwera po ostatniej zmianie w zadaniu piątym (klienci oczekują aktywnie). Aby wygenerować go, należy usunąć pierwszą linijkę z pliku `server.txt`. 

Jak widać nie zachowano początkowej wartości okresu próbkowania równej 10000. 

Związano to z tym, że w kodzie programu serwera użyto funkcji `sleep`, która nie jest dobrym rozwiązaniem w przypadku problemów czasu rzeczywistego
