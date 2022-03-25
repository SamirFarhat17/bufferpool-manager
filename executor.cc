#include <iostream>
#include <cmath>
#include <sys/time.h>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include "parameter.h"
#include "executor.h"

using namespace std;
using namespace bufmanager;

Buffer *Buffer::buffer_instance;
long Buffer::max_buffer_size = 0;
int Buffer::buffer_hit = 0;
int Buffer::buffer_miss = 0;
int Buffer::read_io = 0;
int Buffer::write_io = 0;

Buffer::Buffer(Simulation_Environment *_env)
{
  //candidate is for LRU list 
  candidate.clear();
  //initialize the cache
  for(int i = 0; i < max_buffer_size; i++){
    bufferpool.push_back(make_pair(-1, false));
  }
}

Buffer *Buffer::getBufferInstance(Simulation_Environment *_env)
{
  if (buffer_instance == 0)
    buffer_instance = new Buffer(_env);
  return buffer_instance;
}
//search in the bufferpool
int WorkloadExecutor::search(Buffer* buffer_instance, int pageId)
{ //simply perform linear search
  for(int i = 0; i < buffer_instance->max_buffer_size; i++){
    //find the page in the bufferpool, hit
    if(buffer_instance->bufferpool[i].first == pageId){
      buffer_instance->buffer_hit += 1;
      return i;
    }
  }
  //did not find the page, miss
  buffer_instance->buffer_miss += 1;
  return -1;
}

//allocate empty frame and read the page into that frame
int WorkloadExecutor::read(Buffer* buffer_instance, int pageId, int algorithm)
{ int cur_size = buffer_instance->bufferpool.size();
  int capacity = buffer_instance->max_buffer_size;
  int pos = search(buffer_instance, pageId);
  
  if(pos != -1){
    //found, only need to update lru
    deque<int>::iterator it=buffer_instance->candidate.begin();
    while(*it != pageId) it++;
    buffer_instance->candidate.erase(it);
    buffer_instance->candidate.push_front(pageId);
  }
  //miss
  else{
    //cache is not full
    if(cur_size < capacity){
      //read the page from disk, mark the page clean
      buffer_instance->bufferpool.push_back(make_pair(pageId, false));
      //add read_io
      buffer_instance->read_io += 1;

    }
    //cache is full
    else{
      //find the position to evict
      int pos = buffer_instance->LRU();
      //if the page is dirty, write the page into the disk
      if(buffer_instance->bufferpool[pos].second = true){
        buffer_instance->write_io += 1;
      }
      //erase the target page
      buffer_instance->bufferpool[pos].first = -1;
      buffer_instance->bufferpool[pos].second = false;

      //put new page in the blank
      buffer_instance->bufferpool[pos].first = pageId;
      //add read_io
      buffer_instance->read_io += 1;
      
    }
    //update lru
    buffer_instance->candidate.push_front(pageId);

  }

  return -1;
}


int WorkloadExecutor::write(Buffer* buffer_instance, int pageId, int algorithm)
{
  // Implement Write in the Bufferpool
  int cur_size = buffer_instance->bufferpool.size();
  int capacity = buffer_instance->max_buffer_size;
  int pos = search(buffer_instance, pageId);
  //if page is already in the bufferpool, update it
  if(pos != -1){
    //found, only need to update lru
    deque<int>::iterator it=buffer_instance->candidate.begin();
    while(*it != pageId) it++;
    buffer_instance->candidate.erase(it);
    buffer_instance->candidate.push_front(pageId);
    //marking the page as dirty bc it's been updated
    buffer_instance->bufferpool.push_back(make_pair(pageId, true));

  }
  //otherwise this is a miss
  else{
    //if bufferpool isnt empty, simply add the page
    if(cur_size < capacity){
      //read the page from disk, mark the page dirty since we are updatin git
      buffer_instance->bufferpool.push_back(make_pair(pageId, true));
      buffer_instance->read_io += 1;
    }
    //otherwise, the cache is full
    else{
      //find the position to evict
      int pos = buffer_instance->LRU();
      //if the page is dirty, write the page into the disk
      if(buffer_instance->bufferpool[pos].second = true){
        buffer_instance->write_io += 1;
      }
      //erase the target page
      buffer_instance->bufferpool[pos].first = -1;
      buffer_instance->bufferpool[pos].second = false;

      //put new page in the blank, and then set that page to be dirty
      buffer_instance->bufferpool[pos].first = pageId;
      buffer_instance->bufferpool[pos].second = true;
      //add read_io
      buffer_instance->read_io += 1;

    }
  }



  return 1;
}
//is not applicable in this program setting, ignore
int WorkloadExecutor::unpin(Buffer* buffer_instance, int pageId)
{
  // This is optional
  return -1;
}

// void Buffer::update_lru(deque< pair<int, int> > candidate, int pageId, int pos, bool hit){
//   if(hit == true){
//     deque< pair<int, int> >::iterator it=candidate.begin();
//     while(it->first != pageId){
//             it++;
//         }
//     candidate.erase(it);
//     candidate.push_front(make_pair(pageId, pos));
//   }
//   else{
//     if(buffer_instance->bufferpool.size() < buffer_instance->max_buffer_size){
//       deque<int>::iterator it=cache.begin();
//       while(*it!=key)it++;
//       cache.erase(it);
//       map.erase(key);
//     }

//   }
// }

//return the evict position in bufferpool 
int Buffer::LRU()
{ //get the least used page id 
  int pageId = candidate.back();
  //delete it from the LRU list
  candidate.pop_back();
  // the below fails b/c it tries do bool == int in the include due to pair
  //vector< pair<int, bool> >::iterator i = find(bufferpool.begin(), bufferpool.end(), pageId);
  //find it's position in the buffer pool
  //int index = distance(bufferpool.begin(), i);
  //return index;
  return 0;
}

int Buffer::LRUWSR()
{
  // Implement LRUWSR
  
  return -1;
}

int Buffer::printBuffer()
{
  return -1;
}

int Buffer::printStats()
{
  Simulation_Environment* _env = Simulation_Environment::getInstance();
  cout << "******************************************************" << endl;
  cout << "Printing Stats..." << endl;
  cout << "Number of operations: " << _env->num_operations << endl;
  cout << "Buffer Hit: " << buffer_hit << endl;
  cout << "Buffer Miss: " << buffer_miss << endl;
  cout << "Read IO: " << read_io << endl;
  cout << "Write IO: " << write_io << endl;  
  cout << "Global Clock: " << endl;
  cout << "******************************************************" << endl;
  return 0;
}