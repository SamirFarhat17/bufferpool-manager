#!/usr/bin/python
from os import system

def build_command(b, n, x, e, s, d, a):
    command = "./../buffermanager -b {} -n {} -x {} -e {} -s {} -d {} -a {}".format(b, n, x, e, s, d, a)
    return command

buffer_size = [150] #b
disk_size = [1500] #n
num_operations = [7500] #x
perct_reads_writes = [60] #e
skewed_perct = [90] #s
skewed_data_perct = [10] #d
algorithm = [1, 2, 3, 4, 5] #a

parameters = [buffer_size, disk_size, num_operations, 
     perct_reads_writes, skewed_perct, skewed_data_perct, algorithm]


for buf_size in buffer_size:
    for dsk_size in disk_size:
        for num_ops in num_operations:
            for pct_read in perct_reads_writes:
                for skew_p in skewed_perct:
                    for skew_d_p in skewed_data_perct:
                        for algo in algorithm:
                            system(build_command(buf_size, dsk_size, 
                                num_ops, pct_read, skew_p, skew_d_p, algo))