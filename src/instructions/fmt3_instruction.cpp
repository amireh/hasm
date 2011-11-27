/*
 *  This file is part of Hax.
 *
 *  HASM - an assembler for the open-source language Hax
 *  Copyright (C) 2011  Ahmad Amireh <ahmad@amireh.net>
 *
 *  HASM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HASM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HASM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fmt3_instruction.hpp"
#include "symbol.hpp"
#include "parser.hpp"
#include "symbol_manager.hpp"
#include <cassert>

namespace hax
{
  using utility::stringify;

	fmt3_instruction::fmt3_instruction(opcode_t in_opcode, string_t const& in_mnemonic, pblock_t* block)
  : instruction(in_opcode, in_mnemonic, block)
  {
    format_ = format::fmt_three;
    addr_mode_ = addressing_mode::simple;
	}

	fmt3_instruction::~fmt3_instruction()
	{
	}

  fmt3_instruction::fmt3_instruction(const fmt3_instruction& src)
  : instruction(src)
  {
    copy_from(src);
  }

  fmt3_instruction& fmt3_instruction::operator=(const fmt3_instruction& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void fmt3_instruction::copy_from(const fmt3_instruction& src)
  {
    instruction::copy_from(src);
  }

  void fmt3_instruction::preprocess()
  {
    instruction::preprocess();

    length_ = 3;

    if (mnemonic_ == "RSUB")
    {
      //if (operands_.empty())
      //  operands_.push_back("0");
      assign_operand("0");
    }

  }

  loc_t fmt3_instruction::length() const
  {
    return length_;
  }

  void fmt3_instruction::assign_operand(string_t const& in_operand)
  {
    instruction::assign_operand(in_operand);

    switch(in_operand[0]) {
      case '#':
        addr_mode_ = addressing_mode::immediate;
        break;
      case '@':
        addr_mode_ = addressing_mode::indirect;
        break;
      default:
        addr_mode_ = addressing_mode::simple;
    }
  }

  bool fmt3_instruction::pc_relative_viable(int& address) const
  {
    int lower_bound = -2048 + location();
    int upper_bound = 2047 + location();

    return !(address < lower_bound || address > upper_bound);
  }

  bool fmt3_instruction::base_relative_viable(int& address) const
  {
    loc_t base = parser::singleton().base();

    int lower_bound = 0 + base;
    int upper_bound = 4096 + base;

    //std::cout
    //  << "\tchecking if address is viable for base-relative targeting: "
    //  << std::hex << address << " <> base: " << base << "\n";

    return !(address < lower_bound || address > upper_bound);
  }

  bool fmt3_instruction::immediate_viable(int& address) const
  {
    // since SIC/XE is not a multi-programming architecture, a program
    // is allowed to reference all memory locations which is [0 ... 2^20]
    int lower_bound = 0;
    int upper_bound = 1048576;

    return !(address < lower_bound || address > upper_bound);
  }

  void fmt3_instruction::assemble()
  {
    int target_address = 0x0;
    int disp = 0x0;
    uint16_t targeting_flags = 0x0;
    operand_->evaluate();
    if (addr_mode_ != addressing_mode::immediate && operand_->is_symbol())
      disp = target_address = static_cast<symbol*>(operand_)->address();
    else
      disp = target_address = operand_->value();

    if (indexed_)
      targeting_flags |= 0x008000;

    // try calculating the displacement using PC, then base, then immediate addressing
    if (!operand_->is_constant())
    {
      //~ construct_relocation_records();

      if (pc_relative_viable(target_address))
      {
        disp = target_address - (location() + length());

        if (VERBOSE)
        std::cout
          << "PC-relative displacement = " << std::hex << std::uppercase
          << target_address << " - "
          << (location() + length()) << " = " << disp << "\n";

        targeting_flags |= 0x002000;
      } else if (base_relative_viable(target_address))
      {
        disp = target_address - parser::singleton().base();

        if (VERBOSE)
        std::cout
          << "Base-relative displacement = " << std::hex << std::uppercase
          << target_address << " - "
          << (location() + length()) << " = " << disp << "\n";

        //~ targeting_flags |= indexed_ ? 0x00C000 : 0x004000;
        targeting_flags |= 0x004000;
      } else if (immediate_viable(target_address))
      {
        disp = target_address;

        if (VERBOSE)
        std::cout
          << "Immediate target address = " << std::hex << std::uppercase
          << target_address << "\n";

      } else {
        throw target_out_of_bounds(utility::stringify(target_address), this->line_);
      }
    }

    // discard the 5 highest-order half-bytes (for negative values calculated with 2's complement)
    objcode_ = (opcode_ << 16) | addr_mode_ | targeting_flags;
    objcode_ = utility::overwrite_bits<int>(disp, objcode_, 12, 20);
  }

  bool fmt3_instruction::is_valid() const
  {
    return true;
  }

} // end of namespace
