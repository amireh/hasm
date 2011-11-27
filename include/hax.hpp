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

#ifndef h_hax_h
#define h_hax_h

#include <string>
#include <tuple>
#include <iomanip>
#include <cassert>

#include "hax_exception.hpp"
#include "hax_utility.hpp"

namespace hax {

  typedef uint8_t opcode_t;
  typedef uint16_t loc_t;
  typedef uint32_t objcode_t;
  typedef std::string string_t;
  typedef char format_t;
  typedef std::tuple<opcode_t, format_t> opcode_fmt_t;

}
#endif
