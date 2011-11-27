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
  class constant : public operand {
    public:

		explicit constant(string_t const& in_token, instruction* in_inst);
    constant()=delete;
    constant(const constant& src);
		constant& operator=(const constant& rhs);
		virtual ~constant();

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
