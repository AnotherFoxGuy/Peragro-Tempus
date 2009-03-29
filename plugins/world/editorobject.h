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
 * @file world.h
 *
 * @basic Manages the game world.
 */

#ifndef EDITOROBJECT_H
#define EDITOROBJECT_H

#include "include/world.h"

#include <cssysdef.h>
#include <csutil/scf_implementation.h>
#include <iengine/movable.h>
#include <iengine/mesh.h>
#include <iutil/object.h>

#include "common/world/world.h"
#include "common/util/geomhelper.h"

struct iObjectRegistry;


/**
 * Wrapper for an object that can be edited.
 */
struct EditorObject
  : public scfImplementation2<EditorObject, iObject, iMovableListener>
{
  /// The object registry.
  iObjectRegistry* object_reg;
  /// The mesh wrapper.
  csWeakRef<iMeshWrapper> wrap;
  /// The object.
  Common::World::Objectp _object;

  WFMath::Point<3> currentPosition;
  WFMath::RotMatrix<3> currentRotation;
  WFMath::AxisBox<3> currentShape;

  /// Construct from object and mesh.
  EditorObject (Common::World::Objectp object, iObjectRegistry* obj_reg,
    iMeshWrapper* mesh);

  /// Save changes to this object in the world manager.
  void CommitChanges();

  /// Callback function for iMovableListener.
  void MovableChanged (iMovable* movable);
  /// Callback function for iMovableListener.
  void MovableDestroyed (iMovable* movable);

  /// Set object name.
  void SetName (const char *iName);
  /// Get object name.
  const char* GetName () const;
  /// Get object id.
  uint GetID () const;

  void SetObjectParent (iObject *obj);
  iObject* GetObjectParent () const;
  void ObjAdd (iObject *obj);
  void ObjRemove (iObject *obj);
  void ObjRemoveAll ();
  void ObjAddChildren (iObject *Parent);
  iObject* GetChild (int iInterfaceID, int iVersion, const char *Name,
    bool FirstName) const;
  iObject* GetChild (const char *Name) const;
  csPtr<iObjectIterator> GetIterator ();
  void ObjReleaseOld (iObject *obj);
  void AddNameChangeListener (iObjectNameChangeListener* listener);
  void RemoveNameChangeListener (iObjectNameChangeListener* listener);
  iObject* GetChild (int iInterfaceID, int iVersion, const char *Name = 0)
    const;
};

inline EditorObject::EditorObject(Common::World::Objectp object,
  iObjectRegistry* obj_reg, iMeshWrapper* mesh)
  : scfImplementationType (this), object_reg(obj_reg), wrap(mesh),
  _object(object)
{
  wrap->GetMovable()->AddListener(this);
  wrap->QueryObject()->ObjAdd(this);

  currentPosition = _object->position;
  currentRotation = _object->rotation;
  currentShape = _object->GetShape();
}

inline void EditorObject::CommitChanges()
{
  if ((currentPosition != _object->position)
    ||(currentRotation != _object->rotation)
    /*||(currentShape != _object->GetShape())*/)
  {
    csRef<iWorld> world = csQueryRegistry<iWorld> (object_reg);
    world->CommitChanges(_object);

    currentPosition = _object->position;
    currentRotation = _object->rotation;
    currentShape = _object->GetShape();
  }
}

// iMovableListener
inline void EditorObject::MovableChanged (iMovable* movable)
{
  csBox3 box = wrap->GetWorldBoundingBox();
  _object->position = VectorHelper::Convert(movable->GetTransform().GetOrigin());
  //TODO _object->rotation = MatrixHelper::Convert(movable->GetTransform().GetO2T());
  _object->SetShape(WFMath::AxisBox<3>(VectorHelper::Convert(box.Min()), VectorHelper::Convert(box.Max())));
  CommitChanges();
}

inline void EditorObject::MovableDestroyed (iMovable* movable)
{}

// iObject
inline void EditorObject::SetName (const char *iName)
{ _object->name = iName; CommitChanges(); }

inline const char* EditorObject::GetName () const
{ return _object->name.c_str(); }

inline uint EditorObject::GetID () const
{ return (uint)_object->id; }

inline void EditorObject::SetObjectParent (iObject *obj)
{}

inline iObject* EditorObject::GetObjectParent () const
{ return 0; }

inline void EditorObject::ObjAdd (iObject *obj)
{}

inline void EditorObject::ObjRemove (iObject *obj)
{}

inline void EditorObject::ObjRemoveAll ()
{}

inline void EditorObject::ObjAddChildren (iObject *Parent)
{}

inline iObject* EditorObject::GetChild (int iInterfaceID, int iVersion,
  const char *Name, bool FirstName) const
{ return 0; }

inline iObject* EditorObject::GetChild (const char *Name) const
{ return 0; }

inline csPtr<iObjectIterator> EditorObject::GetIterator ()
{ return 0; }

inline void EditorObject::ObjReleaseOld (iObject *obj)
{}

inline void EditorObject::AddNameChangeListener (
  iObjectNameChangeListener* listener)
{}

inline void EditorObject::RemoveNameChangeListener (
  iObjectNameChangeListener* listener)
{}

inline iObject* EditorObject::GetChild (int iInterfaceID, int iVersion,
  const char *Name) const
{ return 0; }

#endif
