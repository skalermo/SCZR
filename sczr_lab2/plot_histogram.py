#!/bin/python

import sys
import matplotlib.pyplot as plt
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="plot_histogram",
        description="SCZR histogram",
    )

    # parser.add_argument('plik', help="Plik server.txt lub cli_*.txt")
    
    # This line allows to take multiple arguments
    parser.add_argument('pliki', nargs='+', help="Pliki server.txt lub cli_*.txt")
    
    parser.add_argument('-s', action='store_true', help="Zapisz histogram do pliku .png")
    args = parser.parse_args()

    for plik in args.pliki:

        data = []

        with open(plik, 'r') as aux:
            for l in aux:
                l = l.replace(',', ' ')
                data.append(int(l.split()[-1]))
            file_name = plik.split('/')[-1]
            
        if file_name == 'server.txt':
            tytul = 'server'
        else:
            client_number = file_name.replace('.','_').split('_')[1]
            delay = file_name.replace('.','_').split('_')[2]
            num_cpus = file_name.replace('.','_').split('_')[3]
            # tytul = f"CPUs = {num_cpus}, client = {client_number}, N = {str(len(data))}, delay = {delay}"
            
            # python2/3 compatible
            tytul = "CPUs = %s, client = %s, N = %s, delay = %s" % (num_cpus, client_number, len(data), delay)

        plt.figure()
        plt.hist(data, 50)

        plt.grid(axis='y', alpha=0.5)
        plt.grid(axis='x', alpha=0.5)
        plt.xlabel('Czas dostarczenia danych do klienta [us]')
        plt.ylabel('Liczba')
        plt.title(tytul)

        if args.s:
            plt.savefig(tytul.replace(' ', '_') + '.png')

        plt.show()
