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
      /*
      std::list<Listener*>::iterator iter;
      for( iter = listeners.begin(); iter != listeners.end(); iter++ )
        (*iter)->Destroyed(this);
        */
    }

    //inline operator T() const { return *parent; }
    inline const T& GetParent() const { return *parent; }

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
    typedef std::set<T> QueryResult;

  private:

    struct compare
    {
      bool operator()(const T& obj1, const T& obj2) const
      {
        return obj1 < obj2;
      }
    };

    class Node : public Shape::Listener
    {
    private:
      /// Copy constructor.
      Node(const Node& o);
      Node& operator=(const Node& o);

      typedef typename std::set<T, compare> Shapes;
      typedef typename Shapes::iterator Iterator;
      typedef typename Shapes::const_iterator ConstIterator;

    private:
      Node* parent;
      Box box;
      Shapes shapes;
      std::vector<Node*> children;

    private:
      virtual void Moved(Shape* s)
      {
        printf("Node::Moved: %s\n", s->GetParent().name.c_str());
        if (box.Contains(s->Get()))
          return;
        else if (parent)
        {
          //MoveUp();
        }
        else
        {
          //MakeQuadTreeBigger();
          //MoveUp();
        }
      }

      virtual void Destroyed(Shape* s)
      {
        printf("Node::Destroyed: %s\n", s->GetParent().name.c_str());
        shapes.erase(s->GetParent());
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

      bool Add(T shape)
      {
        // Make sure there are no doubles!
        shapes.erase(shape);
        shapes.insert(shape);
        shape.worldBB.AddListener(this);
        return true;
      }

      bool Remove(T shape)
      {
        shape.worldBB.RemoveListener(this);
        return shapes.erase(shape);
      }

      QueryResult Query(const Sphere& s) const
      {
        QueryResult result;
        ConstIterator it;
        for (it = shapes.begin() ; it != shapes.end(); it++ )
        {
          if (it->worldBB.ContainedIn(s))
            result.insert((*it));
        }

        return result;
      }

      bool IsPartitioned() const { return 0 != children[0]; }

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

/*
    size_t Size()
    {
      return entries.size();
    }
*/

    bool Add(T shape)
    {
      return rootNode->Add(shape);
    }

    bool Remove(T shape)
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
