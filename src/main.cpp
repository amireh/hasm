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

bool is_operator(char c)
{
  switch (c)
  {
    case '(':
    case ')':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '^':
    return true;
    default:
    return false;
  }

  return false;
}

std::map<char, int> operator_weights;
bool has_precedence(char op1, char op2)
{
  operator_weights.insert(std::make_pair('(', 2));
  operator_weights.insert(std::make_pair(')', 2));
  operator_weights.insert(std::make_pair('*', 0));
  operator_weights.insert(std::make_pair('/', 0));
  operator_weights.insert(std::make_pair('%', 0));
  operator_weights.insert(std::make_pair('+', 1));
  operator_weights.insert(std::make_pair('-', 1));

  assert(is_operator(op1) && is_operator(op2));

  return operator_weights[op1] <= operator_weights[op2];
}

hax::string_t evalute_postfix(std::string in_expr, std::vector<std::string> in_operands)
{
  hax::string_t out = "";
  std::cout << "converting a postfix expr with " << in_operands.size() << " '" << in_expr << "'\n";
  std::vector<hax::string_t> operands;
  hax::string_t operand = "";
  for (char c : in_expr)
  {
    if (c == ' ')
    {
      if (!operand.empty())
      {
        operands.push_back(operand);
        operand = "";
      }
      continue;
    }

    if (!is_operator(c))
    {
      operand += c;
      continue;
    }

    std::cout << "applying op " << c << " on 2-top operands : " << operands.size() << "\n";

    assert(operands.size() >= 2);
    hax::string_t tmp = operands.back();
    operands.pop_back();
    hax::string_t res = "[" + operands.back() + " " + c + " " + tmp + "]";
    operands.pop_back();
    operands.push_back(res);
  }

  out = operands.back();

  return out;
}

hax::string_t to_postfix(hax::string_t const& in)
{
  std::vector<char> operators;
  std::vector<std::string> operands;

  std::string out, operand = "";
  for (char c : in)
  {
    std::cout << "checking " << c << " in " << in << "\n";
    if (!is_operator(c))
    {
      operand.push_back(c);
      //~ out.push_back(c);
      continue;
    }

    operands.push_back(operand);
    out += operand + " ";
    operand = "";

    // handle operators
    if (operators.empty())
    {
      if (c == ')')
        throw hax::invalid_expression("no equivalent '(' for ')'");

      operators.push_back(c);
      continue;
    }

    if (c == ')')
    {
      while (!operators.empty() && operators.back() != '(')
      {
        out.push_back(operators.back());
        operators.pop_back();
      }

      // remove the left paranthesis
      assert(!operators.empty());
      operators.pop_back();

      continue;
    } else if (c == '(')
    {
      operators.push_back(c);
      continue;
    }

    // pop any operators in the stack with higher priority than this one, add
    // them to the postfix string, and finally push this operator onto the stack
    while (!operators.empty() && has_precedence(operators.back(), c))
    {
      out.push_back(operators.back());
      operators.pop_back();
    }
    operators.push_back(c);
  }

  if (!operand.empty())
    out += operand;

  while (!operators.empty())
  {
    out.push_back(operators.back());
    operators.pop_back();
  }

  for (auto _operand : operands)
  {
    std::cout << "\tOperand: " << _operand << "\n";
  }

  return evalute_postfix(out, operands);
}


int main(int argc, char** argv)
{
  if (argc <= 1)
  {
    print_usage();
    return 0;
  }
  //~ hax::string_t infix_expr = "cool-bar*a*(f-3)";
  //~ std::cout << "infix: " << infix_expr << "\npostfix: " << to_postfix(infix_expr) << "\n";
  //~ return 1;

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
