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

#include "operand_factory.hpp"
#include "symbol_manager.hpp"
#include "parser.hpp"

namespace hax
{

	operand_factory* operand_factory::__instance = 0;

	operand_factory::operand_factory()
  {
	}

	operand_factory::~operand_factory()
	{
	}

	operand_factory* operand_factory::singleton_ptr()
  {
		return __instance = (!__instance) ? new operand_factory() : __instance;
	}

	operand_factory& operand_factory::singleton()
  {
		return *singleton_ptr();
	}

  operand_t*
  operand_factory::create(string_t const& in_token)
  {

    string_t operand_str(in_token);

    // find out what kind of operand it is, there are three options:
    //  1. a constant, which could be an ASCII or HEX literal, or a decimal number
    //  2. a symbol
    //  3. expression

    operand* _operand = 0;
    // strip out any leading addressing mode flags (@ or #)
    if (operand_str[0] == '@' || operand_str[0] == '#')
      operand_str = operand_str.substr(1, operand_str.size()-1);

    // a constant
    if (__is_constant(operand_str)) {
      _operand = new constant(in_token);
    } else if (__is_expression(operand_str)) {
      _operand = new expression(in_token);
    } else {
      // we do not own symbol objects, so we grab a reference and release it
      // when we're destructed
      _operand = parser::singleton().current_section()->symmgr()->declare(operand_str);
    }
    return _operand;
  }

  bool operand_factory::__is_constant(string_t const& token)
  {
    return token.find("C'") < 2
        || token.find("X'") < 2
        || token[0] == '*'
        || utility::is_decimal_nr(token);
  }

  bool operand_factory::__is_literal(string_t const& token)
  {
    return false;
  }

  bool operand_factory::__is_symbol(string_t const& token)
  {
    if (token.empty())
      return false;

    // symbols can not begin with a number and must not contain any operator chars
    char c = token[0];
    if((c >= '0' && c <= '9') ||
      token.find('-') != std::string::npos ||
      token.find('+') != std::string::npos ||
      token.find('/') != std::string::npos ||
      token.find('*') != std::string::npos ||
      token.find('(') != std::string::npos ||
      token.find(')') != std::string::npos)
      return false;

    return true;
  }

  bool operand_factory::__is_expression(string_t const& token)
  {
    // although we can use C++ TR1 regex here for a more robust solution,
    // for simplicity, I choose to stupidly scan the token for any operators
    // as they are only allowed within expressions
    for (auto c : token)
      if (utility::is_operator(c))
        return true;

    return false;
  }
} // end of namespace
