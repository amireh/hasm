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

#ifndef h_fmt3_instruction_h
#define h_fmt3_instruction_h

#include "instruction.hpp"

namespace hax
{
  class fmt3_instruction : public instruction {
    public:

		fmt3_instruction() = delete;
		explicit fmt3_instruction(opcode_t, const string_t&, pblock_t* block);
    fmt3_instruction(const fmt3_instruction& src);
		fmt3_instruction& operator=(const fmt3_instruction& rhs);
		virtual ~fmt3_instruction();

    /**
     * overridden from instruction::assign_operand()
     *
     * determines addressing mode based on the given operand field, possible modes:
     *  1. immediate
     *  2. indirect
     *  3. simple
     **/
    virtual void assign_operand(string_t const&);

    virtual loc_t length() const;
    virtual void assemble();
    virtual bool is_valid() const;

    virtual void preprocess();

    protected:
    void copy_from(const fmt3_instruction&);

    bool pc_relative_viable(int& address) const;
    bool base_relative_viable(int& address) const;
    bool immediate_viable(int& address) const;

    private:
	};
} // end of namespace
#endif // h_fmt3_instruction_h
