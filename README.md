# CS 561: Data Systems Architecture - TemplateBufferpool


## About

TemplateBufferpool is a simple template for the students of CS 561. It can be used for the systems projects where the student need
to implement a simple buffermanage of a DBMS. Note that this is a simple template, it is
not extensive, rather it is meant to help guide you on what we expect when
you implement the bufferpool. You can use this as base code or start from scratch.


## Requirements

You will need the following on your system (or alternatively develop on the
CSA machines). Note that, we are considering a linux-based system. But the code should work in other 
environments as well provided that your system has all the dependencies supported.

    1. Make
    2. C++ Compiler


## Usage

You can clean using


```bash
make clean
```

Afterwards, build using


```bash
make
```

The executable should be generated in the directory. Currently this template generates a workload and writes the workload
in a file named 'workload.txt'. It then reads this file and issues read/write requests based on the workload. An example command can be: 

```bash
./buffermanager -b 150 -n 1500 -x 7500 -e 60 -a 4 -s 90 -d 10 -k 4096 -w 20
```

This generates a workload of 7500 operations with 60% reads where the bufferpool size is 150 pages and disk size is 1500 pages. 
The workload is skewed (90% operations are on 10% data). The parameter 'a' shall determine the eviction policy to be CFLRU. The parameter 'k' will set each page size to 4096.
The parameter 'w' is only useful if you will be using the CFLRU eviction policy, and in this case it would set the window size to be 20% of the current number of pages in the bufferpool. 
Currently, in the program all the parameters default value is set. So, running only ./buffermanager will generate a default workload.
You can view the default values by running the following command

```bash
./buffermanager -h
```
These are the following parameters of the bufferpool. Further explanation on them can be found above.
```bash
int buffer_size_in_pages;	// b
int disk_size_in_pages;   	// n
int num_operations;    		// x
int perct_reads;       		// e
float skewed_perct;      	// s
float skewed_data_perct; 	// d
int algorithm;         		// a (1: LRU, 2: LRU-WSR, 3: FIFO, 4:CFLRU)
int pin_mode;   		// p
int verbosity;         		// v
int page_size               //k
int perct_window            //w
```

Currently this implementation has no bufferpool and no way to simulate read/writes. 
Your job is to implement them and implement two/three eviction policies.


## Contact

If you have any questions please feel free to see Papon in office hours, or
email him at papon@bu.edu.
