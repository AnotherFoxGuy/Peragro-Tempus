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
 * @file interior.h
 *
 * @basic An interior space.
 */

#ifndef INTERIOR_H
#define INTERIOR_H

#include "level.h"

namespace PT
{
  namespace World
  {
    /**
     * The interior interface.
     */
    struct iInterior : public virtual iBase
    {
      SCF_INTERFACE(iInterior, 1,0,0);
    };

    struct Interior;

    /**
     * An interior factory.
     */
    struct InteriorFactory
      : public scfImplementation1<InteriorFactory, iInterior>, Level
    {
    private:
      /// An array containing the document node of every instance in this
      /// interior factory.
      csRefArray<iDocumentNode> instanceNodes;

      /**
       * Load an instance into this interior. (meshes/portals/lights/...)
       * @param meshNode The document node of the mesh.
       */
      void LoadInstance(iDocumentNode* meshNode);

    public:

      /**
       * Creates a new interior factory.
       * @param fileName The mesh file path.
       * @param world The world the interior is part of.
       */
      InteriorFactory(const std::string& fileName, World* world);

      /// Destructor.
      ~InteriorFactory();

      /// The mesh file path.
      std::string fileName;

      friend struct Interior;
    };

    /**
     * An interior space of the world.
     * For example, a room. This creates one CS sector.
     */
    struct Interior : public scfImplementation1<Interior, iInterior>
    {
    private:
      /// Whether the interior has loaded.
      bool finished;
      /// Whether the interior is loading.
      bool loading;
      /// The document node for this interior.
      csRef<iDocumentNode> interiorNode;
      /// The interior factory.
      csRef<InteriorFactory> factory;
      /// The collection containing all instances in this interior.
      csRef<iCollection> instances;
      /// The sector for this interior.
      csRef<iSector> sector;
      /// A position. Not currently used.
      csVector3 position;

      /**
       * Get the Y rotation angle of a portal in this interior.
       * @param portalMesh The portal mesh.
       * @return The Y rotation angle.
       */
      float GetPortalYRotation(iMeshWrapper* portalMesh);

      /**
       * Get the position coordinates of a portal in this interior.
       * @param portalMesh The portal mesh.
       * @return The position.
       */
      csVector3 GetPortalPosition(iMeshWrapper* portalMesh);

      /**
       * Check if a mesh in this interior is a portal.
       * @param portalMesh The mesh.
       * @return True if the mesh is a portal, otherwise false.
       */
      bool IsPortal(iMeshWrapper* portalMesh);

      /**
       * Load an instance into this interior. (meshes/portals/lights/...)
       * @param meshNode The document node of the mesh.
       */
      void LoadInstance(iDocumentNode* meshNode);

      /**
       * Check the status and process the resources each frame.
       */
      bool CheckResources();

      struct FrameCallBack
        : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
      {
        Interior* inter;
        FrameCallBack (Interior* interior)
          : scfImplementationType (this) { inter = interior; }
        virtual void StartFrame (iEngine* engine, iRenderView* rview);
      };
      friend struct FrameCallBack;
      /// Callback from the engine to check progress.
      csRef<FrameCallBack> cb;

    public:
      /**
       * Create a new interior.
       * @param interiorNode The document node of the interior.
       * @param factory The factory of the interior.
       */
      Interior(iDocumentNode* interiorNode, InteriorFactory* factory);

      /// Destructor.
      ~Interior();

      /// The name of this interior.
      std::string interiorName;

      /**
       * Start loading the interior, add a frame callback to the engine to
       * check progress.
       */
      void Load();

      /**
       * Check if this interior is loaded.
       * @return True if finished, false otherwise.
       */
      bool IsReady() const { return finished; }
    };

  } // World namespace
} // PT namespace

#endif // INTERIOR_H
