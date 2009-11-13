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

//CS_IMPLEMENT_PLUGIN

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
    //printf("Solarsys::Initialize:Failed to create view!\n");
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

  if (!c) return;

  if ( rootbody  )
  {
    // display orbits from body's surface POV
    rootbody->Update_Body(ts);
    rootbody->Update_Mesh_Pos();
    rootbody->Update_Lights();

    if ( solarview )
    {
      if ( surbody )
      { // Selected body, draw solar system from bodys' perspective

        csReversibleTransform c1 = c->GetTransform ();
        csReversibleTransform c2 = surbody->GetSurfaceOrthoTransform(lon,lat);  // solar body surface position
        csVector3 surup = surbody->GetSurfaceVector(lon,lat);

        c2.SetO2T( c1.GetO2T() * c2.GetO2T() ) ;

        solarview->GetCamera ()->SetTransform (c2);
        DrawStarbox( solarview->GetCamera () ); // draw the starbox

        // do this to have free camera move
        // with surface camera axis drawn
        // DrawCameraAxis (c ,  solarview->GetCamera () );
        // solarview->GetCamera ()->SetTransform (c1);


      } else
      { // No selected body , draw everything from world camera position
        DrawStarbox( solarview->GetCamera () ); // draw the starbox if loaded
        solarview->GetCamera ()->SetTransform ( c->GetTransform());
      }
      if (solarsys_report_lvl) rootbody->Draw_Orbit( solarview->GetCamera () );
      rm->RenderView(solarview);
    } else
    {
      CreateCamera();
    }

  } // end if rootbody
}

csVector3 Solarsys::GetRelatveRootPos()
{
  csVector3 vsb(0);
  csVector3 vrb(0);
  csVector3 r(0);

  if (surbody)
  {

    csReversibleTransform c2 = surbody->GetSurfaceOrthoTransform(lon,lat);  //
    vsb = surbody->GetSurfacePos(lon,lat);
    vsb += surbody->GetPos();
    vrb = rootbody->GetPos(); // should really be parent

    r = c2.This2OtherRelative(vsb-vrb);

    // up seems to be down, so rotate point througn 180 on x and z axis
    csQuaternion rot;
    rot.SetAxisAngle ( csVector3(1,0,0) , (180 + surbody->GetBodyInc() + lat ) * (PI/180) );
    r = rot.Rotate(r);
    rot.SetAxisAngle ( csVector3(0,0,1) , 180 * (PI/180) );
    r = rot.Rotate(r);

  } else
  {
    vrb = rootbody->GetPos();
  }

  return r;
}

void Solarsys::UpdateSystemTime( long ts )
{
  last_update_seconds = ts;
}

void Solarsys::SetSector(csRef<iSector>& sect)
{

  if (!sect)
  {
    printf("Warning:Solarsys::SetSector():Unable to set sector!\n");
  } else
  {
    sector = sect;
    if (solarsys_report_lvl) printf("Solarsys::SetSector():Sector set!\n");
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
      if (solarsys_report_lvl) printf("Solorsys::DrawStarbox():: No starbox!\n");
    }
}


bool Solarsys::CreateCamera()
{
  // Now we need to position the camera in our world.
  // First some sanity checks
  if (!sector)
  {
   if (solarsys_report_lvl) printf ("Solarsys::CreateCamera():Can't set sector!\n");
   return false;
  }
  if (!g3d) printf("Solarsys::CreateCamera(): no g3d ref!\n");
  if (!engine) printf("Solarsys::CreateCamera(): no engine ref!\n");
  if (solarview) delete solarview;

  solarview.AttachNew(new csView (engine, g3d));
  solarview->GetCamera ()->SetSector (sector);
  // We use the full window to draw the solarsys.
  iGraphics2D* g2d = g3d->GetDriver2D ();
  solarview->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());
  iCamera* cam = solarview->GetCamera ();
  cam->SetViewportSize (g2d->GetWidth (), g2d->GetHeight ());

  csVector3 pos(0,0,0);
  solarview->GetCamera ()->GetTransform ().SetOrigin (pos);

  // setup render manager
  rm = engine->GetRenderManager();
  if (!rm) { printf("Solarsys::CreateCamera(): No Render Manager!\n"); return false;}

  return true;
}

void Solarsys::DrawCameraAxis(iCamera* view_c, iCamera* surface_c)
{
  // debug function to help work out the camera axis in the solarsystem sector
  csVector3  v3start, v3end;
  csVector3 v3s, v3e;
  int green, red, blue;
  float fov;
  csOrthoTransform csCameraOrth;
  csOrthoTransform csSurfaceOrth;
  csCameraOrth = view_c->GetTransform();
  csSurfaceOrth = surface_c->GetTransform();

  if (!g3d) printf("Orbit::draw_orbit: Failed to locate 3D renderer!\n");
  csRef<iGraphics2D> g2d = g3d->GetDriver2D();
  red = g3d->GetDriver2D()->FindRGB(255, 0, 0);
  green = g3d->GetDriver2D()->FindRGB(0, 255, 0);
  blue = g3d->GetDriver2D()->FindRGB(0, 0, 255);
  fov = g3d->GetPerspectiveAspect();

  // Draw the up vector
  csVector3 origin = csSurfaceOrth.GetOrigin();
  csVector3 up = surbody->GetSurfaceVector(lon,lat)*100;
  g3d->DrawLine(csCameraOrth * (origin), (csCameraOrth * (origin + up)), fov, red);

  // Draw the other axis vectors
  csVector3 sur_vec;
  float adjlon;
  float adjlat;
  // Since lon and lat are relative to the sphere need to adjust to
  // absoulte position relative to solarplane
  // and since we want a point on the z axis add 90deg
  adjlon = (lon - surbody->GetBodyRotation() )*(PI / 180.0);
  adjlat = (90 + lat + surbody->GetBodyInc() )*(PI / 180.0);
  // Get axis vector from camera origin
  sur_vec.x = cos( adjlon ) * cos( adjlat );
  sur_vec.y = sin( adjlon ) * cos( adjlat );
  sur_vec.z = sin( adjlat );
  sur_vec = sur_vec * 100;
  g3d->DrawLine(csCameraOrth * (origin), (csCameraOrth * (origin + sur_vec)), fov, green);

  // Since lon and lat are relative to the sphere need to adjust to
  // absoulte position relative to solarplane
  adjlon = ( 90 + lon - surbody->GetBodyRotation() )*(PI / 180.0);
  adjlat = (lat + surbody->GetBodyInc() )*(PI / 180.0);

  // Get axis vector from camera origin
  sur_vec.x = cos( adjlon ) * cos( adjlat );
  sur_vec.y = sin( adjlon ) * cos( adjlat );
  sur_vec.z = sin( adjlat );

  sur_vec = sur_vec * 100;

  g3d->DrawLine(csCameraOrth * (origin), (csCameraOrth * (origin + sur_vec)), fov, blue);

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
