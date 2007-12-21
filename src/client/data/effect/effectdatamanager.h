/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef PT_EFFECTDATAMANAGER_H
#define PT_EFFECTDATAMANAGER_H

#include <vector>
#include <string>

#include <csutil/ref.h>
#include <iutil/vfs.h>

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class Effect;

    /**
     * @ingroup data_handlers
     * Handles the loading of effects data from XML definition file. The data
     * resides in $DATA/xml/effects/effects.xml file, where $DATA is the parent
     * directory where the Peragro Tempus data resides.
     * @author Jelle Hellemans
     */
    class EffectDataManager
    {
    private:
      ///Effects descriptions. Instances are owned by EffectDataManager.
      ///@internal We're using a std::vector here since it's being populated
      ///only during application startup. It also offers speed when accessing
      ///elements.
      std::vector<Effect*> effects;

      csRef<iVFS> vfs;

    public:
      /**
       * Base constructor
       */
      EffectDataManager();
      ~EffectDataManager();

      /**
       * Loads all the effects information located in effects definition file.
       * @return True if successful, false if an error occured.
       */
      bool LoadEffectData();

      /**
       * @param id Unique ID of the wanted effect.
       * @return Pointer to effect with given ID, or 0 if none was found.
       */
      Effect* GetEffectById(unsigned int id) const;
      /**
       * @param name Name of the wanted effect.
       * @return Pointer to effect with given Name, or 0 if none was found.
       */
      Effect* GetEffectByName(const std::string& name) const;
    };
  } // Data namespace
} // PT namespace

#endif // PT_EFFECTDATAMANAGER_H
