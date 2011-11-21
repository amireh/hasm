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
#include <vector>

namespace hax
{
  extern bool VERBOSE;

  class symbol;
  typedef symbol symbol_t;

  typedef enum {
    fmt_undefined = 0x00,
    fmt_one = 0x01,
    fmt_two = 0x02,
    fmt_three = 0x03,
    fmt_four = 0x04,
    fmt_directive = 0x05
  } format;

  class instruction : public loggable {
    public:

    enum addressing_mode {
      undefined = 0xFFFFFF,
      simple = 0x030000,
      indirect = 0x020000,
      immediate = 0x010000
    };

    typedef addressing_mode addressing_mode_t;

    instruction() = delete;
		explicit instruction(opcode_t, const string_t&);
    instruction(const instruction& src);
		instruction& operator=(const instruction& rhs);
		virtual ~instruction();

    opcode_t opcode() const;
    loc_t location() const;

    /**
     * the length of an instruction is calculated based on its format
     **/
    virtual loc_t length() const=0;

    virtual void calc_target_address()=0;

    /**
     * an instruction is valid when:
     *  1 - the opcode exists is registered in the optable
     *  2 - all operands registered are valid and match the format specifications
     **/
    virtual bool is_valid() const=0;

    /**
     * this routine gives a chance for certain format objects to "bootstrap"
     * before any attempt to resolve operands or calculate object code is made
     *
     * @note
     * this method will be called AFTER the label, opcode, and all operands are assigned
     **/
    virtual void preprocess();

    //void register_token(string_t const&);
    void assign_location(loc_t);
    void assign_label(symbol_t* const);
    void assign_operand(string_t const&);

    void resolve_references();

    void assign_line(string_t const&);


    //int nr_tokens() const;

    /**
     * is this instruction labelled?
     **/
    bool has_label() const;

    symbol_t const* const label() const;

    /**
     * are there no dependencies left for this symbol_manager's object code to be
     * computed?
     **/
    bool is_fulfilled() const;

    virtual string_t dump() const;

    protected:
    typedef std::vector<string_t> operands_t;

    virtual void copy_from(const instruction&);
    virtual std::ostream& to_stream(std::ostream&) const;

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

    symbol_t* label_;

    operands_t operands_;

    format_t format_;

    addressing_mode_t addr_mode_;
    string_t line_;
    uint32_t objcode_;
    bool indexed_;
    string_t mnemonic_;
    uint8_t objcode_width_;

    private:
	};

  typedef instruction instruction_t;
} // end of namespace
#endif // h_instruction_h
