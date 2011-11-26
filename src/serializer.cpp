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

#include "serializer.hpp"
#include "instruction.hpp"
#include "symbol_manager.hpp"
#include "parser.hpp"
#include <fstream>
#include <ostream>
#include <exception>
#include <stdexcept>

namespace hax
{
  extern bool VERBOSE;

	serializer* serializer::__instance = 0;
	const uint8_t serializer::t_record::maxlen = 0x1E;

	serializer::serializer()
  {
	}

	serializer::~serializer()
	{
	}

	serializer* serializer::singleton_ptr()
  {
		return __instance = (!__instance) ? new serializer() : __instance;
	}

	serializer& serializer::singleton()
  {
		return *singleton_ptr();
	}

  bool serializer::requires_new_trecord(t_record* rec, instruction_t* inst)
  {
    if (rec->length >= t_record::maxlen)
      return true;
    if (inst->mnemonic() == "RESW" || inst->mnemonic() == "RESB" || inst->mnemonic() == "USE")
      return true;
    if (rec->length + inst->length() > t_record::maxlen) {
      std::cout << "IM HERE ! " << rec->length << " + " << inst->length() << "\n";
      return true;
    }

    return false;
  }

  void serializer::process(string_t const& out_path)
  {
    std::ofstream out(out_path);
    if (!out.is_open() || !out.good())
    {
      throw std::runtime_error("can not open output file: " + out_path);
    }

    std::cout << "+- Serializer: writing object program\n";
    std::list<instruction_t*> const& instructions = parser::singleton().instructions();
    //~ std::list<instruction_t*> instructions;
    //~ for (auto pblock : parser::singleton().pblocks())
      //~ for (auto inst : pblock->instructions())
        //~ instructions.push_back(inst);

    if (instructions.empty())
    {
      std::cerr << "FATAL: no instructions parsed to serialize! aborting\n";
      return;
    }

    std::vector<t_record*> t_records;
    std::vector<m_record*> m_records;

    instruction_t const* inst = 0;

    // TODO: write HEADER record
    inst = instructions.front();
    std::string prog_name = inst->label()->token();
    if (prog_name.size() > 7)
      throw std::runtime_error("program name is too long");

    while (prog_name.size() < 6)
      prog_name.push_back(' ');

    out << 'H' << prog_name;
    out << "000000"; // relocatable program
    out << std::hex << std::setw(6) << std::setfill('0') << instructions.back()->location(); // object program length
    // ...
    out << '\n';
    inst = 0;

    int guard = 0;
    const int start_entry = 0;
    const int end_entry = instructions.size()-1;

    // prepare the T records
    t_record *rec = new t_record();
    rec->address = instructions.front()->location();
    rec->length = 0x00;
    for (std::list<instruction_t*>::const_iterator _itr = instructions.begin();
      _itr != instructions.end();
      ++_itr, ++guard)
    {
      instruction_t *inst = *_itr;

      if (guard == start_entry || guard == end_entry) {
        continue;
      }

      if (!inst->is_assemblable())
      {
        std::cout << "Info: non-assemblable instruction: '" << inst->mnemonic() << "', skipping\n";
        if (rec && (inst->mnemonic() == "RESB" || inst->mnemonic() == "RESW" || inst->mnemonic() == "USE")) {
          t_records.push_back(rec);
          rec = 0;
        }
        continue;
      }

      // if the T record's length has been exceeded, or it can't contain
      // the next instruction, we create a new one
      if (!rec || requires_new_trecord(rec, inst))
      {
        if (rec)
          t_records.push_back(rec);
        rec = new t_record();
        rec->address = inst->location();
        rec->length = 0;
      }

      if (inst->is_relocatable()) {
        m_record *mrec = new m_record();
        mrec->location = rec->length + rec->address + 1;
        mrec->length = 0x05; // TODO: determine M record length
        mrec->inst = inst;
        m_records.push_back(mrec);
      }

      rec->length += inst->length();
      std::cout << "added inst: " << inst << " to t_record " << t_records.size() + 1 << "\n";
      //~ std::cout << "incremented t_record by length " << inst->length() << ", acc=" << rec->length << "\n";
      rec->instructions.push_back(inst);
    }
    if (rec)
      t_records.push_back(rec);

    std::cout << "dumping " << t_records.size() << " text records\n";

    for (t_record* rec : t_records)
    {
      out << std::uppercase << std::hex << std::setfill('0');
      out << 'T' << '^' << std::setw(6) << rec->address;
      out << '^' << std::setw(2) << rec->length;
      //~ out << std::resetiosflags;

      //~ out << std::hex << std::uppercase << std::setw(6) << std::setfill('0');
      for (auto inst : rec->instructions)
      {
        out << '^';
        out << std::setw(inst->length() * 2) << std::setfill('0') << inst->objcode();
      }
      //~ out << std::resetiosflags;

      out << '\n';
      rec->instructions.clear();
    }

    while (!t_records.empty())
    {
      delete t_records.back();
      t_records.pop_back();
    }

    // write M records
    for (m_record *rec : m_records)
    {
      out << std::uppercase << std::hex << std::setfill('0');
      out << 'M' << '^' << std::setw(6) << rec->location;
      out << '^' << std::setw(2) << rec->length;
      out << '\n';
    }

    while (!m_records.empty())
    {
      delete m_records.back();
      m_records.pop_back();
    }

    // TODO: write END record
    inst = instructions.back();
    out << 'E';
    out << std::hex << std::setw(6) << std::setfill('0') << inst->objcode(); // object program length
    out << '\n';
    inst = 0;

    out.close();
  }
} // end of namespace
