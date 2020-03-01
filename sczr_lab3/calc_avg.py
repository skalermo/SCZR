from argparse import ArgumentParser


def parseArgs():
    parser = ArgumentParser(prog='calc_avg.py', description='Calculate average value for each provided file')
    parser.add_argument('files', nargs='+', help='Files to process')
    return vars(parser.parse_args())


if __name__ == '__main__':
    args = parseArgs()

    lines = []

    for filename in args['files']:
        with open(filename, 'r') as f:
            lines = f.readlines()

        sum = 0
        count = 0

        for line in lines:
            val = line.split()[-1]
            sum += int(val)
            count += 1

        print('file: {}, lines: {}, avg: {}'.format(filename, count, sum/count))