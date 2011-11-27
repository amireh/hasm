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

#ifndef h_literal_h
#define h_literal_h

#include "instruction.hpp"
#include "operand.hpp"

namespace hax
{
  class literal : public instruction {
    public:
    typedef std::list<operand*> deps_t;

    literal() = delete;
		explicit literal(string_t const& in_value);
    literal(const literal& src);
		literal& operator=(const literal& rhs);
		virtual ~literal();

    virtual loc_t length() const;
    virtual void preprocess();
    virtual void assemble();

    void add_dependency(operand*);
    deps_t& dependencies();

    bool is_assembled() const;

    protected:
    deps_t deps_;
    bool is_ascii_;
    string_t stripped_;
    bool assembled_;

    void copy_from(const literal&);
	};
} // end of namespace
#endif // h_literal_h
