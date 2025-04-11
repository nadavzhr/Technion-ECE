#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstdint>


using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

#define UNUSED -1
#define ADDRSIZE 32
#define LRU 0


class cacheLine{
	public:
		bool is_valid;
		bool is_dirty;
		unsigned tag;
		unsigned long int original_addr;
		// unsigned* block;

		// Constructor
		cacheLine(){
			is_valid = false;
			is_dirty = false;
			tag = 0;
			original_addr = 0;
			// this->block = new unsigned[block_size/4];
		}
};

class cacheSet{
	public:
		cacheLine** ways;
		int* lru;
		int num_ways_used;
		int num_ways;

		cacheSet(unsigned num_ways){
			this->ways = new cacheLine*[num_ways];
			this->lru = new int[num_ways];
			for(unsigned i=0; i<num_ways; i++){
				this->ways[i] = new cacheLine();
				this->lru[i] = UNUSED;
			}
			this->num_ways_used = 0;
			this->num_ways = num_ways;
		}
		cacheSet(){
			this->ways = NULL;
			this->lru = NULL;
			this->num_ways_used = 0;
			this->num_ways =0;
		}

};

unsigned getSet(unsigned long int addr,unsigned set_bits,unsigned offset_bits);
unsigned getTag(unsigned long int addr,unsigned tag_bits);
void cache_read(cacheSet* L1, cacheSet* L2,unsigned L1Assoc,unsigned L2Assoc, unsigned long int addr);
void cache_write(cacheSet* L1, cacheSet* L2,unsigned WrAlloc, unsigned L1Assoc,unsigned L2Assoc,unsigned long int addr);
void insert_cacheline_to_L1(cacheSet* cache_set, cacheLine* curr, cacheSet* l2_cache);
void insert_cacheline_to_L2(cacheSet* cache_set, cacheLine* curr, cacheSet* l1_cache);
void update_LRU(cacheSet* cache_set, int recently_used);
void find_and_delete(cacheSet* l1_cache, unsigned long int addr);
void find_and_update(cacheSet* l2_cache, unsigned long int addr);



double L1_acc = 0;
double L1_miss = 0;
double L2_miss = 0;

unsigned L1_set;
unsigned L2_set;
unsigned L1_tag;
unsigned L2_tag;

unsigned offset_bits;
unsigned L1_set_bits;
unsigned L2_set_bits;
unsigned L1_tag_bits;
unsigned L2_tag_bits;

int main(int argc, char **argv) {

	if (argc < 19) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// Get input arguments

	// File
	// Assuming it is the first argument
	char* fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}

	// Create L1 and L2 cache's
	// cout << "L1 = " << L1Size << " Bsize = " << BSize << " L1ASSOC= "<< L1Assoc << endl;

	int L1_sets = static_cast<int>(pow(2,L1Size-BSize-L1Assoc)); 
	int L2_sets = static_cast<int>(pow(2,L2Size-BSize-L2Assoc));

	cacheSet* L1 = new cacheSet[L1_sets];
	cacheSet* L2 = new cacheSet[L2_sets];
	unsigned L1assoc_num = static_cast<unsigned>(pow(2,L1Assoc));
	unsigned L2assoc_num = static_cast<unsigned>(pow(2,L2Assoc));

	for(int i=0; i<L1_sets; i++){
		L1[i] = cacheSet(L1assoc_num);
	}
	for(int i=0; i<L2_sets; i++){
		L2[i] = cacheSet(L2assoc_num);
	}
	offset_bits = BSize;
	L1_set_bits = static_cast<unsigned>(log2(L1_sets));
	L2_set_bits = static_cast<unsigned>(log2(L2_sets));
	L1_tag_bits = ADDRSIZE - L1_set_bits - offset_bits;
	L2_tag_bits = ADDRSIZE - L2_set_bits - offset_bits;


	while (getline(file, line)) {

		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

		// DEBUG - remove this line
		// cout << "operation: " << operation;

		string cutAddress = address.substr(2); // Removing the "0x" part of the address

		// DEBUG - remove this line
		// cout << ", address (hex)" << cutAddress;

		unsigned long int addr = 0;
		addr = strtoul(cutAddress.c_str(), NULL, 16);

		// DEBUG - remove this line
		// cout << " addr(dec) " << addr << endl;

		L1_set = getSet(addr,L1_set_bits,offset_bits);
		L2_set = getSet(addr,L2_set_bits,offset_bits);
		L1_tag = getTag(addr,L1_tag_bits);
		L2_tag = getTag(addr,L2_tag_bits);
		
		if(operation == 'r'){
			cache_read(L1,L2,L1assoc_num,L2assoc_num,addr);
		}else{
			cache_write(L1,L2,WrAlloc,L1assoc_num,L2assoc_num,addr);
		}


	}
	// cout << "L1 MISS = " << L1_miss << endl;
	// cout << "L2 MISS = " << L2_miss << endl;
	// cout << "L1 ACC = " << L1_acc << endl;



	double tot_L1_time = L1_acc*L1Cyc;
	double tot_L2_time = L1_miss*L2Cyc;
	double tot_mem_time = L2_miss*MemCyc;
	

	double L1MissRate = L1_miss/L1_acc;
	double L2MissRate = L2_miss/L1_miss;
	double avgAccTime = (tot_L1_time+tot_L2_time+tot_mem_time)/L1_acc;
	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
}

