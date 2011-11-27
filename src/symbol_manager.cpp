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

#include "symbol_manager.hpp"
#include "control_section.hpp"
#include "instruction.hpp"
#include "parser.hpp"
#include "instructions/directive.hpp"
#include <fstream>
#include <ostream>
#include <exception>
#include <stdexcept>

namespace hax
{
  extern bool VERBOSE;
	//~ symbol_manager* symbol_manager::__instance = 0;

	symbol_manager::symbol_manager(control_section* in_sect)
  : sect_(in_sect)
  {

    // special symbol for internal usage:
    // format3/4 instructions that require no arguments (like RSUB) are assigned
    // "0" as their operand, hence we define
    // "0" as an actual symbol that points to address 0x0000, in the spirit of
    // keeping a uniform interface for all format3/4 instructions
    define(declare("0"), 0, true);

    // SIC/XE registers
    define(declare("A"),  0x00, true);
    define(declare("X"),  0x01, true);
    define(declare("L"),  0x02, true);
    define(declare("B"),  0x03, true);
    define(declare("S"),  0x04, true);
    define(declare("T"),  0x05, true);
    define(declare("F"),  0x06, true);
    define(declare("PC"), 0x08, true);
    define(declare("SW"), 0x09, true);

    define(declare("*"), 0x0, false);
	}

	symbol_manager::~symbol_manager()
	{
    literals_.clear();

    for (auto entry : symbols_)
    {
      delete entry.second;
    }
    symbols_.clear();

    sect_ = 0;
	}

	/*symbol_manager* symbol_manager::singleton_ptr()
  {
		return __instance = (!__instance) ? new symbol_manager() : __instance;
	}

	symbol_manager& symbol_manager::singleton()
  {
		return *singleton_ptr();
	}*/

  symbol_t *const symbol_manager::declare(string_t const& in_symbol)
  {
    symbols_t::const_iterator finder = symbols_.find(in_symbol);
    if (finder != symbols_.end())
      return finder->second;

    symbol_t *sym = new symbol_t(in_symbol);
    symbols_.insert(std::make_pair(in_symbol, sym));
    return sym;
  }

  symbol_t* symbol_manager::define(symbol_t *in_symbol, loc_t in_loc, bool value_and_address)
  {
    in_symbol->assign_address(in_loc);
    if (value_and_address)
      in_symbol->_assign_value(in_loc);

    return in_symbol;
  }

  symbol_t *const symbol_manager::lookup(string_t const& in_label) const
  {
    symbols_t::const_iterator entry = symbols_.find(in_label);
    if (entry == symbols_.end())
      return 0;

    return entry->second;
  }

  bool symbol_manager::is_declared(string_t const& in_name) const
  {
    return symbols_.find(in_name) != symbols_.end();
  }

  bool symbol_manager::is_defined(string_t const& in_name) const
  {
    symbol_t *const sym = lookup(in_name);
    if (!sym)
      return false;

    return sym->is_evaluated();
  }

  void symbol_manager::dump(std::ostream& out) const
  {
    out << "+- Listing " << symbols_.size() << " symbols:\n";
    int i=0;
    for (auto entry : symbols_)
    {
      symbol_t *symbol = entry.second;
      out << std::uppercase
      << "  " << ++i << ". "
      << symbol->token() << "\t"
      << (symbol->is_evaluated() ? "TRUE" : "FALSE") << "\t"
      << std::hex << symbol->value() << "\n";
    }
  }

  symbol_manager::symbols_t const&
  symbol_manager::symbols() const
  {
    return symbols_;
  }

  void symbol_manager::__undefine(string_t const& in_sym)
  {
    symbol_t *sym = lookup(in_sym);
    if (sym) {
      //~ delete sym;
      symbols_.erase(in_sym);
    }
  }

  instruction* symbol_manager::declare_literal(string_t const& in_value, operand* in_dep)
  {
    // if this literal has been declared in this pool before, do nothing
    literals_t::iterator finder = literals_.find(in_value);
    if (finder != literals_.end()) {
      finder->second->add_dependency(in_dep);
      return finder->second;
    }


    literal* lit = new literal(in_value, sect_->block());
    lit->add_dependency(in_dep);
    //literal->assign_label(lookup("*"));
    //~ operand* oper = new constant(in_value);
    //~ symbol* oper = new symbol(in_value);
    //~ literal->assign_operand(oper);
    literals_.insert(std::make_pair(in_value, lit));

    std::cout << "registered literal with value: " << in_value << "\n";
    return lit;
  }

  void symbol_manager::dump_literal_pool(bool do_step)
  {
    program_block* block = sect_->block();

    if (VERBOSE)
      std::cout << "-- Dumping the literal pool \n";

    for (auto entry : literals_)
    {
      literal* lit = entry.second;
      if (lit->is_assembled())
        continue;

      block->add_instruction(lit);
      lit->assign_operand(entry.first);
      lit->preprocess();

      if (VERBOSE)
        std::cout << "Literal : " << lit << "\n";

      //~ do_step = !lit->is_assembled();
      lit->assemble();

      //~ if (do_step)
        block->step();
    }
    std::cout << "-- Literal pool created\n";
    //~ literals_.clear();
  }

  instruction* symbol_manager::lookup_literal(string_t const& in_value)
  {
    literals_t::iterator finder = literals_.find(in_value);
    if (finder != literals_.end())
      return finder->second;

    std::cout
      << "\tFATAL: Unable to find requested literal " << in_value
      << ", current literal table in " << sect_->name() << " is:\n";
    for (auto entry : literals_)
      std::cout << "\t" << entry.first << " => " << entry.second << "\n";

    throw internal_error("unable to find literal with value: " + in_value, "literal table corruption");
  }
} // end of namespace
