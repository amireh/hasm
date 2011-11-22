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

#include "symbol.hpp"
#include "symbol.hpp"
#include "parser.hpp"
#include "symbol_manager.hpp"

namespace hax
{
  using utility::stringify;

	symbol::symbol(string_t const& in_label)
  : label_(in_label),
    address_(0x0000),
    value_(0),
    is_resolved_(false)
  {
	}

	symbol::~symbol()
	{
	}

  symbol::symbol(const symbol& src)
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
    this->label_ = src.label_;
    this->address_ = src.address_;
    this->is_resolved_ = src.is_resolved_;
  }

  std::ostream& symbol::to_stream(std::ostream& in) const
  {
    in
      << (is_resolved_ ? "defined" : "undefined")
      << "[symbol: '" << label_ << "' @ "
      << std::hex << std::setw(3) << std::setfill('0') << address_;

    in << "]";

    return in;
  }

  void symbol::set_address(loc_t in_addr)
  {
    if (is_resolved_)
      throw symbol_redifinition(std::string("attempting to re-define symbol ") + this->dump());

    address_ = in_addr;
    is_resolved_ = true;
  }

  string_t const& symbol::label() const
  {
    return label_;
  }

  bool symbol::is_resolved() const
  {
    return is_resolved_;
  }

  loc_t symbol::address() const
  {
    return address_;
  }

  int symbol::value() const
  {
    return value_;
  }

} // end of namespace
