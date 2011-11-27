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

#include "literal.hpp"
#include "program_block.hpp"
#include <cassert>
#include <cmath>

namespace hax
{
  extern bool VERBOSE;
  using utility::stringify;

	literal::literal(string_t const& in_value, pblock_t* block)
  : instruction(0x0, in_value, block),
    is_ascii_(false),
    assembled_(false)
  {
    format_ = format::fmt_literal;
    std::cout << "Literal created in block: " << block->name() << "\n";
	}

	literal::~literal()
	{
	}

  literal::literal(const literal& src)
  : instruction(src)
  {
    copy_from(src);
  }

  literal& literal::operator=(const literal& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void literal::copy_from(const literal& src)
  {
    instruction::copy_from(src);
  }

  void literal::preprocess()
  {
    instruction::preprocess();

    is_ascii_ = false;
    if (mnemonic_.find("=C'") != std::string::npos)
    {
      stripped_ = mnemonic_.substr(3, mnemonic_.size()-4);
      is_ascii_ = true;

    } else if (mnemonic_.find("=X'") != std::string::npos) {
      stripped_ = mnemonic_.substr(3, mnemonic_.size()-4);

    } else if (mnemonic_.find("X'") != std::string::npos) {
      stripped_ = mnemonic_.substr(2, mnemonic_.size()-3);
    }

    length_ = stripped_.size();

    std::cout << "Literal " << this << " original length = " << length_ << "\n";

    // since one byte holds 2 hex digits, we divide the length by two
    if (!is_ascii_) {
      length_ = std::ceil(length_ / 2);
    }
  }

  loc_t literal::length() const
  {
    return length_;
  }

  void literal::assemble()
  {
    if (assembled_)
      return;

    std::stringstream hex_repr;
    hex_repr << std::hex;
    for (auto c : stripped_)
      if (is_ascii_)
        hex_repr << (int)c;
      else
        hex_repr << c;

    hex_repr >> objcode_;

    for (auto dep : deps_)
    {
      dep->evaluate();
    }
    deps_.clear();

    assembled_ = true;
  }

  void literal::add_dependency(operand* in_operand)
  {
    deps_.push_back(in_operand);
  }
  literal::deps_t& literal::dependencies()
  {
    return deps_;
  }
  bool literal::is_assembled() const
  {
    return assembled_;
  }
} // end of namespace
