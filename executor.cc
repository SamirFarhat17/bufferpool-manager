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
using random_bytes_engine = std::independent_bits_engine<
    std::default_random_engine, CHAR_BIT, unsigned char>;

random_bytes_engine rbe;
std::vector<unsigned char> rand_data(1000);

Buffer *Buffer::buffer_instance;
long Buffer::max_buffer_size = 0;
int Buffer::buffer_hit = 0;
int Buffer::buffer_miss = 0;
int Buffer::read_io = 0;
int Buffer::write_io = 0;
chrono::duration <double, milli> Buffer::timing;
const vector<string> algorithms{ "LRU", "LRUWSR", "FIFO", "CFLRU", "LFU"};


Buffer::Buffer(Simulation_Environment *_env) {
    // lru_candidate is for LRU list
	//chrono::duration <double, milli> (chrono::steady_clock::now() - chrono::steady_clock::now());
    lru_candidate.clear();
    max_buffer_size = _env->buffer_size_in_pages;

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

				}
				// cache is full
				else {
					// find the position to evict
					// cout << "enter LRU func!!!" << endl;
					int pos = buffer_instance->LRU();
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[pos].second == true) {
						buffer_instance->write_io += 1;
						// add disk write functionality
					}
					// erase the target page
					buffer_instance->bufferpool[pos].first = -1;
					buffer_instance->bufferpool[pos].second = false;

					// put new page in the blank
					buffer_instance->bufferpool[pos].first = pageId;
					// add read_io
					buffer_instance->read_io += 1;

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
					}
					// erase the target page
					get<0>(buffer_instance->bufferpool_wsr[pos]) = -1;
					get<1>(buffer_instance->bufferpool_wsr[pos]) = false;

					// put new page in the blank
					get<0>(buffer_instance->bufferpool_wsr[pos]) = pageId;
					// add read_io
					buffer_instance->read_io += 1;

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
						// add disk write functionality
					}
					// erase the target page
					buffer_instance->bufferpool[replacement_pos].first = -1;
					buffer_instance->bufferpool[replacement_pos].second = false;

					// put new page in the blank
					buffer_instance->bufferpool[replacement_pos].first = pageId;
					// add read_io
					buffer_instance->read_io += 1;
				}
				// need to add disk read when we eventually add that functionality
			}
			break;
		}
    }

	/*
    std::cout << "READ LRU List ";
    for (deque<int>::iterator it = buffer_instance->lru_candidate.begin();
            it != buffer_instance->lru_candidate.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << endl;
	*/
	std::cout << "Read FIFO List " << pageId << " ";
    for (deque<int>::iterator it = buffer_instance->fifo_candidates.begin();
		it != buffer_instance->fifo_candidates.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << endl;
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
				}
				// otherwise, the cache is full
				else {
					// find the position to evict
					int pos = buffer_instance->LRU();
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[pos].second == true) {
						buffer_instance->write_io += 1;
					}
					// erase the target page
					buffer_instance->bufferpool[pos].first = -1;
					buffer_instance->bufferpool[pos].second = false;
					// add read_io
					buffer_instance->read_io += 1;
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
				}
				// otherwise, the cache is full
				else {
					// find the position to evict
					int pos = buffer_instance->LRUWSR();
					// if the page is dirty, write the page into the disk
					if(get<1>(buffer_instance->bufferpool_wsr[pos]) == true) {
						buffer_instance->write_io += 1;
					}
					// erase the target page in the bufferpool
					get<0>(buffer_instance->bufferpool_wsr[pos]) = -1;
					// put new page in the blank, and then set that page to be dirty
					get<0>(buffer_instance->bufferpool_wsr[pos]) = pageId;
					get<1>(buffer_instance->bufferpool_wsr[pos]) = true;
					get<2>(buffer_instance->bufferpool_wsr[pos]) = false;
					// add read_io
					buffer_instance->read_io += 1;
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
					// add disk read
				}
				// full, time to replace
				else {
					// get position to replace (pops in FIFO function)
					int replacement_pos =  buffer_instance->FIFO();
					buffer_instance->fifo_candidates.push_back(pageId);
					// if the page is dirty, write the page into the disk
					if(buffer_instance->bufferpool[replacement_pos].second == true) {
						buffer_instance->write_io += 1;
						// add disk write functionality
					}
					// erase the target page put new page in the blank
					buffer_instance->bufferpool[replacement_pos].second = true;
					buffer_instance->bufferpool[replacement_pos].first = pageId;
					// add read_io
					buffer_instance->read_io += 1;
				}
			}
			break;
		}
    }
    //  Implement Write in the Bufferpool
	/*
    std::cout << "WRITE LRU List ";
    for (deque<int>::iterator it = buffer_instance->lru_candidate.begin();
		it != buffer_instance->lru_candidate.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << endl;
	*/
	std::cout << "WRITE FIFO List " << pageId << " ";
    for (deque<int>::iterator it = buffer_instance->fifo_candidates.begin();
		it != buffer_instance->fifo_candidates.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << endl;
    return -1;
}

// Need to calculate disk sector and page size to accomplish proper functionality
// Perform disk read or write
void WorkloadExecutor::diskOp(
	Buffer* buffer_instance, 
	int operation, 
	int pageID, 
	vector<vector<int>> sectorsPages
	) {
	
	// open file
	
	// read
	if(operation == 0) {
		buffer_instance->read_io += 1;
	}
	// write
	else if(operation == 1) {
		buffer_instance->write_io += 1;
		// generate arbitrary bytes
		std::generate(begin(rand_data), end(rand_data), std::ref(rbe));

	}

	// close file
}


/*
// is not applicable in this program setting, ignore
int WorkloadExecutor::unpin(Buffer* buffer_instance, int pageId)
{
This is optional
return -1;
}
*/

// return the evict position in bufferpool
int Buffer::LRU() {
    // get the least used page id
    int index = -1;
    // cout << lru_candidate.size() << endl;
    if(lru_candidate.size() > 0) {
        int pageId = lru_candidate.back();
        // delete it from the LRU list
        lru_candidate.pop_back();
        /*
		the below fails b/c it tries do bool == int in the include due to pair
        vector< pair<int, bool> >::iterator i = find(bufferpool.begin(), bufferpool.end(), pageId);
        find it's position in the buffer pool
        int index = distance(bufferpool.begin(), i);
        return index;
		*/
        for(int i = 0; i < bufferpool.size(); i++) {
            if(pageId == bufferpool[i].first) {
                index = i;
                break;
            }
        }
    }
    std::cout << "currently using LRU" << endl;
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
    std::cout << "currently using LRU-WSR" << endl;
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
    int algo = _env->algorithm; // a

	string u = "_";
	string filename = "b-" + buffer_size_in_pages + u + "n-" + disk_size_in_pages + u + "x-" 
		+ num_operations + u + "e-" + perct_reads + perct_writes + u + "s-" + skewed_perct
		+ "d-" + skewed_data_perct + u + "v-" + verbosity + u + "a-" + algorithms.at(algo);

	ofstream statFile;
	statFile.open("workload_suite/runs/" + filename + ".txt");
	statFile << buffer_hit << endl;
    statFile << buffer_miss << endl;
    statFile << read_io << endl;
    statFile << write_io << endl;
    statFile << Buffer::timing.count(); 
	statFile.close();

	return;
}