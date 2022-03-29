#!/usr/bin/python3
from os import system
import itertools
from testing_helpers import *
import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument("-a", "--algo", help="Algorithm", default="all")


def main():

    buffer_size = [150]  # b
    disk_size = [1500]  # n
    num_operations = [7500]  # x
    perct_reads_writes = [60]  # e
    skewed_perct = [90]  # s
    skewed_data_perct = [10]  # d
    algorithm = [1, 2, 3, 4, 5]  # a

    parameters = [buffer_size, disk_size, num_operations,
                  perct_reads_writes, skewed_perct, skewed_data_perct]
    if sys.argv[0] == "all":
        parameters.append(algorithm)
        for combination in itertools.product(*parameters):
            system(build_command(combination[0], combination[1], combination[2],
                                 combination[3], combination[4], combination[5], combination[6]))
    else:
        if sys.argv[0] == "lru":
            for combination in itertools.product(*parameters):
                system(build_command(combination[0], combination[1], combination[2],
                                     combination[3], combination[4], combination[5], 0))
        elif sys.argv[0] == "lru-wsr":
            for combination in itertools.product(*parameters):
                system(build_command(combination[0], combination[1], combination[2],
                                     combination[3], combination[4], combination[5], 1))
        elif sys.argv[0] == "lru-cf":
            for combination in itertools.product(*parameters):
                system(build_command(combination[0], combination[1], combination[2],
                                     combination[3], combination[4], combination[5], 2))
        elif sys.argv[0] == "fifo":
            for combination in itertools.product(*parameters):
                system(build_command(combination[0], combination[1], combination[2],
                                     combination[3], combination[4], combination[5], 3))



if __name__ == '__main__':
    main()