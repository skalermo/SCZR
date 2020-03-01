import os
from argparse import ArgumentParser
import matplotlib.pyplot as plt

plt.style.use('seaborn-deep')


def parseArgs():
    parser = ArgumentParser(prog='hist_nonoverlapping.py', description='Plot comparison histogram')
    parser.add_argument('files', nargs='+', help='Files with data to plot')
    return vars(parser.parse_args())


def genUniqueFilename():
    if os.path.exists('chart.png'):
        i = 1
        while os.path.exists('chart({}).png'.format(i)):
            i += 1
        return 'chart({}).png'.format(i)
    return 'chart.png'


if __name__ == '__main__':
    args = parseArgs()

    fig, ax = plt.subplots()

    # You can name chart labels
    ax.set_xlabel('')
    ax.set_ylabel('')

    data = []
    labels = []

    for filename in args['files']:
        data.append([])
        labels.append(filename)

        with open(filename, 'r') as f:
            lines = f.readlines()

        for line in lines:
            val = line.split()[-1]
            data[-1].append(int(val))

    ax.hist(data, label=labels)
    ax.legend()

    # Output image parameters
    dpi = 72
    size = (640, 480)
    
    size = [x/dpi for x in size]
    fig.set_size_inches(size, forward=True)

    # Taking care not to overwrite existing chart
    fig_name = genUniqueFilename()

    fig.savefig(fig_name, dpi=dpi)
    plt.show()