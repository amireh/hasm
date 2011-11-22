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

	instruction::instruction(opcode_t in_opcode, string_t const& in_mnemonic)
  : opcode_(in_opcode),
    location_(0),
    length_(0),
    label_(0),
    format_(format::fmt_undefined),
    addr_mode_(addressing_mode::undefined),
    objcode_(0x000000),
    indexed_(false),
    mnemonic_(in_mnemonic),
    objcode_width_(6)
  {
	}

	instruction::~instruction()
	{
    label_ = 0;
    operands_.clear();
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
    this->operands_ = src.operands_;
    this->format_ = src.format_;
    this->length_ = src.length_;
    this->addr_mode_ = src.addr_mode_;
    this->indexed_ = src.indexed_;
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
    operands_.push_back(in_token);
    if (in_token.find(",X") != std::string::npos)
    {
      indexed_ = true;
      string_t &operand = operands_.back();
      operand = operand.substr(0, operand.size()-2);
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
      out << "'" << label_->label() << "'";

    if (!label_ || label_->label().size() <= 5)
      out << "\t";

    out << "\t";
    out << mnemonic_;
    if (format_ != format::fmt_directive)
      out << "\t(0x" << std::hex << std::setw(2) << std::setfill('0') << (int)opcode_ << ")";
    else
      out << "\t";

    out << "\t";

    for (auto token : operands_) out << " " << token;

    if (objcode_)
      out << "\t\t" << std::hex << std::setw(objcode_width_) << std::setfill('0') << objcode_;

    out << std::nouppercase;

    return out;
  }

  string_t instruction::dump() const
  {
    std::ostringstream out;

    out << "Instruction:\n";
    out << "\tOpcode: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)opcode_ << std::endl;
    out << "\tLocation: 0x" << std::hex << std::setw(3) << std::setfill('0') << (int)location_ << std::endl;
    out << "\tLabel: " << (label_ ? label_->label() : "undefined") << "\n";

    out << "\tTokens [" << operands_.size() << "]:\n";
    int ctr = 0;
    for (auto token : operands_)
    {
      out << "\t\t" << ctr << ". " << token << "\n";
      ++ctr;
    }

    return out.str();
  }

  /*void instruction::process_tokens()
  {
    if (operands_.empty())
      return;

    // if there's one token registered, then it's either a Format 1 instruction
    // or an assembler directive
    if (operands_.size() == 1)
    {
      // TODO: implement assembler directives
      // TODO: implement format 1 instruction handler
      format_ = format::directive;
      std::cout << "warning: assembler directives and format 1 instructions are not yet supported\n";
      return;
    }

    // idx is used to point at the current token we'll be processing
    int idx = 0;

    // if ec is set to anything other than 0 then the mnemonic for this instruction
    // is unrecognized (used by parser::opcode_from_token)
    int ec = -1;

    // 1. find out whether this instruction is labelled, we do that by checking
    //    if the first registered token is a mnemonic for an opcode, if it isn't
    //    then the first token is a label
    bool has_label = false;
    {
      std::string token = operands_.at(idx);
      if (token[0] == '+')
        token = token.substr(1, token.size());

      parser::singleton().opcode_from_token(token, &ec);
      has_label = ec != 0;
      if (has_label)
      {
        // declare the symbol in the symtable
        label_ = symbol_manager::singleton().declare(token);
        //operands_.pop_front();
      } else
        --idx;

      ec = -1;
    }

    // parse the opcode & format now
    //~ std::string &token = operands_.front();
    std::string token = operands_.at(++idx);
    if (token[0] == '+')
        token = token.substr(1, token.size());
    opcode_fmt_t tuple = parser::singleton().opcode_from_token(token, &ec);
    if (ec != 0)
    {
      std::string msg = "";
      for (auto token : operands_) msg += " " + token;

      throw invalid_opcode("unrecognized opcode for mnemonic:" + msg);
    }

    opcode_ = std::get<0>(tuple);
    format_ = std::get<1>(tuple);

    // determine whether it's a format 3 or 4 instruction
    if (is_sicxe())
    {
      // extended instruction opcode fields (format 4) are prefixed with '+'
      format_ = (token[0] == '+') ? format::four : format::three;
    }
    // discard this token, we're done with it
    ++idx;
    //~ operands_.pop_front();

    // is the operand indexed?
    indexed_ =
      operands_.back().size() > 2 &&
      operands_.back().substr(operands_.back().size() - 2) == ",X";

    // rest of the tokens now are operands
    //calc_target_address();
  }

  void instruction::calc_target_address()
  {
    string_t &token = operands_.front();

    if (is_sicxe())
    {
      // format 3 + 4 always have n and i bits set to 1
      // is it format 4?
      if ((format_ & format::four) == format::four)
      {
        // strip out the leading '#'
        token = token.substr(1, token.size());
      } else
      {

      }
      // find out
    }
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
    return operands_.size();
  }*/

  /*loc_t instruction::length() const
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
      case format::directive:
        return 3;
        // TODO: implement length count for assembler directives
        break;
      default:
        std::ostringstream out;
        out << this;
        throw invalid_format("attempting to index length of an instruction with no format: " + out.str());
    }
  }*/
  void instruction::resolve_references()
  {
    //if (resolved_)
    //  return;

    //for (auto operand : operands_)
    //{
    //
    //}
  }

  void instruction::assign_line(string_t const& in_entry)
  {
    line_ = in_entry;
  }
} // end of namespace
