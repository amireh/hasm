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

#include "fmt4_instruction.hpp"
#include "symbol.hpp"
#include "parser.hpp"
#include "symbol_manager.hpp"
#include <cassert>

namespace hax
{
  using utility::stringify;

	fmt4_instruction::fmt4_instruction(opcode_t in_opcode, string_t const& in_mnemonic)
  : instruction(in_opcode, in_mnemonic)
  {
    format_ = format::fmt_four;
    objcode_width_ = 8;
	}

	fmt4_instruction::~fmt4_instruction()
	{
	}

  fmt4_instruction::fmt4_instruction(const fmt4_instruction& src)
  : instruction(src)
  {
    copy_from(src);
  }

  fmt4_instruction& fmt4_instruction::operator=(const fmt4_instruction& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void fmt4_instruction::copy_from(const fmt4_instruction& src)
  {
    instruction::copy_from(src);
  }

  loc_t fmt4_instruction::length() const
  {
    return 4;
  }

  void fmt4_instruction::assign_operand(string_t const& in_operand)
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

  void fmt4_instruction::assemble()
  {
    int target_address = 0x0;
    int disp = 0x0;
    uint32_t targeting_flags = 0x0;

    assert(operand_);

    // all extended format operations require relocation except constant-operanded ones
    relocatable_ = true;

    // extract the target address
    operand_->evaluate();
    disp = target_address = operand_->value();

    // assign addressing flags
    if (addr_mode_ == addressing_mode::immediate)
    {
      // if it's an immediate operand
      if (operand_->is_constant()) {
        targeting_flags = 0x1 << 24;
        relocatable_ = false;
      }
    } else if (addr_mode_ == addressing_mode::simple) {
      targeting_flags = 0x3 << 24; // neither indirect nor immediate
    } else {
      throw invalid_addressing_mode(this->line_);
    }

    if (VERBOSE)
    std::cout
      << "Immediate target address = "
      << std::hex << std::uppercase
      << target_address << "\n";

    // discard the 5 highest-order half-bytes (for negative values calculated with 2's complement)
    objcode_ = (opcode_ << 24) | addr_mode_ | targeting_flags;
    objcode_ = utility::overwrite_bits<int>(disp, objcode_, 20, 12) | (0x1 << 20);
  }

  bool fmt4_instruction::is_valid() const
  {
    return true;
  }

} // end of namespace
