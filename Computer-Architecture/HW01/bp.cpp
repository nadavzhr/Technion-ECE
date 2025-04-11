#include "bp_api.h"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <algorithm>



#define TAG 0
#define TARGET 1
#define VALID_BIT 2
#define VALID_BIT_SIZE 1
#define TARGET_SIZE 30

enum SHARE_TYPE {not_using_share = 0, using_share_lsb = 1, using_share_mid = 2};
enum FSM_STEP {SNT = 0, WNT = 1, WT = 2, ST = 3};
using namespace std;



// Used to get the line in the BTB tags and targets table, in which the PC will be inserted to
uint32_t getLine(uint32_t number, unsigned btb_size);
// Used to get the tag of a PC
uint32_t getTag(uint32_t pc, unsigned btb_size, unsigned tag_size);
// Used to get the history of a given branch found in "line" in the BTB table
uint32_t getHistory(uint32_t line, bool is_global_hist);
// Given a pc, it finds the correct FSM for it according to its history
uint32_t getFSM_index(uint32_t pc, bool is_global_hist, bool is_global_table, uint32_t shared_mode);
// Given an array and size hist, this function sets the fsm's to the default fsm_state
void setToDefault(uint32_t array[], uint32_t hist_size);
// Update the state of a specific fsm, given a pointer to the state
void updateFsm(uint32_t *fsm_state,bool taken);
// Calculate the theoretical size of the btb table
int getBTBsize(unsigned btbSize, unsigned historySize, unsigned tagSize,
				  bool isGlobalHist, bool isGlobalTable);



class BTB {
public:

    unsigned btb_size; // amount of possible entries: 1,2,4,6,8,16,32
    unsigned hist_size; // number of history bits: range(1,8)
    unsigned tag_size; // number of bits in tag field 
    unsigned fsm_state; // initialized fsm state
    bool is_global_hist; // local or global bhr
    bool is_global_table; // local or global fsm state table 
    int shared; // Lshare or Gshare (relevant only when fsm table is global)


	
	
	uint32_t** tags_targets_table; // // 2-D Table that contains the tags and targets of each branch seen. 
	uint32_t* hist_table; // *int if global or 1-D array if local
	uint32_t** fsm_table; // if global: array of size 2^h. if local: 2-D matrix of size 2^h * btb_size
	// Constructor
	BTB(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
				bool isGlobalHist, bool isGlobalTable, int Shared){

		this->btb_size = btbSize;
		this->hist_size = historySize;
		this->tag_size = tagSize;
		this->fsm_state = fsmState;
		this->is_global_hist = isGlobalHist;
		this->is_global_table = isGlobalTable;
		this->shared = Shared;

		// Build tags_targets_table
		uint32_t** matrix = new uint32_t*[btbSize];
		for (unsigned i = 0; i < btbSize; i++) {
        	matrix[i] = new uint32_t[3];
			matrix[i][TAG] = 0;
			matrix[i][TARGET] = 0;
			matrix[i][VALID_BIT] = 0;
    	}
		this->tags_targets_table = matrix;


		/*** Build History Table ***/
		if (isGlobalHist){
			this->hist_table = new uint32_t;
			*(this->hist_table) = 0;
		}
		// if localHist
		else {
			this->hist_table = new uint32_t[btbSize];
			for (unsigned i = 0; i < btbSize; i++){
				this->hist_table[i] = 0;
			}
		}
		
		/*** Build FSM Table ***/
		int FSM_num = static_cast<int>(std::pow(2,historySize));
		if(isGlobalTable){
			this->fsm_table = new uint32_t*[FSM_num];
			for (int i = 0; i < FSM_num; i++) {
				this->fsm_table[i] = new uint32_t;
				*(this->fsm_table[i]) = fsmState;
			}
		}
		// localTable
		else {
			this->fsm_table = new uint32_t*[btbSize];
			for (unsigned i = 0; i < btbSize; i++ ){
				this->fsm_table[i] = new uint32_t[FSM_num];
				for (int j = 0; j < FSM_num; j++){
					this->fsm_table[i][j] = fsmState;
				}
			}
		}

		
	}
};

//Global variables
BTB* btb;
SIM_stats stats = {0, 0, 0};

