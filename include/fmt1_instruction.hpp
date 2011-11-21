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

#ifndef h_fmt1_instruction_h
#define h_fmt1_instruction_h

#include "instruction.hpp"

namespace hax
{
  class fmt1_instruction : public instruction {
    public:

    fmt1_instruction() = delete;
		explicit fmt1_instruction(opcode_t, const string_t&);
    fmt1_instruction(const fmt1_instruction& src);
		fmt1_instruction& operator=(const fmt1_instruction& rhs);
		virtual ~fmt1_instruction();

    virtual loc_t length() const;
    virtual void calc_target_address();
    virtual bool is_valid() const;

    protected:
    void copy_from(const fmt1_instruction&);

    private:
	};
} // end of namespace
#endif // h_fmt1_instruction_h
