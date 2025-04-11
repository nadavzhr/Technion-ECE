/* 046267 Computer Architecture - HW #4 */

#include "core_api.h"
#include "sim_api.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int b_num_threads;
int f_num_threads;

int b_cyc_count = 0;
int b_inst_count = 0;

int f_cyc_count = 0;
int f_inst_count = 0;

int load_latency;
int store_latency;
int switch_latency;


class thread{
	public:
		int tid;
		Instruction* curr_INST;
		int last_read_cyc;
		int last_write_cyc;
		tcontext reg_context;
		uint32_t curr_line;
		bool is_finished;

		thread(int tid){
			this->tid = tid;
			this->curr_INST = new Instruction;
			this->last_read_cyc = -load_latency-1;
			this->last_write_cyc = -store_latency-1;
			for(int i=0; i<REGS_COUNT; i++){
				this->reg_context.reg[i] = 0;
			}
			this->curr_line = 0;
			this->is_finished = false;
		}

		//Executes the the next command, and updates if the thread is finished
		void operate(int* curr_running_threads, int cyc_count){
			if(this->curr_INST == nullptr){
				cout << "NO INTRUCTION SET" << endl;
			}
			
			int address;
			switch (this->curr_INST->opcode) {
				case CMD_NOP: // NOP
					break;

				case CMD_ADDI:
					this->reg_context.reg[this->curr_INST->dst_index] = this->reg_context.reg[this->curr_INST->src1_index] + this->curr_INST->src2_index_imm;
					break;

				case CMD_SUBI:
					this->reg_context.reg[this->curr_INST->dst_index] = this->reg_context.reg[this->curr_INST->src1_index] - this->curr_INST->src2_index_imm;
					break;

				case CMD_ADD:
					this->reg_context.reg[this->curr_INST->dst_index] = this->reg_context.reg[this->curr_INST->src1_index] + this->reg_context.reg[this->curr_INST->src2_index_imm];
					break;

				case CMD_SUB:
					this->reg_context.reg[this->curr_INST->dst_index] = this->reg_context.reg[this->curr_INST->src1_index] - this->reg_context.reg[this->curr_INST->src2_index_imm];
					break;

				case CMD_LOAD:
					if(this->curr_INST->isSrc2Imm){
						address = this->reg_context.reg[this->curr_INST->src1_index] + this->curr_INST->src2_index_imm;
					}else{
						address = this->reg_context.reg[this->curr_INST->src1_index] + this->reg_context.reg[this->curr_INST->src2_index_imm];
					}
					SIM_MemDataRead(address,&this->reg_context.reg[this->curr_INST->dst_index]);
					this->last_read_cyc = cyc_count;
					break;

				case CMD_STORE:
					if(this->curr_INST->isSrc2Imm){
						address = this->reg_context.reg[this->curr_INST->dst_index] + this->curr_INST->src2_index_imm;
					}else{
						address = this->reg_context.reg[this->curr_INST->dst_index] + this->reg_context.reg[this->curr_INST->src2_index_imm];
					}
					SIM_MemDataWrite(address, this->reg_context.reg[this->curr_INST->src1_index]);
					this->last_write_cyc = cyc_count;
					break;

				case CMD_HALT:
					*curr_running_threads = *curr_running_threads-1;

					this->is_finished = true;
					break;
 	 		}
			this->curr_line++;

		}
};

//Calculate the next thread to be ran, in a Round Robin manner.
thread* RR_calc(vector<thread*>* all_threads, int former_tid, int curr_cycle);
vector<thread*>* b_all_threads = new vector<thread*>();
vector<thread*>* f_all_threads = new vector<thread*>();

