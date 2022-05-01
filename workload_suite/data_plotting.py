#!/usr/bin/python3
from os import system, chdir, listdir, path
import itertools
from testing_helpers import *
import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--type", help="Algorithm", default="algorithm")

lru = []
lruwsr = []
fifo = []
cflru = []

def main():
    if sys.argv[2] == "algorithm":
        for filename in listdir(path.abspath("runs")):
            if(discriminate_skew(filename)):
                print(filename)
        for filename in listdir(path.abspath("runs")):
            if(discriminate(filename)):
                if get_params(filename, "a") == "LRU":
                    lru.append(filename)
                if get_params(filename, "a") == "LRUWSR":
                    lruwsr.append(filename)
                if get_params(filename, "a") == "FIFO":
                    fifo.append(filename)
                if get_params(filename, "a") == "CFLRU":
                    cflru.append(filename)


if __name__ == '__main__':
    main()