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

#ifndef WFMATH_OCTREE_H
#define WFMATH_OCTREE_H

#define TREE_H_NOT_INCLUDED_DIRECTLY
#include "tree.h"
#undef TREE_H_NOT_INCLUDED_DIRECTLY

namespace WFMath
{
  /**
   * An octree class.
   * AllowOverlap is wether to allow intersection when adding to the tree.
   **/
  template<typename G, bool AllowOverlap>
  struct OcTree
  {
    typedef Tree<G, 3, 2, 8, AllowOverlap> Type;
  };

} // namespace WFMath

#endif // WFMATH_OCTREE_H
