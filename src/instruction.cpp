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

namespace hax
{
  using utility::stringify;

	instruction::instruction()
  : opcode_(0),
    location_(0),
    label_(0),
    format_(format::undefined)
  {
	}

	instruction::~instruction()
	{
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
    this->tokens_ = src.tokens_;
    this->format_ = src.format_;
    this->length_ = src.length_;
  }

  void instruction::register_token(string_t const& in_token)
  {
    tokens_.push_back(in_token);
  }

  void instruction::set_location(loc_t in_loc)
  {
    location_ = in_loc;

    if (label_)
    {
      symbol_manager::singleton().define(label_, location());
    }
  }

  loc_t instruction::location() const
  {
    return location_;
  }

  std::ostream& instruction::to_stream(std::ostream& out) const
  {
    out << "Instruction:\n";
    out << "\tOpcode: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)opcode_ << std::endl;
    out << "\tLocation: 0x" << std::hex << std::setw(3) << std::setfill('0') << (int)location_ << std::endl;
    out << "\tLabel: " << (label_ ? label_->label() : "undefined") << "\n";

    out << "\tTokens [" << tokens_.size() << "]:\n";
    int ctr = 0;
    for (auto token : tokens_)
    {
      out << "\t\t" << ctr << ". " << token << "\n";
      ++ctr;
    }

    return out;
  }

  void instruction::process_tokens()
  {
    // 1. find out whether this instruction is labelled, we do that by checking
    //    if the first registered token is a mnemonic for an opcode, if it isn't
    //    then the first token is a label
    bool has_label = false;
    int ec = -1;

    {
      std::string &token = tokens_.front();
      parser::singleton().opcode_from_token(token, &ec);
      has_label = ec != 0;
      if (has_label)
      {
        // declare the symbol in the symtable
        label_ = symbol_manager::singleton().declare(token);
        tokens_.pop_front();
      }

      ec = -1;
    }

    // parse the opcode & format now
    std::string &token = tokens_.front();
    opcode_fmt_t tuple = parser::singleton().opcode_from_token(token, &ec);
    if (ec != 0)
    {
      throw invalid_opcode("unregistered or identified opcode for mnemonic: " + token);
    }

    opcode_ = std::get<0>(tuple);
    format_ = std::get<1>(tuple);

    // determine whether it's a format 3 or 4 instruction
    if (is_sicxe())
    {
      // extended instruction opcode fields (format 4) are prefixed with '+'
      format_ = (token[0] == '+') ? format::four : format::three;
    }

    // rest of the tokens now are operands

  }

  bool instruction::is_sicxe() const
  {
    if ((format_ & format::three) == format::three)
      return true;

    if ((format_ & format::four) == format::four)
      return true;

    return false;
  }

  int instruction::nr_tokens() const
  {
    return tokens_.size();
  }

  loc_t instruction::length() const
  {
    switch(format_)
    {
      case format::one:
        return 1;
        break;
      case format::two:
        return 2;
        break;
      case format::three:
        return 3;
        break;
      case format::four:
        return 4;
        break;
      default:
        throw invalid_format("attempting to index length of an instruction with no format!");
    }
  }

} // end of namespace
