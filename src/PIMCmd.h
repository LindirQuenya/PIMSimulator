/***************************************************************************************************
 * Copyright (C) 2021 Samsung Electronics Co. LTD
 *
 * This software is a property of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed,
 * transmitted, transcribed, stored in a retrieval system, or translated into any human
 * or computer language in any form by any means,electronic, mechanical, manual or otherwise,
 * or disclosed to third parties without the express written permission of Samsung Electronics.
 * (Use of the Software is restricted to non-commercial, personal or academic, research purpose
 * only)
 **************************************************************************************************/

#ifndef __PIM_CMD_HPP__
#define __PIM_CMD_HPP__

#include <bitset>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <string>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

namespace DRAMSim
{
enum class PIMCmdType
{
    NOP,
    ADD,
    MUL,
    MAC,
    MAD,
    REV0,
    REV1,
    REV2,
    MOV,
    FILL,
    REV3,
    REV4,
    REV5,
    REV6,
    JUMP,
    EXIT
};

enum class PIMOpdType
{
    A_OUT,
    M_OUT,
    EVEN_BANK,
    ODD_BANK,
    GRF_A,
    GRF_B,
    SRF_M,
    SRF_A
};

PIMCmdType CmdFromString(char *s);

class PIMCmd
{
  public:
    PIMCmdType type_;
    PIMOpdType dst_;
    PIMOpdType src0_;
    PIMOpdType src1_;
    PIMOpdType src2_;
    int loopCounter_;
    int loopOffset_;
    int isAuto_;
    int dstIdx_;
    int src0Idx_;
    int src1Idx_;
    int isRelu_;

    PIMCmd()
        : type_(PIMCmdType::NOP),
          dst_(PIMOpdType::A_OUT),
          src0_(PIMOpdType::A_OUT),
          src1_(PIMOpdType::A_OUT),
          src2_(PIMOpdType::A_OUT),
          loopCounter_(0),
          loopOffset_(0),
          isAuto_(0),
          dstIdx_(0),
          src0Idx_(0),
          src1Idx_(0)
    {
    }

    // NOP, CTRL
    PIMCmd(PIMCmdType type, int loopCounter)
        : type_(type),
          dst_(PIMOpdType::A_OUT),
          src0_(PIMOpdType::A_OUT),
          src1_(PIMOpdType::A_OUT),
          src2_(PIMOpdType::A_OUT),
          loopCounter_(loopCounter),
          loopOffset_(0),
          isAuto_(0),
          dstIdx_(0),
          src0Idx_(0),
          src1Idx_(0)
    {
    }

    // JUMP
    PIMCmd(PIMCmdType type, int loopCounter, int loop_offset)
        : type_(type),
          dst_(PIMOpdType::A_OUT),
          src0_(PIMOpdType::A_OUT),
          src1_(PIMOpdType::A_OUT),
          src2_(PIMOpdType::A_OUT),
          loopCounter_(loopCounter),
          loopOffset_(loop_offset),
          isAuto_(0),
          dstIdx_(0),
          src0Idx_(0),
          src1Idx_(0)
    {
    }

    PIMCmd(PIMCmdType type, PIMOpdType dst, PIMOpdType src0, int is_auto = 0, int dst_idx = 0,
           int src0_idx = 0, int src1_idx = 0, int is_relu = 0)
        : type_(type),
          dst_(dst),
          src0_(src0),
          src1_(PIMOpdType::A_OUT),
          src2_(PIMOpdType::A_OUT),
          loopCounter_(0),
          loopOffset_(0),
          isAuto_(is_auto),
          dstIdx_(dst_idx),
          src0Idx_(src0_idx),
          src1Idx_(src1_idx),
          isRelu_(is_relu)
    {
    }

    PIMCmd(PIMCmdType type, PIMOpdType dst, PIMOpdType src0, PIMOpdType src1, int is_auto = 0,
           int dst_idx = 0, int src0_idx = 0, int src1_idx = 0)
        : type_(type),
          dst_(dst),
          src0_(src0),
          src1_(src1),
          src2_(PIMOpdType::A_OUT),
          loopCounter_(0),
          loopOffset_(0),
          isAuto_(is_auto),
          dstIdx_(dst_idx),
          src0Idx_(src0_idx),
          src1Idx_(src1_idx)
    {
    }

