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

#ifndef h_hax_exception_h
#define h_hax_exception_h

#include <exception>
#include <stdexcept>

namespace hax {

	class bad_conversion : public std::runtime_error {
	public:
		inline bad_conversion(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

	class invalid_format : public std::runtime_error {
	public:
		inline invalid_format(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

	class invalid_opcode : public std::runtime_error {
	public:
		inline invalid_opcode(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

	class undefined_symbol : public std::runtime_error {
	public:
		inline undefined_symbol(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	class symbol_redifinition : public std::runtime_error {
	public:
		inline symbol_redifinition(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

} // end of namespace hax

#endif