void CORE_BlockedMT() {
	b_num_threads = SIM_GetThreadsNum();
	load_latency = SIM_GetLoadLat();
	store_latency = SIM_GetStoreLat();
	switch_latency = SIM_GetSwitchCycles();

	//Initialize the threads vector
	for(int i=0; i<b_num_threads; i++){
		thread* new_thread = new thread(i);
		b_all_threads->push_back(new_thread);
	}


	thread* curr_thread = b_all_threads->front();
	//The sim will end when there will be no more threads to run.
	while(b_num_threads){
		while(!curr_thread->is_finished && ((b_cyc_count - curr_thread->last_read_cyc) >= load_latency) && ((b_cyc_count - curr_thread->last_write_cyc) >= store_latency) ){
			SIM_MemInstRead(curr_thread->curr_line,curr_thread->curr_INST,curr_thread->tid); 
			b_cyc_count++;
			b_inst_count++;
			curr_thread->operate(&b_num_threads,b_cyc_count); 
		}
		if(b_num_threads == 0){
			break;
		}

		int former_tid = curr_thread->tid;
		curr_thread = RR_calc(b_all_threads,former_tid,b_cyc_count);
		if(former_tid != curr_thread->tid){
			// context switch
			b_cyc_count+=switch_latency;
		}else{
			// idle
			b_cyc_count++;
		}
		
	}
	
}

void CORE_FinegrainedMT() {
	int f_num_threads = SIM_GetThreadsNum();
	load_latency = SIM_GetLoadLat();
	store_latency = SIM_GetStoreLat();
	//Initialize the threads vector
	for(int i=0; i<f_num_threads; i++){
		thread* new_thread = new thread(i);
		f_all_threads->push_back(new_thread);
	}

	thread* curr_thread = f_all_threads->front();
	
	//The sim will end when there will be no more threads to run.
	while(f_num_threads){
		if(!curr_thread->is_finished && ((f_cyc_count - curr_thread->last_read_cyc) >= load_latency) && ((f_cyc_count - curr_thread->last_write_cyc) >= store_latency)){
			SIM_MemInstRead(curr_thread->curr_line,curr_thread->curr_INST,curr_thread->tid); 
			f_cyc_count++;
			f_inst_count++;
			curr_thread->operate(&f_num_threads, f_cyc_count); 
		}else{
			f_cyc_count++;
		}
		if(f_num_threads == 0){
			break;
		}
		int former_tid = curr_thread->tid;
		curr_thread = RR_calc(f_all_threads,former_tid,f_cyc_count);
		
	}
}

double CORE_BlockedMT_CPI(){
	double cpi = double(b_cyc_count) / double(b_inst_count);
	for (auto thread_ptr : *b_all_threads) {
		delete thread_ptr;
	}

	b_all_threads->clear();
	delete b_all_threads;

	return cpi;
	}

double CORE_FinegrainedMT_CPI(){
	double cpi = double(f_cyc_count) / double(f_inst_count);
	for (auto thread_ptr : *f_all_threads) {
		delete thread_ptr;
	}

	f_all_threads->clear();
	delete f_all_threads;

	return cpi;
}

void CORE_BlockedMT_CTX(tcontext* context, int threadid) {
	
	context[threadid] = (*b_all_threads)[threadid]->reg_context;
}

void CORE_FinegrainedMT_CTX(tcontext* context, int threadid) {
	context[threadid] = (*f_all_threads)[threadid]->reg_context;
}


thread* RR_calc(vector<thread*>* all_threads, int former_tid, int curr_cycle){
	int num_of_threads = all_threads->size();
	int curr_tid = (former_tid+1)%num_of_threads;
	vector<thread*>::iterator it = all_threads->begin()+curr_tid;

	while((*it)->tid != former_tid){
		if(!(*it)->is_finished && ((curr_cycle - (*it)->last_read_cyc) >= load_latency) && ((curr_cycle - (*it)->last_write_cyc) >= store_latency)){
			return &(**it);
		}
		if(curr_tid == num_of_threads-1){
			it = all_threads->begin();
			curr_tid = 0;
		}else{
			it++;
			curr_tid++;
		}
	}
	return &(**it);
}