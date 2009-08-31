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

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace WFMath
{
  class iShape
  {
  public:
    struct Listener
    {
      virtual ~Listener() {}
      virtual void Moved(iShape*) = 0;
      virtual void Destroyed(iShape*) = 0;
    };

  private:
    std::list<Listener*> listeners;

  protected:
    void NotifyDestroyed()
    {
      std::list<Listener*>::const_iterator iter;
      for( iter = listeners.begin(); iter != listeners.end(); iter++ )
        (*iter)->Destroyed(this);
    }

    void NotifyMoved()
    {
      std::list<Listener*>::const_iterator iter;
      for( iter = listeners.begin(); iter != listeners.end(); ++iter )
        (*iter)->Moved(this);
    }

  public:
    iShape() {}
    virtual ~iShape()
    {
      NotifyDestroyed();
    }

    void AddListener(Listener* listener) { RemoveListener(listener); listeners.push_back(listener); }
    void RemoveListener(Listener* listener) { listeners.remove(listener); }
  };

  template<typename G>
  class Shape : public iShape
  {
  private:
    G geom;

  public:
    Shape() {}
    virtual ~Shape() {}

    inline const G& GetShape() const { return geom; }
    virtual void SetShape(const G& v)
    {
      geom = v;
      NotifyMoved();
    }

    inline bool ContainedIn(const Point<2>& s) const { return Intersect<2>(geom, s, false); }
    inline bool ContainedIn(const Point<3>& s) const { return Intersect<3>(geom, s, false); }

    inline bool ContainedIn(const AxisBox<2>& s) const { return Intersect<2>(geom, s, false); }
    inline bool ContainedIn(const AxisBox<3>& s) const { return Intersect<3>(geom, s, false); }

    template<const int Dim>
    inline bool ContainedIn(const Ball<Dim>& s) const { return Intersect<Dim>(s, geom, false); }
  };

  // TODO: Implement me!
  // It's just a basic vector now.

  /**
   * Tree class.
   */
  template<typename G, const int Dim, unsigned short MAXSHAPES, unsigned short CHILDREN, bool ALLOWOVERLAP>
  class Tree
  {
  public:
    typedef std::list<boost::shared_ptr<iShape> > QueryResult;

  private:
    static Shape<G>* Cast(iShape* shape)
    {
      return static_cast<Shape<G>*>(shape);
    }
    
    static boost::shared_ptr<Shape<G> > Cast(boost::shared_ptr<iShape> shape)
    {
      return boost::shared_static_cast<Shape<G> >(shape);
    }

    class Node : public iShape::Listener
    {
    private:
      /// Copy constructor.
      Node(const Node& o);
      Node& operator=(const Node& o);

      typedef typename std::list<boost::weak_ptr<iShape> > Shapes;
      typedef typename Shapes::iterator Iterator;
      typedef typename Shapes::const_iterator ConstIterator;

    private:
      Node* parent;
      AxisBox<Dim> nodeShape;
      Shapes shapes;
      std::vector<Node*> children;

      class Equal
      {
        iShape* s;
      public:
        Equal(iShape* shape) : s(shape) {}
        bool operator() (const boost::weak_ptr<iShape>& v) { return !v.lock() || s == v.lock().get(); }
      };

    private:
      virtual void Moved(iShape* shape)
      {
        //printf("Node::Moved!\n");
        if (Contains<Dim>(Cast(shape)->GetShape(), nodeShape, false)) // TODO: is this false correct?
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

      virtual void Destroyed(iShape* shape)
      {
        //printf("Node::Destroyed!\n");
        shapes.remove_if(Equal(shape));
      }

    public:
      /** Construct a node with a given volume and without children.
       * @param size of the new node
       */
      Node(Node* p, const AxisBox<Dim>& size) : parent(parent), nodeShape(size), children(CHILDREN, (Node*)0) {}

      /** Destructor. */
      ~Node()
      {
        ConstIterator it;
        for (it = shapes.begin() ; it != shapes.end(); it++ )
          if ((*it).lock())
            (*it).lock()->RemoveListener(this);

        for (size_t i = 0; i < children.size(); ++i)
          delete children[i];
      }

      bool Add(boost::shared_ptr<iShape> shape)
      {
        if (!ALLOWOVERLAP)
        {
          ConstIterator it;
          for (it = shapes.begin() ; it != shapes.end(); it++ )
            if ((*it).lock())
              if (Intersect<Dim>(Cast((*it).lock())->GetShape(), Cast(shape)->GetShape(), true))
                return false;
        }

        // Make sure there are no doubles!
        Remove(shape);
        shapes.push_back(shape);
        shape->AddListener(this);
        return true;
      }

      bool Remove(boost::shared_ptr<iShape> shape)
      {
        shape->RemoveListener(this);
        shapes.remove_if(Equal(shape.get()));
        return true;
      }

      template <template <const int> class S>
      QueryResult Query(const S<Dim>& s) const
      {
        QueryResult result;
        ConstIterator it;
        for (it = shapes.begin() ; it != shapes.end(); it++ )
        {
          if ((*it).lock())
            if (Cast((*it).lock())->ContainedIn(s))
              result.push_back((*it).lock());
        }

        return result;
      }

      template<typename T, template <const int> class S>
      std::list<boost::shared_ptr<T> > Query(const S<Dim>& s) const
      {
        std::list<boost::shared_ptr<T> > result;
        ConstIterator it;
        for (it = shapes.begin() ; it != shapes.end(); it++ )
        {
          if ((*it).lock())
            if (Cast((*it).lock())->ContainedIn(s))
            {
              boost::shared_ptr<T> t = boost::shared_dynamic_cast<T>((*it).lock());
              if (t) result.push_back(t);
            }
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

    bool Add(boost::shared_ptr<iShape> shape)
    {
      return rootNode->Add(shape);
    }

    bool Remove(boost::shared_ptr<iShape> shape)
    {
      return rootNode->Remove(shape);
    }

    template <template <const int> class S>
    QueryResult Query(const S<Dim>& s) const
    {
      return rootNode->Query(s);
    }

    template<typename T, template <const int> class S>
    std::list<boost::shared_ptr<T> > Query(const S<Dim>& s) const
    {
      return rootNode->Query<T>(s);
    }
  };

} // namespace WFMath

#endif // WFMATH_TREE_H
