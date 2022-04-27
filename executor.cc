#include <iostream>
#include <cmath>
#include <sys/time.h>
#include <vector>
#include <cstdlib>
#include <stdint.h>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>
#include <random>
#include <climits>
#include <functional>

#include "parameter.h"
#include "executor.h"

using namespace bufmanager;

// Parameter initialization
Buffer *Buffer::buffer_instance;
long Buffer::max_buffer_size = 0;
int Buffer::buffer_hit = 0;
int Buffer::buffer_miss = 0;
int Buffer::read_io = 0;
int Buffer::write_io = 0;
int Buffer::pageSize = 0;
char Buffer::disk_write_char = '0';
chrono::duration <double, milli> Buffer::timing;
const vector<string> algorithms{ "LRU", "LRUWSR", "FIFO", "CFLRU", "LFU"};


Buffer::Buffer(Simulation_Environment *_env) {
    // lru_candidate is for LRU list
	//chrono::duration <double, milli> (chrono::steady_clock::now() - chrono::steady_clock::now());
    lru_candidate.clear();
    max_buffer_size = _env->buffer_size_in_pages;
	pageSize = _env->page_size;

	// initialize the cache
	// for(int i = 0; i < max_buffer_size; i++) bufferpool.push_back(make_pair(-1, false));
}


Buffer *Buffer::getBufferInstance(Simulation_Environment *_env) {
    if (buffer_instance == 0) buffer_instance = new Buffer(_env);
    return buffer_instance;
}


// search in the bufferpool
int WorkloadExecutor::search(Buffer* buffer_instance, int pageId, int algorithm) {
    switch (algorithm) {
		// 1 -> LRU
		case 1: {
			// the bufferpool still empty
			if(buffer_instance->bufferpool.size() == 0) {
				return -1;
			}
			for(int i = 0; i < buffer_instance->max_buffer_size; i++) {
				// find the page in the bufferpool, hit
				if(buffer_instance->bufferpool[i].first == pageId) {
					buffer_instance->buffer_hit += 1;
					return i;
				}
			}
			break;
		}
		// 2 -> LRU_WSR
		case 2: {
			if(buffer_instance->bufferpool_wsr.size() == 0) {
				return -1;
			}
			for(int i = 0; i < buffer_instance->max_buffer_size; i++) {
				// find the page in the bufferpool, hit
				if(get<0>(buffer_instance->bufferpool_wsr[i]) == pageId) {
					buffer_instance->buffer_hit += 1;
					return i;
				}
			}
			break;
		}
		//  FIFO
		case 3: {
			// bufferpool is empty
			if(buffer_instance->bufferpool.size() == 0) {
				return -1;
			}
			for(int i = 0; i < buffer_instance->max_buffer_size; i++) {
				// find the page in the bufferpool, hit
				if(buffer_instance->bufferpool[i].first == pageId) {
					buffer_instance->buffer_hit += 1;
					return i;
				}
			}
			break;
		}
		//4 -> CFLRU
		case 4: {
			// the bufferpool still empty
			if(buffer_instance->bufferpool.size() == 0) {
				return -1;
			}
			//search through both the working part and clean-first part for a hit, just in case
			for(int i = 0; i < buffer_instance->max_buffer_size; i++) {
				// find the page in the bufferpool, hit
				if(buffer_instance->bufferpool[i].first == pageId) {
					buffer_instance->buffer_hit += 1;
					return i;
				}
			}
			break;
		}
    }

    // did not find the page, miss
    buffer_instance->buffer_miss += 1;
    return -1;
}


