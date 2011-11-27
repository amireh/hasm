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

#ifndef h_symbol_manager_h
#define h_symbol_manager_h

#include "hax.hpp"
#include "instruction.hpp"
#include "instructions/literal.hpp"
#include "operands/symbol.hpp"
#include <map>

namespace hax
{
  class control_section;
  class symbol_manager {
    public:
    typedef std::map<string_t, symbol_t*> symbols_t;

		symbol_manager(control_section* in_sect);
		virtual ~symbol_manager();

    symbol_manager(const symbol_manager& src)=delete;
		symbol_manager& operator=(const symbol_manager& rhs)=delete;

    /**
     * registers a symbol with the given name in the symbol table if it hasn't
     * been declared already
     **/
    symbol_t* const declare(string_t const& in_name);

    /**
     * assigns the address in_loc to the given symbol in_symbol
     **/
    symbol_t* define(symbol_t* in_symbol, loc_t in_loc, bool value_and_address = false);

    /**
     * returns the symbol identified by name in_name, or 0 in case it wasn't found
     **/
    symbol_t* const lookup(string_t const& in_name) const;

    /**
     * convenience method for checking whether a symbol has been declared
     **/
    bool is_declared(string_t const& in_name) const;

    /**
     * convenience method for checking whether a symbol has been fully registered
     **/
    bool is_defined(string_t const& in_name) const;

    void dump(std::ostream& out) const;

    symbols_t const& symbols() const;

    void __undefine(string_t const& in_sym);

    instruction* declare_literal(string_t const& in_value, operand* in_dependency);
    instruction* lookup_literal(string_t const& in_value);

    /**
     * Extracts all literals registered in the pool to the current location
     * in the program block, and assembles them. This is called when LTORG
     * is encountered, and when a control section is assembled.
     *
     * When do_step is set to true, the program_block of this section will
     * step its location counter when the literal table is dumped. This is required
     * when no LTORG is specified, and the pool is dumped at the end of the file
     **/
    void dump_literal_pool(bool do_step = false);

    protected:
    typedef std::map<string_t, literal*> literals_t;
    literals_t literals_;

    symbols_t symbols_;
    control_section *sect_;

    private:
    //~ static symbol_manager *__instance;

	};
} // end of namespace
#endif // h_symbol_manager_h
