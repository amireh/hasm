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
#include "parser.hpp"

namespace hax
{
  control_section::control_section(string_t in_name)
  : name_(in_name),
    pblock_(new program_block("Unnamed", this)),
    symmgr_(new symbol_manager(this)),
    starting_addr_(0x0),
    starting_addr_set_(false)
  {
    pblocks_.push_back(pblock_);
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
    symmgr_ = 0;
    pblock_ = 0;
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
        std::cout << "switching to existing program block: " << pblock_->name() << "\n";
        return;
      }


    pblock_ = new program_block(in_name, this);
    pblocks_.push_back(pblock_);
    std::cout << "switching to new program block: " << pblock_->name() << "\n";
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
    //~ symmgr_->dump_literal_pool(true);

    int idx = 0;
    for (auto block : pblocks_) {
      std::cout << "Assigning address to program block '" << block->name() << "' = " << idx << "\n";
      block->shift(idx);
      idx += block->length();
    }

    bool failed = false;
    for (auto inst : instructions_)
    {
      try {
        inst->assemble();
      } catch (hax_error& e) {
        parser::singleton().track_error(e);
      }
      //~ std::cout << inst << "\n";
    }

    if (failed) {
      return parser::singleton().report_errors();
    }

    for (auto inst : instructions_)
    {
      try {
        inst->postprocess();
      } catch (hax_error& e) {
        parser::singleton().track_error(e);
        failed = true;
      }
    }

    if (failed) {
      parser::singleton().report_errors();
    }
  }

  void
  control_section::serialize(string_t const& out_path)
  {
    serializer::singleton().process(this, out_path);
  }

  bool
  control_section::has_starting_address() const
  {
    return starting_addr_set_;
  }

  loc_t
  control_section::starting_address()
  {
    return starting_addr_;
  }

  void
  control_section::assign_starting_address(loc_t in_addr)
  {
    starting_addr_ = in_addr;
    starting_addr_set_ = true;
  }
} // end of namespace hax
