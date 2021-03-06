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
  extern bool DELIMITED_OUTPUT;

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
      //~ std::cout << "IM HERE ! " << rec->length << " + " << inst->length() << "\n";
      return true;
    }

    return false;
  }

  void serializer::process(csect_t* in_sect, string_t const& out_path)
  {
    std::ofstream out(out_path);
    if (!out.is_open() || !out.good())
    {
      throw std::runtime_error("can not open output file: " + out_path);
    }

    std::cout << "+- Serializer: writing object program\n";
    std::list<instruction_t*> const& instructions = in_sect->instructions();
    symbol_manager *symmgr = in_sect->symmgr();
    //~ std::list<instruction_t*> const& instructions = parser::singleton().instructions();
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
    //std::vector<d_record*> d_records;
    //std::vector<r_record*> r_records;

    instruction_t const* inst = 0;

    // TODO: write HEADER record
    inst = instructions.front();
    std::string prog_name = inst->label()->token();
    if (prog_name.size() > 6)
      throw std::runtime_error("program name is too long");

    prog_name = utility::expand(prog_name, 6, ' ');

    out << std::uppercase;
    out << 'H' << prog_name;
    out << "000000"; // relocatable program
    out << std::hex << std::setw(6) << std::setfill('0') << in_sect->length(); // object program length
    // ...
    out << '\n';
    inst = 0;

    // prepare the D and R records
    // TODO: optimize the symbols fetched here (only get user-defined ones)
    string_t d_record = "D";
    string_t r_record = "R";
    std::ostringstream d_record_str;
    d_record_str << 'D'  << std::uppercase;
    for (auto entry : symmgr->symbols())
    {
      symbol_t *sym = entry.second;
      //~ std::cout << "\tchecking whether symbol '" << sym->token() << "' is an external ref or definition\n";
      if (sym->is_external_def())
      {
        if (DELIMITED_OUTPUT)
          d_record_str << '^';
        d_record_str << utility::expand(sym->token(), 6, ' ');
        if (DELIMITED_OUTPUT)
          d_record_str << '^';
        d_record_str << std::hex << std::setw(6) << std::setfill('0') << sym->address();
        std::cout << "found an external definition: " << sym << "\n";
      } else if (sym->is_external_ref()) {
        if (DELIMITED_OUTPUT)
          d_record_str << '^';
        r_record += utility::expand(sym->token(), 6, ' ');
        std::cout << "found an external reference: " << sym << "\n";
      }
    }

    d_record = d_record_str.str();
    if (d_record.size() > 1)
      out << d_record << "\n";
    if (r_record.size() > 1)
      out << r_record << "\n";

    // prepare T and M records
    t_record *rec = new t_record();
    rec->address = instructions.front()->location();
    rec->length = 0x00;
    for (auto inst : instructions)
    {
      // skip assembler directives
      if (!inst->is_assemblable())
      {
        if (VERBOSE)
        std::cout << "Info: skipping non-assemblable directive '" << inst->mnemonic() << "'\n";

        // some assembler directives require us to create a new T record, such as
        // RESB, RESW, USE
        if (rec && requires_new_trecord(rec, inst)) {
          t_records.push_back(rec);
          rec = 0;
        }

        continue;
      }

      // create a new record if there's none (case1), or if the current one's length
      // has been or will be exceeded (case2)
      if (!rec || requires_new_trecord(rec, inst))
      {
        if (rec)
          t_records.push_back(rec);

        rec = new t_record();
        rec->address = inst->location();
        rec->length = 0;
      }

      // does this instruction require an M record for relocation?
      if (inst->is_relocatable()) {
        for (auto reloc_rec : inst->reloc_records())
        {
          m_record *mrec = new m_record();
          //~ mrec->location = rec->length + rec->address + 1;
          mrec->location = inst->location() + (0x06 - reloc_rec->length);
          mrec->length = reloc_rec->length;
          mrec->value = reloc_rec->value;
          //mrec->inst = inst;
          m_records.push_back(mrec);
        }
      }

      // step the T record's length by this instruction's length
      rec->length += inst->length();

      std::cout << "t_record[" << t_records.size() + 1 << "] =>: " << inst << '\n';

      // finally, track this instruction and process the next
      rec->instructions.push_back(inst);
    }

    // track the trailing T record, if any
    if (rec)
      t_records.push_back(rec);

    if (VERBOSE)
      std::cout << "dumping " << t_records.size() << " text records\n";

    // do actually write the T records now
    for (t_record* rec : t_records)
    {
      out << std::uppercase << std::hex << std::setfill('0');
      out << 'T';
      if (DELIMITED_OUTPUT)
        out << '^';
      out << std::setw(6) << rec->address;
      if (DELIMITED_OUTPUT)
        out << '^';
      out << std::setw(2) << rec->length;
      //~ out << std::resetiosflags;

      //~ out << std::hex << std::uppercase << std::setw(6) << std::setfill('0');
      for (auto inst : rec->instructions)
      {
        if (DELIMITED_OUTPUT)
          out << '^';
        out << std::setw(inst->length() * 2) << std::setfill('0') << inst->objcode();
      }
      //~ out << std::resetiosflags;

      out << '\n';
      rec->instructions.clear();
    }

    // clean up the T records
    while (!t_records.empty())
    {
      delete t_records.back();
      t_records.pop_back();
    }

    // write the M records
    for (m_record *rec : m_records)
    {
      out << std::uppercase << std::hex << std::setfill('0');
      out << 'M';

      if (DELIMITED_OUTPUT)
        out << '^';

      out << std::setw(6) << rec->location;

      if (DELIMITED_OUTPUT)
        out << '^';

      out << std::setw(2) << rec->length;

      if (DELIMITED_OUTPUT)
        out << '^';

      out << rec->value;
      out << '\n';
    }
    // and clean em up
    while (!m_records.empty())
    {
      delete m_records.back();
      m_records.pop_back();
    }

    // write the END record
    inst = instructions.back();
    out << 'E';
    if (in_sect->has_starting_address())
      out << std::hex << std::setw(6) << std::setfill('0') << in_sect->starting_address();
    out << '\n';
    inst = 0;

    out.close();

    // go play some quake3!! :)
  }
} // end of namespace
