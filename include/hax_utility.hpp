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

#ifndef h_hax_utility_h
#define h_hax_utility_h

#include "hax_exception.hpp"
#include <typeinfo>
#include <sstream>
#include <vector>
#include <iostream>

namespace hax { namespace utility {

	template<typename T>
	inline std::string stringify(const T& x)
	{
		std::ostringstream o;
		if (!(o << x))
			throw bad_conversion(std::string("stringify(")
								+ typeid(x).name() + ")");
		return o.str();
	}

	// helper; converts an integer-based type to a string
	template<typename T>
	inline void convert(const std::string& inString, T& inValue,
						bool failIfLeftoverChars = true)
	{
		std::istringstream _buffer(inString);
		char c;
		if (!(_buffer >> inValue) || (failIfLeftoverChars && _buffer.get(c)))
			throw bad_conversion(inString);
	}

	template<typename T>
	inline T convertTo(const std::string& inString,
					   bool failIfLeftoverChars = true)
	{
		T _value;
		convert(inString, _value, failIfLeftoverChars);
		return _value;
	}

  /* splits a string s using the delimiter delim */
  inline static
  std::vector<std::string> split(const std::string &s, char delim) {
      std::vector<std::string> elems;
      std::stringstream ss(s);
      std::string item;
      while(std::getline(ss, item, delim)) {
          elems.push_back(item);
      }
      return elems;
  }

  inline static
  void ijoin(const std::vector<std::string>& tokens, std::string &out, char delim) {
    for (std::vector<std::string>::const_iterator token = tokens.begin();
    token != tokens.end();
    ++token)
    {
      out += *token;
      out.push_back(delim);
    }
  }

  inline static
  std::string join(const std::vector<std::string>& tokens, char delim) {
    std::string out;
    ijoin(tokens, out, delim);
    return out;
  }

  inline static
  void itrim(std::string &s) {
    int ws_ctr = 0;
    for (char c : s)
    {
      if (c == ' ' || c == '\t')
        ++ws_ctr;
      else
        break;
    }

    int reverse_ws_ctr = 0;
    char c;
    for (int i=s.size()-1; i >= 0; --i)
    {
      c = s[i];
      if (c == ' ' || c == '\t')
        ++reverse_ws_ctr;
      else
        break;
    }

    if (ws_ctr > 0 || reverse_ws_ctr > 0)
      s = s.substr(ws_ctr, s.size() - (ws_ctr + reverse_ws_ctr));
  }

  inline static
  std::string trim(std::string const& in)
  {
    std::string out(in);
    utility::itrim(out);
    return out;
  }

  inline static
  void iremove_in_string(std::string& in, char delim)
  {
    size_t idx;
    while ((idx = in.find(delim)) != std::string::npos)
      in.erase(idx, 1);
  }

  inline static
  std::string remove_in_string(std::string const& in, char delim)
  {
    std::string out(in);
    hax::utility::iremove_in_string(out, delim);
    return out;
  }

  inline static
  bool is_decimal_nr(std::string const& in)
  {
    for (auto c : in)
      if (c < '0' || c > '9')
        return false;

    return true;
  }

  /**
   * suffixes given string with `fill` until it spans `size` characters
   **/
  inline static
  std::string expand(std::string const& in, size_t size, char fill)
  {
    if (in.size() >= size)
      return in;

    std::string out(in);
    while (out.size() < size)
      out.push_back(fill);

    return out;
  }

  inline static
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

  template<typename IntType>
  IntType overwrite_bits(IntType dst, IntType src, int pos, int len) {
      IntType mask = (((IntType)1 << len) - 1) << pos;
      return (dst & ~mask) | (src & mask);
  }
} // utility namespace
} // hax namespace

#endif
