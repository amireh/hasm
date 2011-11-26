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

#ifndef h_parser_h
#define h_parser_h

#include "hax.hpp"
#include "instruction.hpp"
//~ #include "program_block.hpp"
#include "control_section.hpp"
#include <map>
#include <list>
#include <tuple>

namespace hax
{
  class symbol;
  class serializer;
  class parser {
    public:
    //typedef std::list<pblock_t*> pblocks_t;
    //typedef std::list<instruction_t*> instructions_t;
    typedef std::list<csect_t*> csects_t;

    static parser* singleton_ptr();
    static parser& singleton();

		virtual ~parser();

    void process(string_t const& in, string_t const& out);

    /**
     * looks up the opcode table for the given mnemonic token, if the op was not
     * found to be registered, ec will be set to 1, otherwise the opcode is returned
     * and ec is set to 0
     **/
    opcode_fmt_t opcode_from_token(string_t const&, int* ec);

    bool is_op(string_t const& token) const;
    bool is_directive(string_t const& token) const;

    loc_t base() const;
    void set_base(loc_t in_loc);

    //instructions_t const& instructions() const;
    //~ loc_t locctr() const;
    //~ pblock_t *pblock() const;
    //pblocks_t const& pblocks() const;

    /**
     * creates a new control section identified by in_name and assigns it as
     * the current section of the parser
     *
     * @note
     * this is called internally by directive::preprocess() when a START or CSECT
     * assembler directive is encountered
     **/
    void __register_section(std::string in_name);

    /**
     * returns a reference to the current control section being parsed
     **/
    csect_t* current_section() const;
    csect_t* sect() const;

    /**
     * assigns the block identified by in_name to be the currently used one in
     * the current control section
     **/
    //~ void switch_to_block(std::string in_name = "Unnamed");

    protected:
    //~ friend class serializer;

    private:
    static parser *__instance;

    typedef std::map<string_t, std::tuple<int, char> > optable_t;

    bool is_delimiter(char);
    void populate_optable();
    void register_op(std::string, opcode_t, format_t);

    instruction_t* parse_instruction(std::string const& in_line);

    //~ loc_t locctr_;

    //~ pblock_t *pblock_; // current program block
    csect_t *csect_; // current control section
    //pblocks_t pblocks_;

    optable_t optable_;
    //instructions_t instructions_;
    csects_t csects_;
    loc_t base_;

		parser();
    parser(const parser& src);
		parser& operator=(const parser& rhs);
	};
} // end of namespace
#endif // h_parser_h
