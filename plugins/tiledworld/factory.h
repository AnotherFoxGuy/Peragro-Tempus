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
/**
 * @file factory.h
 *
 * @basic Used to create instances of a model.
 */

#ifndef FACTORY_H
#define FACTORY_H

#include <cssysdef.h>

#include <string>

struct iObjectRegistry;
struct iCollection;
class FileLoader;

namespace PT
{
  namespace World
  {
    /**
     * The model factory interface.
     */
    struct iModelFactory : public virtual iBase
    {
      SCF_INTERFACE(iModelFactory, 1,0,0);
    };

    /**
     * Used to create instances of a model.
     */
    struct Factory : public scfImplementation1<Factory, iModelFactory>
    {
    private:
      /// The collection that is used for this factory.
      csRef<iCollection> collection;
      /// The file loader.
      FileLoader* fileLoader;
      /// The object registry.
      iObjectRegistry* object_reg;
      /// Whether all textures have been precached.
      bool isPrecached;

    public:
      /**
       * Constructor.
       * @param fileName The file name.
       * @param object_reg The object registry.
       */
      Factory(const std::string& fileName, iObjectRegistry* object_reg);
      /// Destructor.
      ~Factory();

      /// The file name.
      std::string fileName;

      /**
       * Start loading the file. Don't call this function yourself!
       * This is called by ModelManager::Get().
       */
      void Load();
      /// Returns true if finished loading, false otherwise.
      bool IsReady() const;
      /// Returns true if all textures have been precached.
      bool IsPrecached() const { return isPrecached; }

      /**
       * Call this each frame to process the loading of this factory.
       */
      void Process();

      /**
       * Precaches 'one' texture (if any) and returns.
       * Call this repeatedly over a period of time to smooth out texture
       * loading.
       * Call IsPrecached() to see if everything has been precached.
       */
      void Precache();

      /// Get the collection that is used internally for holding the data.
      iCollection* GetCollection() { return collection; }
    };

  } // World namespace
} // PT namespace

#endif // FACTORY_H
