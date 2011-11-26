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

#include "parser.hpp"
#include "instruction.hpp"
#include "symbol_manager.hpp"
#include "instruction_factory.hpp"
#include "serializer.hpp"
#include <fstream>
#include <ostream>
#include <exception>
#include <stdexcept>

namespace hax
{
  extern bool VERBOSE;

	parser* parser::__instance = 0;

	parser::parser()
  {
    populate_optable();
	}

	parser::~parser()
	{
    while (!pblocks_.empty())
    {
      delete pblocks_.back();
      pblocks_.pop_back();
    }
	}

	parser* parser::singleton_ptr()
  {
		return __instance = (!__instance) ? new parser() : __instance;
	}

	parser& parser::singleton()
  {
		return *singleton_ptr();
	}

  void parser::register_op(std::string in_mnemonic, opcode_t in_code, format_t in_fmt)
  {
    optable_.insert(std::make_pair(in_mnemonic, std::make_tuple(in_code, in_fmt)));
  }

  void parser::populate_optable()
  {
    register_op("ADD",    0x18, format::fmt_three | format::fmt_four);
    register_op("ADDR",   0x90, format::fmt_two);
    register_op("AND",    0x40, format::fmt_three | format::fmt_four);
    register_op("CLEAR",  0xB4, format::fmt_two);
    register_op("COMP",   0x28, format::fmt_three | format::fmt_four);
    register_op("COMPR",  0xA0, format::fmt_two);
    register_op("DIV",    0x24, format::fmt_three | format::fmt_four);
    register_op("DIVR",   0x9C, format::fmt_two);
    register_op("HIO",    0xF4, format::fmt_one);
    register_op("J",      0x3C, format::fmt_three | format::fmt_four);
    register_op("JEQ",    0x30, format::fmt_three | format::fmt_four);
    register_op("JGT",    0x34, format::fmt_three | format::fmt_four);
    register_op("JLT",    0x38, format::fmt_three | format::fmt_four);
    register_op("JSUB",   0x48, format::fmt_three | format::fmt_four);
    register_op("LDA",    0x00, format::fmt_three | format::fmt_four);
    register_op("LDB",    0x68, format::fmt_three | format::fmt_four);
    register_op("LDCH",   0x50, format::fmt_three | format::fmt_four);
    register_op("LDF",    0x70, format::fmt_three | format::fmt_four);
    register_op("LDL",    0x08, format::fmt_three | format::fmt_four);
    register_op("LDS",    0x6C, format::fmt_three | format::fmt_four);
    register_op("LDT",    0x74, format::fmt_three | format::fmt_four);
    register_op("LDX",    0x04, format::fmt_three | format::fmt_four);
    register_op("MUL",    0x20, format::fmt_three | format::fmt_four);
    register_op("OR",     0x44, format::fmt_two);
    register_op("RD",     0xD8, format::fmt_three | format::fmt_four);
    register_op("RSUB",   0x4C, format::fmt_three | format::fmt_four);
    register_op("SHIFTL", 0xA4, format::fmt_two);
    register_op("SHIFTR", 0xA8, format::fmt_two);
    register_op("SIO",    0xF0, format::fmt_one);
    register_op("STA",    0x0C, format::fmt_three | format::fmt_four);
    register_op("STB",    0x78, format::fmt_three | format::fmt_four);
    register_op("STCH",   0x54, format::fmt_three | format::fmt_four);
    register_op("STI",    0xD4, format::fmt_three | format::fmt_four);
    register_op("STL",    0x14, format::fmt_three | format::fmt_four);
    register_op("STS",    0x7C, format::fmt_three | format::fmt_four);
    register_op("STSW",   0xE8, format::fmt_three | format::fmt_four);
    register_op("STT",    0x84, format::fmt_three | format::fmt_four);
    register_op("STX",    0x10, format::fmt_three | format::fmt_four);
    register_op("SUB",    0x1C, format::fmt_three | format::fmt_four);
    register_op("SUBR",   0x94, format::fmt_two);
    register_op("TD",     0xE0, format::fmt_three | format::fmt_four);
    register_op("TIO",    0xF8, format::fmt_one);
    register_op("TIX",    0x2C, format::fmt_three | format::fmt_four);
    register_op("TIXR",   0xB8, format::fmt_two);
    register_op("WD",     0xDC, format::fmt_three | format::fmt_four);

    register_op("START",  0x00, format::fmt_directive);
    register_op("END",    0x00, format::fmt_directive);
    register_op("USE",    0x00, format::fmt_directive);
    register_op("CLEAR",  0x00, format::fmt_directive);
    register_op("EQU",    0x00, format::fmt_directive);
    register_op("RESW",   0x00, format::fmt_directive);
    register_op("RESB",   0x00, format::fmt_directive);
    register_op("BYTE",   0x00, format::fmt_directive);
    register_op("WORD",   0x00, format::fmt_directive);
    register_op("ORG",    0x00, format::fmt_directive);
    register_op("LTORG",  0x00, format::fmt_directive);
    register_op("BASE",  0x00, format::fmt_directive);
    register_op("*",      0x00, format::fmt_directive);

    std::cout << "+- Registered " << optable_.size() << " SIC/XE operations & assembler directives.\n";
  }

  bool parser::is_op(string_t const& in_token) const
  {
    string_t token(in_token);
    if (token[0] == '+')
      token = token.substr(1, token.size());

    return optable_.find(token) != optable_.end();
  }

