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
#include <cmath>

namespace hax
{
  using utility::stringify;

	constant::constant(string_t const& in_token)
  : operand(in_token)
  {
    type_ = t_constant;

    if (in_token.find("=C'") != std::string::npos)
    {
      handler_ = &constant::handle_ascii_literal;
      token_ = token_.substr(3, token_.size()-4);

    } else if (in_token.find("=X'") != std::string::npos) {
      handler_ = &constant::handle_hex_literal;
      token_ = token_.substr(3, token_.size()-4);

    } else {
      if (token_[0] == '#')
        token_ = token_.substr(1, token_.size()-1);
      handler_ = &constant::handle_constant;
    }

	}

	constant::~constant()
	{
    handler_ = 0;
	}

  constant::constant(const constant& src) : operand(src.token_)
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

  void constant::handle_ascii_literal()
  {
    handle_literal(true);
  }

  void constant::handle_hex_literal()
  {
    handle_literal(false);
  }

  void constant::handle_literal(bool is_ascii)
  {
    std::stringstream hex_repr;
    hex_repr << std::hex;
    for (auto c : token_)
      if (is_ascii)
        hex_repr << (int)c;
      else
        hex_repr << c;

    hex_repr >> value_;

    length_ = token_.size();

    // since one byte holds 2 hex digits, we divide the length by two
    if (!is_ascii)
      length_ /= std::ceil(2);
  }

  void constant::handle_constant()
  {
    value_ = utility::convertTo<int>(token_);
    length_ = token_.size();
    //~ std::cout << "converted constant '" << token_ << "' into " << value_ << "\n";
  }
} // end of namespace
