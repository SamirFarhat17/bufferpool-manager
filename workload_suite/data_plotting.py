#!/usr/bin/python3
from os import system, chdir, listdir, path
import itertools
from time import time

from numpy import average
from testing_helpers import *
import argparse
import sys
import matplotlib.pyplot as plt

# parse arguments
parser = argparse.ArgumentParser()
parser.add_argument("-t", "--type", help="Algorithm", default="algorithm")


# bar plot for skews
def plot_skews(data_points, title, xlabel, ylabel):
    fig = plt.figure()
    ax = fig.add_axes([0.12,0.1,0.8,0.8])
    algos = ['10%', '30%', '50%', '70%', '100%']
    ax.bar(algos, data_points, color=['purple', 'red', 'green', 'blue', 'black'])
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    plt.savefig("processed_results/" + (title).join('.png'))
    plt.show()

# bar plot for algorithm comparisons
def plot_algos(data_points, title, xlabel, ylabel):
    fig = plt.figure()
    ax = fig.add_axes([0.12,0.1,0.8,0.8])
    algos = ['LRU', 'LRUWSR', 'FIFO', 'CFLRU']
    ax.bar(algos, data_points, color=['purple', 'red', 'green', 'blue'])
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    plt.savefig("processed_results/" + (title).join('.png'))
    plt.show()

# to be used to discriminate 
num_ops = 0
buffersize = 0



