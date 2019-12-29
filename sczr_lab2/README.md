Plik ![SCZR_LabV2_Cw2.pdf](/sczr_lab2/SCZR_LabV2_Cw2.pdf) zawiera informacje wstępne i zadania do laboratorium 2.

Rozwiązanie zadań znajduje się w pliku ![Walkthrough2.md](/sczr_lab2/Walkthrough2.md)

Zmodyfikowałem dwa pliki:
- ![owrt_ext4_9p.sh](/sczr_lab2/owrt_ext4_9p.sh) 
    - teraz przyjmuje jeden parametr - liczbę procesorów (nie trzeba zmieniać za każdym razem w skrypcie)
- ![plot_histogram.py](/sczr_lab2/plot_histogram.py) 
    - skrypt działa zarówno w `python2` jak i `python3`
    - teraz może przyjmować dowolną liczbę argumentów (a nie jeden)

**O co w ogóle chodzi w zadaniu**

Program **cw2a** (serwer) przekazuje dane do przetwarzania programom **cw2b** (klienci). Mamy przeanalizować zależność czasu (jaki upływa pomiędzy pozyskaniem danych przez serwer, a rozpoczęciem ich przetwarzania przez klientów) od liczby rdzeni cpu, liczby programów-klientów, obciążenia systemu, sposobu oczekiwania klienta na dane.

**cw2a** sposób użycia:
`./cw2a` *`liczba_klientów liczba_próbek okres_próbkowania czas_przetwarzania`*

**Sugerowane wartości:**
- liczba_klientów - **1** lub **3**
- liczba_próbek - w zakresie **od 100 do 500**
- okres_próbkowania - **10000**
- czas_przetwarzania (delay) - w zakresie od 100,000 do 3,000,000 - jest dobierany ręcznie

