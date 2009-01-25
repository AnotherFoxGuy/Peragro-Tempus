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
 * @file octree.h
 * @brief octree class.
 */

#ifndef GEOM_QUADTREE_H
#define GEOM_QUADTREE_H

#define TREE_H_NOT_INCLUDED_DIRECTLY
#include "tree.h"
#undef TREE_H_NOT_INCLUDED_DIRECTLY

#include "rectangle.h"

namespace Geom
{
  template<typename T, typename G, bool AllowCollission>
  struct QuadTree
  {
    typedef Tree<T, G, 2, 4, Rectangle, AllowCollission> Type;
  };

} // namespace Geom

#endif // GEOM_QUADTREE_H
