//  Copyright (C) 2009 Development Team of Peragro Tempus
//
//  Author: Jelle Hellemans  (sueastside) (C) 2009
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "const.h"
#include "axisbox.h"
#include "quadtree.h"
#include "octree.h"

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "general_test.h"

using namespace WFMath;

template<const int dim>
struct Object : public Shape<AxisBox<dim> >
{
private:
  Point<dim> pos;
public:
  Object(const AxisBox<dim>& b) 
  {
    SetShape(b);
  }

  void SetPosition(const Point<dim>& v)
  {
    pos = v;
    WFMath::AxisBox<dim> t(this->GetShape());
    t += pos;
    SetShape(t);
  }
};

struct Monitor : public iShape::Listener
{
private:
  size_t nrOfMoves;
  size_t nrOfDestroys;
public:
  Monitor() : nrOfMoves(0), nrOfDestroys(0) {}

  virtual void Moved(iShape*) { nrOfMoves++; }
  virtual void Destroyed(iShape*) { nrOfDestroys++; }

  size_t GetNrOfMoves() { return nrOfMoves; }
  size_t GetNrOfDestroys() { return nrOfDestroys; }

  void Reset() { nrOfMoves = 0; nrOfDestroys = 0; }
};

template<typename T, const int dim>
void test_tree(T& t, bool allowOverlap)
{
  std::cout << "Testing Tree<" << dim << (allowOverlap?", allowOverlap":"") << ">:" << std::endl;

  Monitor monitor;

  Point<dim> furthest;

  const size_t NR = 1000;
  std::list<boost::shared_ptr<Object<dim> > > objects;
  for (size_t i = 0; i < NR; i++)
  {
    furthest = Point<dim>(i+1);
    AxisBox<dim> b(Point<dim>(i), Point<dim>(i+1));
    boost::shared_ptr<Object<dim> > o(new Object<dim>(b));
    o->AddListener(&monitor);
    objects.push_back(o);
  }

  {
    // Add objects.
    typename std::list<boost::shared_ptr<Object<dim> > >::const_iterator it;
    for (it = objects.begin(); it != objects.end(); it++)
    {
      assert(t.Add(*it));
    }
    assert(NR == t.Size());
  }

  {
    // Add overlapping objects.
    std::list<boost::shared_ptr<Object<dim> > > overlappingObjects;
    for (size_t i = 0; i < NR; i++)
    {
      AxisBox<dim> b(Point<dim>(i), Point<dim>(i+1));
      boost::shared_ptr<Object<dim> > o(new Object<dim>(b));
      overlappingObjects.push_back(o);
    }
    typename std::list<boost::shared_ptr<Object<dim> > >::const_iterator it;
    for (it = overlappingObjects.begin(); it != overlappingObjects.end(); it++)
    {
      if (allowOverlap) assert(t.Add(*it));
      else assert(!t.Add(*it));
    }
  }

  // Something went wrong here, either an object was added
  // or not removed when it was destroyed.
  std::cout << "Count: " << t.Size() << " =?= " << NR << std::endl;
  assert(NR == t.Size());

  // Calculate the query distance.
  CoordType radius( sqrt(SquaredDistance(Point<dim>(0), furthest)) );
  std::cout << "The distance used to query: " << radius << std::endl;

  Ball<dim> ball(Point<dim>(0), radius);

  {
    // Normal Query.
    typename T::QueryResult result;
    result = t.Query(ball);
    std::cout << "Normal query: Found " << result.size() 
                          << " out of " << t.Size() << "." << std::endl;
    assert(result.size() == t.Size());
  }

  {
    // Type Query.
    std::list<boost::shared_ptr<Object<dim> > > result;
    result = t.template Query<Object<dim> >(ball);
    std::cout << "Type query: Found " << result.size() 
                        << " out of " << t.Size() << "." << std::endl;
    assert(result.size() == t.Size());
  }

  // Move objects
  {
    typename std::list<boost::shared_ptr<Object<dim> > >::const_iterator it;
    for (size_t i = 0; i < NR; i++)
    {
      for (it = objects.begin(); it != objects.end(); it++)
      {
        (*it)->SetPosition(Point<dim>(0));
      }
    }
    std::cout << "Move objects: Found " << monitor.GetNrOfMoves()
                          << " out of " << NR*NR << "." << std::endl;
    assert(NR*NR == monitor.GetNrOfMoves());
  }

  {
    // Remove objects.
    typename std::list<boost::shared_ptr<Object<dim> > >::const_iterator it;
    for (it = objects.begin(); it != objects.end(); it++)
    {
      assert(t.Remove(*it));
    }
    assert(0 == t.Size());
  }

  // Destroy objects
  {
    objects.clear();
    std::cout << "Destroy objects: Found " << monitor.GetNrOfDestroys()
                             << " out of " << NR << "." << std::endl;
    assert(NR == monitor.GetNrOfDestroys());
  }

  std::cout << "Tree<" << dim << ">:" " done." << std::endl << std::endl;
}

int main()
{
  typedef QuadTree<AxisBox<2>, true>::Type QuadTreeOverlap;
  typedef OcTree<AxisBox<3>, true>::Type OcTreeOverlap;

  typedef QuadTree<AxisBox<2>, false>::Type QuadTreeNoOverlap;
  typedef OcTree<AxisBox<3>, false>::Type OcTreeNoOverlap;

  QuadTreeOverlap quadTreeOverlap;
  OcTreeOverlap ocTreeOverlap;

  QuadTreeNoOverlap quadTreeNoOverlap;
  OcTreeNoOverlap ocTreeNoOverlap;

  test_tree<QuadTreeOverlap, 2>(quadTreeOverlap, true);
  test_tree<OcTreeOverlap, 3>(ocTreeOverlap, true);

  test_tree<QuadTreeNoOverlap, 2>(quadTreeNoOverlap, false);
  test_tree<OcTreeNoOverlap, 3>(ocTreeNoOverlap, false);

  return 0;
}
