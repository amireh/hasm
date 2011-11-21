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

  void fmt4_instruction::calc_target_address()
  {
    symbol_manager &sym_mgr = symbol_manager::singleton();
    int target_address = 0x000;
    int disp = 0x0;
    //~ uint32_t objcode = 0x0;
    uint32_t targeting_flags = 0x0;
    bool is_constant = false;

    assert(!operands_.empty());

    string_t &operand_str = operands_.front();
    switch(operand_str[0]) {
      case '#':
        addr_mode_ = addressing_mode::immediate;
        break;
      case '@':
        addr_mode_ = addressing_mode::indirect;
        break;
      default:
        addr_mode_ = addressing_mode::simple;
    }

    symbol_t *operand = 0;

    // extract the target address
    if (addr_mode_ == addressing_mode::immediate)
    {
      // strip the leading '#' or '@'
      operand_str = operand_str.substr(1, operand_str.size());
      operand = sym_mgr.lookup(operand_str);

      // if it's an immediate symbol:
      // TA = symbol address - PC/B
      if (operand)
      {
        target_address = operand->address();
      } else
      {
        // it is an immediate constant
        // TA = constant - PC/B
        targeting_flags = 0x1 << 24; // immediate
        is_constant = true;
        target_address = utility::convertTo<int>(operand_str);
      }

    } else if (addr_mode_ == addressing_mode::simple) {

      // TA = operand address - PC/B
      operand = sym_mgr.lookup(operand_str);
      assert(operand);

      target_address = operand->address();
      targeting_flags = 0x3 << 24; // neither indirect nor immediate
    } else {
      throw invalid_addressing_mode(this->line_);
    }

    disp = target_address;
    //~ targeting_flags |= 0x000001;

    if (VERBOSE)
    std::cout
      << "Immediate target address = "
      << std::hex << std::uppercase
      << target_address << "\n";

    // discard the 5 highest-order half-bytes (for negative values calculated with 2's complement)
    int foo = (opcode_ << 24) | addr_mode_ | targeting_flags;
    objcode_ = utility::overwrite_bits<int>(disp, foo, 20, 12) | (0x1 << 20);
  }

  bool fmt4_instruction::is_valid() const
  {
    return true;
  }

} // end of namespace
