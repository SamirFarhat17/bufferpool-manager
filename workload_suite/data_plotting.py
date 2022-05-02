#!/usr/bin/python3
from os import system, chdir, listdir, path
import itertools
from testing_helpers import *
import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--type", help="Algorithm", default="algorithm")

lru_time = []
lru_missrate = []
lruwsr_time = []
lruwsr_missrate = []
fifo_time = []
fifo_missrate = []
cflru_time = []
cflru_missrate = []

num_ops = 0
buffersize = 0

def main():
    if sys.argv[2] == "algorithm":
        '''for filename in listdir(path.abspath("runs")):
            #print(filename)
            if(discriminate_skew(filename)):
                print("filename")'''
        for filename in listdir(path.abspath("runs")):
            if(discriminate(filename)):
                buffersize = int(get_params(filename, "b"))
                num_ops = int(get_params(filename, "x"))
                if get_params(filename, "a") == "LRU":
                    print(filename)
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lru_missrate.append(int(lines[2])/(num_ops*buffersize))
                    print(lines[5])
                    lru_time.append(float(lines[2])/num_ops)
                    file.close()
                if get_params(filename, "a") == "LRUWSR":
                    file = open("runs/" + filename, "r")
                    file.close()
                if get_params(filename, "a") == "FIFO":
                    file = open("runs/" + filename, "r")
                    file.close()
                if get_params(filename, "a") == "CFLRU":
                    file = open("runs/" + filename, "r")
                    file.close()


if __name__ == '__main__':
    main()