unsigned getSet(unsigned long int addr,unsigned set_bits,unsigned offset_bits){
	unsigned mask = (1 << set_bits) - 1;
	unsigned res = (addr >> offset_bits) & mask;
	return res;

}
unsigned getTag(unsigned long int addr,unsigned tag_bits){
	unsigned mask = (1 << tag_bits) - 1;
	unsigned res = (addr >> (ADDRSIZE - tag_bits)) & mask;
	return res;
}


void cache_read(cacheSet* L1, cacheSet* L2,unsigned L1Assoc,unsigned L2Assoc, unsigned long int addr){
	L1_acc++;
	for(unsigned i=0; i<L1Assoc; i++){
		
		cacheLine* curr1 = L1[L1_set].ways[i];
		// L1 read HIT
		if(curr1->tag == L1_tag && curr1->is_valid){
			update_LRU(&L1[L1_set], i);
			return;
		}
	}
	// L1 MISS, search in L2
	// cout << "L1 MISS, TAG IS: " << L1_tag << endl;
	L1_miss++;
	cacheLine* curr2;
	for(unsigned i=0; i<L2Assoc; i++){
		curr2 = L2[L2_set].ways[i];
		// L2 HIT
		if(curr2->tag == L2_tag && curr2->is_valid){
			update_LRU(&L2[L2_set], i);
			cacheLine* new_cache1 = new cacheLine();
			new_cache1->tag = L1_tag;
			new_cache1->is_valid = true;
			new_cache1->original_addr = addr;
			insert_cacheline_to_L1(&L1[L1_set],new_cache1,L2);
			return;
		}
	}
	// cout << "L2 MISS, TAG IS: " << L2_tag << endl;

	// L2 MISS, go to mem
	cacheLine* new_cache1 = new cacheLine();
	cacheLine* new_cache2 = new cacheLine();
	new_cache1->tag = L1_tag;
	new_cache1->is_valid = true;
	new_cache1->original_addr = addr;
	new_cache2->tag = L2_tag;
	new_cache2->is_valid = true;
	new_cache2->original_addr = addr;
	insert_cacheline_to_L1(&L1[L1_set],new_cache1,L2);
	insert_cacheline_to_L2(&L2[L2_set],new_cache2,L1);
	L2_miss++;
}

void cache_write(cacheSet* L1, cacheSet* L2,unsigned WrAlloc, unsigned L1Assoc,unsigned L2Assoc, unsigned long int addr){
	L1_acc++;
	cacheLine* curr1;
	for(unsigned i=0; i<L1Assoc; i++){
		curr1 = L1[L1_set].ways[i];
		// L1 write HIT
		if(curr1->tag == L1_tag && curr1->is_valid){
			curr1->is_dirty = true;
			update_LRU(&L1[L1_set], i);
			return;
		}
	}
	// cout << "L1 write MISS, TAG IS: " << L1_tag << endl;

	//L1 write miss
	L1_miss++;
	cacheLine* curr2;
	for(unsigned i=0; i<L2Assoc; i++){
		curr2 = L2[L2_set].ways[i];
		// L2 HIT
		if(curr2->tag == L2_tag && curr2->is_valid){
			curr2->is_dirty = true;
			update_LRU(&L2[L2_set], i);
			if(WrAlloc == 1){
				cacheLine* new_cache1 = new cacheLine();
				new_cache1->tag = L1_tag;
				new_cache1->is_valid = true;
				new_cache1->original_addr = addr;
				new_cache1->is_dirty = true;
				insert_cacheline_to_L1(&L1[L1_set],new_cache1,L2);
			}
			return;
		}
	}
	// cout << "L2 write MISS, TAG IS: " << L2_tag << endl;

	L2_miss++;
	if(WrAlloc == 1){
		cacheLine* new_cache1 = new cacheLine();
		cacheLine* new_cache2 = new cacheLine();
		new_cache1->is_dirty = true;
		new_cache1->tag = L1_tag;
		new_cache1->is_valid = true;
		new_cache1->original_addr = addr;
		new_cache2->tag = L2_tag;
		new_cache2->is_valid = true;
		new_cache2->original_addr = addr;
		insert_cacheline_to_L1(&L1[L1_set],new_cache1,L2);
		insert_cacheline_to_L2(&L2[L2_set],new_cache2,L1);
	}
}

