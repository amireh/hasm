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

#ifndef h_instruction_h
#define h_instruction_h

#include "hax.hpp"
#include "loggable.hpp"
#include "symbol.hpp"
#include <list>

namespace hax
{
  typedef enum {
    one = 0x01,
    two = 0x02,
    three = 0x04,
    four = 0x08,
    undefined = 0x16
  } format;

  class instruction : public loggable {
    public:

    enum addressing_mode {
      simple = 0x00,
      indirect = 0x01,
      immediate = 0x02
    };

    typedef addressing_mode addressing_mode_t;

		instruction();
    instruction(const instruction& src);
		instruction& operator=(const instruction& rhs);
		virtual ~instruction();

    opcode_t opcode() const;

    loc_t location() const;
    void set_location(loc_t);

    loc_t length() const;

    void register_token(string_t const&);

    /**
     * once all tokens are registered, this routine determines the format of
     * this instruction, the opcode, its symbols and operands
     **/
    virtual void process_tokens();

    int nr_tokens() const;

    /**
     * is this instruction labelled?
     **/
    bool has_label() const;

    symbol_t const* const label() const;

    /**
     * an instruction is valid when:
     *  1 - the opcode exists is registered in the optable
     *  2 - all operands registered are valid and match the format specifications
     **/
    bool is_valid() const;

    /**
     * are there no dependencies left for this symbol_manager's object code to be
     * computed?
     **/
    bool is_fulfilled() const;

    protected:
    typedef std::list<string_t> tokens_t;

    /**
     * the opcode is automatically set when the instruction is created by looking
     * up the mnemonic code in the master optable
     **/
    opcode_t opcode_;

    /**
     * the location is specified by the assembler in pass 2 and is required for
     * this instruction object to calculate its object code
     **/
    loc_t location_;
    loc_t length_;

    symbol_t *label_;

    tokens_t tokens_;

    format_t format_;

    addressing_mode_t addr_mode_;

    void copy_from(const instruction&);
    bool is_sicxe() const;
    virtual std::ostream& to_stream(std::ostream&) const;

    private:
	};
} // end of namespace
#endif // h_instruction_h
