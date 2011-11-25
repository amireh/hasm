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

#include "operands/symbol.hpp"
#include "parser.hpp"
#include "symbol_manager.hpp"
#include <cassert>

namespace hax
{
  using utility::stringify;

	symbol::symbol(string_t const& in_label)
  : operand(in_label)
  {

    type_ = t_symbol;

    //~ if (in_inst)
      //~ evaluate();
	}

	symbol::~symbol()
	{
	}

  symbol::symbol(const symbol& src) : operand(src.token_)
  {
    copy_from(src);
  }

  symbol& symbol::operator=(const symbol& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void symbol::copy_from(const symbol& src)
  {
    //~ this->label_ = src.label_;
    //~ this->address_ = src.address_;
    //~ this->evaluated_ = src.evaluated_;
  }

  std::ostream& symbol::to_stream(std::ostream& in) const
  {
    in
      << (evaluated_ ? "defined" : "undefined")
      << "[symbol: '" << token_ << "' @ "
      << std::hex << std::setw(3) << std::setfill('0') << value();

    in << "]";

    return in;
  }

  //~ void symbol::set_address(loc_t in_addr)
  //~ {
    //~ if (evaluated_)
      //~ throw symbol_redifinition(std::string("attempting to re-define symbol ") + this->dump());
//~
    //~ address_ = in_addr;
    //~ evaluated_ = true;
  //~ }

  //~ string_t const& symbol::label() const
  //~ {
    //~ return label_;
  //~ }

  //~ void symbol::assign_instruction(instruction_t const* in_inst)
  //~ {
    //~ operand::assign_instruction(in_inst);
    //~ evaluate();
  //~ }

  void symbol::evaluate()
  {
    evaluated_ = true;
  }

} // end of namespace
