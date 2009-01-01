/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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
 * @file octree.h
 * @brief octree class.
 */

#ifndef GEOM_OCTREE_H
#define GEOM_OCTREE_H

#include <vector>
#include <set>
#include <algorithm>

#include "sphere.h"

namespace Geom
{
  // TODO: Implement me!
  // It's just a basic vector now.

  /**
   * Octree class.
   */
  template<typename Geom, typename T>
  class OcTree
  {
  private:
    struct Entry
    {
      T _t;
      Geom _g;
      Entry(T t, const Geom& g)
      {
        _t = t;
        _g = g;
      }

      Entry(const Entry& e) : _t(e._t), _g(e._g){}

      operator T()
      {
        return _t;
      }

      bool ContainedIn(const Sphere& s)
      {
        return s.Intersect(_g);
      }
    };

  private:
    /// Copy constructor.
    OcTree(const OcTree& o) {}

    typedef typename std::vector<Entry>::iterator Iterator;
    std::vector<Entry> entries;

  public:
    /// Default constructor.
    OcTree() {} 
    ~OcTree() {}

    typedef std::set<T> QueryResult;

    bool Add(Geom g, T t) 
    {
      Entry entry(t, g);
      entries.push_back(entry);
      return true;
    }

    bool Remove(T t) 
    {
      Iterator it;
      for (it = entries.begin() ; it != entries.end(); it++ )
      {
        if (*it == t)
        {
          entries.erase(it);
          return true;
        }
      }
      return false;
    }

    QueryResult Query(const Sphere& s) 
    {
      QueryResult result;
      Iterator it;
      for (it = entries.begin() ; it != entries.end(); it++ )
      {
        if ((*it).ContainedIn(s))
          result.insert((T)(*it));
      }

      return result;
    }

    /// Get the union of two queries.
    // 2*(N+M)-1
    QueryResult Union(const QueryResult& a, const QueryResult& b) 
    {
      QueryResult c;
      std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
      return c;
    }

    /// Get the difference of two queries.
    // 2*(N+M)-1
    QueryResult Difference(const QueryResult& a, const QueryResult& b) 
    {
      QueryResult c;
      std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
      return c;
    }

    /*
     * @param a The old query.
     * @param b The new query.
     * @return a std::pair with .first a QueryResult with new entries
     * and .second a QueryResult with old entries
     */
    std::pair<QueryResult,QueryResult> AddAndDiscard(const QueryResult& a, const QueryResult& b) 
    {
      QueryResult c = Union(a, b);

      //Add: c - a
      QueryResult add = Difference(c, a);

      //Discard: c - b
      QueryResult discard = Difference(c, b);

      std::pair<QueryResult,QueryResult> addAndDiscard(add, discard);
      return addAndDiscard;
    }

  };

} // namespace Geom

#endif // GEOM_OCTREE_H
