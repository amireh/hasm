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

#ifndef h_symbol_h
#define h_symbol_h

#include "hax.hpp"
#include "operand.hpp"
#include <list>

namespace hax
{
  class symbol : public operand {
    public:

		explicit symbol(string_t const& in_label);
    symbol()=delete;
    symbol(const symbol& src);
		symbol& operator=(const symbol& rhs);
		virtual ~symbol();

    //~ string_t const& label() const;
    virtual void evaluate();

    void assign_address(loc_t in_address);

    virtual uint32_t value() const;

    loc_t address() const;
    void set_has_real_value(bool f);

    /**
     * symbols defined with an EXTREF asm directive are flagged as external
     * references and evaluate to 0 and must be targetting in immediate (fmt4) mode
     *
     * external references will be consumed in an R record by the Serializer
     **/
    bool is_external_ref() const;

    /**
     * symbols defined with an EXTDEF asm directive are flagged as external
     * definitions and will be consumed by the Serializer in a D record
     **/
    bool is_external_def() const;

    void set_external_ref(bool f);
    void set_external_def(bool f);

    protected:

    loc_t address_;
    bool has_real_value_;
    bool external_ref_;
    bool external_def_;
    //~ string_t label_;

    void copy_from(const symbol&);
    virtual std::ostream& to_stream(std::ostream&) const;
	};

  typedef symbol symbol_t;
} // end of namespace
#endif // h_symbol_h
