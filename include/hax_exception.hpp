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

#include "hax_types.hpp"
#include <exception>
#include <stdexcept>

namespace hax {

  /* ------------------------------------------------------------------------ */
  /* generic errors */
	class hax_error : public std::runtime_error {
	public:
		inline hax_error(const string_t& s, string_t type)
		: std::runtime_error(s),
      type_(type)
		{}
    inline virtual ~hax_error() throw() { }

    string_t const& type() { return type_; }
    string_t const& source() { return line_; }

    virtual const char* formatted_what() const throw()
    {
      string_t out = "ERROR '" + type_ + "': " + std::runtime_error::what();
      return out.c_str();
    }

    protected:
    string_t type_;
    string_t line_;
	};


  /* invalid_context
   *
   * this error is raised if the input program does not begin with a START
   * or CSECT operations, thus defining no context in which the entries should
   * be processed
   **/
	class invalid_context : public hax_error {
	public:
		inline invalid_context(const string_t& s)
		: hax_error(s, "invalid context")
		{ }
	};

  /* undefined symbol:
   *
   * raised when a symbol was referenced, was not flagged as an external reference
   * using EXTREF, and has not been defined throughout the control section
   **/
	class undefined_symbol : public hax_error {
	public:
		inline undefined_symbol(const string_t& s)
		: hax_error(s, "undefined symbol")
		{ }
	};

  /* ------------------------------------------------------------------------ */
  /* parser errors
   *
   * all of these exceptions are raised due to bad input, and for every error
   * the source line is passed so it would be conveniently reported to the user
   * where the error has occured
   **/
	class parser_error : public hax_error {
	public:
		inline parser_error(const string_t& s, string_t type, string_t const& line)
		: hax_error(s, type)
		{
      line_ = line;
    }

    inline virtual ~parser_error() throw() { }

    virtual const char* formatted_what() const throw()
    {
      string_t out = hax_error::what();
      out += ", source: " + line_;
      return out.c_str();
    }

	};

  /* invalid entry:
   *
   * raised when the parser encounters an invalid entry, which could be:
   *  1. an unrecognized opcode
   *  2. an attempt to redefine a control section
   *  3. opcode and/or operands are missing
   **/
	class invalid_entry : public parser_error {
	public:
		inline invalid_entry(const string_t& s, const string_t& line)
		: parser_error(s, "invalid entry", line)
		{ }
	};

  /* invalid addressing mode:
   *
   * raised if the input line specifies an addressing mode that doesn't make sense,
   * like using the indirect mode in an extended (format 4) instruction
   **/
	class invalid_addressing_mode : public parser_error {
	public:
		inline invalid_addressing_mode(const string_t& s, string_t const& line)
		: parser_error(s, "invalid addressing mode", line)
		{
    }
	};

  /* invalid operand:
   *
   * raised when:
   *  1. an external reference's sign could not be evaluated inside an expression
   *  2. RESB, RESW, BYTE, and WORD operations given a non-constant or absolute
   *     expression operand
   *  3. RESB and RESW given a non-evaluated expression
   *  4. EQU passed non-evaluated arguments, they all must be previously defined
   **/
	class invalid_operand : public parser_error {
	public:
		inline invalid_operand(const string_t& s, string_t const& line)
		: parser_error(s, "invalid operand", line)
		{ }
	};

  /* invalid operator:
   *
   * raised when an expression contains any operator that is not supported.
   * supported operators are: *  + - / * ( )
   **/
	class invalid_operator : public parser_error {
	public:
		inline invalid_operator(const string_t& s, string_t const& line)
		: parser_error(s, "invalid operator", line)
		{ }
	};

  /* invalid expression:
   *
   * denotes an expression that might:
   *  1. contain invalid correspondence in paranthesis
   *  2. contain an external reference that can not be substituted
   **/
	class invalid_expression : public parser_error {
	public:
		inline invalid_expression(const string_t& s, string_t const& line)
		: parser_error(s, "invalid expression", line)
		{ }
	};

  /* unrecognized operation:
   *
   * thrown when a given OPCODE could not be found in the master op table
   **/
	class unrecognized_operation : public parser_error {
	public:
		inline unrecognized_operation(const string_t& s, const string_t& line )
		: parser_error(s, "unrecognized operation", line)
		{ }
	};

  /* symbol redefinition:
   *
   * this error will be thrown if a label has been encountered more than once
   * at different locations in a single control section
   **/
	class symbol_redifinition : public parser_error {
	public:
		inline symbol_redifinition(const string_t& s, const string_t& line)
		: parser_error(s, "symbol redifinition", line)
		{ }
	};

  /* out of bounds:
   *
   * raised if a format 3 operand can not be addressed using neither PC or base-relative
   * modes, nor the immediate mode
   **/
	class target_out_of_bounds : public parser_error {
	public:
		inline target_out_of_bounds(const string_t& s, const string_t& line)
		: parser_error(s, "target out of bounds", line)
		{ }
	};

  /* ------------------------------------------------------------------------ */
  /* internal errors:
   *
   * these are not based on user input and indicate a bug or flaw in the
   * application's routines
   **/
	class internal_error : public hax_error {
	public:
		inline internal_error(const string_t& s, string_t type)
		: hax_error(s, type)
		{
    }
	};

  /* bad conversion
   *
   * thrown when an argument passed to utility::convertTo<> is not a number
   * and thus can not be converted
   **/
	class bad_conversion : public internal_error {
	public:
		inline bad_conversion(const string_t& s)
		: internal_error(s, "bad_conversion")
		{ }
	};

  /* unevaluated operand:
   *
   * raised when attempting to access an operand's value before calling its
   * operand::evaluate() routine, this is an internal error and should really
   * not happen
   **/
	class unevaluated_operand : public internal_error {
	public:
		inline unevaluated_operand(const string_t& s)
		: internal_error(s, "unevaluated operand")
		{ }
	};
} // end of namespace hax

#endif
