/*
  Copyright (C) 2009 Development Team of Peragro Tempus

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/**
 * @file stringid.h
 *
 * @brief Wrapper around boost flyweight.
 */

#ifndef PT_UTILITY_STRINGID_H
#define PT_UTILITY_STRINGID_H

#include <boost/signals2.hpp> // Hack for conflicting BOOST_PARAMETER_MAX_ARITY
#include <boost/flyweight.hpp>
#include <boost/flyweight/no_tracking.hpp>
#include <boost/operators.hpp>
#include <boost/functional/hash.hpp>

namespace PT
{
  /**
   * Wrapper for boost::flyweight<std::string> to add a few functions of
   *   std::string, and so boost::hash works by default so it can be the
   *   key in a hash set or map.
   */
  template<typename Tag>
  class StringId : public boost::flyweights::flyweight<
    std::string, boost::flyweights::tag<Tag>, boost::flyweights::no_tracking>,
    public boost::totally_ordered<StringId<Tag> >
  {
  public:
    typedef boost::flyweights::flyweight<std::string,
      boost::flyweights::tag<Tag>, boost::flyweights::no_tracking> FlyweightType;

    /// Forwarding constructor.
    StringId() : FlyweightType() {}
    /// Forwarding constructor.
    StringId(const std::string& str) : FlyweightType(str) {}
    /// Construct from C string or char array.
    StringId(const char* cstr) : FlyweightType(cstr) {}
    /// Forwarding constructor.
    StringId(const FlyweightType& other) : FlyweightType(other) {}
    /// Forwarding constructor.
    StringId(FlyweightType& other) : FlyweightType(other) {}
    /// Emulate std::string.
    const char* c_str() const { return this->get().c_str(); }
    /// Emulate std::string.
    bool empty() const { return this->get().empty(); }
  };

  /// Resolve abmibiguity between operator==(flyweight, flyweight) and
  ///   operator==(T, flyweight)
  template<typename Tag>
  inline bool operator==(const StringId<Tag>& l, const StringId<Tag>& r)
  { return &l.get() == &r.get(); }

  /// Resolve abmibiguity between operator<(flyweight, flyweight) and
  ///   operator<(T, flyweight)
  template<typename Tag>
  inline bool operator<(const StringId<Tag>& l, const StringId<Tag>& r)
  { return &l.get() < &r.get(); }

  /// For boost::hash, hash sets and maps.
  template<typename Tag>
  std::size_t hash_value(const StringId<Tag>& strId)
  { boost::hash<const char*> hasher; return hasher(strId.get().c_str()); }

} // PT namespace

#endif
