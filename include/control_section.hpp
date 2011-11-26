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

#ifndef h_control_section_h
#define h_control_section_h

#include "hax.hpp"
#include "program_block.hpp"
#include "symbol_manager.hpp"
#include "loggable.hpp"

namespace hax
{
  /**
   * Control sections represent independent object programs. A CS contains
   * many program blocks, has a unique identifying label, and state.
   *
   * Symbols defined in one control section are exclusive to that section, and
   * can only be referenced by explicitly defining the reference using EXTREF.
   *
   * See assembler specification for more info.
   **/
  class control_section : public loggable {
    public:

    typedef std::list<instruction_t*> instructions_t;
    typedef std::list<pblock_t*> pblocks_t;

		control_section(string_t in_name);
		virtual ~control_section();

    // control sections can not be copied
    control_section(const control_section& src)=delete;
		control_section& operator=(const control_section& rhs)=delete;

    string_t const& name() const;
    symbol_manager* symmgr() const;

    /**
     * changes the current block to the one identified by in_name, creating it
     * if necessary
     **/
    void switch_to_block(string_t in_name = "Unnamed");

    /**
     * returns the current program block
     **/
    pblock_t* block() const;

    pblocks_t const& program_blocks() const;

    /**
     * registers the given instruction in the control section's master instruction list
     *
     * @warning
     * ownership of all instruction objects contained in a control section's
     * program blocks belongs to the CS object itself and must not be freed
     * explicitly
     *
     * @note
     * this method is called internally by program_block::add_instruction()
     **/
    void __add_instruction(instruction_t* in_inst);

    /**
     * assigns addresses to all registered program blocks, then attempts to assemble
     * all registered instructions
     **/
    void assemble();

    void serialize(string_t const& out_path);

    protected:

    virtual std::ostream& to_stream(std::ostream&) const;

    string_t name_;
    pblocks_t pblocks_;
    pblock_t *pblock_;
    symbol_manager *symmgr_;
    instructions_t instructions_;
	};

  typedef control_section csect_t;
} // end of namespace
#endif // h_control_section_h