// allocate empty frame and read the page into that frame
int WorkloadExecutor::read(Buffer* buffer_instance, int pageId, int algorithm) {
    // TODO
    switch (algorithm) {
		case 1: {
			int cur_size = buffer_instance->bufferpool.size();
			int capacity = buffer_instance->max_buffer_size;
			int pos = search(buffer_instance, pageId, algorithm);

			if(pos != -1) {
				// found, only need to update lru
				deque<int>::iterator it = buffer_instance->lru_candidate.begin();
				while(*it != pageId) it++;
				buffer_instance->lru_candidate.erase(it);
				buffer_instance->lru_candidate.push_front(pageId);
			}
			// miss
			else {
				// cache is not full
				if(cur_size < capacity) {
					// read the page from disk, mark the page clean
					// TODO
					buffer_instance->bufferpool.push_back(make_pair(pageId, false));
					// add read_io
					// add disk read functionality
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);

				}
				// cache is full
				else {
					// find the position to evict
					// cout << "enter LRU func!!!" << endl;
					int pos = buffer_instance->LRU();
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[pos].second == true) {
						buffer_instance->write_io += 1;
						diskOp(buffer_instance, 1, pageId);
						// add disk write functionality
					}
					// erase the target page
					buffer_instance->bufferpool[pos].first = -1;
					buffer_instance->bufferpool[pos].second = false;

					// put new page in the blank
					buffer_instance->bufferpool[pos].first = pageId;
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);

				}
				// update lru
				buffer_instance->lru_candidate.push_front(pageId);
			}
			break;
		}
		case 2: {
			int cur_size = buffer_instance->bufferpool_wsr.size();
			int capacity = buffer_instance->max_buffer_size;
			int pos = search(buffer_instance, pageId, algorithm);

			if(pos != -1) {
				// found, only need to update lru
				deque<int>::iterator it = buffer_instance->lru_candidate.begin();
				while(*it != pageId) it++;
				buffer_instance->lru_candidate.erase(it);
				buffer_instance->lru_candidate.push_front(pageId);
			}
			// miss
			else {
				// cache is not full
				if(cur_size < capacity) {
					// read the page from disk, mark the page clean
					buffer_instance->bufferpool_wsr.push_back(make_tuple(pageId, false, false));
					// add read_io
					// add disk read functionality
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);

				}
				// cache is full
				else {
					// find the position to evict
					// cout << "enter LRU func!!!" << endl;
					int pos = buffer_instance->LRUWSR();
					// if the page is dirty, write the page into the disk
					if(get<1>(buffer_instance->bufferpool_wsr[pos]) == true) {
						// add disk write functionality
						buffer_instance->write_io += 1;
						diskOp(buffer_instance, 1, pageId);
					}
					// erase the target page
					get<0>(buffer_instance->bufferpool_wsr[pos]) = -1;
					get<1>(buffer_instance->bufferpool_wsr[pos]) = false;

					// put new page in the blank
					get<0>(buffer_instance->bufferpool_wsr[pos]) = pageId;
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);

				}
				// update lru
				buffer_instance->lru_candidate.push_front(pageId);
			}
			break;
		}
		//  FIFO
		case 3: {
			int cur_size = buffer_instance->bufferpool.size();
			int capacity = buffer_instance->max_buffer_size;
			int pos = search(buffer_instance, pageId, algorithm);
			// if page is already in the bufferpool do nothing except read from bufferpool
			if(pos != -1) {
				
			}
			// otherwise this is a miss 
			else {
				// not full, no need to evict
				if(cur_size < capacity) {
					buffer_instance->bufferpool.push_back(make_pair(pageId, false));
					buffer_instance->fifo_candidates.push_back(pageId);
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
				}
				// full, perform eviction
				else {
					// get position to replace (pops in FIFO function)
					int replacement_pos = buffer_instance->FIFO();
					// add incoming page to FIFO queue
					buffer_instance->fifo_candidates.push_back(pageId);
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[replacement_pos].second == true) {
						buffer_instance->write_io += 1;
						diskOp(buffer_instance, 1, pageId);
						// add disk write functionality
					}
					// erase the target page
					buffer_instance->bufferpool[replacement_pos].first = -1;
					buffer_instance->bufferpool[replacement_pos].second = false;

					// put new page in the blank
					buffer_instance->bufferpool[replacement_pos].first = pageId;
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
				}
				// need to add disk read when we eventually add that functionality
			}
			break;
		}
		case 4:{ //CFLRU
			int cur_size = buffer_instance->bufferpool.size();
			int capacity = buffer_instance->max_buffer_size;
			int pos = search(buffer_instance, pageId, algorithm);
			if(pos != -1) {
				// found, only need to update bufferpool
				deque<int>::iterator it = buffer_instance->lru_candidate.begin();
				while(*it != pageId) it++;
				buffer_instance->lru_candidate.erase(it);
				//pushing it to the very front bc it will be in the working zone there
				buffer_instance->lru_candidate.push_front(pageId);
			}
			else{//bufferpool miss
				if(cur_size < capacity) { //space in cache
					// read the page from disk, mark the page clean
					//add to the front so it is the first page in the working zone
					buffer_instance->bufferpool.push_back(make_pair(pageId, false));
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
				}
				else { //cache is full
					// find the position to evict
					int pos = buffer_instance->CFLRU();
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[pos].second == true) {
						buffer_instance->write_io += 1;
						diskOp(buffer_instance, 1, pageId);
					}
					// erase the target page
					buffer_instance->bufferpool[pos].first = -1;
					buffer_instance->bufferpool[pos].second = false;

					// put new page in the blank
					buffer_instance->bufferpool[pos].first = pageId;
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);

				}
				// update lru
				buffer_instance->lru_candidate.push_front(pageId);
				
			}

			break;
		}
    }

	return -1;
}


