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

#ifndef h_instruction_factory_h
#define h_instruction_factory_h

#include "instruction.hpp"
#include "program_block.hpp"
#include "fmt1_instruction.hpp"
#include "fmt2_instruction.hpp"
#include "fmt3_instruction.hpp"
#include "fmt4_instruction.hpp"
#include "directive.hpp"

namespace hax
{
  class instruction_factory {
    public:

    static instruction_factory* singleton_ptr();
    static instruction_factory& singleton();

		virtual ~instruction_factory();

    /**
     * @brief
     * creates a new instruction instance of the correct type based on the format
     * of the given opcode:
     *
     *  1. format 1: creates an instance of fmt1_instruction
     *  2. format 2: creates an instance of fmt2_instruction
     *  3. format 3/4: creates an instance of either fmt3_instruction or fmt4_instruction[1]
     *
     * this method can throw an exception of type hax::unrecognized_operation()
     * if no registered operations could be found with the given id
     *
     * @note
     * [1] when the operation could belong to either format 3 or format 4, the token
     *     is scanned for the prefix '+' that denotes if it's an extended format
     *     instruction or not
     *
     * @warning
     * the instruction factory does not retain ownership of newly created instances,
     * it is the responsibility of the caller to free the allocated objects
     **/
    instruction_t* create(string_t const& opcode_token, program_block *in_block);

    private:
    static instruction_factory *__instance;

		instruction_factory();
    instruction_factory(const instruction_factory& src);
		instruction_factory& operator=(const instruction_factory& rhs);
	};
} // end of namespace
#endif // h_instruction_factory_h
