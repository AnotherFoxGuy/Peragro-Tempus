/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef IEFFECTSMANAGER_H
#define IEFFECTSMANAGER_H

#include <cssysdef.h>
#include <csutil/scf_implementation.h>

#include <string>

struct iSector;
struct iMeshWrapper;
class csVector3;

/**
 * @ingroup effects
 * Handles the creation of particle/mesh effects.
 */
struct iEffectsManager : public virtual iBase
{
  SCF_INTERFACE(iEffectsManager, 1,0,0);

  /**
   * @param fileName From where to load the effect templates.
   * @return True if successful, false if an error occured.
   */
  virtual bool LoadEffectTemplates(const std::string& fileName) = 0;

  /**
   * @param effectName Unique name for the wanted effect.
   * @param parent iMeshWrapper to bind this effect to.
   * @return True if successful, false if an error occured.
   */
  virtual bool CreateEffect (const std::string& effectName, iMeshWrapper* parent) = 0;

  /**
   * @param effectName Unique name for the wanted effect.
   * @param pos Position to where to create this effect.
   * @return True if successful, false if an error occured.
   */
  virtual bool CreateEffect (const std::string& effectName, const csVector3& pos, iSector* sector = 0) = 0;

  /**
   * @param pos Position to where to create this effect.
   * @return True if successful, false if an error occured.
   */
  virtual bool CreateDecal (csVector3 pos) = 0;

};

#endif // IEFFECTSMANAGER_H
