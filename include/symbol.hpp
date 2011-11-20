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
#include "loggable.hpp"
#include <vector>

namespace hax
{
  class symbol : public loggable {
    public:

    symbol()=delete;
		explicit symbol(string_t const& in_label);
    symbol(const symbol& src);
		symbol& operator=(const symbol& rhs);
		virtual ~symbol();

    string_t const& label() const;
    loc_t address() const;
    bool is_resolved() const;

    void set_address(loc_t);

    protected:

    string_t label_;
    loc_t address_;

    /**
     * the reason we use a flag to indicate whether this symbol has already been
     * defined is because we can't rely on address_ being 0x000 as that is a valid
     * address
     **/
    bool is_resolved_;

    void copy_from(const symbol&);

    private:

    virtual std::ostream& to_stream(std::ostream&) const;
	};

  typedef symbol symbol_t;
} // end of namespace
#endif // h_symbol_h
