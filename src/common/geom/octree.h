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
#include <list>
#include <set>
#include <algorithm>

#include "box.h"
#include "sphere.h"

namespace Geom
{

  template<typename T, typename G>
  struct ShapeListener;

  template<typename T, typename G>
  class Shape
  {
  public:
    struct Listener
    {
      virtual ~Listener() {}
      virtual void Moved(Shape*) = 0;
      virtual void Destroyed(Shape*) = 0;
    };

  private:
    T* parent;
    G geom;
    std::list<Listener*> listeners;

  public:
    Shape(T* p) : parent(p) {}
    ~Shape()
    {
      std::list<Listener*>::iterator iter;
      for( iter = listeners.begin(); iter != listeners.end(); iter++ )
        (*iter)->Destroyed(this);
    }

    inline T* GetParent() const { return parent; }

    G* operator->() const { return (G*)&geom; }

    template<typename O>
    inline G& operator+=(const O& o)
    { geom += o; Set(geom); return geom; }

    inline const G& Get() const { return geom; }
    void Set(const G& v)
    {
      geom = v;
      typename std::list<Listener*>::iterator iter;
      for( iter = listeners.begin(); iter != listeners.end(); iter++ )
      {
        (*iter)->Moved(this);
        //printf("Shape::Moved: %d\n", iter);
      }
      //printf("Shape::Moved: (%d)\n", listeners.size());
    }

    void operator=(const G& v) { Set(v); }

    inline bool ContainedIn(const Sphere& s) const { return s.Intersect(geom); }

    void AddListener(Listener* listener) { RemoveListener(listener); listeners.push_back(listener); }
    void RemoveListener(Listener* listener) { listeners.remove(listener); }
  };

  // TODO: Implement me!
  // It's just a basic vector now.

  /**
   * Octree class.
   */
  template<typename T, typename G>
  class OcTree
  {
  public:
    typedef typename Geom::Shape<T, G> Shape;
    typedef std::list<T*> QueryResult;

  private:
    class Node : public Shape::Listener
    {
    private:
      /// Copy constructor.
      Node(const Node& o);
      Node& operator=(const Node& o);

      typedef typename std::list<Shape*> Shapes;
      typedef typename Shapes::iterator Iterator;
      typedef typename Shapes::const_iterator ConstIterator;

    private:
      Node* parent;
      Box box;
      Shapes shapes;
      std::vector<Node*> children;

    private:
      virtual void Moved(Shape* shape)
      {
        printf("Node::Moved: %s\n", shape->GetParent()->name.c_str());
        if (box.Contains(shape->Get()))
          return;
        else if (parent)
        {
          //MoveUp();
        }
        else
        {
          //MakeTreeBigger();
        }
      }

      virtual void Destroyed(Shape* shape)
      {
        printf("Node::Destroyed: %s\n", shape->GetParent()->name.c_str());
        //Remove(s);
        shapes.remove(shape);
      }

    public:
      /** Construct a node with a given volume and without children.
       * @param size of the new node
       */
      Node(const Box& size) : parent(0), box(size), children(8, (Node*)0) {}

      /** Destructor. */
      ~Node()
      {
        for (size_t i = 0; i < children.size(); ++i)
          delete children[i];
      }

      bool Add(Shape* shape)
      {
        // Make sure there are no doubles!
        Remove(shape);
        shapes.push_back(shape);
        shape->AddListener(this);
        return true;
      }

      bool Remove(Shape* shape)
      {
        shape->RemoveListener(this);
        shapes.remove(shape);
        return true;
      }

      QueryResult Query(const Sphere& s) const
      {
        QueryResult result;
        ConstIterator it;
        for (it = shapes.begin() ; it != shapes.end(); it++ )
        {
          if ((*it)->ContainedIn(s))
            result.push_back((*it)->GetParent());
        }

        return result;
      }

      bool IsPartitioned() const { return 0 != children[0]; }

      size_t Size() { return shapes.size(); }

    };

  private:
    /// Copy constructor.
    OcTree(const OcTree& o);
    OcTree& operator=(const OcTree& o);

  private:
    Node* rootNode;

  public:
    /// Default constructor.
    OcTree()
    {
      rootNode = new Node(Box(Vector3(-100000.0f), Vector3(100000.0f)));
    }

    ~OcTree()
    {
      delete rootNode;
    }

    size_t Size()
    {
      return rootNode->Size();
    }

    bool Add(Shape* shape)
    {
      return rootNode->Add(shape);
    }

    bool Remove(Shape* shape)
    {
      return rootNode->Remove(shape);
    }

    QueryResult Query(const Sphere& s) const
    {
      return rootNode->Query(s);
    }

  };

} // namespace Geom

#endif // GEOM_OCTREE_H