    PIMCmd(PIMCmdType type, PIMOpdType dst, PIMOpdType src0, PIMOpdType src1, PIMOpdType src2,
           int is_auto = 0, int dst_idx = 0, int src0_idx = 0, int src1_idx = 0)
        : type_(type),
          dst_(dst),
          src0_(src0),
          src1_(src1),
          src2_(src2),
          loopCounter_(0),
          loopOffset_(0),
          isAuto_(is_auto),
          dstIdx_(dst_idx),
          src0Idx_(src0_idx),
          src1Idx_(src1_idx)
    {
    }

    uint32_t bitmask(int bit) const
    {
        return (1 << bit) - 1;
    }

    uint32_t toBit(uint32_t val, int bit_len, int bit_pos) const
    {
        return ((val & bitmask(bit_len)) << bit_pos);
    }
    uint32_t fromBit(uint32_t val, int bit_len, int bit_pos) const
    {
        return ((val >> bit_pos) & bitmask(bit_len));
    }

    static std::string opdToStr(PIMOpdType opd, int idx = 0)
    {
        switch (opd)
        {
            case PIMOpdType::A_OUT:
                return "A_OUT";
            case PIMOpdType::M_OUT:
                return "M_OUT";
            case PIMOpdType::EVEN_BANK:
                return "EVEN_BANK";
            case PIMOpdType::ODD_BANK:
                return "ODD_BANK";
            case PIMOpdType::GRF_A:
                return "GRF_A[" + to_string(idx) + "]";
            case PIMOpdType::GRF_B:
                return "GRF_B[" + to_string(idx) + "]";
            case PIMOpdType::SRF_M:
                return "SRF_M[" + to_string(idx) + "]";
            case PIMOpdType::SRF_A:
                return "SRF_A[" + to_string(idx) + "]";
            default:
                return "NOT_DEFINED";
        }
    }

    static std::string cmdToStr(PIMCmdType type)
    {
        switch (type)
        {
            case PIMCmdType::EXIT:
                return "EXIT";
            case PIMCmdType::NOP:
                return "NOP";
            case PIMCmdType::JUMP:
                return "JUMP";
            case PIMCmdType::FILL:
                return "FILL";
            case PIMCmdType::MOV:
                return "MOV";
            case PIMCmdType::ADD:
                return "ADD";
            case PIMCmdType::MUL:
                return "MUL";
            case PIMCmdType::MAC:
                return "MAC";
            case PIMCmdType::MAD:
                return "MAD";
            default:
                return "NOT_DEFINED";
        }
    }

    void fromInt(uint32_t val);
    void validationCheck() const;
    uint32_t toInt() const;
    std::string toStr() const;
    int n_cycles() const;
};

bool operator==(const PIMCmd& lhs, const PIMCmd& rhs);
bool operator!=(const PIMCmd& lhs, const PIMCmd& rhs);
NLOHMANN_JSON_SERIALIZE_ENUM( PIMCmdType, {
    {PIMCmdType::NOP, "NOP"},
    {PIMCmdType::ADD, "ADD"},
    {PIMCmdType::MUL, "MUL"},
    {PIMCmdType::MAC, "MAC"},
    {PIMCmdType::MAD, "MAD"},
    {PIMCmdType::REV0, "REV0"},
    {PIMCmdType::REV1, "REV1"},
    {PIMCmdType::REV2, "REV2"},
    {PIMCmdType::MOV, "MOV"},
    {PIMCmdType::FILL, "FILL"},
    {PIMCmdType::REV3, "REV3"},
    {PIMCmdType::REV4, "REV4"},
    {PIMCmdType::REV5, "REV5"},
    {PIMCmdType::REV6, "REV6"},
    {PIMCmdType::JUMP, "JUMP"},
    {PIMCmdType::EXIT, "EXIT"}
})

NLOHMANN_JSON_SERIALIZE_ENUM( PIMOpdType, {
    {PIMOpdType::A_OUT, "A_OUT"},
    {PIMOpdType::M_OUT, "M_OUT"},
    {PIMOpdType::EVEN_BANK, "EVEN_BANK"},
    {PIMOpdType::ODD_BANK, "ODD_BANK"},
    {PIMOpdType::GRF_A, "GRF_A"},
    {PIMOpdType::GRF_B, "GRF_B"},
    {PIMOpdType::SRF_M, "SRF_M"},
    {PIMOpdType::SRF_A, "SRF_A"}
})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PIMCmd, type_, dst_, src0_, src1_, src2_, loopCounter_, loopOffset_, isAuto_, dstIdx_, src0Idx_, src1Idx_, isRelu_)

}  // namespace DRAMSim
#endif