  bool parser::is_directive(string_t const& in_token) const
  {
    string_t token(in_token);
    if (token[0] == '+')
      token = token.substr(1, token.size());

    optable_t::const_iterator entry = optable_.find(token);
    if (entry == optable_.end())
      return false;

    return std::get<1>(entry->second) == format::fmt_directive;
  }

  bool parser::is_delimiter(char c)
  {
    switch(c)
    {
      case ' ':
      case '\t':
      return true;

      default:
      return false;
    }

    return false;
  }

  opcode_fmt_t parser::opcode_from_token(string_t const& in_token, int* ec)
  {
    string_t token = in_token;
    if (token[0] == '+')
      token = token.substr(1, token.size());

    optable_t::const_iterator opcode = optable_.find(token);
    if (opcode == optable_.end())
    {
      *ec = 1;
      return std::make_tuple(0,0);
    }

    *ec = 0;
    return opcode->second;
  }

  void parser::process(string_t const& in_path, string_t const& out_path)
  {
    std::ifstream in(in_path);
    if (!in.is_open() || !in.good())
    {
      throw std::runtime_error("can not open input file: " + in_path);
    }

    // __DEBUG__ : skip the START record
    //~ while (in.get() != '\n');;

    std::cout << "+- Pass1: \n";
    std::cout << "+- \n";
    std::cout << "+- Analyzing entries...\n";

    // begin the default program block
    switch_to_block("Unnamed");

    while (!in.eof())
    //~ for (int i=0; i < 4; ++i)
    {
      string_t line = "";

      // parse the line first
      while (true)
      {
        char c = in.get();
        if (c == '\n' || in.eof())
          break;

        line += c;
      }

      // discard the last empty-line entry
      if (in.eof())
        break;

      { // prepare the entry for parsing

        // trim whitespace
        utility::itrim(line);

        // is it a full comment? if so, discard this entry
        if (line.front() == '.' || line.front() == ';')
          continue;

        // remove any inline comments
        size_t comment_idx = line.find('.');
        if (comment_idx != std::string::npos) {
          line = line.substr(0, comment_idx);
          // re-trim in case there were spaces before the comment
          utility::itrim(line);
        }

        comment_idx = line.find(';');
        if (comment_idx != std::string::npos) {
          line = line.substr(0, comment_idx);
          // re-trim in case there were spaces before the comment
          utility::itrim(line);
        }
      } // entry preparation block

      //~ std::cout << "Entry is now ready for parsing: '" << line << "'\n";

      // parse tokens and register them

      std::list<string_t> tokens;
      string_t token = "";
      for (char c : line)
      {
        if (is_delimiter(c))
        {
          if (!token.empty())
          {
            //inst->register_token(token);
            tokens.push_back(token);
            token.clear();
          }
        } else
        {
          token += c;
        }
      }

      if (!token.empty())
        tokens.push_back(token);

      if (tokens.empty())
        continue;

      instruction* inst = 0;
      symbol_t* label = 0;

      // find out whether the first token is a label or an opcode
      if (!is_op(tokens.front()))
      {
        label = symbol_manager::singleton().declare(tokens.front());
        tokens.pop_front();
      }

      // validation check: was it only a label entry?
      if (tokens.empty())
        throw invalid_entry("missing opcode and operands in entry: " + line);

      else if (!is_op(tokens.front()))
        throw invalid_opcode("unrecognized operation: " + tokens.front());

      inst = instruction_factory::singleton().create(tokens.front());
      tokens.pop_front();

      if (label)
        inst->assign_label(label);


      // assign label & operands
      assert(tokens.size() <= 1);
      for (auto _token : tokens)
      {
        inst->assign_operand(_token);
      }

      inst->assign_line(line);
      pblock_->add_instruction(inst);
      //~ inst->assign_location(locctr_);
      inst->preprocess();
      pblock_->step();
      //~ locctr_ += inst->length();

      std::cout << inst << "\n";

      instructions_.push_back(inst);
      inst = 0;
    }

    std::cout << "\n+-\n";
    if (VERBOSE)
      symbol_manager::singleton().dump(std::cout);
    std::cout << "+- Pass2\n";
    std::cout << "+- Calculating object code...\n";

    int idx = 0;
    for (auto block : pblocks_) {
      std::cout << "Assembling instructions in program block '" << block->name() << "'\n";
      block->shift(idx);
      idx += block->locctr();
    }

    //~ for (auto block : pblocks_) {
      for (auto inst : instructions_)
      {
        //~ inst->resolve_references();
        inst->assemble();
        std::cout << inst << "\n";
      }
    //~ }

    serializer::singleton().process(out_path);

    in.close();
  }

  loc_t parser::base() const
  {
    return base_;
  }
  void parser::set_base(loc_t in_loc)
  {
    base_ = in_loc;
  }

  pblock_t* parser::pblock() const
  {
    return pblock_;
  }
  parser::pblocks_t const& parser::pblocks() const
  {
    return pblocks_;
  }

  void parser::switch_to_block(std::string in_name)
  {
    for (auto block : pblocks_)
      if (block->name() == in_name) {
        pblock_ = block;
        return;
      }

    pblock_ = new program_block(in_name);
    pblocks_.push_back(pblock_);
  }

  const parser::instructions_t& parser::instructions() const
  {
    return instructions_;
  }
} // end of namespace
