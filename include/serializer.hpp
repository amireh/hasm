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
  /**
   * converts a control section assembly into an object program and writes it
   * to an output file
   **/
  class serializer {
    public:

    static serializer* singleton_ptr();
    static serializer& singleton();

		virtual ~serializer();

    /**
     * writes the object program in out using the given control section
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
      //~ instruction_t* inst;
      string_t value;
    };

    /**
     * a new T record is required when:
     *  1. the current rec's length will be exceeded with the given inst
     *  2. the current rec's length is already at a maximum
     *  3. an instruction is encountered that requires a new record such as
     *     USE, RESB, RESW
     **/
    bool requires_new_trecord(t_record* rec, instruction_t* inst);
	};
} // end of namespace
#endif // h_serializer_h
