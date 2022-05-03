#!/usr/bin/python3
from os import system, chdir, listdir, path
import itertools
from time import time

from numpy import average
from testing_helpers import *
import argparse
import sys
import matplotlib.pyplot as plt


parser = argparse.ArgumentParser()
parser.add_argument("-t", "--type", help="Algorithm", default="algorithm")

def plot(data_points, title, xlabel, ylabel):
    fig = plt.figure()
    ax = fig.add_axes([0.12,0.1,0.8,0.8])
    algos = ['LRU', 'LRUWSR', 'FIFO', 'CFLRU']
    ax.bar(algos, data_points, color=['purple', 'red', 'green', 'blue'])
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    plt.tight_layout()
    plt.savefig("processed_results/" + (title).join('.png'))
    plt.show()


num_ops = 0
buffersize = 0



def main():
    lru_time = []
    lru_hitrate = []
    lruwsr_time = []
    lruwsr_hitrate = []
    fifo_time = []
    fifo_hitrate = []
    cflru_time = []
    cflru_hitrate = []

    average_hitrate = []
    average_timings = []

    if sys.argv[2] == "algorithm":
        
        for filename in listdir(path.abspath("runs")):
            if discriminate(filename) and get_params(filename, "e") == "1000":
                buffersize = int(get_params(filename, "b"))
                num_ops = int(get_params(filename, "x"))
                if get_params(filename, "a") == "LRU":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lru_hitrate.append(int(lines[1])/num_ops)
                    lru_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "LRUWSR":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lruwsr_hitrate.append(int(lines[1])/num_ops)
                    lruwsr_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "FIFO":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    fifo_hitrate.append(int(lines[1])/num_ops)
                    fifo_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "CFLRU":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    cflru_hitrate.append(int(lines[1])/num_ops)
                    cflru_time.append(float(lines[5])/num_ops)
                    file.close()


        average_hitrate.append(sum(lru_hitrate)/len(lru_hitrate))
        average_hitrate.append(sum(lruwsr_hitrate)/len(lruwsr_hitrate))
        average_hitrate.append(sum(fifo_hitrate)/len(fifo_hitrate))
        average_hitrate.append(sum(cflru_hitrate)/len(cflru_hitrate))
        print(len(fifo_hitrate))
        print(len(lru_hitrate))
        print(len(lruwsr_hitrate))
        print(len(cflru_hitrate))
        print(average_hitrate)
        plot(average_hitrate, "Hitrate for read-intensive workloads", "policy", "hit-rate")
        average_timings.append(sum(lru_time)/len(lru_time))
        average_timings.append(sum(lruwsr_time)/len(lruwsr_time))
        average_timings.append(sum(fifo_time)/len(fifo_time))
        average_timings.append(sum(cflru_time)/len(cflru_time))
        print(average_timings)
        plot(average_timings, "Timing for read-intensive workloads", "policy", "time-per-op(ms)")

        lru_time = []
        lru_hitrate = []
        lruwsr_time = []
        lruwsr_hitrate = []
        fifo_time = []
        fifo_hitrate = []
        cflru_time = []
        cflru_hitrate = []

        average_hitrate = []
        average_timings = []

        for filename in listdir(path.abspath("runs")):
            if discriminate(filename) and get_params(filename, "e") == "5050":
                buffersize = int(get_params(filename, "b"))
                num_ops = int(get_params(filename, "x"))
                if get_params(filename, "a") == "LRU":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lru_hitrate.append(int(lines[1])/num_ops)
                    lru_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "LRUWSR":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lruwsr_hitrate.append(int(lines[1])/num_ops)
                    lruwsr_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "FIFO":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    fifo_hitrate.append(int(lines[1])/num_ops)
                    fifo_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "CFLRU":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    cflru_hitrate.append(int(lines[1])/num_ops)
                    cflru_time.append(float(lines[5])/num_ops)
                    file.close()

        average_hitrate.append(sum(lru_hitrate)/len(lru_hitrate))
        average_hitrate.append(sum(lruwsr_hitrate)/len(lruwsr_hitrate))
        average_hitrate.append(sum(fifo_hitrate)/len(fifo_hitrate))
        average_hitrate.append(sum(cflru_hitrate)/len(cflru_hitrate))
        plot(average_hitrate, "Hitrate for balanced workloads", "policy", "hit-rate")
        average_timings.append(sum(lru_time)/len(lru_time))
        average_timings.append(sum(lruwsr_time)/len(lruwsr_time))
        average_timings.append(sum(fifo_time)/len(fifo_time))
        average_timings.append(sum(cflru_time)/len(cflru_time))
        plot(average_timings, "Timing for balanced workloads", "policy", "time-per-op(ms)")

        lru_time = []
        lru_hitrate = []
        lruwsr_time = []
        lruwsr_hitrate = []
        fifo_time = []
        fifo_hitrate = []
        cflru_time = []
        cflru_hitrate = []

        average_hitrate = []
        average_timings = []

        for filename in listdir(path.abspath("runs")):
            if discriminate(filename) and get_params(filename, "e") == "1090":
                buffersize = int(get_params(filename, "b"))
                num_ops = int(get_params(filename, "x"))
                if get_params(filename, "a") == "LRU":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lru_hitrate.append(int(lines[1])/num_ops)
                    lru_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "LRUWSR":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lruwsr_hitrate.append(int(lines[1])/num_ops)
                    lruwsr_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "FIFO":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    fifo_hitrate.append(int(lines[1])/num_ops)
                    fifo_time.append(float(lines[5])/num_ops)
                    file.close()
                if get_params(filename, "a") == "CFLRU":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    cflru_hitrate.append(int(lines[1])/num_ops)
                    cflru_time.append(float(lines[5])/num_ops)
                    file.close()

        average_hitrate.append(sum(lru_hitrate)/len(lru_hitrate))
        average_hitrate.append(sum(lruwsr_hitrate)/len(lruwsr_hitrate))
        average_hitrate.append(sum(fifo_hitrate)/len(fifo_hitrate))
        average_hitrate.append(sum(cflru_hitrate)/len(cflru_hitrate))
        plot(average_hitrate, "Hitrate for write-intensive workloads", "policy", "hit-rate")
        average_timings.append(sum(lru_time)/len(lru_time))
        average_timings.append(sum(lruwsr_time)/len(lruwsr_time))
        average_timings.append(sum(fifo_time)/len(fifo_time))
        average_timings.append(sum(cflru_time)/len(cflru_time))
        plot(average_timings, "Timing for write-intensive workloads", "policy", "time-per-op(ms)")



    elif sys.argv[2] == "skew":
        for filename in listdir(path.abspath("runs")):
            #print(filename)
            if(discriminate_skew(filename)):
                #print("filename")
                return
        


if __name__ == '__main__':
    main()