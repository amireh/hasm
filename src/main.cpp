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

#include <iostream>
#include <cassert>
#include <map>
#include "hax.hpp"
#include "parser.hpp"
#include "hax_utility.hpp"

namespace hax {
  bool VERBOSE = false;
}

using hax::string_t;

void print_usage()
{
  std::cout << "Usage: hasm INPUT [OUTFILE]\n";
  std::cout << "Re-run with --help for a list of supported arguments\n";
}

std::map<string_t, string_t> commands_;

void print_help()
{
  std::cout << "Usage: hasm INPUT [OUTFILE]\n";
  std::cout
    << "Hax Assembler: translates SIC/XE compatible assembly listings "
    << "into loadable object programs.\n";

  commands_.insert(std::make_pair("-v", "runs in verbose mode"));

  std::cout << "Optional arguments:\n";
  for (auto pair : commands_)
    std::cout << "  " << pair.first << "\t\t\t" << pair.second << "\n";
}


int main(int argc, char** argv)
{
  if (argc <= 1)
  {
    print_usage();
    return 0;
  }

  string_t first_arg(argv[1]);
  if (first_arg.find("help") != std::string::npos)
  {
    print_help();
    return 1;
  }

  for (int i=2; i < argc; ++i)
  {
    if (std::string(argv[i]) == "-v")
      hax::VERBOSE = true;
  }

  // input hasm file
  std::string _in = argv[1];

  // destination of object program
  std::string _out = "a.obj";
  if (argc > 2)
    _out = argv[2];

  std::cout << "+- Hax Assembler engaged -+\n";
  std::cout << "+-\tInput: " << _in << '\n';
  std::cout << "+-\tOutput: " << _out << '\n';
  std::cout << "+-\tRerun with --help for list of supported arguments\n";


  hax::parser& _parser = hax::parser::singleton();
  //try {
    _parser.process(_in, _out);
  /*} catch (std::exception& e) {
    std::cerr << "fatal: an exception was raised during parsing `" << e.what() << "`\n";
    std::cerr << "aborted\n";
    return 1;
  }*/

  return 0;
}
