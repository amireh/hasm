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
#include "instruction.hpp"
#include <fstream>
#include <ostream>
#include <exception>
#include <stdexcept>

namespace hax
{

	symbol_manager* symbol_manager::__instance = 0;

	symbol_manager::symbol_manager()
  {
	}

	symbol_manager::~symbol_manager()
	{
    for (auto entry : symbols_)
    {
      delete entry.second;
    }
    symbols_.clear();
	}

	symbol_manager* symbol_manager::singleton_ptr()
  {
		return __instance = (!__instance) ? new symbol_manager() : __instance;
	}

	symbol_manager& symbol_manager::singleton()
  {
		return *singleton_ptr();
	}

  symbol_t *const symbol_manager::declare(string_t const& in_symbol)
  {
    symbol_t *sym = new symbol_t(in_symbol);
    symbols_.insert(std::make_pair(in_symbol, sym));
    return sym;
  }

  void symbol_manager::define(symbol_t *in_symbol, loc_t in_loc)
  {

  }

} // end of namespace
