/*
    Copyright (C) 2008 by Mogue Carpenter

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
//File: `solarsys.h'

#ifndef _H_SOLARSYS
#define _H_SOLARSYS

#define solarsys_report_lvl false

#include <string>

#include "crystalspace.h"
#include "isolarsys.h"
#include "imybody.h"
#include "imystarbox.h"

class SolarSysCameraCatcher;

// Implementation header
class Solarsys : public scfImplementation1<Solarsys, iSolarsys>
{
  private:
    iObjectRegistry* object_reg;
    csRef<iEngine> engine;
    csRef<iGraphics3D> g3d;
    bool initialized ;

    /// The render manager, cares about selecting lights+meshes to render
    csRef<FrameBegin3DDraw> drawer;
    csRef<FramePrinter> printer;
    csRef<iRenderManager> rm;

    /// The objects to create the background
    csRef<iMyBody> rootbody ;
    csRef<iMyBody> surbody ;   // Selected body to place the camera on
    float lat;
    float lon;
    csRef<iMyStarbox> starbox;

    long  last_update_seconds;

    /// A pointer to the view which contains the camera.
    csRef<iView> solarview;
    csRef<iSector> sector;      // Sector should be different from you main game/map sector default solarsys

    std::string solarsys_name;

    // The camera catcher.
    csRef<SolarSysCameraCatcher> catcher;

  public:
    Solarsys(iObjectRegistry* r);
    virtual ~Solarsys();
    virtual bool Initialize(iObjectRegistry *object_reg);

    virtual void SetName(char const* name) { solarsys_name = name; };
    virtual void SetSector(csRef<iSector>& sector);

    virtual void SetRootbody (csRef<iMyBody>& body) { rootbody = body; };
    virtual void SetLongitude(long longitude ) { lon = longitude; };
    virtual void SetLatitude(long latitude ) { lat = latitude; };
    virtual void SetSurfaceBody (csRef<iMyBody>& body) { surbody = body; };
    virtual void SetStarbox (csRef<iMyStarbox>& sb ) { starbox = sb; };

    virtual char const* GetName() const { return solarsys_name.c_str(); };
    virtual iSector const* GetSector() const { return sector; };
    virtual csVector3 GetRelatveRootPos();

    virtual void DrawSolarSys( iCamera* c );
    virtual void DrawSolarSys( iCamera* c , long ts );
    virtual void UpdateSystemTime( long ts );

    //virtual void Draw( iCamera c, long ts );

  private:

    bool CreateCamera();
    void DrawStarbox(iCamera* c);
    void DrawCameraAxis(iCamera* view_c, iCamera* surface_c );
};


// Factory Implementation header
class SolarsysFactory : public scfImplementation2<SolarsysFactory, iSolarsysFactory, iComponent>
{
  private:
    iObjectRegistry* object_reg;

  public:
    SolarsysFactory (iBase* p);
    virtual csPtr<iSolarsys> CreateObject ();
    bool Initialize (iObjectRegistry*);
};


//--------------------------------------------------------------------------

class SolarSysCameraCatcher : public scfImplementation1<SolarSysCameraCatcher, iEngineFrameCallback>
{
  public:
    iCamera* camera ;
    iSolarsys* solarsys ;

  public:
    SolarSysCameraCatcher(iSolarsys* asolarsys) : scfImplementationType(this)
    {
       camera = 0;
       solarsys = asolarsys;
    }

    virtual ~SolarSysCameraCatcher(){}

    virtual void StartFrame(iEngine* engine, iRenderView* rview)
    {
      if (!rview || !solarsys ) return;

      if (rview->GetCamera ()->GetSector () != solarsys->GetSector () )
      {
        solarsys->DrawSolarSys(rview->GetCamera());
      }
    }

}; // end class csCameraCatcher


#endif
// _H_SOLARSYS
