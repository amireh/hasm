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
#include <fstream>
#include <ostream>
#include <exception>
#include <stdexcept>

namespace hax
{

	parser* parser::__instance = 0;

	parser::parser()
  : locctr_(0)
  {
    populate_optable();
	}

	parser::~parser()
	{
    while (!instructions.empty())
    {
      delete instructions.back();
      instructions.pop_back();
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

  void parser::populate_optable()
  {
    using std::make_pair;
    using std::make_tuple;

    optable_.insert(make_pair("ADD", make_tuple<opcode_t, format_t>(0x18, format::three | format::four)));
    optable_.insert(make_pair("STL", make_tuple<opcode_t, format_t>(0x14, format::three | format::four)));
    optable_.insert(make_pair("JSUB", make_tuple<opcode_t, format_t>(0x48, format::three | format::four)));
    optable_.insert(make_pair("LDA", make_tuple<opcode_t, format_t>(0x00, format::three | format::four)));
    optable_.insert(make_pair("COMP", make_tuple<opcode_t, format_t>(0x28, format::three | format::four)));
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
    optable_t::const_iterator opcode = optable_.find(in_token);
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

    std::ofstream out(out_path);
    if (!out.is_open() || !out.good())
    {
      throw std::runtime_error("can not open output file: " + out_path);
    }

    // __DEBUG__ : skip the START record
    while (in.get() != '\n');;

    //~ while (!in.eof())
    for (int i=0; i < 4; ++i)
    {
      string_t token = "";
      instruction* inst = new instruction();
      while (true)
      {
        char c = in.get();

        if (c == '\n' || in.eof())
        {
          //std::cout << "found an end of line : " << c << ", breaking\n";
          if (!token.empty())
          {
            inst->register_token(token);
            token = "";
          }
          break;
        }

        if (is_delimiter(c))
        {
          if (!token.empty())
          {
            inst->register_token(token);
            token = "";
          }
        } else
        {
          token += c;
        }
      }

      inst->process_tokens();
      inst->set_location(locctr_);

      locctr_ += inst->length();

      std::cout << inst;

      instructions.push_back(inst);
      inst = 0;
    }

    // sometimes editors append an empty line to files for historical reasons,
    // so we remove that entry since the parser mistook it for an empty mnemonic
    // instruction
    if (instructions.back()->nr_tokens() == 0)
    {
      delete instructions.back();
      instructions.pop_back();
    }

    in.close();
    out.close();

  }

} // end of namespace
