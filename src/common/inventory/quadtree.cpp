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

#include "quadtree.h"

namespace Common
{
  namespace Inventory
  {
    namespace Math
    {
      //=======================================================================
      void QuadTree::Node::Moved(Shape* shape)
      {
        //printf("Node::Moved!\n");
        if (nodeShape.Contains(shape->GetShape(), false)) // TODO: is this false correct?
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

      void QuadTree::Node::Destroyed(Shape* shape)
      {
        //printf("Node::Destroyed!\n");
        shapes.remove_if(Equal(shape));
      }

      QuadTree::Node::~Node()
      {
        ConstIterator it;
        for (it = shapes.begin() ; it != shapes.end(); it++ )
          if ((*it).lock())
            (*it).lock()->RemoveListener(this);

        for (size_t i = 0; i < children.size(); ++i)
          delete children[i];
      }

      bool QuadTree::Node::Add(boost::shared_ptr<Shape> shape)
      {
        { // Check for overlaps
          ConstIterator it;
          for (it = shapes.begin() ; it != shapes.end(); it++ )
            if ((*it).lock())
              if ((*it).lock()->GetShape().Intersects(shape->GetShape(), true))
                return false;
        }

        // Make sure there are no doubles!
        Remove(shape);
        shapes.push_back(shape);
        shape->AddListener(this);
        return true;
      }

      bool QuadTree::Node::Remove(boost::shared_ptr<Shape> shape)
      {
        shape->RemoveListener(this);
        shapes.remove_if(Equal(shape.get()));
        return true;
      }
      //=======================================================================

    } // Math namespace
  } // Inventory namespace
} // Common namespace
