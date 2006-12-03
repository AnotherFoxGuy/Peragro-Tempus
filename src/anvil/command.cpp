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

#include "command.h"
#include "engine.h"

#include <cssysdef.h>
#include <cstool/keyval.h>
#include <csutil/stringarray.h>
#include <iengine/movable.h>
#include <imesh/object.h>

SCF_IMPLEMENT_IBASE (anvSelectCommand)
  SCF_IMPLEMENTS_INTERFACE (iAnvSelectCommand)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (anvTransformCommand)
  SCF_IMPLEMENTS_INTERFACE (iAnvTransformCommand)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (anvSetPropertyCommand)
  SCF_IMPLEMENTS_INTERFACE (iAnvSetPropertyCommand)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (anvCreateCommand)
  SCF_IMPLEMENTS_INTERFACE (iAnvCreateCommand)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE (anvRemoveCommand)
  SCF_IMPLEMENTS_INTERFACE (iAnvRemoveCommand)
SCF_IMPLEMENT_IBASE_END

void anvSelectCommand::Initialize(anvSelection newSelection, anvSelection oldSelection)
{
  this->newSelection = newSelection;
  this->oldSelection = oldSelection;
}

void anvTransformCommand::Initialize(csReversibleTransform transform, bool local)
{
  this->transform = transform;
  this->local = local;
}

void anvSetPropertyCommand::Initialize(csString name, csStringArray newValues, csStringArray oldValues)
{
  this->name = name;
  this->newValues = newValues;
  this->oldValues = oldValues;
}

void anvCreateCommand::Initialize(csRef<iMeshWrapper> mesh)
{
  this->mesh = mesh;
}

void anvRemoveCommand::Initialize(anvSelection selection)
{
  this->selection = selection;
}

void anvSelectCommand::Perform()
{
  anvEngine::GetAnvil()->SetSelection(newSelection);
}

void anvSelectCommand::Revert()
{
  anvEngine::GetAnvil()->SetSelection(oldSelection);
}

void anvTransformCommand::Perform()
{
  Transform(transform, local);
}

void anvTransformCommand::Revert()
{
  Transform(transform.GetInverse(), local);
}

void anvTransformCommand::Transform(csReversibleTransform transform, bool local)
{
  anvEngine *engine = anvEngine::GetAnvil();
  anvSelection selection = engine->GetSelection();
  int editmode = engine->GetEditMode();
  
  if (editmode == anvEngine::MeshEditMode)
  {
    anvSelection::Iterator iterator = selection.GetIterator();
    if (local)
    {
      csBox3 bbox;
      while (iterator.HasNext())
      {
        csRef<iMeshWrapper> meshwrapper = scfQueryInterface<iMeshWrapper> (iterator.Next());
        SoftTransform(meshwrapper, transform);
        csBox3 cbox;
        meshwrapper->GetWorldBoundingBox(cbox);
        bbox += cbox;
      }
      
      engine->UpdateSelection(bbox);
    }
    else
    {
      // @@@: To be implemented
      // Rotate about or scale from the selection center
    }
  }
}

inline void anvTransformCommand::SoftTransform(iMeshWrapper* meshwrapper, csReversibleTransform transform)
{
  meshwrapper->GetMovable()->Transform(transform.GetT2O());
  meshwrapper->GetMovable()->MovePosition(transform.GetOrigin());
  meshwrapper->GetMovable()->UpdateMove();
}

void anvSetPropertyCommand::Perform()
{
  SetProperty(newValues);
}

void anvSetPropertyCommand::Revert()
{
  SetProperty(oldValues);
}

void anvSetPropertyCommand::SetProperty(csStringArray values)
{
  int editmode = anvEngine::GetAnvil()->GetEditMode();
  anvSelection selection = anvEngine::GetAnvil()->GetSelection();
  
  if (editmode == anvEngine::MeshEditMode)
  {
    anvSelection::Iterator iterator = selection.GetIterator();
    while (iterator.HasNext())
    {
      csRef<iMeshWrapper> meshwrapper = scfQueryInterface<iMeshWrapper> (iterator.Next());
      csKeyValuePair* cskvp = new csKeyValuePair (name);
      csStringArray::Iterator iterator = values.GetIterator();
      while (iterator.HasNext())
      {
        const char* value = iterator.Next();
        cskvp->SetValue (name, value);
      }
      meshwrapper->QueryObject()->ObjAdd(cskvp);
    }
  }
  else if (editmode == anvEngine::TerrainEditMode)
  {
    anvSelection::Iterator iterator = selection.GetIterator();
    while (iterator.HasNext())
    {
      // @@@: To be implemented
    }
  }
}

void anvCreateCommand::Perform()
{
  int editmode = anvEngine::GetAnvil()->GetEditMode();
  if (editmode != anvEngine::MeshEditMode)
    return;
    
  anvEngine::GetAnvil()->GetEngine()->AddMeshAndChildren(mesh);
}

void anvCreateCommand::Revert()
{
  int editmode = anvEngine::GetAnvil()->GetEditMode();
  if (editmode != anvEngine::MeshEditMode)
    return;
  
  anvEngine::GetAnvil()->GetEngine()->RemoveObject(mesh);
}

void anvRemoveCommand::Perform()
{
  int editmode = anvEngine::GetAnvil()->GetEditMode();
  if (editmode != anvEngine::MeshEditMode)
    return;
    
  anvSelection::Iterator iterator = selection.GetIterator();
  while (iterator.HasNext())
  {
    csRef<iMeshWrapper> meshwrapper = scfQueryInterface<iMeshWrapper> (iterator.Next());
    anvEngine::GetAnvil()->GetEngine()->RemoveObject(meshwrapper);
  }
}

void anvRemoveCommand::Revert()
{
  int editmode = anvEngine::GetAnvil()->GetEditMode();
  if (editmode != anvEngine::MeshEditMode)
    return;
    
  anvSelection::Iterator iterator = selection.GetIterator();
  while (iterator.HasNext())
  {
    csRef<iMeshWrapper> meshwrapper = scfQueryInterface<iMeshWrapper> (iterator.Next());
    anvEngine::GetAnvil()->GetEngine()->AddMeshAndChildren(meshwrapper);
  }
}
