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

#ifndef h_fmt4_instruction_h
#define h_fmt4_instruction_h

#include "instruction.hpp"

namespace hax
{
  class fmt4_instruction : public instruction {
    public:

    fmt4_instruction() = delete;
		explicit fmt4_instruction(opcode_t, const string_t&);
    fmt4_instruction(const fmt4_instruction& src);
		fmt4_instruction& operator=(const fmt4_instruction& rhs);
		virtual ~fmt4_instruction();

    virtual loc_t length() const;
    virtual void assemble();
    virtual bool is_valid() const;

    void assign_operand(string_t const& in_operand);

    protected:
    void copy_from(const fmt4_instruction&);

    private:
	};
} // end of namespace
#endif // h_fmt4_instruction_h
