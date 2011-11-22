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

#include "fmt2_instruction.hpp"
#include "symbol.hpp"
#include "parser.hpp"
#include "symbol_manager.hpp"
#include <cassert>

namespace hax
{
  using utility::stringify;

	fmt2_instruction::fmt2_instruction(opcode_t in_opcode, string_t const& in_mnemonic)
  : instruction(in_opcode, in_mnemonic)
  {
    format_ = format::fmt_two;
    objcode_width_ = 4;
	}

	fmt2_instruction::~fmt2_instruction()
	{
	}

  fmt2_instruction::fmt2_instruction(const fmt2_instruction& src)
  : instruction(src)
  {
    copy_from(src);
  }

  fmt2_instruction& fmt2_instruction::operator=(const fmt2_instruction& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void fmt2_instruction::copy_from(const fmt2_instruction& src)
  {
    instruction::copy_from(src);
  }

  loc_t fmt2_instruction::length() const
  {
    return 2;
  }

  void fmt2_instruction::calc_target_address()
  {
    symbol_manager &sym_mgr = symbol_manager::singleton();

    assert(!operands_.empty());

    // we have to split the operands, if there's more than one
    string_t &operand_str = operands_.front();
    bool has_two_operands = operand_str.find(",") != std::string::npos;
    if (has_two_operands)
    {
      std::vector<string_t> operands_str = utility::split(operand_str, ',');
      assert(operands_str.size() == 2);
      // replace the first operand with the current one (the joined one)
      operand_str = operands_str.front();
      // and assign the new one
      this->assign_operand(operands_str.back());
    } else {
      string_t operand_str = "0";
      this->assign_operand(operand_str); // second operand is nil
    }

    assert(operands_.size() == 2);

    symbol_t *lhs = sym_mgr.lookup(operands_.front());
    symbol_t *rhs = sym_mgr.lookup(operands_.back());

    std::stringstream target_address;
    target_address << std::hex << (int)opcode_ << lhs->address() << rhs->address();
    target_address >> objcode_;
  }

  bool fmt2_instruction::is_valid() const
  {
    return true;
  }

} // end of namespace
