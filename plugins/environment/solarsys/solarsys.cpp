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
//File: `myobject.cpp' 
 	  
#include "solarsys.h"

CS_IMPLEMENT_PLUGIN 

SCF_IMPLEMENT_FACTORY(SolarsysFactory);
 
Solarsys::Solarsys (iObjectRegistry* r)
  : scfImplementationType (this)
{
  last_update_seconds = 0 ;
  initialized = false;
  Initialize ( r );
}

Solarsys::~Solarsys() 
{
  // Shut down the event handlers we spawned earlier.
  drawer.Invalidate();
  printer.Invalidate();
}

bool Solarsys::Initialize(iObjectRegistry *object_reg)
{
 
  if (initialized) return true;

  printf("Solarsys::Initialize\n");

  engine = csQueryRegistry<iEngine> (object_reg);
  if (!engine) 
  {
    printf ("Solarsys::Initialize: Error, cant find iEngine\n");
  }

  g3d = csQueryRegistry<iGraphics3D> (object_reg);
  if (!g3d)
  {
    printf ("Solarsys::Initialize: Error, cant find iEngine\n");
  }

  // Set up camera catcher so solarsys can autodraw on frame events
  catcher.AttachNew(new SolarSysCameraCatcher(this));
  if (!catcher) printf("Solarbody No Camera Catcher\n");
  if ( engine && catcher) engine->AddEngineFrameCallback(catcher);


  if ( !engine || !g3d ) 
  {
    printf("Solarsys::Initialize:Failed to Initialize!\n");
    return false;
  }

  if (!CreateCamera() )
  {
    printf("Solarsys::Initialize:Failed to create view!\n");
    return false;
  }    

  initialized = true;
  return initialized;

}
 
void Solarsys::DrawSolarSys(iCamera* c) 
{
   DrawSolarSys( c , last_update_seconds );
}



void Solarsys::DrawSolarSys( iCamera* c , long ts )
{
  last_update_seconds = ts;

  if ( rootbody  ) 
  {
//    DrawStarbox(c); // draw the starbox if loaded 

    // display orbits from body's surface POV
    rootbody->Update_Body( ts );
    rootbody->Update_Mesh_Pos();
    rootbody->Update_Lights();
    if ( solarview ) 
    { 
      if ( surbody )
      {
        csOrthoTransform surface_pos;
        surface_pos = surbody->GetSurfaceOrthoTransform(lon,lat);

// attempt 1
        surface_pos = c->GetTransform () * surface_pos ;
        solarview->GetCamera ()->SetTransform (surface_pos);
//        solarview->GetCamera ()->SetTransform ( c->GetTransform());
      } else 
      {
        solarview->GetCamera ()->SetTransform ( c->GetTransform());
      }
      DrawStarbox(solarview->GetCamera ()); // draw the starbox if loaded 
      rootbody->Draw_Orbit( solarview->GetCamera () );
      solarview->Draw();
    } else 
    {
      CreateCamera();
    }

  } // end if rootbody
}

void Solarsys::UpdateSystemTime( long ts )
{
  last_update_seconds = ts;
}

void Solarsys::SetSector(csRef<iSector>& sect)
{

  if (!sect) 
  {
    printf("Warning:Solarsys::SetSector:unable to set sector!\n");
  } else
  {
    sector = sect;
    printf("Solarsys::SetSector:sector set!\n");
// set starbox sector TBD
// set rootbodys sector TBD

  }
}

void Solarsys::DrawStarbox(iCamera* c)
{
    // Draw starbox
    if (starbox) 
    {
      starbox->Background(c);
    } else 
    {
      printf("Solorsys::drawstarbox:: no starbox\n"); 
    }
}


bool Solarsys::CreateCamera()
{
  // Now we need to position the camera in our world.
  if (sector) 
  {
    if (solarview) delete solarview;

    solarview.AttachNew(new csView (engine, g3d));
    solarview->GetCamera ()->SetSector (sector);
    // We use the full window to draw the solarsys.
    iGraphics2D* g2d = g3d->GetDriver2D ();
    solarview->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());
    iCamera* cam = solarview->GetCamera ();
    cam->SetViewportSize (g2d->GetWidth (), g2d->GetHeight ());
  }
  else
  {
   printf ("Solarsys::CreateCamera():cant set sector\n");
   return false;
  }

  csVector3 pos(0,0,0);
  solarview->GetCamera ()->GetTransform ().SetOrigin (pos);
  rm = engine->GetRenderManager();
  if (!rm) { printf("Solarsys::CreateCamera(): no rm\n"); return false;}
  printf ("Created solarview\n");
  return true;
}

//---------- Factory Implementation ---------

SolarsysFactory::SolarsysFactory (iBase* parent)
  : scfImplementationType (this, parent)
{}
 
csPtr<iSolarsys> SolarsysFactory::CreateObject ()
{
  return new Solarsys(object_reg);
}
 
bool SolarsysFactory::Initialize (iObjectRegistry* r)
{
  object_reg = r;
  return true;
}
