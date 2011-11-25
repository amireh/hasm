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

#include "fmt1_instruction.hpp"

namespace hax
{
  using utility::stringify;

	fmt1_instruction::fmt1_instruction(opcode_t in_opcode, string_t const& in_mnemonic)
  : instruction(in_opcode, in_mnemonic)
  {
    format_ = format::fmt_one;
	}

	fmt1_instruction::~fmt1_instruction()
	{
	}

  fmt1_instruction::fmt1_instruction(const fmt1_instruction& src)
  : instruction(src)
  {
    copy_from(src);
  }

  fmt1_instruction& fmt1_instruction::operator=(const fmt1_instruction& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void fmt1_instruction::copy_from(const fmt1_instruction& src)
  {
    instruction::copy_from(src);
  }

  loc_t fmt1_instruction::length() const
  {
    return 1;
  }

  void fmt1_instruction::assemble()
  {
  }

  bool fmt1_instruction::is_valid() const
  {
    return true;
  }

} // end of namespace