int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Shared){
		// Check arguments
		if (!((btbSize >= 1) && (btbSize <= 32) && ((btbSize & (btbSize -1 )) == 0)) ){
			return -1;
		}

		if (historySize < 1 || historySize > 8) {
			return -1;
		}

		if (tagSize < 0 || tagSize > (30 - std::log2(btbSize))){
			return -1;
		}

		if (fsmState < 0 || fsmState > 3){
			return -1;
		}
		// Create the class object
		btb = new BTB( btbSize,  historySize,  tagSize,  fsmState,
			 isGlobalHist,  isGlobalTable,  Shared);

		return 0;

}

bool BP_predict(uint32_t pc, uint32_t *dst){
	uint32_t line = getLine(pc, btb->btb_size);
	uint32_t tag = getTag(pc, btb->btb_size, btb->tag_size);

	int predict = 0;

	// branch table HIT
	if (btb->tags_targets_table[line][VALID_BIT] == 1 && btb->tags_targets_table[line][TAG] == tag){
		
		uint32_t fsm_index = getFSM_index( pc, btb->is_global_hist, btb->is_global_table, btb->shared);
		if(btb->is_global_table){
			predict = *(btb->fsm_table[fsm_index]);
		}else{
			predict = btb->fsm_table[line][fsm_index];
		}
		// predict that the branch will be taken
		if(predict > WNT){
			*dst = btb->tags_targets_table[line][TARGET];
			return true;
		// predict that the branch will NOT be taken
		}else{
			*dst = pc+4;
			return false;
		}		
	}
	else{
		*dst = pc+4;
		return false;
	}
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	stats.br_num++;
	uint32_t line = getLine(pc, btb->btb_size);
	uint32_t tag = getTag(pc, btb->btb_size, btb->tag_size);
	uint32_t fsm_index = getFSM_index(pc, btb->is_global_hist, btb->is_global_table, btb->shared);
	// used for updating the history register
	uint32_t mask = (1 << btb->hist_size) - 1;

	// determine if the prediction was correct, if not - flush
	uint32_t real_dest = taken ? targetPc : pc + 4;
	if (real_dest != pred_dst){
			stats.flush_num++;
	}
	// Check for a btb HIT
	if (btb->tags_targets_table[line][TAG] == tag && btb->tags_targets_table[line][VALID_BIT] == 1){
		btb->tags_targets_table[line][TARGET] = targetPc;
		if(btb->is_global_table){
			updateFsm(btb->fsm_table[fsm_index], taken);
		}else{
			updateFsm(&(btb->fsm_table[line][fsm_index]), taken);
		}
		if(btb->is_global_hist){
			*(btb->hist_table) = ((*(btb->hist_table) << 1) + uint32_t(taken)) & mask;
		}else{
			btb->hist_table[line] = ((btb->hist_table[line] << 1) + uint32_t(taken)) & mask;
		}

	//btb MISS, and need to update the new branch
	}else{
		btb->tags_targets_table[line][TAG] = tag;
		btb->tags_targets_table[line][TARGET] = targetPc;
		btb->tags_targets_table[line][VALID_BIT] = 1;
		

		// update local history first in case of local histories and new branch
		if (!(btb->is_global_hist)){
			btb->hist_table[line] = 0 ; // Set history to default
			// need to calc fsm_index again in case of a new branch and local histories
		    fsm_index = getFSM_index(pc, btb->is_global_hist, btb->is_global_table, btb->shared);
		}


		if(btb->is_global_table){
			updateFsm(btb->fsm_table[fsm_index], taken);
		}else{
			//if got to here, we have local fsm's
			setToDefault(btb->fsm_table[line], btb->hist_size);
			updateFsm(&(btb->fsm_table[line][fsm_index]), taken);
		}

		// update global history in case of global histories - after updating fsm
		if(btb->is_global_hist){
			*(btb->hist_table) = ((*(btb->hist_table) << 1) + uint32_t(taken)) & mask;
		}
		// update local history
		else {
			btb->hist_table[line] = uint32_t(taken);
		}
	}
}

