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
#include <cassert>

namespace hax
{
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
      string_t &operand = operands_.front();
      size_t nr_bytes = operand.size();

      // is it a stream of characters or a device identifier?
      if (operand.size() > 3)
      {
        // character streams are denoted by C`stream` in plain letters
        if (operand.find("C`") == 0)
          nr_bytes = operand.size() - 3;
        // device identifiers are denoted by X`identifier` in hex
        else if (operand.find("X`") == 0)
          nr_bytes = (operand.size() - 3) / 2;
      }

      length_ = 1 * nr_bytes;

    } else if (mnemonic_ == "WORD")
    {
      length_ = 3;
    } else if (mnemonic_ == "RESB")
    {
      length_ = 1 * utility::convertTo<int>(operands_.front());
    } else if (mnemonic_ == "RESW")
    {
      length_ = 3 * utility::convertTo<int>(operands_.front());
    }
  }

  loc_t directive::length() const
  {
    return length_;
  }

  void directive::calc_target_address()
  {
    if (mnemonic_ == "BASE")
    {
      //~ std::cout << "-- assigned base register @ ";
      symbol_t *operand = symbol_manager::singleton().lookup(operands_.front());
      assert(operand);
      parser::singleton().set_base(operand->address());
      std::cout << std::hex << std::setw(4) << std::setfill('0') << parser::singleton().base() << "\n";
    } else if (mnemonic_ == "BYTE")
    {
      string_t operand = operands_.front();

      if (operand.find("C`") != std::string::npos)
      {
        // strip C``
        operand = operand.substr(2, operand.size()-3);

        std::stringstream hex_repr;
        hex_repr << std::hex;
        for (auto c : operand)
          hex_repr << (int)c;

        hex_repr >> objcode_;
        objcode_width_ = operand.size();

      } else if (operand.find("X`") != std::string::npos) {
        // strip C``
        operand = operand.substr(2, operand.size());
        operand = operand.substr(0, operand.size()-1);

        std::stringstream hex_repr;
        hex_repr << std::hex;
        for (auto c : operand)
          hex_repr << c;

        hex_repr >> objcode_;
        objcode_width_ = operand.size();
      } else {
        throw invalid_operand("unrecognized token in BYTE operation: " + operand);
      }
    }
  }

  bool directive::is_valid() const
  {
    return true;
  }

} // end of namespace
