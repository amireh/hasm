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

#include "directive.hpp"
#include "parser.hpp"
#include "symbol_manager.hpp"
#include "operands/expression.hpp"
#include <cassert>

namespace hax
{
  extern bool VERBOSE;
  using utility::stringify;

	directive::directive(opcode_t in_opcode, string_t const& in_mnemonic, pblock_t* block)
  : instruction(in_opcode, in_mnemonic, block)
  {
    format_ = format::fmt_directive;
	}

	directive::~directive()
	{
	}

  directive::directive(const directive& src)
  : instruction(src)
  {
    copy_from(src);
  }

  directive& directive::operator=(const directive& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void directive::copy_from(const directive& src)
  {
    instruction::copy_from(src);
  }

  void directive::preprocess()
  {
    instruction::preprocess();
    length_ = 0;

    assemblable_ = false;
    symbol_manager* symmgr = pblock_->sect()->symmgr();

    if (mnemonic_ == "BYTE" || mnemonic_ == "WORD")
    {
      // BYTE and WORD directive operands need be either immediate constants, or a constant
      // absolute expression
      if (!operand_->is_constant() && !operand_->is_expression())
        throw invalid_operand(mnemonic_ + " directives operands can only be constant decimal integers", line_);

      bool is_word = mnemonic_ == "WORD";
      operand_->evaluate();
      if (is_word)
        length_ = 3;
      else
        length_ = operand_->length();

      assemblable_ = true;

    } else if (mnemonic_ == "RESB" || mnemonic_ == "RESW") {

      operand_->evaluate();
      if (operand_->is_expression() && !operand_->is_evaluated())
        throw invalid_operand("expressions in RESB and RESW operands must be evaluated", line_);

      bool is_word = mnemonic_ == "RESW";
      length_ = (is_word ? 3 : 1) * operand_->value();

      // assign the value of the label as the number of bytes/words reserved
      if (label_) {
        //~ label_->_assign_value(operand_->value());
      }

    } else if (mnemonic_ == "BASE") {

    } else if (mnemonic_ == "EQU") {
      length_ = 0;

      try {
        operand_->evaluate();
        label_->_assign_value(operand_->value());
        label_->set_has_real_value(true);
        //~ label_->assign_address(operand_->value());
      } catch (unevaluated_operand& e) {
        std::cerr << "Warning: " << e.what() << "\n";
      }

      if (operand_->is_symbol())
        static_cast<symbol*>(operand_)->set_has_real_value(true);

      if (!operand_->is_evaluated()) {
        string_t msg = "EQU operands must be either constant decimal integers, ";
        msg += "previously defined symbols, or expressions of previously defined symbols";
        throw invalid_operand(msg.c_str(), line_);
      }

    } else if (mnemonic_ == "USE") {
      std::string block_name;
      if (!operand_)
        block_name = "Unnamed";
      else
        block_name = operand_->token();

      pblock_->sect()->switch_to_block(block_name);
    } /*else if (mnemonic_ == "START" || mnemonic_ == "CSECT")
    {
      if (!operand_ || operand_->token().empty())
        throw invalid_operand(mnemonic_ + " entries must contain a name operand");

      parser::singleton().__register_section(operand_->token());
    }*/
    else if (mnemonic_ == "EXTREF") {
      std::vector<std::string> tokens = utility::split(operand_->token(), ',');
      for (auto token : tokens) {
        symbol_t* sym = symmgr->declare(token);
        symmgr->define(sym, 0x0, true /* assign both value and address to 0 */);
        sym->set_external_ref(true);
      }

      symmgr->__undefine(operand_->token());
      operand_ = 0;
    } else if (mnemonic_ == "EXTDEF") {
      std::cout << "Registering external symbol definitions:";

      //~ std::vector<std::string> tokens = ;
      for (auto token : utility::split(operand_->token(), ',')) {
        symbol_t* sym = symmgr->declare(token);
        sym->set_external_def(true);

        std::cout << sym->token() << " ";
      }
      std::cout << "\n";

      symmgr->__undefine(operand_->token());
      operand_ = 0;
    } else if (mnemonic_ == "LTORG") {
      parser::singleton().sect()->symmgr()->dump_literal_pool();
      //~ symmgr->dump_literal_pool();
    } else if (mnemonic_ == "END")
    {
      // if no starting instruction was assigned, just leave the control section's
      // starting address as 0x0
      if (!(operand_ && operand_->is_evaluated()))
        return;

      // extract the location of the instruction
      symbol_manager *symmgr = pblock_->sect()->symmgr();
      symbol_t *oper = symmgr->lookup(operand_->token());

      if (!oper)
        throw undefined_symbol("in END instruction: " + operand_->token());

      objcode_ = oper->address();
      pblock_->sect()->assign_starting_address(objcode_);
    }

    //~ construct_relocation_records();
  }

  loc_t directive::length() const
  {
    return length_;
  }

  void directive::assemble()
  {
    if (mnemonic_ == "BASE")
    {
      //~ std::cout << "-- assigned base register @ ";
      //symbol_t *operand = symbol_manager::singleton().lookup(operand_str_);
      //assert(operand);
      parser::singleton().set_base(operand_->value());

      if (VERBOSE)
      std::cout
        << "-- base register assigned @ "
        << std::hex << std::setw(4) << std::setfill('0') << parser::singleton().base()
        << "\n";

    } else if (mnemonic_ == "BYTE" || mnemonic_ == "WORD")
    {
      // BYTE constant values have already been evaluated in preprocess()
      if (!operand_->is_evaluated())
        operand_->evaluate();
      objcode_ = operand_->value();
      objcode_width_ = operand_->length() * 2;
    }
  }

  bool directive::is_valid() const
  {
    return true;
  }

} // end of namespace
