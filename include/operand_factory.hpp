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

#ifndef h_operand_factory_h
#define h_operand_factory_h

#include "operand.hpp"
#include "operands/constant.hpp"
#include "operands/expression.hpp"
#include "operands/symbol.hpp"

namespace hax
{
  class instruction;
  class operand_factory {
    public:

    static operand_factory* singleton_ptr();
    static operand_factory& singleton();

		virtual ~operand_factory();

    /**
     * @brief
     * creates a new operand instance of the correct type based on the format
     * of the given token
     *
     * @warning
     * the operand factory does not retain ownership of newly created instances,
     * it is the responsibility of the caller to free the allocated objects
     **/
    operand_t* create(string_t const& opcode_token, instruction* in_inst);

    bool __is_constant(string_t const& token);
    bool __is_literal(string_t const& token);
    bool __is_symbol(string_t const& token);
    bool __is_expression(string_t const& token);

    private:
    static operand_factory *__instance;

		operand_factory();
    operand_factory(const operand_factory& src);
		operand_factory& operator=(const operand_factory& rhs);
	};
} // end of namespace
#endif // h_operand_factory_h
