#include "Burst.h"
#include "FP16.h"
#include "PIMCmd.h"
#include "SystemConfiguration.h"
#include "tests/PIMKernel.h"
#include <cstring>
#include "json.hpp"

shared_ptr<PIMKernel> pim_kernel_;
shared_ptr<MultiChannelMemorySystem> mem_;

size_t cycle_;
AddrMapping* addr_mapping_;

int main(int argc, char* argv[]) {
	mem_ = make_shared<MultiChannelMemorySystem>("ini/HBM2_samsung_2M_16B_x64.ini", "system_hbm.ini", ".",
		"example_app", 2048);
	addr_mapping_ = mem_->addrMapping;
	pim_kernel_ = make_shared<PIMKernel>(mem_, 1, 1);
	vector<PIMCmd> program{
		PIMCmd(PIMCmdType::ADD, PIMOpdType::GRF_A, PIMOpdType::GRF_A, PIMOpdType::EVEN_BANK, 1),
		PIMCmd(PIMCmdType::ADD, PIMOpdType::GRF_B, PIMOpdType::GRF_B, PIMOpdType::ODD_BANK, 1)
	};
	nlohmann::json j = program;
	std::ofstream o("partial_program.json");
	o << std::setw(4) << j << std::endl;

	BurstType data[8];
	float float_view[8][16];
	data[4].set(convertF2H(0));
	data[5].set(convertF2H(0));
	data[6].set(convertF2H(0));
	data[7].set(convertF2H(0));
	for (int i = 0; i < 16; i++) {
		data[0].fp16Data_[i] = convertF2H(i);
		data[1].fp16Data_[i] = convertF2H(16+i);
		data[2].fp16Data_[i] = convertF2H(12+i);
		data[3].fp16Data_[i] = convertF2H(1+i);
	}

	pim_kernel_->writeData(&data[0], 1);
	pim_kernel_->writeData(&data[1], 1, 1);
	pim_kernel_->writeData(&data[2], 1, 0, 0, 1);
	pim_kernel_->writeData(&data[3], 1, 1, 0, 1);
	pim_kernel_->multiStep(program, {1,1});
	pim_kernel_->readData(&data[6], 1, 0);
	pim_kernel_->readData(&data[7], 1, 0, 0, 1);
	pim_kernel_->runPIM();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			float_view[i][j] = convertH2F(data[i].fp16Data_[j]);
		}
	}
	// Dummy line for a breakpoint.
	float_view[0][0] = convertH2F(data[0].fp16Data_[0]);
	return 0;
}