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

#ifndef __ANV_COMMAND_H
#define __ANV_COMMAND_H

#include "ianvil.h"

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/csbaseeventh.h>
#include <csutil/refarr.h>
#include <cstool/csapplicationframework.h>
#include <iengine/engine.h>
#include <ivideo/graph3d.h>
#include <csutil/stringarray.h>

class anvSelectCommand : public iAnvSelectCommand
{
  private:
    anvSelection newSelection, oldSelection;
    
  public:
    SCF_DECLARE_IBASE;
    
    anvSelectCommand() {}
    anvSelectCommand(anvSelection newSelection, anvSelection oldSelection)
    {
      Initialize(newSelection, oldSelection);
    }
    
    virtual ~anvSelectCommand() {}
    
    virtual void Initialize(anvSelection newSelection, anvSelection oldSelection);
    virtual void Perform();
    virtual void Revert();
};

class anvTransformCommand : public iAnvTransformCommand
{
  private:
    csReversibleTransform transform;
    bool local;
    
  public:
    SCF_DECLARE_IBASE;
    
    anvTransformCommand() {}
    anvTransformCommand(csReversibleTransform transform, bool local)
    {
      Initialize(transform, local);
    }
    
    virtual ~anvTransformCommand() {}
    
    virtual void Initialize(csReversibleTransform transform, bool local);
    virtual void Perform();
    virtual void Revert();
    
    /// This performs the transform but does not store anything. It is irreversible.
    static void Transform(csReversibleTransform transform, bool local);
    
    static inline void SoftTransform(iMeshWrapper* meshwrapper, csReversibleTransform transform);
};

class anvSetPropertyCommand : public iAnvSetPropertyCommand
{
  private:
    csString name;
    csStringArray newValues, oldValues;
    
    void SetProperty(csStringArray values);
    
  public:
    SCF_DECLARE_IBASE;
    
    anvSetPropertyCommand() {}
    anvSetPropertyCommand(csString name, csStringArray newValues, csStringArray oldValues)
    {
      Initialize(name, newValues, oldValues);
    }
    
    virtual ~anvSetPropertyCommand() {}
    
    virtual void Initialize(csString name, csStringArray newValues, csStringArray oldValues);
    virtual void Perform();
    virtual void Revert();
};

class anvCreateCommand : public iAnvCreateCommand
{
  private:
    csRef<iMeshWrapper> mesh;
    
  public:
    SCF_DECLARE_IBASE;
    
    anvCreateCommand() {}
    anvCreateCommand(csRef<iMeshWrapper> mesh)
    {
      Initialize(mesh);
    }
    
    virtual ~anvCreateCommand() {}
    
    virtual void Initialize(csRef<iMeshWrapper> mesh);
    virtual void Perform();
    virtual void Revert();
};

class anvRemoveCommand : public iAnvRemoveCommand
{
  private:
    anvSelection selection;
    
  public:
    SCF_DECLARE_IBASE;
    
    anvRemoveCommand() {}
    anvRemoveCommand(anvSelection selection)
    {
      Initialize(selection);
    }
    
    virtual ~anvRemoveCommand() {}
      
    virtual void Initialize(anvSelection selection);
    virtual void Perform();
    virtual void Revert();
};

#endif // __ANV_COMMAND_H
