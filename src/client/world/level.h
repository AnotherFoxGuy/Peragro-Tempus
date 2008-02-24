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

namespace PT
{
  class World;
  class ModelManager;
  struct Factory;
  class InteriorManager;
  struct Interior;

  class Level 
  {
  private: 
    struct MissingData;

  protected:
    bool finished;
    LevelLoader* levelLoader;
    std::string regionName;

    csRef<MissingData> missingData;

    iObjectRegistry* object_reg;
    ModelManager* modelManager;
    InteriorManager* interiorManager;

    // Resources.
    csRefArray<Factory> factories;
    csRef<iRegion> instances;
    csRefArray<iDocumentNode> instanceNodes;
    csRefArray<Interior> interiors;

    void OpenFile(const std::string& path, const std::string& fileName);
    virtual void LoadInstance(iDocumentNode* meshNode) {}

  private:
    void ParseFactories();
    void ParseInstances();
    void ParseInteriors();
    bool CheckResources();

    // Check the status and process the resources each frame.
    struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
    {
      Level* level;
      FrameCallBack (Level* lev) : scfImplementationType (this) { level = lev; }
      virtual void StartFrame (iEngine* engine, iRenderView* rview);
    };
    friend struct FrameCallBack;
    csRef<FrameCallBack> cb;

    // Search for missing data in our own Factories.
    struct MissingData : public scfImplementation1<MissingData, iMissingLoaderData>
    {
      Level* level;
      MissingData (Level* lev) : scfImplementationType (this) { level = lev; }

      virtual iMaterialWrapper* MissingMaterial (const char* name, const char* filename) { return 0; }
      virtual iTextureWrapper* MissingTexture (const char* name, const char* filename);
      virtual iShader* MissingShader (const char* name) { return 0; }
      virtual iMeshFactoryWrapper* MissingFactory (const char* name);
      virtual iMeshWrapper* MissingMesh (const char* name) { return 0; }
      virtual iSector* MissingSector (const char* name){ return 0; }
      virtual iLight* MissingLight (const char* name) { return 0; }
    };
    friend struct MissingData;

  public:
    Level(World* world);
    virtual ~Level();

    /// @return True if finished loading, false otherwise.
    bool IsReady() const { return finished; }
  };

} // PT namespace

#endif // LEVEL_H
