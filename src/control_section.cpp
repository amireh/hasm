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

#include "control_section.hpp"
#include "serializer.hpp"

namespace hax
{
  control_section::control_section(string_t in_name)
  : name_(in_name),
    pblock_(new program_block("Unnamed", this)),
    symmgr_(new symbol_manager())
  {
  }

  control_section::~control_section()
  {
    while (!pblocks_.empty())
    {
      delete pblocks_.back();
      pblocks_.pop_back();
    }

    while (!instructions_.empty())
    {
      delete instructions_.back();
      instructions_.pop_back();
    }

    delete symmgr_;
  }

  std::ostream&
  control_section::to_stream(std::ostream& out) const
  {
    out
      << "[Control Section '" << name_ << "']\n"
      << "\tNumber of instructions: " << instructions_.size() << "\n"
      << "\tNumber of program blocks: " << pblocks_.size() << "\n";

    return out;
  }

  string_t const&
  control_section::name() const
  {
    return name_;
  }

  symbol_manager*
  control_section::symmgr() const
  {
    return symmgr_;
  }

  pblock_t*
  control_section::block() const
  {
    return pblock_;
  }

  control_section::pblocks_t const&
  control_section::program_blocks() const
  {
    return pblocks_;
  }

  void
  control_section::switch_to_block(std::string in_name)
  {
    for (auto block : pblocks_)
      if (block->name() == in_name) {
        pblock_ = block;
        return;
      }

    pblock_ = new program_block(in_name, this);
    pblocks_.push_back(pblock_);
  }

  void
  control_section::__add_instruction(instruction_t* in_inst)
  {
    instructions_.push_back(in_inst);
  }

  const control_section::instructions_t&
  control_section::instructions() const
  {
    return instructions_;
  }

  void
  control_section::assemble()
  {
    int idx = 0;
    for (auto block : pblocks_) {
      std::cout << "Assigning address to program block '" << block->name() << "'\n";
      block->shift(idx);
      idx += block->locctr();
    }

    for (auto inst : instructions_)
    {
      //~ inst->resolve_references();
      inst->assemble();
      //~ std::cout << inst << "\n";
    }

    for (auto inst : instructions_)
    {
      inst->postprocess();
    }
  }

  void
  control_section::serialize(string_t const& out_path)
  {
    serializer::singleton().process(this, out_path);
  }

} // end of namespace hax
