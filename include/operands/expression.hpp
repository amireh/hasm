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

#ifndef h_expression_h
#define h_expression_h

#include "hax.hpp"
#include "operand.hpp"
#include <map>
#include <list>

namespace hax
{
  class symbol;
  typedef symbol symbol_t;

  /**
   * Expressions can contain already defined symbols, decimal constants, and any
   * of the following operators:
   *  + - * / ( )
   *
   * An expression which contains a term that is an external reference will not
   * be evaluated.
   **/
  class expression : public operand {
    public:
    typedef std::map<char, int> weights_t;
    typedef std::list<symbol_t*> extrefs_t;
    static weights_t operator_weights;

    /**
     * When an expression is created, the given token is attempted to be converted
     * into postfix notation, and tracks every symbolic term referenced in the
     * expression body.
     **/
		explicit expression(string_t const& in_token, instruction* in_inst);
    expression()=delete;
    expression(const expression& src);
		expression& operator=(const expression& rhs);
		virtual ~expression();

    /**
     * Substitutes every reference defined in the expression body with its
     * actual value, then attempts to calculate the value of the postfix expression.
     *
     * References to symbols must be fully evaluated before attempting to evaluate
     * an expression, otherwise an exception of type hax::unevaluated_operand()
     * will be thrown.
     **/
    virtual void evaluate();

    extrefs_t& references();

    protected:
    void copy_from(const expression&);
    int evaluate_postfix(string_t in_expr/*, std::vector<string_t> in_operands*/);

    /**
     * converts an infix expression into a postfix one
     **/
    void to_postfix(string_t const& in, string_t &out);
    bool has_precedence(char op1, char op2);
    int apply_operator(char op, int lhs, int rhs);

    extrefs_t extrefs_;

    string_t postfix_expr_;

    bool absolute_;

	};

  typedef expression expression_t;
} // end of namespace
#endif // h_expression_h
