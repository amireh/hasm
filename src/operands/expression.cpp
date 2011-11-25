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

#include "operands/expression.hpp"
#include "symbol_manager.hpp"

namespace hax
{
  using utility::stringify;
  using utility::is_operator;

  expression::weights_t expression::operator_weights;

	expression::expression(string_t const& in_token)
  : operand(in_token),
    absolute_(false)
  {
    type_ = t_expression;

    if (operator_weights.empty())
    {
      operator_weights.insert(std::make_pair('(', 2));
      operator_weights.insert(std::make_pair(')', 2));
      operator_weights.insert(std::make_pair('*', 0));
      operator_weights.insert(std::make_pair('/', 0));
      operator_weights.insert(std::make_pair('%', 0));
      operator_weights.insert(std::make_pair('+', 1));
      operator_weights.insert(std::make_pair('-', 1));
    }
	}

	expression::~expression()
	{
	}

  expression::expression(const expression& src) : operand(src.token_)
  {
    copy_from(src);
  }

  expression& expression::operator=(const expression& rhs)
  {
    if (this != &rhs)
      copy_from(rhs);

    return *this;
  }

  void expression::copy_from(const expression& src)
  {
  }

  void expression::evaluate()
  {
    value_ = evaluate_postfix(to_postfix(token_));
    evaluated_ = true;
  }

  bool expression::has_precedence(char op1, char op2)
  {
    assert(is_operator(op1) && is_operator(op2));
    return operator_weights[op1] <= operator_weights[op2];
  }


  string_t expression::to_postfix(string_t const& in)
  {
    absolute_ = true;

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
      out += operand + " ";

    while (!operators.empty())
    {
      out.push_back(operators.back());
      operators.pop_back();
    }

    for (auto _operand : operands)
    {
      if (_operand[0] < '0' || _operand[0] > '9')
      {
        absolute_ = false;
        break;
      }
      std::cout << "\tOperand: " << _operand << "\n";
    }

    std::cout << "evaluating " << (absolute_ ? "constant" : "relative") << " postfix expression: " << out << "\n";
    //~ return evalute_postfix(out/*, operands*/);
    return out;
  }

  int expression::evaluate_postfix(std::string in_expr/*, std::vector<std::string> in_operands*/)
  {
    string_t out = "";
    //~ std::cout << "converting a postfix expr with " << in_operands.size() << " '" << in_expr << "'\n";
    std::vector<string_t> operands;
    uint64_t result_ = 0x0;
    string_t operand_str = "";
    for (char c : in_expr)
    {
      if (c == ' ')
      {
        if (!operand_str.empty())
        {
          operands.push_back(operand_str);
          operand_str = "";
        }
        continue;
      }

      if (!is_operator(c))
      {
        operand_str += c;
        continue;
      }

      std::cout << "applying op " << c << " on 2-top operands : " << operands.size() << "\n";

      assert(operands.size() >= 2);
      string_t tmp = operands.back();
      operands.pop_back();
      string_t res = "[" + operands.back() + " " + c + " " + tmp + "]";
      operand* lhs = new operand(tmp);
      operand* rhs = new operand(operands.back());
      //~ result_ = apply_operator(c, utility::convertTo<int>(operands.back()), utility::convertTo<int>(tmp));
      result_ = apply_operator(c, lhs->value(), rhs->value());
      operands.pop_back();
      operands.push_back(res);
    }

    out = operands.back();

    return result_;
  }

  bool expression::is_absolute() const
  {
    return absolute_;
  }

  int expression::apply_operator(char op, int lhs, int rhs)
  {
    switch (op)
    {
      case '*':
      return lhs * rhs;
      case '/':
      return lhs / rhs;
      case '+':
      return lhs + rhs;
      case '-':
      return lhs - rhs;
      case '%':
      return lhs % rhs;
      default:
      throw invalid_operator("unrecognized expression operation");
    }

    return 0;
  }

} // end of namespace

