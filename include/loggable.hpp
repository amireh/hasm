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

#ifndef h_loggable_h
#define h_loggable_h

namespace hax
{
	/*!
   * @brief
   * loggable objects respond to the operator<< for logging to an std::ostream.
	 */
	class loggable
	{

	public:
		loggable() {}
		loggable& operator=(const loggable& rhs) { return *this; }
		loggable(const loggable& src) {}
		virtual ~loggable() {}

    inline friend std::ostream& operator<<(std::ostream& in_stream, loggable const* in_loggable)
    {
      return in_loggable->to_stream(in_stream);
    }

    inline friend std::ostream& operator<<(std::ostream& in_stream, loggable const& in_loggable)
    {
      return in_loggable.to_stream(in_stream);
    }

    inline virtual std::string dump() const {
      std::ostringstream out;
      out << this;
      return out.str();
    };

		protected:

    virtual std::ostream& to_stream(std::ostream&) const=0;

	}; // end of loggable class

  typedef loggable loggable_t;
} // end of Pixy namespace
#endif
