
#include "parameter.h"
#include <deque>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <queue>
#include <string>
#include <time.h>
#include <chrono>

using namespace std;

namespace bufmanager {
	class Buffer {
		// This class maintains specific property of the buffer.
		// You definitely need to modify this part
		// You need to add more variables here for your implementation. For example, currently the bufferpool itself is missing

		private:
			Buffer(Simulation_Environment* _env);
			static Buffer* buffer_instance;

		public:
			//initizate bufferpool with <page size, dirty page bit>
			vector< pair<int, bool> > bufferpool;
			//initizate bufferpool for LRU WSR with <page size, dirty page bit, cold flag>
			vector< tuple<int, bool, bool> > bufferpool_wsr;
			//deck as the lru candidate list
			deque<int> lru_candidate;
			//for cflru: going to store either pageid of clean or -1
			deque<int> fifo_candidates;
			static long max_buffer_size;  //in pages

			static Buffer* getBufferInstance(Simulation_Environment* _env);
			// statistics to track
			static int buffer_hit;
			static int buffer_miss;
			static int read_io;
			static int write_io;
			static char disk_write_char;
			static std::chrono::duration <double, milli> timing;

			// Page replacement algorithm
			int LRU();
			int LRUWSR();
			int FIFO();
			int CFLRU();

			// Disk parameters
			std::fstream disk;
			static int pageSize;

			// Statistics collection
			static void writeResults();
			static int printBuffer();
			static int printStats();
	};

	class WorkloadExecutor {
		public:
			static int read(Buffer* buffer_instance, int pageId, int algorithm);
			static int write(Buffer* buffer_instance, int pageId, int algorithm);
			static int search(Buffer* buffer_instance, int pageId, int algorithm);
			// Disk management functions
			static void diskOp(Buffer* buffer_instance, int operation, int pageID);
			static void writeDisk(Buffer* buffer_instance);
			static int unpin(Buffer* buffer_instance, int pageId);
	};
}
