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
 * @file level.h
 *
 * @basic A level in the world.
 */

#ifndef LEVEL_H
#define LEVEL_H

#include <cssysdef.h>
#include <csutil/refarr.h>
#include <iengine/engine.h>
#include <imap/loader.h>

#include <string>

struct iObjectRegistry;
struct iEngineFrameCallback;

class LevelLoader;
class WorldManager;

namespace PT
{
  namespace World
  {
    class ModelManager;
    struct Factory;
    class InteriorManager;
    struct Interior;

    /**
     * A level, can be a map tile or an interior.
     */
    class Level
    {
    private:
      struct MissingData;

    protected:
      /// Whether the level has finished loading.
      bool finished;
      /// Mark the level as finished loading.
      virtual void SetReady() { finished = true; }

      /// The level loader.
      LevelLoader* levelLoader;
      /// The name of the level's collection.
      std::string collectionName;

      /// The structure to store the missing data for this level
      csRef<MissingData> missingData;

      /// The object registry.
      iObjectRegistry* object_reg;
      /// The model manager.
      ModelManager* modelManager;
      /// The interior manager.
      InteriorManager* interiorManager;

      /// The factories.
      csRefArray<Factory> factories;
      /// The collection containing all instances in this interior.
      csRef<iCollection> instances;
      /// An array containing the document node of every instance in this
      /// level.
      csRefArray<iDocumentNode> instanceNodes;
      /// The array of interiors.
      csRefArray<Interior> interiors;

      /**
       * Open a file and start loading it into this level.
       * @param path The path to the file.
       * @param fileName The file name.
       */
      void OpenFile(const std::string& path, const std::string& fileName);

      /**
       * Load an instance into this level. (meshes/portals/lights/...)
       * @param meshNode The document node of the mesh.
       */
      virtual void LoadInstance(iDocumentNode* meshNode) {}

    private:
      /// Add factories from the loader.
      void ParseFactories();
      /// Add instances from the loader.
      void ParseInstances();
      /// Add interiors from the loader.
      void ParseInteriors();
      /// Check the status and process the resources each frame.
      bool CheckResources();

      struct FrameCallBack
        : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
      {
        Level* level;
        FrameCallBack (Level* lev) : scfImplementationType (this)
        {
          level = lev;
        }
        virtual void StartFrame (iEngine* engine, iRenderView* rview);
      };
      friend struct FrameCallBack;
      /// Callback from the engine to check progress.
      csRef<FrameCallBack> cb;

      /**
       * Used to search for any missing data in our own instances.
       * (meshes/portals/lights/...)
       */
      struct MissingData
        : public scfImplementation1<MissingData, iMissingLoaderData>
      {
        /// The level.
        Level* level;
        /// Constructor.
        MissingData (Level* lev) : scfImplementationType (this)
        {
          level = lev;
        }

        /// Search for the missing materials, load them if found.
        virtual iMaterialWrapper* MissingMaterial (const char* name,
          const char* filename) { return 0; }
        /// Search for the missing textures, load them if found.
        virtual iTextureWrapper* MissingTexture (const char* name,
          const char* filename);
        /// Search for the missing shaders, load them if found.
        virtual iShader* MissingShader (const char* name) { return 0; }
        /// Search for the missing materials, load them if found.
        virtual iMeshFactoryWrapper* MissingFactory (const char* name);
        /// Search for the missing meshes, load them if found.
        virtual iMeshWrapper* MissingMesh (const char* name) { return 0; }
        /// Search for the missing sectors, load them if found.
        virtual iSector* MissingSector (const char* name){ return 0; }
        /// Search for the missing lights, load them if found.
        virtual iLight* MissingLight (const char* name) { return 0; }
      };
      friend struct MissingData;

    public:
      /**
       * Constructor.
       * @param world The world this level is part of.
       */
      Level(WorldManager* world);

      /// Destructor.
      virtual ~Level();

      /**
       * Check if this level is loaded.
       * @return True if finished, false otherwise.
       */
      bool IsReady() const { return finished; }
    };

  } // World namespace
} // PT namespace

#endif // LEVEL_H
