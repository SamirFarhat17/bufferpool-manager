#!/usr/bin/python3
from os import system, chdir
import itertools
from testing_helpers import *
import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument("-a", "--algo", help="Algorithm", default="all")


def main():
    chdir("..")
    count = 0
    buffer_size = [10, 100, 250, 500]  # b
    disk_size = [1000, 1500, 3000, 10000]  # n
    num_operations = [1000, 5000, 10000]  # x
    perct_reads_writes = [10, 50, 100]  # e
    skewed_perct = [90]  # s
    skewed_data_perct = [10]  # d
    algorithm = [1, 2, 3, 4]  # a
    page_size = [128, 512, 1024, 4096, 8192]
    print(sys.argv[2])
    parameters = [buffer_size, disk_size, num_operations, perct_reads_writes, 
                skewed_perct, skewed_data_perct, page_size]
    skew_parameters = [[10, 100, 300], [2000], [5000], [50], 
                [10, 30, 50, 70, 100], [90], [512, 1024]]

    if sys.argv[2] == "all":
        parameters.append(algorithm)
        for combination in itertools.product(*parameters):
            count += 1
            print(count)
            print(build_command(combination[0], combination[1], combination[2], combination[3], 
                            combination[4], combination[5], combination[7], combination[6]))
            system(build_command(combination[0], combination[1], combination[2], combination[3], 
                            combination[4], combination[5], combination[7], combination[6]))
    else:
        if sys.argv[2] == "lru":
            for combination in itertools.product(*parameters):
                system(build_command(combination[0], combination[1], combination[2],
                            combination[3], combination[4], combination[5], 0, combination[6]))
        elif sys.argv[2] == "lruwsr":
            for combination in itertools.product(*parameters):
                system(build_command(combination[0], combination[1], combination[2],
                            combination[3], combination[4], combination[5], 1, combination[6]))
        elif sys.argv[2] == "fifo":
            for combination in itertools.product(*parameters):
                system(build_command(combination[0], combination[1], combination[2],
                            combination[3], combination[4], combination[5], 2, combination[6]))
    
        elif sys.argv[2] == "cflru":
            for combination in itertools.product(*parameters):
                system(build_command(combination[0], combination[1], combination[2],
                            combination[3], combination[4], combination[5], 3, combination[6]))
       
        elif sys.argv[2] == "skew":
            skew_parameters.append(algorithm)
            for combination in itertools.product(*skew_parameters):
                print(build_command(combination[0], combination[1], combination[2], combination[3], 
                            combination[4], combination[5], combination[7], combination[6]))
                system(build_command(combination[0], combination[1], combination[2],
                            combination[3], combination[4], combination[5], combination[7], combination[6]))

if __name__ == '__main__':
    main()