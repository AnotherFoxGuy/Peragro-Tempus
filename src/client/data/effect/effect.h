/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef EFFECT_H
#define EFFECT_H


#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"
#include "csutil/ref.h"

#include "iengine/mesh.h"

namespace PT
{
  namespace Effect
  {
    class Effect 
    {
    private:
      csRef<iMeshWrapper> mesh;
      csRef<iMeshWrapper> parent;
      int timeleft;

    public:

      Effect (iMeshWrapper* mesh, int timeleft);
      ~Effect();

      bool Handle (csTicks elapsed_ticks);

      iMeshWrapper* GetMesh () const { return mesh; }	  
    };

  } // Data namespace 
} // PT namespace 

#endif // EFFECT_H
