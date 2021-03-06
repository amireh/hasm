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

#ifndef h_operand_h
#define h_operand_h

#include "hax.hpp"
#include "loggable.hpp"

namespace hax
{
  class instruction;

  /**
   * operands belong to instructions and produce a value when evaluated that is
   * directly required by the instruction to assemble itself
   *
   * the evaluation logic of every operand is deduced based on its type.
   *
   * @note
   * operands should not be created directly, see operand_factory
   **/
  class operand : public loggable {
    public:

		explicit operand(string_t const& in_token, instruction* in_inst);
    operand()=delete;
    operand(const operand& src);
		operand& operator=(const operand& rhs);
		virtual ~operand();

    /**
     * attempts to produce the value of this operand, the result can be tested
     * by calling operand::is_evaluated()
     **/
    virtual void evaluate()=0;

    virtual uint32_t value() const;
    virtual string_t const& token() const;
    uint16_t length() const;

    bool is_evaluated() const;

    bool is_symbol() const;
    bool is_constant() const;
    bool is_literal() const;
    bool is_expression() const;

    instruction* inst() const;

    /**
     * overrides the current value of the operand, and flags it as evaluated.
     *
     * this is internally used to assign values to symbols
     **/
    virtual void _assign_value(uint32_t);

    protected:

    void copy_from(const operand&);

    enum {
      t_undefined,
      t_symbol,
      t_constant,
      t_expression,
      t_literal
    };

    string_t token_;
    uint32_t value_;
    char type_;
    uint16_t length_;

    instruction* inst_;

    /**
     * the reason we use a flag to indicate whether this operand has already been
     * eval'd is because we can't rely on value_ being 0x0 as that is a valid
     * value
     **/
    bool evaluated_;

    virtual std::ostream& to_stream(std::ostream&) const;
	};

  typedef operand operand_t;
} // end of namespace
#endif // h_operand_h
