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

#include "program_block.hpp"
#include "control_section.hpp"

namespace hax
{
  program_block::program_block(string_t in_name, csect_t* in_sect)
  : locctr_(0),
    name_(in_name),
    sect_(in_sect)
  {
  }

  program_block::~program_block()
  {
  }

  loc_t program_block::locctr() const
  {
    return locctr_;
  }

  string_t const& program_block::name() const
  {
    return name_;
  }

  void program_block::add_instruction(instruction_t* in_inst)
  {
    in_inst->assign_location(locctr_);

    sect_->__add_instruction(in_inst);
    instructions_.push_back(in_inst);
  }

  program_block::instructions_t const&
  program_block::instructions() const
  {
    return instructions_;
  }

  void program_block::step()
  {
    if (!instructions_.empty())
      locctr_ += instructions_.back()->length();

  }

  void program_block::shift(int in_amount)
  {
    for (auto inst : instructions_)
    {
      inst->assign_location(inst->location() + in_amount);
    }
  }

  size_t program_block::length() const
  {
    return locctr_;
  }

  control_section* program_block::sect() const
  {
    return sect_;
  }
}