def main():
    # intialize lists to eventually average and plot
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
    # plot algorithm specific tests
    if sys.argv[2] == "algorithm":
        # parse runs directory
        for filename in listdir(path.abspath("runs")):
            # if file is not relevant skip
            if discriminate(filename) and get_params(filename, "e") == "1000":
                # get buffersize and number of operations of file
                buffersize = int(get_params(filename, "b"))
                num_ops = int(get_params(filename, "x"))
                # get lru
                if get_params(filename, "a") == "LRU":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lru_hitrate.append(int(lines[1])/num_ops)
                    lru_time.append(float(lines[5])/num_ops)
                    file.close()
                # get lruwsr
                if get_params(filename, "a") == "LRUWSR":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    lruwsr_hitrate.append(int(lines[1])/num_ops)
                    lruwsr_time.append(float(lines[5])/num_ops)
                    file.close()
                # get fifo
                if get_params(filename, "a") == "FIFO":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    fifo_hitrate.append(int(lines[1])/num_ops)
                    fifo_time.append(float(lines[5])/num_ops)
                    file.close()
                # get cflru
                if get_params(filename, "a") == "CFLRU":
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    cflru_hitrate.append(int(lines[1])/num_ops)
                    cflru_time.append(float(lines[5])/num_ops)
                    file.close()

        # compute averages of data collected
        average_hitrate.append(sum(lru_hitrate)/len(lru_hitrate))
        average_hitrate.append(sum(lruwsr_hitrate)/len(lruwsr_hitrate))
        average_hitrate.append(sum(fifo_hitrate)/len(fifo_hitrate))
        average_hitrate.append(sum(cflru_hitrate)/len(cflru_hitrate))
        print(len(fifo_hitrate))
        print(len(lru_hitrate))
        print(len(lruwsr_hitrate))
        print(len(cflru_hitrate))
        print(average_hitrate)
        # plot hitrate
        plot_algos(average_hitrate, "Hitrate-for-read-intensive-workloads", "policy", "hit-rate")
        average_timings.append(sum(lru_time)/len(lru_time))
        average_timings.append(sum(lruwsr_time)/len(lruwsr_time))
        average_timings.append(sum(fifo_time)/len(fifo_time))
        average_timings.append(sum(cflru_time)/len(cflru_time))
        print(average_timings)
        # plot timings
        plot_algos(average_timings, "Timing-for-read-intensive-workloads", "policy", "time-per-op(ms)")

        # same, but specific to balanced and write-intensive respectively
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

        # balanced
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
        plot_algos(average_hitrate, "Hitrate-for-balanced-workloads", "policy", "hit-rate")
        average_timings.append(sum(lru_time)/len(lru_time))
        average_timings.append(sum(lruwsr_time)/len(lruwsr_time))
        average_timings.append(sum(fifo_time)/len(fifo_time))
        average_timings.append(sum(cflru_time)/len(cflru_time))
        plot_algos(average_timings, "Timing-for-balanced-workloads", "policy", "time-per-op(ms)")

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

        # write-intensive
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
        plot_algos(average_hitrate, "Hitrate-for-write-intensive-workloads", "policy", "hit-rate")
        average_timings.append(sum(lru_time)/len(lru_time))
        average_timings.append(sum(lruwsr_time)/len(lruwsr_time))
        average_timings.append(sum(fifo_time)/len(fifo_time))
        average_timings.append(sum(cflru_time)/len(cflru_time))
        plot_algos(average_timings, "Timing-for-write-intensive-workloads", "policy", "time-per-op(ms)")


    # process and plot skew specific
    if sys.argv[2] == "skew":
        ten_hitrate = []
        thirty_hitrate = []
        fifty_hitrate = []
        seventy_hitrate = []
        all_hitrate = []

        average_hitrate = []

        for filename in listdir(path.abspath("runs")):
            # collet and process data from relevant lru runs
            if discriminate_skew(filename) and get_params(filename, "a") == "LRU":
                num_ops = int(get_params(filename, "x"))
                if int(float(get_params(filename, "s"))) == 10:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    ten_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 30:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    thirty_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 50:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    fifty_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 70:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    seventy_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 100:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    all_hitrate.append(int(lines[1])/num_ops)
                    file.close()
        # average and plot
        average_hitrate.append(sum(ten_hitrate)/len(ten_hitrate))
        average_hitrate.append(sum(thirty_hitrate)/len(thirty_hitrate))
        average_hitrate.append(sum(fifty_hitrate)/len(fifty_hitrate))
        average_hitrate.append(sum(seventy_hitrate)/len(seventy_hitrate))
        average_hitrate.append(sum(all_hitrate)/len(all_hitrate))
        plot_skews(average_hitrate, "LRU-hitrate-variation-with-skew", "skew-percentage", "average hit per-operation")

        ten_hitrate = []
        thirty_hitrate = []
        fifty_hitrate = []
        seventy_hitrate = []
        all_hitrate = []

        average_hitrate = []

        # collet and process data from relevant lruwsr runs
        for filename in listdir(path.abspath("runs")):
            if discriminate_skew(filename) and get_params(filename, "a") == "LRUWSR":
                num_ops = int(get_params(filename, "x"))
                if int(float(get_params(filename, "s"))) == 10:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    ten_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 30:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    thirty_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 50:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    fifty_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 70:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    seventy_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 100:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    all_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                
        average_hitrate.append(sum(ten_hitrate)/len(ten_hitrate))
        average_hitrate.append(sum(thirty_hitrate)/len(thirty_hitrate))
        average_hitrate.append(sum(fifty_hitrate)/len(fifty_hitrate))
        average_hitrate.append(sum(seventy_hitrate)/len(seventy_hitrate))
        average_hitrate.append(sum(all_hitrate)/len(all_hitrate))

        plot_skews(average_hitrate, "LRUWSR-hitrate-variation-with-skew", "skew-percentage", "average hit per-operation")
        
        
        ten_hitrate = []
        thirty_hitrate = []
        fifty_hitrate = []
        seventy_hitrate = []
        all_hitrate = []

        average_hitrate = []
        # collet and process data from relevant fifo runs
        for filename in listdir(path.abspath("runs")):
            if discriminate_skew(filename) and get_params(filename, "a") == "FIFO":
                num_ops = int(get_params(filename, "x"))
                if int(float(get_params(filename, "s"))) == 10:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    ten_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 30:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    thirty_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 50:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    fifty_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 70:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    seventy_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 100:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    all_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                
        average_hitrate.append(sum(ten_hitrate)/len(ten_hitrate))
        average_hitrate.append(sum(thirty_hitrate)/len(thirty_hitrate))
        average_hitrate.append(sum(fifty_hitrate)/len(fifty_hitrate))
        average_hitrate.append(sum(seventy_hitrate)/len(seventy_hitrate))
        average_hitrate.append(sum(all_hitrate)/len(all_hitrate))
        plot_skews(average_hitrate, "FIFO-hitrate-variation-with-skew", "skew-percentage", "average hit per-operation")

        ten_hitrate = []
        thirty_hitrate = []
        fifty_hitrate = []
        seventy_hitrate = []
        all_hitrate = []

        average_hitrate = []
        # collet and process data from relevant cflru runs
        for filename in listdir(path.abspath("runs")):
            if discriminate_skew(filename) and get_params(filename, "a") == "CFLRU":
                num_ops = int(get_params(filename, "x"))
                if int(float(get_params(filename, "s"))) == 10:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    ten_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 30:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    thirty_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 50:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    fifty_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 70:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    seventy_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                if int(float(get_params(filename, "s"))) == 100:
                    file = open("runs/" + filename, "r")
                    lines = file.readlines()
                    all_hitrate.append(int(lines[1])/num_ops)
                    file.close()
                
        average_hitrate.append(sum(ten_hitrate)/len(ten_hitrate))
        average_hitrate.append(sum(thirty_hitrate)/len(thirty_hitrate))
        average_hitrate.append(sum(fifty_hitrate)/len(fifty_hitrate))
        average_hitrate.append(sum(seventy_hitrate)/len(seventy_hitrate))
        average_hitrate.append(sum(all_hitrate)/len(all_hitrate))
        plot_skews(average_hitrate, "CFLRU-hitrate-variation-with-skew", "skew-percentage", "average hit per-operation")


if __name__ == '__main__':
    main()