int WorkloadExecutor::write(Buffer* buffer_instance, int pageId, int algorithm) {
    switch (algorithm) {
		case 1: {
			int cur_size = buffer_instance->bufferpool.size();
			int capacity = buffer_instance->max_buffer_size;
			int pos = search(buffer_instance, pageId, algorithm);
			// if page is already in the bufferpool, update it
			if(pos != -1) {
				// found, update lru
				deque<int>::iterator it = buffer_instance->lru_candidate.begin();
				while(*it != pageId) it++;
				buffer_instance->lru_candidate.erase(it);
				buffer_instance->lru_candidate.push_front(pageId);
				// marking the page as dirty bc it's been updated
				// buffer_instance->bufferpool.push_back(make_pair(pageId, true));
				// BUG FIXED
				buffer_instance->bufferpool[pos].second = true;

			}
			// otherwise this is a miss
			else {
				// if bufferpool isnt empty, simply add the page
				if(cur_size < capacity) {
					// read the page from disk, mark the page dirty since we are updatin git
					buffer_instance->bufferpool.push_back(make_pair(pageId, true));
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
				}
				// otherwise, the cache is full
				else {
					// find the position to evict
					int pos = buffer_instance->LRU();
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[pos].second == true) {
						buffer_instance->write_io += 1;
						diskOp(buffer_instance, 1, pageId);
					}
					// erase the target page
					buffer_instance->bufferpool[pos].first = -1;
					buffer_instance->bufferpool[pos].second = false;
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
					// put new page in the blank, and then set that page to be dirty
					buffer_instance->bufferpool[pos].first = pageId;
					buffer_instance->bufferpool[pos].second = true;
				}
				// update lru
				// BUG FIXED
				buffer_instance->lru_candidate.push_front(pageId);
				// return -1;
			}
			break;
		}
		// LRU_WSR
		case 2: {
			int cur_size = buffer_instance->bufferpool_wsr.size();
			int capacity = buffer_instance->max_buffer_size;
			int pos = search(buffer_instance, pageId, algorithm);
			// if page is already in the bufferpool, update it
			if(pos != -1) {
				// found, update lru
				deque<int>::iterator it = buffer_instance->lru_candidate.begin();
				while(*it != pageId) it++;
				buffer_instance->lru_candidate.erase(it);
				buffer_instance->lru_candidate.push_front(pageId);
				// marking the page as dirty bc it's been updated
				// BUG FIXED
				get<1>(buffer_instance->bufferpool_wsr[pos]) = true;
				// make sure cold flag is cleared
				get<2>(buffer_instance->bufferpool_wsr[pos]) = false;
			}
			// otherwise this is a miss
			else {
				// if bufferpool isnt empty, simply add the page
				if(cur_size < capacity) {
					// read the page from disk, mark the page dirty since we are updatin git
					buffer_instance->bufferpool_wsr.push_back(make_tuple(pageId, true, false));
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
				}
				// otherwise, the cache is full
				else {
					// find the position to evict
					int pos = buffer_instance->LRUWSR();
					// if the page is dirty, write the page into the disk
					if(get<1>(buffer_instance->bufferpool_wsr[pos]) == true) {
						buffer_instance->write_io += 1;
						diskOp(buffer_instance, 1, pageId);
					}
					// erase the target page in the bufferpool
					get<0>(buffer_instance->bufferpool_wsr[pos]) = -1;
					// put new page in the blank, and then set that page to be dirty
					get<0>(buffer_instance->bufferpool_wsr[pos]) = pageId;
					get<1>(buffer_instance->bufferpool_wsr[pos]) = true;
					get<2>(buffer_instance->bufferpool_wsr[pos]) = false;
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
					// get<1>(buffer_instance->bufferpool_wsr[pos]) = true;
				}
				// update lru
				// BUG FIXED
				buffer_instance->lru_candidate.push_front(pageId);
				// return -1;
			}
			break;
		}
		//  FIFO
		case 3: {
			int cur_size = buffer_instance->bufferpool.size();
			int capacity = buffer_instance->max_buffer_size;
			int pos = search(buffer_instance, pageId, algorithm);
			// if page is already in the bufferpool, update it
			if(pos != -1) {
				// found, mark dirty
				buffer_instance->bufferpool[pos].second = true;
			}
			// otherwise this is a miss
			else {
				if(cur_size < capacity) {
					buffer_instance->bufferpool.push_back(make_pair(pageId, true));
					buffer_instance->fifo_candidates.push_back(pageId);
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
		
				}
				// full, time to replace
				else {
					// get position to replace (pops in FIFO function)
					int replacement_pos =  buffer_instance->FIFO();
					buffer_instance->fifo_candidates.push_back(pageId);
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[replacement_pos].second == true) {
						buffer_instance->write_io += 1;
						diskOp(buffer_instance, 1, pageId);
						// add disk write functionality
					}
					// erase the target page put new page in the blank
					buffer_instance->bufferpool[replacement_pos].second = true;
					buffer_instance->bufferpool[replacement_pos].first = pageId;
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
				}
			}
			break;
		}
		// CFLRU
		case 4: {
			//cflru write logic
			int cur_size = buffer_instance->bufferpool.size();
			int capacity = buffer_instance->max_buffer_size;
			int pos = search(buffer_instance, pageId, algorithm);

			if(pos != -1) {
				// found, update bufferpool
				deque<int>::iterator it = buffer_instance->lru_candidate.begin();
				while(*it != pageId) it++;
				buffer_instance->lru_candidate.erase(it);
				//pushing page to front to be first element in working zone
				buffer_instance->lru_candidate.push_front(pageId);
				// marking the page as dirty bc it's been updated
				buffer_instance->bufferpool[pos].second = true;

			}
			else{ //page not in cache
				// if bufferpool isnt empty, simply add the page
				if(cur_size < capacity) {
					// read the page from disk, mark the page dirty since we are updatin git
					//adding to front to be first element in working zone
					buffer_instance->bufferpool.push_back(make_pair(pageId, true));
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
				}
				// otherwise, the cache is full
				else {
					// find the position to evict
					int pos = buffer_instance->CFLRU();
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[pos].second == true) {
						buffer_instance->write_io += 1;
						diskOp(buffer_instance, 1, pageId);
					}
					// erase the target page
					buffer_instance->bufferpool[pos].first = -1;
					buffer_instance->bufferpool[pos].second = false;
					// add read_io
					buffer_instance->read_io += 1;
					diskOp(buffer_instance, 0, pageId);
					// put new page in the blank, and then set that page to be dirty
					buffer_instance->bufferpool[pos].first = pageId;
					buffer_instance->bufferpool[pos].second = true;
				}
				buffer_instance->lru_candidate.push_front(pageId);
			}
			break;
		}
    }

    return -1;
}


