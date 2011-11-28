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

  /**
   * control sections can contain an arbitrary number of program blocks which
   * can hold a number of instructions, and maintain their own location counter
   *
   * once a control section is being assembled, its program blocks are logically
   * rearranged based on their length (determined by their locctr) and are
   * assigned an offset address which will be added to every instruction they
   * contain thus producing a correct address just as if the user had physically
   * rearranged entries in the input file
   *
   * program blocks are uniquely identified by name strings and are created when
   * the USE assembler directive is encountered
   **/
  class program_block {
    public:

    typedef instruction inst_t;
    typedef std::list<inst_t*> instructions_t;

		program_block(string_t in_name, csect_t* in_section);
		virtual ~program_block();

    // program blocks can not be copied
    program_block(const program_block& src)=delete;
		program_block& operator=(const program_block& rhs)=delete;

    /**
     *
     **/
    loc_t locctr() const;
    string_t const& name() const;

    /**
     * increments the location counter by an amount equal to the latest instruction's
     * length,
     *
     * @param inst
     *  if an instruction is passed, its length will be used, otherwise the last
     *  instruction in this block's length will be used instead
     **/
    void step(instruction* inst=0);

    /**
     * increments the locctr by in_amount, and shifts the addresses of all registered
     * instructions by the same amount
     **/
    void shift(int in_amount);

    /**
     * the length of a block is equal to the lengths of all registered instructions
     * in it
     **/
    size_t length() const;

    /**
     * the csect this block belongs to
     **/
    control_section *sect() const;

    /**
     * registers the given instruction with this program block and assigns
     * an address to it based on the location counter
     *
     * @warning
     * the location counter is not automatically stepped, see program_block::step()
     * for more info
     **/
    void add_instruction(instruction_t* in_inst);

    instructions_t const& instructions() const;

    protected:

    loc_t locctr_;
    string_t name_;
    instructions_t instructions_;
    csect_t* sect_;
	};

  typedef program_block pblock_t;
} // end of namespace
#endif // h_program_block_h
