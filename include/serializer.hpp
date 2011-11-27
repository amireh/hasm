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

#ifndef h_serializer_h
#define h_serializer_h

#include "hax.hpp"
#include "instruction.hpp"
#include "control_section.hpp"

namespace hax
{
  class serializer {
    public:

    static serializer* singleton_ptr();
    static serializer& singleton();

		virtual ~serializer();

    /**
     * writes the object program in out using the given set of instructions
     **/
    void process(csect_t* in_sect, string_t const& out);

    protected:

    private:
    static serializer *__instance;

		serializer();
    serializer(const serializer& src);
		serializer& operator=(const serializer& rhs);

    struct t_record {
      uint32_t length;
      uint32_t address;
      std::vector<instruction_t*> instructions;

      static const uint8_t maxlen;
    };

    struct m_record {
      uint32_t location;
      uint32_t length;
      instruction_t* inst;
    };

    /*struct d_record {
      uint32_t location;
      symbol_t* sym;
    };

    struct r_record {
      symbol_t* sym;
    };*/

    bool requires_new_trecord(t_record* rec, instruction_t* inst);
	};
} // end of namespace
#endif // h_serializer_h
