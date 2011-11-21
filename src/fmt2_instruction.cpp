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

namespace hax
{
  using utility::stringify;

	fmt2_instruction::fmt2_instruction(opcode_t in_opcode, string_t const& in_mnemonic)
  : instruction(in_opcode, in_mnemonic)
  {
    format_ = format::fmt_two;
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
  }

  bool fmt2_instruction::is_valid() const
  {
    return true;
  }

} // end of namespace
