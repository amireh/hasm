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
  /**
   * Symbols could be labels, user-defined symbols, or external references.
   *
   * A symbol might have a value in the case it's user-defined using the EQU
   * asm directive, an address unless it's an external reference, or both for
   * label definitions.
   **/
  class symbol : public operand {
    public:

		explicit symbol(string_t const& in_label, instruction* in_inst=0);
    symbol()=delete;
    symbol(const symbol& src);
		symbol& operator=(const symbol& rhs);
		virtual ~symbol();

    /**
     * nothing to evaluate here, symbols have their values/addresses externally
     **/
    virtual void evaluate();

    /**
     * sets the address of this symbol equal to in_address, effectively "evaluating" it
     **/
    void assign_address(loc_t in_address);

    /**
     * if it's a user-defined symbol, the returned value is the assigned one,
     * otherwise the address is returned as a value
     **/
    virtual uint32_t value() const;

    /**
     * the address of the instruction in which this symbol has first appeared
     * and is considered a label for
     **/
    loc_t address() const;

    /**
     * if a symbol is flagged as user-defined, symbol::value() will return
     * this symbol's _value_ field (which should be assigned externally),
     * otherwise the symbol will treat its address as its value
     **/
    void set_user_defined(bool f);

    bool is_user_defined() const;

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
    bool user_defined_;
    bool external_ref_;
    bool external_def_;
    //~ string_t label_;

    void copy_from(const symbol&);
    virtual std::ostream& to_stream(std::ostream&) const;
	};

  typedef symbol symbol_t;
} // end of namespace
#endif // h_symbol_h
