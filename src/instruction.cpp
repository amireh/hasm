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

#include "instruction.hpp"
#include "symbol.hpp"
#include "parser.hpp"
#include "symbol_manager.hpp"
#include "operands/constant.hpp"
#include "operands/symbol.hpp"

namespace hax
{
  using utility::stringify;

	instruction::instruction(opcode_t in_opcode, string_t const& in_mnemonic)
  : opcode_(in_opcode),
    location_(0),
    length_(0),
    label_(0),
    operand_(0),
    format_(format::fmt_undefined),
    addr_mode_(addressing_mode::undefined),
    objcode_(0x000000),
    indexed_(false),
    mnemonic_(in_mnemonic),
    objcode_width_(6),
    relocatable_(false),
    assemblable_(true)

  {
	}

	instruction::~instruction()
	{
    label_ = 0;
    if (!operand_->is_symbol())
      delete operand_;

    operand_ = 0;
	}

  instruction::instruction(const instruction& src)
  {
    copy_from(src);
  }

  instruction& instruction::operator=(const instruction& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void instruction::copy_from(const instruction& src)
  {
    this->opcode_ = src.opcode_;
    this->location_ = src.location_;
    this->label_ = src.label_;
    this->operand_str_ = src.operand_str_;
    this->format_ = src.format_;
    this->length_ = src.length_;
    this->addr_mode_ = src.addr_mode_;
    this->indexed_ = src.indexed_;
    this->operand_ = src.operand_;
  }

  /*void instruction::register_token(string_t const& in_token)
  {
    operands_.push_back(in_token);
  }*/

  void instruction::assign_location(loc_t in_loc)
  {
    location_ = in_loc;

    if (label_)
    {
      //~ std::cout << "defining symbol: " << label_->token() << " with address: " << location() << "\n";
      symbol_manager::singleton().define(label_, location());
    }
  }

  void instruction::assign_label(symbol_t* in_label)
  {
    label_ = in_label;

    //symbol_manager::singleton().declare(label_->label(), location());
  }

  void instruction::assign_operand(string_t const& in_token)
  {
    //~ operands_.push_back(in_token);
    operand_str_ = in_token;

    string_t operand_str(in_token);

    if (operand_str.find(",X") != std::string::npos)
    {
      indexed_ = true;
      operand_str = operand_str.substr(0, operand_str.size()-2);
    }

    // find out what kind of operand it is, there are three options:
    //  1. a constant, which could be an ASCII or HEX literal, or a decimal number
    //  2. a symbol
    //  3. expression

    operand_ = 0;
    // strip out any leading addressing mode flags (@ or #)
    if (operand_str[0] == '@' || operand_str[0] == '#')
      operand_str = operand_str.substr(1, operand_str.size()-1);

    // a constant
    if (__is_constant(operand_str)) {
      operand_ = new constant(in_token, this);
    } else if (__is_expression(operand_str)) {
      // TODO: implement expression support
    } else {
      // we do not own symbol objects, so we grab a reference and release it
      // when we're destructed
      operand_ = symbol_manager::singleton().declare(operand_str);
    }
  }

  loc_t instruction::location() const
  {
    return location_;
  }

  void instruction::preprocess()
  {
  }

  std::ostream& instruction::to_stream(std::ostream& out) const
  {
    out << std::uppercase;
    out << std::hex << std::setw(4) << std::setfill('0') << (int)location_;
    out << "\t";

    if (label_)
      out << "'" << label_->token() << "'";

    if (!label_ || label_->token().size() <= 5)
      out << "\t";

    out << "\t";
    out << mnemonic_;
    if (format_ != format::fmt_directive)
      out << "\t(0x" << std::hex << std::setw(2) << std::setfill('0') << (int)opcode_ << ")";
    else
      out << "\t";

    out << "\t";

    //for (auto token : operands_) out << " " << token;
    if (operand_) {
      out << operand_->token();
      if (operand_->token().empty())
        out << "\t";
    }

    if (objcode_)
      out << "\t\t" << std::hex << std::setw(objcode_width_) << std::setfill('0') << objcode_;

    out << std::nouppercase;

    return out;
  }

  string_t instruction::dump() const
  {
    std::ostringstream out;

    out << "Instruction:\n";
    out << "\tLine: " << line_ << "\n";
    out << "\tOpcode: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)opcode_ << std::endl;
    out << "\tLocation: 0x" << std::hex << std::setw(3) << std::setfill('0') << (int)location_ << std::endl;
    out << "\tLabel: " << (label_ ? label_->token() : "undefined") << "\n";

    /*out << "\tTokens [" << operands_.size() << "]:\n";
    int ctr = 0;
    for (auto token : operands_)
    {
      out << "\t\t" << ctr << ". " << token << "\n";
      ++ctr;
    }*/

    return out.str();
  }

  void instruction::resolve_references()
  {
  }

  void instruction::assign_line(string_t const& in_entry)
  {
    line_ = in_entry;
  }

  symbol_t const* const instruction::label() const
  {
    return label_;
  }

  uint32_t instruction::objcode() const
  {
    return objcode_;
  }

  bool instruction::is_assemblable() const
  {
    return assemblable_;
  }

  bool instruction::is_relocatable() const
  {
    return relocatable_;
  }

  string_t const& instruction::mnemonic() const
  {
    return mnemonic_;
  }

  bool instruction::__is_constant(string_t const& token)
  {
    return token.find("=C'") != std::string::npos
        || token.find("=X'") != std::string::npos
        || utility::is_decimal_nr(token);
  }

  bool instruction::__is_literal(string_t const& token)
  {
    return false;
  }

  bool instruction::__is_symbol(string_t const& token)
  {
    return symbol_manager::singleton().lookup(token) != 0;
  }

  bool instruction::__is_expression(string_t const& token)
  {
    // although we can use C++ TR1 regex here for a more robust solution,
    // for simplicity, I choose to stupidly scan the token for any operators
    // as they are only allowed within expressions
    for (auto c : token)
      if (utility::is_operator(c))
        return true;

    return false;
  }

} // end of namespace
