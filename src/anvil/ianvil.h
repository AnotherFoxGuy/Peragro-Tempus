/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef __ANVIL_H
#define __ANVIL_H

#include <cssysdef.h>
#include <csutil/scf.h>
#include <iengine/mesh.h>
#include <iengine/movable.h>

class csStringArray;

typedef csRefArray<iBase> anvSelection;

SCF_VERSION(iAnvCommand, 0, 0, 1);

/**
 * Revertable command interface
 */
struct iAnvCommand : public iBase
{
  /// Perform the command
  virtual void Perform() = 0;
  
  /// Revert the command
  virtual void Revert() = 0;
  
};

typedef csRefArray<iAnvCommand> anvCommandStack;

SCF_VERSION(iAnvSelectCommand, 0, 0, 1);

/** 
 * Set the selection.
 *
 * Edit Modes:
 * - All
 */
struct iAnvSelectCommand : public iAnvCommand
{
  virtual void Initialize(anvSelection newSelection, anvSelection oldSelection) = 0;
};

SCF_VERSION(iAnvTransformCommand, 0, 0, 1);

/**
 * Transform the selection.
 * 
 * Edit Modes:
 * - Mesh
 */
struct iAnvTransformCommand : public iAnvCommand
{
  virtual void Initialize(csReversibleTransform transform, bool local) = 0;
};

SCF_VERSION(iAnvSetPropertyCommand, 0, 0, 1);

/**
 * Set the property of each object in the selection.
 * 
 * Edit Modes:
 * - Mesh
 * - Terrain
 */
struct iAnvSetPropertyCommand : public iAnvCommand
{
  virtual void Initialize(csString name, csStringArray newValues, csStringArray oldValues) = 0;
};

SCF_VERSION(iAnvCreateCommand, 0, 0, 1);

/**
 * Create a mesh.
 * 
 * Edit Modes:
 * - Mesh
 */
struct iAnvCreateCommand : public iAnvCommand
{
  virtual void Initialize(csRef<iMeshWrapper> mesh) = 0;
};

SCF_VERSION(iAnvRemoveCommand, 0, 0, 1);

/**
 * Remove everything in the selection from the engine.
 *
 * Edit Modes:
 * - Mesh
 */
struct iAnvRemoveCommand : public iAnvCommand
{
  virtual void Initialize(anvSelection selection) = 0;
};

#endif // __ANVIL_H
