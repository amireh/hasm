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

#include "directive.hpp"
#include "parser.hpp"
#include "symbol_manager.hpp"
#include "operands/expression.hpp"
#include <cassert>

namespace hax
{
  extern bool VERBOSE;
  using utility::stringify;

	directive::directive(opcode_t in_opcode, string_t const& in_mnemonic)
  : instruction(in_opcode, in_mnemonic)
  {
    format_ = format::fmt_directive;
	}

	directive::~directive()
	{
	}

  directive::directive(const directive& src)
  : instruction(src)
  {
    copy_from(src);
  }

  directive& directive::operator=(const directive& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void directive::copy_from(const directive& src)
  {
    instruction::copy_from(src);
  }

  void directive::preprocess()
  {
    length_ = 0;

    if (mnemonic_ == "BYTE")
    {
      // BYTE directive operands need be either immediate constants, or a constant
      // absolute expression
      bool valid = false;
      if (operand_)
      {
        if (operand_->is_expression()) {
          operand_->evaluate();
          valid = static_cast<expression*>(operand_)->is_absolute();
          //~ valid = true;
        }
        else if (operand_->is_constant()) {
          valid = true;
        }
      }

      if (!valid)
        throw invalid_operand("BYTE operands can only be constant decimal integers or expressions");

      operand_->evaluate();
      length_ = 1 * operand_->length();

    } else if (mnemonic_ == "WORD")
    {
      length_ = 3;
    } else if (mnemonic_ == "RESB")
    {
      assemblable_ = false;
      length_ = 1 * utility::convertTo<int>(operand_str_);
    } else if (mnemonic_ == "RESW")
    {
      assemblable_ = false;
      length_ = 3 * utility::convertTo<int>(operand_str_);
    } else if (mnemonic_ == "BASE")
      assemblable_ = false;
  }

  loc_t directive::length() const
  {
    return length_;
  }

  void directive::assemble()
  {
    if (mnemonic_ == "BASE")
    {
      //~ std::cout << "-- assigned base register @ ";
      //symbol_t *operand = symbol_manager::singleton().lookup(operand_str_);
      //assert(operand);
      parser::singleton().set_base(operand_->value());

      if (VERBOSE)
      std::cout
        << "-- base register assigned @ "
        << std::hex << std::setw(4) << std::setfill('0') << parser::singleton().base()
        << "\n";

    } else if (mnemonic_ == "BYTE")
    {
      // BYTE constant values have already been evaluated in preprocess()
      operand_->evaluate();
      objcode_ = operand_->value();
      objcode_width_ = operand_->length() * 2;
    } else if (mnemonic_ == "END")
    {
        string_t operand_str = "";
        if (!operand_str_.empty())
          operand_str = operand_str_;
        else
          operand_str = "0";

        symbol_t *operand = symbol_manager::singleton().lookup(operand_str);
        if (!operand)
          throw undefined_symbol("in END instruction: " + operand_str);

        objcode_ = operand->value();

    }
  }

  bool directive::is_valid() const
  {
    return true;
  }

} // end of namespace
