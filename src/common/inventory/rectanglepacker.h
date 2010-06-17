/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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
 * @file rectanglepacker.h
 */

#ifndef COMMON_RECTANGLEPACKER_H
#define COMMON_RECTANGLEPACKER_H

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "src/common/inventory/positionref.h"

#include "common/utility/exception.h"

namespace Common
{
  namespace Inventory
  {
    /*
     * RectanglePacker class.
     */
    class RectanglePacker
    {
    private:
      struct Node
      {
        unsigned int x;
        unsigned int y;
        unsigned int w;
        unsigned int h;
        Node* left;
        Node* right;
        bool used;
        Node(unsigned int width, unsigned int height) 
          : x(0), y(0), w(width), h(height), left(0), right(0), used(false) {}
        Node(unsigned int xc, unsigned int yc, unsigned int width, unsigned int height) 
          : x(xc), y(yc), w(width), h(height), left(0), right(0), used(false) {}
        ~Node() { delete left; delete right; }
      };

      Node* root;
      unsigned int usedWidth;
      unsigned int usedHeight;

      PositionRef RecursiveFindFreePosition(Node* node, unsigned int w, unsigned int h) const;

    public:
      /**
       * Base constructor
       */
      RectanglePacker(unsigned int width, unsigned int height);
      ~RectanglePacker();

      /**
       * Find a free position for the given object.
       * @return True if a valid position was found, false otherwise.
       */
      bool FindFreePosition(PositionRef& position, boost::shared_ptr<Object> object) const;

      /**
       * Find free positions for the given objects.
       * @return True if valid positions for all objects were found, false otherwise.
       */
      bool FindFreePositions(std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects) const;
    };

    RectanglePacker::RectanglePacker(unsigned int width, unsigned int height)
      : root(0), usedWidth(0), usedHeight(0)
    {
      root = new Node(width, height);
    }

    RectanglePacker::~RectanglePacker()
    {
      delete root;
    }

    PositionRef RectanglePacker::RecursiveFindFreePosition(Node* node, unsigned int w, unsigned int h) const
    {
      // If we are not at a leaf then go deeper
      if (node->left) 
      {
        // Check first the left branch if not found then go by the right
        PositionRef coords = RecursiveFindFreePosition(node->left, w, h);
        return coords ? coords : RecursiveFindFreePosition(node->right, w, h);
      }
      else
      {
        // If already used or it's too big then return invalid position
        if (node->used || w > node->w || h > node->h)
          return PositionRef();

        // If it fits perfectly then use this gap
        if (w == node->w && h == node->h) 
        {
          node->used = true;
          return PositionRef(node->x, node->y);
        }

        // Initialize the left and right leafs by cloning the current node
        node->left = new Node(node->x, node->y, node->w, node->h);
        node->right = new Node(node->x, node->y, node->w, node->h);

        // Check if we partition vertical or horizontal
        if (node->w - w > node->h - h) 
        {
          node->left->w = w;
          node->right->x = node->x + w;
          node->right->w = node->w - w;
        } 
        else 
        {
          node->left->h = h;
          node->right->y = node->y + h;
          node->right->h = node->h - h;
        }

        return RecursiveFindFreePosition(node->left, w, h);
      }
    }

    bool RectanglePacker::FindFreePosition(PositionRef& position, boost::shared_ptr<Object> object) const
    {
      const Size& size = object->GetSize();
      unsigned int w = size.width;
      unsigned int h = size.height;
      PositionRef coords = RecursiveFindFreePosition(root, w, h);
      if (coords)
      {
        position = coords;
        return true;
      }
      return false;
    }

    bool RectanglePacker::FindFreePositions(std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects) const
    {
      positions.clear();
      std::list<boost::shared_ptr<Object> >::const_iterator it = objects.begin();
      for (; it != objects.end(); it++)
      {
        PositionRef coords;
        if(!FindFreePosition(coords, *it)) return false;
        positions.push_back(coords);
      }
      return true;
    }

  } // Inventory namespace
} // Common namespace

#endif // COMMON_RECTANGLEPACKER_H

