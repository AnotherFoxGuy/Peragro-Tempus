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

#ifndef INTERIOR_H
#define INTERIOR_H

#include "level.h"

namespace PT
{
  struct iInterior : public virtual iBase
  {
    SCF_INTERFACE(iInterior, 1,0,0);
  };

  struct Interior;
  struct InteriorFactory : public scfImplementation1<InteriorFactory, iInterior>, Level
  {
  private:
    csRefArray<iDocumentNode> instanceNodes;
    void LoadInstance(iDocumentNode* meshNode);

  public:
    InteriorFactory(const std::string& fileName, World* world);
    ~InteriorFactory();

    std::string fileName;

    friend struct Interior;
  };

  struct Interior : public scfImplementation1<Interior, iInterior>
  {
  private:
    bool finished;
    bool loading;
    csRef<iDocumentNode> interiorNode;
    csRef<InteriorFactory> factory;
    csRef<iRegion> region;
    csRef<iSector> sector;
    csVector3 position;

    float GetPortalYRotation(iMeshWrapper* portalMesh);
    csVector3 GetPortalPosition(iMeshWrapper* portalMesh);
    bool IsPortal(iMeshWrapper* portalMesh); 

    void LoadInstance(iDocumentNode* meshNode);

    bool CheckResources();
    // Check the status and process the resources each frame.
    struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
    {
      Interior* inter;
      FrameCallBack (Interior* interior) : scfImplementationType (this) { inter = interior; }
      virtual void StartFrame (iEngine* engine, iRenderView* rview);
    };
    friend struct FrameCallBack;
    csRef<FrameCallBack> cb;

  public:
    Interior(iDocumentNode* interiorNode, InteriorFactory* factory);
    ~Interior();

    std::string interiorName;

    void Load();

    /// @return True if finished loading, false otherwise.
    bool IsReady() const { return finished; }
  };

} // PT namespace

#endif // INTERIOR_H