// Need to calculate disk sector and page size to accomplish proper functionality
// Perform disk read or write
void WorkloadExecutor::diskOp(Buffer* buffer_instance, int operation, int pageID) {
	
	Simulation_Environment* _env = Simulation_Environment::getInstance();
	// read
	if(operation == 0) {
		// add in memory variable to the stack 
		string reader;
		// Calculate pointer in the disk
		buffer_instance->disk.seekg(buffer_instance->pageSize * pageID, std::ios::beg);

		for(int i = 0; i < buffer_instance->pageSize; i++) {
			// simulate reading byte by byte into cache/memory/registers
			reader += char(buffer_instance->disk.get());
		}

	}
	// write
	else if(operation == 1) {
		// increment to next ascii value
		buffer_instance->disk_write_char = char((int(buffer_instance->disk_write_char) % 26)+int('a'));
		char c = buffer_instance->disk_write_char;
		// Adjust pointer
		buffer_instance->disk.seekg(buffer_instance->pageSize * pageID, std::ios::beg);

		for(int j = 0; j < buffer_instance->pageSize - 1; j++) {
			// write byte-by-byte
			buffer_instance->disk.put(c);
		}
		// add endline as the last character in the page
		buffer_instance->disk.put('\n');

	}

	return;
	
}


void WorkloadExecutor::writeDisk(Buffer* buffer_instance) {
	Simulation_Environment* _env = Simulation_Environment::getInstance();
	string str = "";
	char c;
	srand(time(NULL));
	for(int i = 0; i < _env->disk_size_in_pages; i++) {
		str = "";
		for(int j = 0; j < buffer_instance->pageSize - 1; j++) {
			c = 'a' + rand()%26;
			str = str + c;
		}
		buffer_instance->disk << str << endl;
	}
	return;
}


