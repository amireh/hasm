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

#include "instruction_factory.hpp"
#include "parser.hpp"

namespace hax
{

	instruction_factory* instruction_factory::__instance = 0;

	instruction_factory::instruction_factory()
  {
	}

	instruction_factory::~instruction_factory()
	{
	}

	instruction_factory* instruction_factory::singleton_ptr()
  {
		return __instance = (!__instance) ? new instruction_factory() : __instance;
	}

	instruction_factory& instruction_factory::singleton()
  {
		return *singleton_ptr();
	}

  instruction_t*
  instruction_factory::create(string_t const& in_token)
  {
    int ec = -1;
    opcode_fmt_t tuple = parser::singleton().opcode_from_token(in_token, &ec);

    if (ec != 0)
      throw unrecognized_operation("attempting to create an instruction of an unrecognized operation: " + in_token);

    instruction_t *inst = 0;
    opcode_t opcode = std::get<0>(tuple);
    switch (std::get<1>(tuple))
    {
      case format::fmt_one:
        inst = new fmt1_instruction(opcode, in_token);
        break;
      case format::fmt_two:
        inst = new fmt2_instruction(opcode, in_token);
        break;
      case format::fmt_three | format::fmt_four:
      case format::fmt_three:
      case format::fmt_four:
        // is it an extended one? (fmt4)
        if (in_token.front() == '+')
        {
          // fmt4
          inst = new fmt4_instruction(opcode, in_token);
        } else
        {
          // fmt3
          inst = new fmt3_instruction(opcode, in_token);
        }
        break;
      case format::fmt_directive:
        inst = new directive(opcode, in_token);
        break;
      default:
        throw invalid_format("unrecognized format for operation: " + in_token);
    }

    return inst;
  }

} // end of namespace
