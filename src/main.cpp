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
  bool DELIMITED_OUTPUT = false;
}

using hax::string_t;

void print_usage()
{
  std::cout << "Usage: hasm [OPTIONS] input_file\n";
  std::cout << "Re-run with --help for a list of supported arguments\n";
}

std::map<string_t, string_t> commands_;

void print_help()
{
  std::cout << "Usage: hasm [OPTIONS] input_file\n";
  std::cout
    << "Hax Assembler: translates SIC/XE compatible assembly listings "
    << "into loadable object programs.\n";

  commands_.insert(std::make_pair("-o FILE", "write object program into FILE (default: ./a.obj)"));
  commands_.insert(std::make_pair("-v", "runs in verbose mode (default: off)"));
  commands_.insert(std::make_pair("-d", "object program fields in output will be delimited \n\
  \t\t\tby '^' to be more human-readable (default: off)"));

  std::cout << "Optional arguments:\n";
  for (auto pair : commands_) {
    std::cout << "  " << pair.first << "\t\t";
    if (pair.first.size() < 3)
      std::cout << "\t";
    std::cout << pair.second << "\n";
  }
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
    return 0;
  }

  // destination of object program
  std::string _out = "a.obj";

  // parse arguments
  for (int i=1; i < argc; ++i)
  {
    if (std::string(argv[i]) == "-v")
      hax::VERBOSE = true;
    else if (std::string(argv[i]) == "-d")
      hax::DELIMITED_OUTPUT = true;
    else if (std::string(argv[i]) == "-o")
    {
      // make sure a path was specified
      if (!argc > i+1)
      {
        std::cerr << "invalid output destination\n";
        print_usage();
        return 0;
      }

      _out = std::string(argv[++i]);
    } else {
      std::cout << "warn: unknown option '" << argv[i] << ", ignoring\n";
    }
  }

  // input hasm file
  std::string _in = argv[argc-1];
  if (_in == _out) {
    std::cerr << "error: invalid input file '" << _in << "'; output file can not be the same as input file\n";
    return 0;
  }

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

  return 1;
}