// return the evict position in bufferpool
int Buffer::LRU() {
    // get the least used page id
    int index = -1;
    if(lru_candidate.size() > 0) {
        int pageId = lru_candidate.back();
        // delete it from the LRU list
        lru_candidate.pop_back();

        for(int i = 0; i < bufferpool.size(); i++) {
            if(pageId == bufferpool[i].first) {
                index = i;
                break;
            }
        }
    }
    return index;
}


//return the evict position in bufferpool for cflru algorithm 
int Buffer::CFLRU(){
	int cur_size = lru_candidate.size();
	int window_size = ceil(cur_size/3); 
	/*
	the windowsize for the clean_first section is the ceilingcurrent amount of 
	items in buffer/3, so that it is flexible based on the current demands 
	(never too big or too small)
	*/
	int index = -1;
	int count = 0;
	int pageId = -1;

	//going to go through the candidates in reverse order in the given window looking for a clean page
	for(auto i = lru_candidate.rbegin(); count < window_size; i++){
		if(i[0] != -1){
			//this is the page to evict
			pageId = i[0];
			lru_candidate.erase(lru_candidate.begin() + (lru_candidate.size() - count - 1)); //erase this element from the cflru
			break;
		}
		count++;
	}

	if(pageId == -1){
		//if there was not a clean page in the window, then you will just take the least accessed page
		pageId = lru_candidate.back();
        // delete it from the LRU list
        lru_candidate.pop_back();
	}

	for(int i = 0; i < bufferpool.size(); i++) {
		if(pageId == bufferpool[i].first) {
			index = i;
			break;
		}
	}

	return index;
}


// return the evict position in bufferpool
// second chance algo
int Buffer::LRUWSR() {
    int index = -1;
    // get the lru page id
    int victim = lru_candidate.back();
    // mapping the page id to actual page in the bufferpool
    for(int i = 0; i < bufferpool_wsr.size(); i++) {
        if(victim == get<0>(bufferpool_wsr[i])) {
            index = i;
            break;
        }
    }
    // while the victim is dirty
    while(get<1>(bufferpool_wsr[index])) {
        // if the cold flag is set
        if(get<2>(bufferpool_wsr[index])) {
            // exit the while loop
            break;
        } else {
            // move victim to MRU position
            lru_candidate.pop_back();
            lru_candidate.push_front(victim);
            // set its cold flag
            get<2>(bufferpool_wsr[index]) = true;
            // replace victim with next LRU page
            victim = lru_candidate.back();
            // update index mapping
            for(int i = 0; i < bufferpool_wsr.size(); i++) {
                if(victim == get<0>(bufferpool_wsr[i])) {
                    index = i;
                    break;
                }
            }
        }
    }
    // remove victim from the LRU list
    lru_candidate.pop_back();
    return index;
}

