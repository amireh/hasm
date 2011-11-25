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

#include "operand.hpp"
#include "parser.hpp"

namespace hax
{
  using utility::stringify;

	operand::operand(string_t const& in_token)
  : token_(in_token),
    value_(0x0),
    type_(t_undefined),
    length_(0x0),
    evaluated_(false)
  {
    //~ std::cout << this << "\n";
	}

	operand::~operand()
	{
	}

  operand::operand(const operand& src)
  {
    copy_from(src);
  }

  operand& operand::operator=(const operand& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void operand::copy_from(const operand& src)
  {
    this->token_ = src.token_;
    this->value_ = src.value_;
    this->type_ = src.type_;
    this->length_ = src.length_;
    this->evaluated_ = src.evaluated_;
  }

  std::ostream& operand::to_stream(std::ostream& in) const
  {
    in
      << (evaluated_ ? "defined" : "undefined")
      << "[operand: '" << token_ << "']";

    return in;
  }

  string_t const& operand::token() const
  {
    return token_;
  }

  uint32_t operand::value() const
  {
    if (!evaluated_)
      throw unevaluated_operand("attempt to retrieve an unevaluated operand's value: " + this->dump());

    return value_;
  }

  bool operand::is_evaluated() const
  {
    return evaluated_;
  }

  void operand::_assign_value(uint32_t in_val)
  {
    value_ = in_val;
    evaluated_ = true;
  }

  bool operand::is_symbol() const {
    return type_ == t_symbol;
  }

  bool operand::is_constant() const {
    return type_ == t_constant;
  }

  bool operand::is_expression() const {
    return type_ == t_expression;
  }

  uint16_t operand::length() const
  {
    return length_;
  }
} // end of namespace
