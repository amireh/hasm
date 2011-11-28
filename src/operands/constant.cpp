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

#include "operands/constant.hpp"
#include "parser.hpp"
#include <cmath>

namespace hax
{
  using utility::stringify;

	constant::constant(string_t const& in_token, instruction* in_inst)
  : operand(in_token, in_inst)
  {
    type_ = t_constant;

    // ASCII literals, format: =C'characters'
    if (in_token.find("=C'") != std::string::npos)
    {
      handler_ = &constant::handle_literal;
      type_ = t_literal;
    }
    // Hexadecimal literals, format: =X'digits'
    else if (in_token.find("=X'") != std::string::npos) {
      handler_ = &constant::handle_literal;
      type_ = t_literal;
    }

    // Hexadecimal constants, format: X'hexdigits'
    else if (in_token.find("X'") != std::string::npos) {
      handler_ = &constant::handle_hex_constant;
      stripped_ = token_.substr(2, token_.size()-3);

    }
    // ASCII constants, format: C'characters'
    else if (in_token.find("C'") != std::string::npos) {
      handler_ = &constant::handle_ascii_constant;
      stripped_ = token_.substr(2, token_.size()-3);

    }
    // Special operator *, denotes the current value of the location counter
    else if (in_token[0] == '*') {
      handler_ = &constant::handle_current_loc;
    }
    // Decimal constant
    else {

      if (token_[0] == '#')
        token_ = token_.substr(1, token_.size()-1);

      handler_ = &constant::handle_constant;
    }

    // if the operand is a literal, declare the dependency
    if (is_literal())
    {
      parser::singleton().sect()->symmgr()->declare_literal(token_, this);
    }
	}

	constant::~constant()
	{
    handler_ = 0;
	}

  constant::constant(const constant& src) : operand(src.token_, src.inst_)
  {
    copy_from(src);
  }

  constant& constant::operator=(const constant& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void constant::copy_from(const constant& src)
  {
  }

  void constant::evaluate()
  {
    (this->*handler_)();
    evaluated_ = true;
  }

  void constant::handle_literal()
  {
    instruction* lit = inst_->block()->sect()->symmgr()->lookup_literal(token_);
    value_ = lit->location();
  }

  void constant::handle_constant()
  {
    value_ = utility::convertTo<int>(token_);
    length_ = token_.size();
  }

  void constant::handle_ascii_constant()
  {
    handle_hex_or_ascii_constant(true);
  }

  void constant::handle_hex_constant()
  {
    handle_hex_or_ascii_constant(false);
  }

  void constant::handle_hex_or_ascii_constant(bool is_ascii)
  {
    std::stringstream hex_repr;
    hex_repr << std::hex;
    for (auto c : stripped_)
      if (is_ascii)
        hex_repr << (int)c;
      else
        hex_repr << c;

    hex_repr >> value_;

    length_ = stripped_.size();

    // since one byte holds 2 hex digits, we divide the length by two
    if (!is_ascii)
      length_ = std::ceil(length_ / 2);
  }

  void constant::handle_current_loc()
  {
    value_ = inst_->block()->locctr();
    length_ = 0;
    //~ std::cout << "assigned location counter " << value_ << "\n";
  }
} // end of namespace
