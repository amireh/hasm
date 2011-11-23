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

	fmt3_instruction::fmt3_instruction(opcode_t in_opcode, string_t const& in_mnemonic)
  : instruction(in_opcode, in_mnemonic)
  {
    format_ = format::fmt_three;
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
    length_ = 3;

    if (mnemonic_ == "RSUB")
    {
      if (operands_.empty())
        operands_.push_back("0");
    }
  }

  loc_t fmt3_instruction::length() const
  {
    return length_;
  }

  void fmt3_instruction::find_addressing_mode()
  {
    string_t &operand = operands_.front();
    switch(operand[0]) {
      case '#':
      case '=':
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

  void fmt3_instruction::calc_target_address()
  {
    symbol_manager &sym_mgr = symbol_manager::singleton();
    int target_address = 0x000;
    int disp = 0x0;
    //~ uint32_t objcode = 0x0;
    uint32_t targeting_flags = 0x0;
    bool is_constant = false;
    bool is_literal = false;

    assert(!operands_.empty());

    find_addressing_mode();

    string_t &operand_str = operands_.front();
    symbol_t *operand = 0;

    // extract the target address
    if (addr_mode_ == addressing_mode::immediate || addr_mode_ == addressing_mode::indirect)
    {
      // TODO: handle literals
      if (operand_str.find("=C'") != std::string::npos || operand_str.find("=X'") != std::string::npos)
      {
        bool is_ascii = operand_str[1] == 'C';

        // strip =C''
        operand_str = operand_str.substr(3, operand_str.size()-4);

        std::stringstream hex_repr;
        hex_repr << std::hex;
        for (auto c : operand_str)
        {
          if (is_ascii)
            hex_repr << (int)c;
          else
            hex_repr << c;
        }

        //hex_repr >> objcode_;
        hex_repr >> objcode_;
        //~ objcode_ = target_address;
        is_literal = true;
        objcode_width_ = operand_str.size();
      } else { // is not a literal

        // strip the leading '#' or '@'
        operand_str = operand_str.substr(1, operand_str.size());
        // TODO: check for edge cases when the immediate operand is a number (like #0)
        if (operand_str.front() != '0')
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
          targeting_flags = 0x000000;
          is_constant = true;
          target_address = disp = utility::convertTo<int>(operand_str);
        }
      }
    } else if (addr_mode_ == addressing_mode::simple) {

      // TA = operand address - PC/B
      operand = sym_mgr.lookup(operand_str);
      if (!operand)
        throw undefined_symbol("symbol: " + operand_str + ", in line: " + line_);
      //assert(operand);

      target_address = operand->address();
    } else {
      throw invalid_addressing_mode(this->line_);
    }

    // try calculating the displacement using PC, then base, then immediate addressing
    if (!(is_constant || is_literal))
    {
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

        targeting_flags |= indexed_ ? 0x00C000 : 0x004000;
      } else if (immediate_viable(target_address))
      {
        disp = target_address;

        if (VERBOSE)
        std::cout
          << "Immediate target address = " << std::hex << std::uppercase
          << target_address << "\n";

      } else {
        throw target_out_of_bounds(utility::stringify(target_address) + " in " + this->line_);
      }
    }

    if (is_constant)
      targeting_flags = 0x0;

    // discard the 5 highest-order half-bytes (for negative values calculated with 2's complement)
    if (!is_literal)
    {
      int foo = (opcode_ << 16) | addr_mode_ | targeting_flags;
      objcode_ = utility::overwrite_bits<int>(disp, foo, 12, 20);
    }
  }

  bool fmt3_instruction::is_valid() const
  {
    return true;
  }

} // end of namespace
