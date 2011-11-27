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
  : operand(in_label),
    address_(0x0),
    has_real_value_(false),
    external_ref_(false),
    external_def_(false)
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
    this->address_ = src.address_;
    //~ this->evaluated_ = src.evaluated_;
  }

  std::ostream& symbol::to_stream(std::ostream& in) const
  {
    in
      << "[symbol: '" << token_ << "' @ "
      << std::hex << std::setw(3) << std::setfill('0') << value_
      << " => "
      << (evaluated_ ? "defined" : "undefined");

    in << "]";

    return in;
  }

  void symbol::evaluate()
  {
  }

  void symbol::assign_address(loc_t in_address)
  {
    address_ = in_address;
    evaluated_ = true;
  }

  uint32_t symbol::value() const {
    //~ if (!evaluated_)
      //~ return address_;
    if (has_real_value_)
      return value_;
    else
      return address_;
    //~ return value_;
  }

  loc_t symbol::address() const
  {
    return address_;
  }

  void symbol::set_has_real_value(bool f)
  {
    has_real_value_ = f;
  }

  bool symbol::is_external_ref() const
  {
    return external_ref_;
  }

  bool symbol::is_external_def() const
  {
    return external_def_;
  }
  void symbol::set_external_ref(bool f)
  {
    external_ref_ = f;
  }
  void symbol::set_external_def(bool f)
  {
    external_def_ = f;
  }
} // end of namespace
