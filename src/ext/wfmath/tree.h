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

#ifndef TREE_H_NOT_INCLUDED_DIRECTLY
#error "Don't include tree.h directly, include octree.h or quadtree.h instead!"
#endif

#ifndef WFMATH_TREE_H
#define WFMATH_TREE_H

#include <stdio.h>

#include <vector>
#include <list>
#include <algorithm>

#include <wfmath/ball.h>
#include <wfmath/intersect.h>

namespace WFMath
{
  template<const int Dim, typename T, typename G>
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
      typename std::list<Listener*>::iterator iter;
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

    inline bool ContainedIn(const Ball<Dim>& s) const { return Intersect<Dim>(s, geom, false); }

    void AddListener(Listener* listener) { RemoveListener(listener); listeners.push_back(listener); }
    void RemoveListener(Listener* listener) { listeners.remove(listener); }
  };

  // TODO: Implement me!
  // It's just a basic vector now.

  /**
   * Tree class.
   */
  template<const int Dim, typename T, typename G, unsigned short MAXSHAPES, unsigned short CHILDREN, bool ALLOWCOLLISSION>
  class Tree
  {
  public:
    typedef typename WFMath::Shape<Dim, T, G> Shape;
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
      AxisBox<Dim> nodeShape;
      Shapes shapes;
      std::vector<Node*> children;

    private:
      virtual void Moved(Shape* shape)
      {
        printf("Node::Moved: %s\n", shape->GetParent()->name.c_str());
        if (Contains<Dim>(shape->Get(), nodeShape, false)) // TODO: is this false correct?
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
      Node(Node* p, const AxisBox<Dim>& size) : parent(parent), nodeShape(size), children(CHILDREN, (Node*)0) {}

      /** Destructor. */
      ~Node()
      {
        for (size_t i = 0; i < children.size(); ++i)
          delete children[i];
      }

      bool Add(Shape* shape)
      {
        if (!ALLOWCOLLISSION)
        {
          ConstIterator it;
          for (it = shapes.begin() ; it != shapes.end(); it++ )
            if (Intersect<Dim>((*it)->Get(), shape->Get(), true))
              return false;
        }

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

      QueryResult Query(const Ball<Dim>& s) const
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
    Tree(const Tree& o);
    Tree& operator=(const Tree& o);

  private:
    Node* rootNode;

  public:
    /// Default constructor.
    Tree()
    {
      rootNode = new Node(0, AxisBox<Dim>(Point<Dim>(-100000.0f), Point<Dim>(100000.0f)));
    }

    ~Tree()
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

    QueryResult Query(const Ball<Dim>& s) const
    {
      return rootNode->Query(s);
    }

  };

} // namespace WFMath

#endif // WFMATH_TREE_H
