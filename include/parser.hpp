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
#include <map>
#include <list>
#include <tuple>

namespace hax
{
  class symbol;
  class serializer;
  class parser {
    public:

    typedef instruction inst_t;
    typedef std::list<inst_t*> instructions_t;

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

    instructions_t const& instructions() const;

    protected:
    friend class serializer;
    loc_t locctr() const;


    private:
    static parser *__instance;

    typedef std::map<string_t, std::tuple<int, char> > optable_t;

    bool is_delimiter(char);
    void populate_optable();
    void register_op(std::string, opcode_t, format_t);

    inst_t* parse_instruction(std::string const& in_line);

    loc_t locctr_;
    optable_t optable_;
    instructions_t instructions_;
    loc_t base_;

		parser();
    parser(const parser& src);
		parser& operator=(const parser& rhs);
	};
} // end of namespace
#endif // h_parser_h
