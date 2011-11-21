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
#include "hax.hpp"
#include "parser.hpp"
#include "hax_utility.hpp"

namespace hax {
  bool VERBOSE = false;
}

int main(int argc, char** argv)
{
  if (argc <= 1)
  {
    std::cout << "usage: hasm input.hasm [outfile]\n";
    return 0;
  }

  for (int i=2; i < argc; ++i)
  {
    if (std::string(argv[i]) == "-v")
      hax::VERBOSE = true;
  }

  //~ std::cout << std::hex << 0x33 - 0x0D << "\n"; return 1;

  // input hasm file
  std::string _in = argv[1];

  // destination of object program
  std::string _out = "a.obj";
  if (argc > 2)
    _out = argv[2];

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
