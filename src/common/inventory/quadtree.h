/*
  Copyright (C) 2010 Development Team of Peragro Tempus

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

#ifndef COMMON_QUADTREE_H
#define COMMON_QUADTREE_H

#include <stdio.h>

#include <vector>
#include <list>
#include <algorithm>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "rectangle.h"

namespace Common
{
  namespace Inventory
  {
    namespace Math
    {
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
        Rectangle rect;
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
        Shape() {}
        virtual ~Shape()
        {
          NotifyDestroyed();
        }

        inline const Rectangle& GetShape() const { return rect; }
        virtual void SetShape(const Rectangle& v)
        {
          rect = v;
          NotifyMoved();
        }

        void AddListener(Listener* listener) { RemoveListener(listener); listeners.push_back(listener); }
        void RemoveListener(Listener* listener) { listeners.remove(listener); }
      };


      // TODO: Implement me!
      // It's just a basic vector now.

      /**
      * QuadTree class.
      */
      class QuadTree
      {
      public:
        typedef std::list<boost::shared_ptr<Shape> > QueryResult;

      private:
        class Node : public Shape::Listener
        {
        private:
          /// Copy constructor.
          Node(const Node& o);
          Node& operator=(const Node& o);

          typedef std::list<boost::weak_ptr<Shape> > Shapes;
          typedef Shapes::iterator Iterator;
          typedef Shapes::const_iterator ConstIterator;

        private:
          Node* parent;
          Rectangle nodeShape;
          Shapes shapes;
          std::vector<Node*> children;

          class Equal
          {
            Shape* s;
          public:
            Equal(Shape* shape) : s(shape) {}
            bool operator() (const boost::weak_ptr<Shape>& v) { return !v.lock() || s == v.lock().get(); }
          };

        private:
          virtual void Moved(Shape* shape);
          virtual void Destroyed(Shape* shape);

        public:
          /** Construct a node with a given volume and without children.
          * @param size of the new node
          */
          Node(Node* p, const Rectangle& size) : parent(parent), nodeShape(size), children(4, (Node*)0) {}

          /** Destructor. */
          ~Node();

          bool Add(boost::shared_ptr<Shape> shape);

          bool Remove(boost::shared_ptr<Shape> shape);

          template <typename S>
          QueryResult Query(const S& s, bool proper = true) const
          {
            QueryResult result;
            ConstIterator it;
            for (it = shapes.begin() ; it != shapes.end(); it++ )
            {
              if ((*it).lock())
                if ((*it).lock()->GetShape().Intersects(s, proper))
                  result.push_back((*it).lock());
            }

            return result;
          }

          template<typename T, typename S>
          std::list<boost::shared_ptr<T> > Query(const S& s, bool proper = true) const
          {
            std::list<boost::shared_ptr<T> > result;
            ConstIterator it;
            for (it = shapes.begin() ; it != shapes.end(); it++ )
            {
              if ((*it).lock())
                if ((*it).lock()->GetShape().Intersects(s, proper))
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
        QuadTree(const QuadTree& o);
        QuadTree& operator=(const QuadTree& o);

      private:
        Node* rootNode;

      public:
        /// Default constructor.
        QuadTree()
        {
          rootNode = new Node(0, Rectangle(PositionRef(0, 0), PositionRef(100000, 100000)));
        }

        ~QuadTree()
        {
          delete rootNode;
        }

        size_t Size()
        {
          return rootNode->Size();
        }

        bool Add(boost::shared_ptr<Shape> shape)
        {
          return rootNode->Add(shape);
        }

        bool Remove(boost::shared_ptr<Shape> shape)
        {
          return rootNode->Remove(shape);
        }

        template <typename S>
        QueryResult Query(const S& s, bool proper = true) const
        {
          return rootNode->Query(s, proper);
        }

        template<typename T, typename S>
        std::list<boost::shared_ptr<T> > Query(const S& s, bool proper = true) const
        {
          return rootNode->Query<T>(s, proper);
        }
      };

    } // Math namespace
  } // Inventory namespace
} // Common namespace

#endif // COMMON_QUADTREE_H
