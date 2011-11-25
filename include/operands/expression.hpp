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

#ifndef h_expression_h
#define h_expression_h

#include "hax.hpp"
#include "loggable.hpp"
#include <vector>

namespace hax
{
  class expression : public loggable {
    public:

    typedef addressing_mode addressing_mode_t;

    expression() = delete;
		explicit expression(opcode_t, const string_t&);
    expression(const expression& src);
		expression& operator=(const expression& rhs);
		virtual ~expression();

    protected:
    private:
	};

  typedef expression expression_t;
} // end of namespace
#endif // h_expression_h