void update_LRU(cacheSet* cache_set, int recently_used){
	int k = cache_set->num_ways;
	// Not all ways are full
	if(cache_set->num_ways_used != k){
		int prev = cache_set->lru[recently_used];
		cache_set->lru[recently_used] = k-1 ;
		// cout << "NOT ALL WAYS ARE FULL, L1 TAG IS: " << L1_tag << " L2 TAG: " << L2_tag << " L1 SET: " << L1_set << " L2 SET "<< L2_set<< endl;
		for(int i=0; i<cache_set->num_ways; i++){
			if(!(cache_set->lru[i] == UNUSED) && i != recently_used && cache_set->lru[i] > prev){
				cache_set->lru[i]--;
			}
		}
		for(int i=0; i<cache_set->num_ways; i++){
			// cout << "way " << i << "lru is:" << cache_set->lru[i] <<endl;
		} 

		// cache_set->num_ways_used++;
	// All ways are full
	}else{
		// cout << "all ways are full!!!!!, L1 TAG IS: " << L1_tag << " L2 TAG: " << L2_tag << endl;
		int prev_val = cache_set->lru[recently_used];
		cache_set->lru[recently_used] = k-1;
		for(int i=0; i<k; i++){
			if(i!=recently_used && (cache_set->lru[i] > prev_val)){
				cache_set->lru[i]--;
			}
		}
		for(int i=0; i<cache_set->num_ways; i++){
			// cout << "way " << i << "lru is:" << cache_set->lru[i] <<endl;
		}
	}
}

void insert_cacheline_to_L1(cacheSet* cache_set, cacheLine* curr, cacheSet* l2_cache){
	// All ways are full
	if(cache_set->num_ways_used == cache_set->num_ways){
		// find the LRU way to evict
		for(int i=0; i<cache_set->num_ways; i++){
			if(cache_set->lru[i] == LRU){
				if(cache_set->ways[i]->is_dirty){ // update L2 if evict a dirty cachline from L1
					unsigned long int addr = cache_set->ways[i]->original_addr;
					find_and_update(l2_cache,addr);
				}
				curr->is_valid = true;
				cache_set->ways[i] = curr;
				update_LRU(cache_set,i);
				break;
			}
		}
	}else{ // Not all ways are full
		for(int i=0; i<cache_set->num_ways; i++){
			if(cache_set->lru[i] == UNUSED){
				curr->is_valid = true;
				cache_set->ways[i] = curr;
				update_LRU(cache_set,i);
				cache_set->num_ways_used++;
				break;
			}
		}

	}
}

void insert_cacheline_to_L2(cacheSet* cache_set, cacheLine* curr, cacheSet* l1_cache){
	// All ways are full
	if(cache_set->num_ways_used == cache_set->num_ways){
		// find the LRU way to evict
		for(int i=0; i<cache_set->num_ways; i++){
			if(cache_set->lru[i] == LRU ){
				// if(is_L1 && cache_set->ways[i]->is_dirty){ // update L2 if evict a dirty cachline from L1
				// 	update_LRU(L2_set,i);
				// }
				cacheLine* prev = cache_set->ways[i];
				find_and_delete(l1_cache,prev->original_addr);
				curr->is_valid = true;
				cache_set->ways[i] = curr;
				update_LRU(cache_set,i);
				break;
			}
		}
	}else{ // Not all ways are full
		for(int i=0; i<cache_set->num_ways; i++){
			if(cache_set->lru[i] == UNUSED){
				curr->is_valid = true;
				cache_set->ways[i] = curr;
				update_LRU(cache_set,i);
				cache_set->num_ways_used++;
				break;
			}
		}

	}
}
void find_and_delete(cacheSet* l1_cache, unsigned long int addr){
	unsigned int tag = getTag(addr,L1_tag_bits);
	unsigned int set = getSet(addr,L1_set_bits,offset_bits);
	// cout << "--------------------IN FIND AND DELETE---------------------------------" << endl;
	for(int i = 0; i < l1_cache->num_ways; i++){
		// found in L1 - delete
		if(l1_cache[set].ways[i]->tag == tag){
			l1_cache[set].ways[i]->is_valid = false;
			int prev_lru = l1_cache[set].lru[i];
			// update lru array
			for(int j=0; j<l1_cache->num_ways; j++){
				if(l1_cache->lru[j]<prev_lru && j!=i && l1_cache->lru[j] != UNUSED){
					l1_cache->lru[j]++;
				}
			}
			l1_cache[set].lru[i] = UNUSED;
			l1_cache[set].num_ways_used--;
			// cout << "way " << i << " = " << l1_cache[set].lru[i];
			break;
		}
		// cout << endl;
	}
}

void find_and_update(cacheSet* l2_cache, unsigned long int addr){
	unsigned int tag = getTag(addr,L2_tag_bits);
	unsigned int set = getSet(addr,L2_set_bits,offset_bits);

	for(int i = 0; i < l2_cache->num_ways; i++){
		// found in L2 - update
		if(l2_cache[set].ways[i]->tag == tag){
			update_LRU(&l2_cache[set],i);
			break;
		}
	}
}