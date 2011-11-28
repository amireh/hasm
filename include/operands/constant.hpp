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

#ifndef h_constant_h
#define h_constant_h

#include "hax.hpp"
#include "operand.hpp"

namespace hax
{
  /**
   * Constant operands are either decimal or hexadecimal numbers, or ASCII characters,
   * or the special operator '*' which denotes the current value of the location counter.
   **/
  class constant : public operand {
    public:

    /**
     * The type of this constant is deduced in the constructor by checking
     * the passed token for occurences of every format specification. If the token
     * includes:
     *
     *  1. C'' then it is assumed to be an ASCII constant
     *  2. X'' then it is assumed to be a hexadecimal constant
     *  3. =C'' then it is assumed to be an ASCII literal
     *  4. =X'' then it is assumed to be a hexadecimal literal
     *  5. * then it is assumed to be the special location operator
     *  6. /(#)*[0-9]/ then it is assumed to be a decimal constant
     *
     **/
		explicit constant(string_t const& in_token, instruction* in_inst);
    constant()=delete;
    constant(const constant& src);
		constant& operator=(const constant& rhs);
		virtual ~constant();

    /**
     * Calculates the value of this constant.
     *
     * For Hex and ASCII constants, the value is extracted from the token and
     * converted one byte at a time into its internal hexadecimal representation.
     *
     * For all literals, the value is simply the location of the assigned literal.
     *
     * For the special operator *, the location counter of the program block of
     * this operand's instruction is used.
     **/
    virtual void evaluate();

    protected:
    void (constant::*handler_)();

    void handle_ascii_constant();
    void handle_hex_constant();
    void handle_hex_or_ascii_constant(bool is_ascii);
    void handle_constant();
    void handle_current_loc();
    void handle_literal();

    string_t stripped_;

    void copy_from(const constant&);
	};

  typedef constant constant_t;
} // end of namespace
#endif // h_constant_h
