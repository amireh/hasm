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
#include "operand_factory.hpp"

namespace hax
{
  using utility::stringify;

	instruction::instruction(opcode_t in_opcode, string_t const& in_mnemonic, pblock_t* in_block)
  : opcode_(in_opcode),
    location_(0),
    length_(0),
    label_(0),
    pblock_(in_block),
    operand_(0),
    format_(format::fmt_undefined),
    addr_mode_(addressing_mode::undefined),
    objcode_(0x000000),
    indexed_(false),
    mnemonic_(in_mnemonic),
    objcode_width_(6),
    assemblable_(true)
  {
	}

	instruction::~instruction()
	{
    pblock_ = 0;
    label_ = 0;
    if (!operand_->is_symbol())
      delete operand_;

    while (!reloc_recs_.empty())
    {
      delete reloc_recs_.back();
      reloc_recs_.pop_back();
    }

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
      symbol_manager *symmgr = parser::singleton().current_section()->symmgr();
      symmgr->define(label_, location());
    }
  }

  void instruction::assign_label(symbol_t* in_label)
  {
    label_ = in_label;

    //parser::singleton().current_section()->symmgr()->declare(label_->label(), location());
  }

  void instruction::assign_operand(string_t const& in_token)
  {
    operand_str_ = in_token;

    // is it an indexed operand?
    if (operand_str_.find(",X") != std::string::npos)
    {
      indexed_ = true;
      operand_str_ = operand_str_.substr(0, operand_str_.size()-2);
    }

    // create the operand object
    operand_ = operand_factory::singleton().create(operand_str_);
  }

  loc_t instruction::location() const
  {
    return location_;
  }

  void instruction::preprocess()
  {
    if (label_)
      label_->assign_address(location());
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

    if (objcode_ || (operand_ && operand_->is_evaluated()))
      out << "\t\t" << std::hex << std::setw(objcode_width_) << std::setfill('0') << objcode_;

    if (is_relocatable())
      out << "\t+relocatable with [" << reloc_recs_.size() << " M records]";

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
    return !reloc_recs_.empty();
  }

  string_t const& instruction::mnemonic() const
  {
    return mnemonic_;
  }

  instruction::reloc_records_t&
  instruction::reloc_records()
  {
    return reloc_recs_;
  }

  void instruction::construct_relocation_records()
  {
    if (!operand_)
      return;

    // if the operand is an expression, we need to go through all its sub-operands,
    // check if they're an external reference, and if so, add a relocation record
    if (operand_->is_expression())
    {
      expression* expr = static_cast<expression*>(operand_);

      std::map<string_t, size_t> index;

      string_t infix = utility::remove_in_string(expr->token(), ' ');
      for (auto ref : expr->references())
      {
        char sign = '+';
        if (ref->is_external_ref())
        {
          // scan the infix expression for this symbol and find its operator

          // register the symbol if it's the first time we're scanning for it
          size_t offset = 0;
          if (index.find(ref->token()) == index.end())
            index.insert(std::make_pair(ref->token(), 0));
          else
            offset = index[ref->token()]; // use the last saved offset

          // find the next occurence of this symbol
          size_t pos = infix.find(ref->token(), offset);

          // if pos == 0, then the sign is positive
          if (pos != 0) {
            if (infix[pos-1] == '-') sign = '-';
            else if (infix[pos-1] == '+' || infix[pos-1] == '(') sign = '+';
            else throw invalid_operand("can not evaluate the sign of external reference '" + ref->token() +
              "' in expression: " + infix);
          }

          // save the last position we found this token at, so if there's another
          // reference to it, we'll be at the correct offset
          index[ref->token()] = pos;

          // finally, save the record
          reloc_record_t* rec = construct_relocation_record(ref);
          rec->value[0] = sign;
          reloc_recs_.push_back(rec);

          std::cout << "** created a reloc record for expression: " << rec->value << "\n";
        }
      }
    }

    // if the operand is an external reference to a symbol, then it's straight-forward
    else if (operand_->is_symbol())
    {
      //~ std::cout << "info: evaluating relocation record for: " << line_;
      symbol_t* sym = static_cast<symbol*>(operand_);
      if (sym->is_external_ref())
      {
        reloc_recs_.push_back(construct_relocation_record(sym));
      }
      std::cout << " => " << reloc_recs_.size() << "\n";
    }
    else {
      //~ std::cout << "warn: unknown type of expression for relocation evaluation: " << line_ << "\n";
    }
  }

  instruction::reloc_record_t* instruction::construct_relocation_record(symbol_t* sym)
  {
    reloc_record_t* rec = new reloc_record_t();
    rec->value = "+" + sym->token();
    rec->length = (format_ == format::fmt_four) ? 0x05 : 0x06; // TODO: verify this
    return rec;
  }

  void instruction::postprocess()
  {
    construct_relocation_records();
  }

} // end of namespace