//  return the evict position in bufferpool
//  First-In-First-Out
int Buffer::FIFO() {
	// get the page position to be replace and pop it
	int index = -1;
	// get page ID to be replaced
	int pageToReplace = fifo_candidates.front();
	fifo_candidates.pop_front();
	// parse bufferpool to find it
	for(int i = 0; i < bufferpool.size(); i++) {
		if(pageToReplace == bufferpool[i].first) {
			index = i;
			break;
		}
	}
	// position in bufferpool to be replaced by incoming page
	return index;
}


int Buffer::printBuffer() {
    int counter = 0;
    if(buffer_instance->bufferpool_wsr.empty()) {
        for(std::pair<int, bool> page : buffer_instance->bufferpool) {
            std::cout << "Page: " << counter << " PageID: " << page.first
                      << " Dirty-Bit: " << page.second << endl;
            counter++;
        }
    } else {
        for(std::tuple<int, bool, bool> page : buffer_instance->bufferpool_wsr) {
            std::cout << "Page: " << counter << " PageId: " << get<0>(page)
                      << " Dirty-Bit: " << get<1>(page) << " Cold Flag: " << get<2>(page) << endl;
            counter++;
        }
    }

    return 0;
}

int Buffer::printStats() {
	auto end = chrono::steady_clock::now();
    Simulation_Environment* _env = Simulation_Environment::getInstance();
    std::cout << "******************************************************" << endl;
    std::cout << "Printing Stats..." << endl;
    std::cout << "Number of operations: " << _env->num_operations << endl;
    std::cout << "Buffer Hit: " << buffer_hit << endl;
    std::cout << "Buffer Miss: " << buffer_miss << endl;
    std::cout << "Read IO: " << read_io << endl;
    std::cout << "Write IO: " << write_io << endl;
    std::cout << "Global Clock: " << Buffer::timing.count() << "ms" << endl; 
    std::cout << "******************************************************" << endl;
    return 0;
}

void Buffer::writeResults() {
	Simulation_Environment* _env = Simulation_Environment::getInstance();

	string buffer_size_in_pages = std::to_string(_env->buffer_size_in_pages); // b
    string disk_size_in_pages = std::to_string(_env->disk_size_in_pages); // n
    string num_operations = std::to_string(_env->num_operations); // x
    string perct_reads = std::to_string(_env->perct_reads); // e
    string perct_writes = std::to_string(_env->perct_writes);
    string skewed_perct = std::to_string(_env->skewed_perct); //s
    string skewed_data_perct = std::to_string(_env->skewed_data_perct); // d
    string pin_mode = std::to_string(_env->pin_mode); // p
    string verbosity = std::to_string(_env->verbosity); // v
	string page_size = std::to_string(_env->page_size); //k
    int algo = _env->algorithm; // a

	string u = "_";
	// Make filename such that it can be parsed for stats of interest
	string filename = "b-" + buffer_size_in_pages + u + "n-" + disk_size_in_pages + u + "x-" 
		+ num_operations + u + "e-" + perct_reads + perct_writes + u + "s-" + skewed_perct
		+ "d-" + skewed_data_perct + u + "v-" + verbosity + u + "a-" + algorithms.at(algo) 
		+ "k-" + page_size;
	// write the file with the run statistics
	ofstream statFile;
	statFile.open("workload_suite/runs/" + filename + ".txt");
	statFile << num_operations << endl;
	statFile << buffer_hit << endl;
    statFile << buffer_miss << endl;
    statFile << read_io << endl;
    statFile << write_io << endl;
    statFile << Buffer::timing.count(); 
	statFile.close();

	return;
}