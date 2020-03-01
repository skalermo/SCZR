Plik ![SCZR_LabV2_Cw3.pdf](/sczr_lab3/SCZR_LabV2_Cw3.pdf) zawiera informacje wstępne i zadania do laboratorium 3.

Rozwiązanie zadań znajduje się w pliku ![Walkthrough3.md](/sczr_lab3/Walkthrough3.md)

Przygotowałem kilka skryptów w python'ie, które powinne ułatwić wykonanie zadania 2.3:

- [calc_avg.py](/sczr_lab3/calc_avg.py)
  - wylicza średni czas opóźnienia na podstawie wejściowego pliku tekstowego
  - może przyjmować kilka plików na raz

- [hist_nonoverlapping.py](/sczr_lab3/hist_nonoverlapping.py)
  - skrypt do sporządzania wykresów 
  - przyjmuje na wejściu pliki tekstowe z danymi do porównania
  - nazwy wejściowych plików są wykorzystywane przy tworzeniu legendy do wykresu

- [hist_transparent.py](/sczr_lab3/hist_transparent.py)
  - prawie to samo co wyżej; zastanawiam się czy nie złączyć je w jeden skrypt.

**user_tim1** oraz **user_tim2** sposób użycia:

`user_tim1` *okres_timera*(w nanosekundach) *liczba_przebiegów_pętli*

**Sugerowane wartości**
- okres_timera - **od 50,000 do 200,000**, dla wartości poniżej 30,000 zawiesza się.
- liczba_przebiegów_pętli - **od 100 do 1000** (generalnie dowolna)
