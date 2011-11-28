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
#include <typeinfo>

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
    register_op("CSECT",  0x00, format::fmt_directive);
    register_op("END",    0x00, format::fmt_directive);
    register_op("EXTREF", 0x00, format::fmt_directive); // TODO: implement
    register_op("EXTDEF", 0x00, format::fmt_directive); // TODO: implement
    register_op("USE",    0x00, format::fmt_directive);
    register_op("CLEAR",  0x00, format::fmt_directive);
    register_op("EQU",    0x00, format::fmt_directive);
    register_op("RESW",   0x00, format::fmt_directive);
    register_op("RESB",   0x00, format::fmt_directive);
    register_op("BYTE",   0x00, format::fmt_directive);
    register_op("WORD",   0x00, format::fmt_directive);
    register_op("ORG",    0x00, format::fmt_directive); // TODO: implement
    register_op("LTORG",  0x00, format::fmt_directive); // TODO: implement
    register_op("BASE",   0x00, format::fmt_directive);
    register_op("*",      0x00, format::fmt_directive); // TODO: implement

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
    int line_nr = 0;

    while (!in.eof())
    //~ for (int i=0; i < 4; ++i)
    {
      string_t line = "";
      ++line_nr;

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

      // check whether this is a CSECT or START entry
      if (tokens.size() >= 2 && (line.find("START") != std::string::npos || line.find("CSECT") != std::string::npos))
      {
        __register_section(tokens.front(), line);
      }

      // if by now we were not assigned a control section, abort
      if (!csect_) {
        throw invalid_context("an input program must begin with a START or CSECT entry to define a control section!");
      }

      // find out whether the first token is a label or an opcode
      if (!is_op(tokens.front()))
      {
        if (csect_->symmgr()->is_defined(tokens.front()))
          throw symbol_redifinition("token '" + tokens.front() + "'", line);

        try {
          label = csect_->symmgr()->declare(tokens.front());
          tokens.pop_front();
        } catch (hax_error& e) {
          track_error(e);
          continue; // can't proceed if label couldn't be defined
        }
      }

      // validation check: was it only a label entry?
      if (tokens.empty())
        throw invalid_entry("missing opcode and operands in entry: ", line);

      else if (!is_op(tokens.front()))
        throw invalid_entry("unrecognized operation: " + tokens.front(), line);

      try {
        inst = instruction_factory::singleton().create(tokens.front(), csect_->block());
        tokens.pop_front();
      } catch (hax_error& e)
      {
        track_error(e);
      }

      if (label)
        inst->assign_label(label);

      // assign label & operands
      assert(tokens.size() <= 1);
      for (auto _token : tokens)
      {
        try {
          inst->assign_operand(_token);
        } catch (hax_error& e)
        {
          track_error(e);
        }
      }

      inst->assign_line(line);
      csect_->block()->add_instruction(inst);
      try {
        inst->preprocess();
      } catch (hax_error& e) {
        track_error(e);
      }
      csect_->block()->step(inst);

      std::cout << inst << "\n";

      inst = 0;
    }

    std::cout << "+-\n";
    if (VERBOSE)
      csect_->symmgr()->dump(std::cout);

    // dump stats
    std::cout
      << "+- Pass1: " << (errors_.empty() ? "complete" : "failed") << "\n";

    if (!errors_.empty())
    {
      return report_errors();
    }

    std::cout
      << "+-\tNumber of control sections: " << csects_.size() << "\n";
    for (auto sect : csects_)
      std::cout << sect;


    std::cout << "+- Pass2\n";
    std::cout << "+- Assembling object code...\n";

    for (auto sect : csects_) {
      sect->assemble();
      sect->serialize(out_path);
    }

    in.close();

    std::cout << "+- Pass2: " << (errors_.empty() ? "complete" : "failed") << "\n";
    if (!errors_.empty())
    {
      return report_errors();
    }
  }

  loc_t parser::base() const
  {
    return base_;
  }
  void parser::set_base(loc_t in_loc)
  {
    base_ = in_loc;
  }

  csect_t* parser::current_section() const
  {
    return csect_;
  }
  csect_t* parser::sect() const
  {
    return csect_;
  }

  void parser::__register_section(std::string in_name, const string_t& in_line)
  {
    // verify no other control section is already registered with this name
    for (auto sect : csects_)
      if (sect->name() == in_name)
        throw invalid_entry("attempt to re-define a control section named '" + in_name + "'", in_line);

    std::cout << "info: registering a control section '" << in_name << "'\n";
    csect_t *new_sect = new control_section(in_name);
    csects_.push_back(new_sect);
    csect_ = new_sect;
  }

  void parser::report_errors() const
  {
    for (const string_t& e : errors_) {
      std::cout << e;
    }
  }

  void parser::track_error(hax_error& err)
  {
    /*hax_error* errp = &err;
    errp = dynamic_cast<parser_error*>(errp);
    if (errp) {
      errors_.push_back(errp->what());
      return;
    }

    errp = dynamic_cast<internal_error*>(errp);
    if (errp) {
      errors_.push_back(errp->what());
      return;
    }*/
    std::ostringstream s;
    s << "+- ERROR '" << err.type() << "': " << err.what()
      << " (in \"" << err.source() << "\")\n";
    errors_.push_back(s.str());
  }
} // end of namespace
