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

#ifndef ISOUNDMANAGER_H
#define ISOUNDMANAGER_H

#include <cssysdef.h>
#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include <string>

class iDocumentNode;

namespace PT
{
  namespace Sound
  {
    struct iSoundManager : public virtual iBase
    {
      SCF_INTERFACE(iSoundManager, 1,0,0);

      virtual bool LoadSoundEvents(const char* fileName) = 0;
      virtual bool LoadSoundEvents(iDocumentNode* node, const char* prefix = 0) = 0;

      virtual bool RemoveSound(csEventID eventId) = 0;
      virtual bool RemoveSound(const char* fileName) = 0;
    };

  } // World namespace
} // PT namespace

#endif