void BP_GetStats(SIM_stats *curStats){
	curStats->br_num = stats.br_num;
	curStats->flush_num = stats.flush_num;
	curStats->size = getBTBsize(btb->btb_size, btb->hist_size, btb->tag_size, btb->is_global_hist, btb->is_global_table);
	int FSM_num = static_cast<int>(std::pow(2,btb->hist_size));
	// Delete Tags and Targets table
	for (unsigned i = 0; i < btb->btb_size; i++) {
		delete[] btb->tags_targets_table[i];
	}
	delete[] btb->tags_targets_table;

	// Delete Histories
	if (btb->is_global_hist) {
		delete btb->hist_table;
	}
	else {
		delete[] btb->hist_table;
	}

	// Delete FSM Table
	if (btb->is_global_table) {
		for (int i = 0; i < FSM_num; i++) {
			delete btb->fsm_table[i];
		}
	}
	else {
		for (unsigned i = 0; i < btb->btb_size; i++) {
			delete[] btb->fsm_table[i];
		}
	}
	delete[] btb->fsm_table;
	delete btb;
}

uint32_t getLine(uint32_t pc, unsigned btb_size) {

    // Create a bitmask
    uint32_t bitmask = btb_size - 1;
	
    // Shift the bits to the right by 2 positions to extract [2:2+log2(btbSize)] bits
    uint32_t result = (pc >> 2) & bitmask;
    return result;
}


uint32_t getTag(uint32_t pc, unsigned btb_size, unsigned tag_size) {

	// used to skip bits that represnt the line
    uint32_t line_Bits = static_cast<uint32_t>(std::log2(btb_size));
 	// Create a bitmask
    uint32_t bitmask = (1 << tag_size) - 1;

	// extract tag
    uint32_t result = (pc >> (2 + line_Bits)) & bitmask;

    return result;
}

// only called when tag's match.
uint32_t getFSM_index(uint32_t pc, bool is_global_hist, bool is_global_table, uint32_t shared_mode) {

	uint32_t line = getLine(pc,btb->btb_size);
	uint32_t hist = getHistory(line, is_global_hist);

	if(is_global_table){
		if(shared_mode == not_using_share){
			return hist; 
		}
		else if(shared_mode == using_share_lsb){
			uint32_t mask = (1 << btb->hist_size) - 1; 
			uint32_t trimmed_pc = (pc >> 2) & mask;
			return trimmed_pc ^ hist ; 
		}
		// using_share_mid
		else {

			uint32_t mask = (1 << btb->hist_size) - 1; 
			uint32_t trimmed_pc = (pc >> 16) & mask;
			return trimmed_pc ^ hist ; 
		}

	}else {
		return hist;
	}
}

uint32_t getHistory(uint32_t line, bool is_global_hist){

	if (is_global_hist){
		return *(btb->hist_table);
	}
	else {
		return btb->hist_table[line];

	}
}

void setToDefault(uint32_t array[], uint32_t hist_size){
	for(unsigned i=0; i < static_cast<uint32_t>(std::pow(2,hist_size)); i++){
		array[i] = btb->fsm_state;
	}
}
void updateFsm(uint32_t *fsm_state,bool taken){
	if(taken){
		if(*fsm_state < ST){
			*fsm_state = *fsm_state +  1;
		}
	}else{
		if(*fsm_state > SNT){
			*fsm_state = *fsm_state - 1;
		}
	}	
}


int getBTBsize(unsigned btbSize, unsigned historySize, unsigned tagSize,
                   bool isGlobalHist, bool isGlobalTable) {
    int64_t total_size = static_cast<int64_t>(btbSize) * (VALID_BIT_SIZE + TARGET_SIZE + tagSize);

    if (isGlobalHist) {
        total_size += static_cast<int64_t>(historySize);
    } else {
        total_size += static_cast<int64_t>(btbSize) * static_cast<int64_t>(historySize);
    }

    if (isGlobalTable) {
        int64_t fsm_size = static_cast<int64_t>(1) << (historySize + 1);
        total_size += fsm_size;
    } else {
        int64_t fsm_size = static_cast<int64_t>(btbSize) * (static_cast<int64_t>(1) << (historySize + 1));
        total_size += fsm_size;
    }

    // Check for overflow
    if (total_size < 0 || total_size > INT64_MAX) {
        // Handle overflow, return an error code or throw an exception
        return -1;
    }

    return static_cast<int>(total_size);
}
