#include "pimapi.h"
#include "PIMCmd.h"
#include "SystemConfiguration.h"
#include "tests/PIMKernel.h"
#include "Burst.h"
#include "FP16.h"
#include "longobject.h"
#include <cstdint>
#include "json.hpp"
#include <set>

#define PIM_REG_ROW (0)
#define PIM_PBTYPE (DRAMSim::pimBankType::ALL_BANK)

shared_ptr<PIMKernel> pim_kernel_;
shared_ptr<MultiChannelMemorySystem> mem_;
size_t cycle_;
AddrMapping* addr_mapping_;
enum GRF_ind {
	READ,
	WRITE
};
BurstType GRF[2][2];

class PimCmdWithRow {
	public:
	PIMCmd cmd;
	int row;
	
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(PimCmdWithRow, cmd, row)
};
vector<PimCmdWithRow> program;
size_t PC = 0;
set<size_t> breakpoints;

static void write_reg_burst(void) {
	for (int i = 0; i < 2; i++) {
		pim_kernel_->writeData(&GRF[GRF_ind::WRITE][i], 1, PIM_REG_ROW, 0, i);
	}
}

static void read_reg_burst(void) {
	for (int i = 0; i < 2; i++) {
		pim_kernel_->readData(&GRF[GRF_ind::READ][i], 1, PIM_REG_ROW, 0, i);
	}
}

static void commit(void) {
	pim_kernel_->runPIM();
}

static void sync_write_reg(void) {
	// Read the current values, in case they've been modified.
	read_reg_burst();
	commit();
	for (int i = 0; i < 2; i++) {
		GRF[GRF_ind::WRITE][i].set(GRF[GRF_ind::READ][i]);
	}
}

void executeprogram(void) {
	if (PC >= program.size()) {
		return;
	}
	vector<PIMCmd> commands;
	vector<int> rows;
	size_t i;
	for (i = PC; i < program.size() && (!breakpoints.contains(i) || i == PC); i++) {
		commands.push_back(program[i].cmd);
		rows.push_back(program[i].row);
	}
	pim_kernel_->multiStep(commands, rows, PIM_PBTYPE, PIM_REG_ROW);
	read_reg_burst();
	pim_kernel_->runPIM();
	PC = i;
}

void setbreakpoint(size_t where) {
	breakpoints.insert(where);
}

void clearbreakpoint(size_t where) {
	breakpoints.erase(where);
}

void singlestep(void) {
	if (PC >= program.size()) {
		return;
	}
	pim_kernel_->singleStep(program[PC].cmd, program[PC].row, PIM_PBTYPE, PIM_REG_ROW);
	read_reg_burst();
	pim_kernel_->runPIM();
	PC++;
}

void pim_init(std::string fname) {
	mem_ = make_shared<MultiChannelMemorySystem>("ini/HBM2_samsung_2M_16B_x64.ini", "system_hbm.ini", ".",
		"example_app", 2048);
	addr_mapping_ = mem_->addrMapping;
	pim_kernel_ = make_shared<PIMKernel>(mem_, 1, 1);

	// Initialize the registers' memory region to zero.
	for (BurstType burst : GRF[GRF_ind::WRITE]) {
		burst.set(convertF2H(0.0));
	}
	write_reg_burst();
	commit();

	// Load the program
	std::ifstream f(fname);
	nlohmann::json data = nlohmann::json::parse(f);
	program = data;
	PC = 0;
}

uint64_t readregister(unsigned int reg_num) {
	if (reg_num >= 32) {
		return PC;
	}
	
	read_reg_burst();
	commit();
	fp16 value;
	// Read the register from the stored burst data.
	value = GRF[GRF_ind::READ][reg_num / 32].fp16Data_[reg_num % 32];
	return fp16i(value).ival;
	
}

void writeregister(unsigned int reg_num, unsigned long input_value) {
	// Some two-byte value.
	fp16i value;
	if (reg_num < 32) {
		sync_write_reg();
		value.ival = (uint16_t) input_value;
		GRF[GRF_ind::WRITE][reg_num / 32].fp16Data_[reg_num % 32] = value.fval;
		write_reg_burst();
		commit();
	} else {
		PC = input_value;
	}
}

unsigned char readbyte(uint64_t addr) {
	BurstType burst;
	unsigned long mask = pim_kernel_->transaction_size_ - 1;
	mem_->addTransaction(false, addr & ~mask, &burst);
	commit();
	return burst.u8Data_[addr & mask];
}

void writebyte(uint64_t addr, unsigned char byte) {
	// Read the existing values out first, because the granularity level is a burst (32 bytes)
	BurstType burst;
	unsigned long mask = pim_kernel_->transaction_size_ - 1;
	mem_->addTransaction(false, addr & ~mask, &burst);
	commit();
	// Now modify the value and write it back.
	burst.u8Data_[addr & mask] = byte;
	mem_->addTransaction(true, addr & ~mask, &burst);
	commit();
}