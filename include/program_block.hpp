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

#ifndef h_program_block_h
#define h_program_block_h

#include "hax.hpp"
#include "instruction.hpp"

namespace hax
{
  class control_section;
  typedef control_section csect_t;

  class program_block {
    public:

    typedef instruction inst_t;
    typedef std::list<inst_t*> instructions_t;

		program_block(string_t in_name, csect_t* in_section);
		virtual ~program_block();

    // program blocks can not be copied
    program_block(const program_block& src)=delete;
		program_block& operator=(const program_block& rhs)=delete;

    loc_t locctr() const;
    string_t const& name() const;
    void step();

    void shift(int in_amount);
    size_t length() const;

    /**
     * registers the given instruction with this program block and assigns
     * an address to it based on this object's location counter, finally the
     * counter is incremented by an amount equal to the instruction's length
     **/
    void add_instruction(instruction_t* in_inst);

    instructions_t const& instructions() const;

    protected:

    loc_t locctr_;
    string_t name_;
    instructions_t instructions_;
    csect_t* csect_;
	};

  typedef program_block pblock_t;
} // end of namespace
#endif // h_program_block_h
