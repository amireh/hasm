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

#ifndef h_directive_h
#define h_directive_h

#include "instruction.hpp"

namespace hax
{
  class directive : public instruction {
    public:

    directive() = delete;
		explicit directive(opcode_t, string_t const& in_mnemonic, pblock_t* block);
    directive(const directive& src);
		directive& operator=(const directive& rhs);
		virtual ~directive();

    virtual loc_t length() const;
    virtual void assemble();
    virtual bool is_valid() const;
    virtual void preprocess();

    protected:
    void copy_from(const directive&);

    string_t op_;

    private:
	};
} // end of namespace
#endif // h_directive_h
