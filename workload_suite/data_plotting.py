#!/usr/bin/python3
from os import system, chdir, listdir, path
import itertools
from testing_helpers import *
import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--type", help="Algorithm", default="algorithm")


def main():
    if sys.argv[2] == "algorithm":
        for filename in listdir(path.abspath("runs")):
            if(discriminate_skew(filename)):
                print(filename)


if __name__ == '__main__':
    main()