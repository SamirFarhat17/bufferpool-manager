
#include <iostream>
#include <cmath>
#include <sys/time.h>
#include "parameter.h"

/*Set up the singleton object with the experiment wide options*/
Simulation_Environment* Simulation_Environment::instance = 0;

Simulation_Environment::Simulation_Environment() {
    int buffer_size_in_pages = 100;
    int disk_size_in_pages = 10000;

    int page_size = 4096;

    int num_operations = 10000;
    float perct_reads = 70;
    float perct_writes = 30;
    int algorithm = 0;

    float skewed_perct = 100;
    float skewed_data_perct = 100;

    int pin_mode = 0;
    int verbosity = 0;
}

Simulation_Environment* Simulation_Environment::getInstance() {
    if (instance == 0)
        instance = new Simulation_Environment();

    return instance;
}
