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
#include "operand.hpp"
#include <vector>
#include <list>

namespace hax
{
  extern bool VERBOSE;

  class symbol;
  class program_block;
  typedef symbol symbol_t;
  typedef program_block pblock_t;

  typedef enum {
    fmt_undefined = 0x00,
    fmt_one = 0x01,
    fmt_two = 0x02,
    fmt_three = 0x03,
    fmt_four = 0x04,
    fmt_directive = 0x05,
    fmt_literal = 0x06
  } format;

  class instruction : public loggable {
    public:

    enum addressing_mode {
      undefined = 0xFFFFFF,
      simple = 0x030000,
      indirect = 0x020000,
      immediate = 0x010000
    };

    /**
     * relocation records are used by the serializer to create M records:
     * they contain the name of the symbol, plus the operation in _value_
     * as well as the length of the field to be modified in _length_
     *
     * example:
     *  value = +LENGTH
     *  length = 0x06
     *
     * an instruction can have one or multiple relocation records (in case it's
     * an expression containing multiple external symbol references)
     *
     * all format 4 instructions require a relocation record unless the operand
     * is addressed using immediate mode
     **/
    struct reloc_record_t {
      string_t value;
      uint16_t length;
    };

    typedef std::list<reloc_record_t*> reloc_records_t;

    typedef addressing_mode addressing_mode_t;

    instruction() = delete;
		explicit instruction(opcode_t, const string_t&, pblock_t* block);
    instruction(const instruction& src);
		instruction& operator=(const instruction& rhs);
		virtual ~instruction();

    opcode_t opcode() const;
    loc_t location() const;
    const string_t& line() const;

    program_block* block() const;

    /**
     * the length of an instruction is calculated based on its format
     **/
    virtual loc_t length() const=0;

    /**
     * this routine gives a chance for certain format objects to "bootstrap"
     * before any attempt to resolve operands or calculate object code is made,
     * in some cases, forward-references of symbol dependencies are declared here
     *
     * @note
     * this method will be called AFTER the label, opcode, and all operands are assigned
     **/
    virtual void preprocess();

    virtual void assemble()=0;

    /**
     * this will be called *after* this instruction has been assembled, a major
     * function that's carried out here is identifying relocatability and constructing
     * relocation records accordingly
     **/
    virtual void postprocess();

    /**
     * the location is assigned by the program block this instruction belongs to
     **/
    void assign_location(loc_t);

    /**
     * instructions can optionally be labelled by a symbol, when a symbol is
     * assigned as an instruction's label, the symbol's address will be reset
     * to this instruction's address
     **/
    void assign_label(symbol_t* const);

    /**
     * in_token will be passed to the operand_factory to parse its type and
     * create the correct operand object
     *
     * an instruction can have _at most_ one operand
     **/
    virtual void assign_operand(string_t const& in_token);
    virtual void assign_operand(operand* in_operand);

    void __assign_block(program_block* block);

    /**
     * the source line of this instruction (used for printing purposes)
     **/
    void assign_line(string_t const&);

    /**
     * is this instruction labelled?
     **/
    bool has_label() const;
    symbol_t const* const label() const;
    objcode_t objcode() const;
    bool is_assemblable() const;
    bool is_relocatable() const;
    string_t const& mnemonic() const;
    operand* get_operand() const;

    /**
     * are there no dependencies left for this instruction's object code to be
     * assembled?
     **/
    bool is_fulfilled() const;

    /**
     * returns all the required relocation records for this instruction
     **/
    reloc_records_t& reloc_records();

    virtual string_t dump() const;

    protected:
    virtual void copy_from(const instruction&);

    virtual std::ostream& to_stream(std::ostream&) const;

    /**
     * determines whether this instruction requires relocation, and if it does
     * then it creates the necessary reloc_record objects which will be later
     * used by the serializer to create M records
     */
    void construct_relocation_records();

    reloc_record_t* construct_relocation_record(symbol_t* sym);

    /* the opcode is automatically set when the instruction is created by looking
     * up the mnemonic code in the master optable
     */
    opcode_t opcode_;

    /* the location is assigned in pass 1 and critical required for assembling */
    loc_t location_;

    /* the length in bytes of this instruction's assembly output */
    size_t length_;

    symbol_t* label_;
    pblock_t* pblock_;

    operand_t *operand_;

    format_t format_;

    /* see instruction::addressing_mode above */
    uint32_t addr_mode_;

    /* the source line from which this instruction was created,
     * used only for printing purposes */
    string_t line_;

    /* this is the value that will contain the output of the assembly */
    objcode_t objcode_;

    /* indexed instructions have their x bit set to 1 in the targeting flags */
    bool indexed_;

    /* the literal representation of the opcode */
    string_t mnemonic_;

    /** used only for printing purposes */
    uint8_t objcode_width_;

    // some assembler directives like RESB and RESW do not construct object code
    // this flag is set to false in such instructions
    bool assemblable_;

    reloc_records_t reloc_recs_;

    private:
	};

  typedef instruction instruction_t;
} // end of namespace
#endif // h_instruction_h
