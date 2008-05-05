/*
    Copyright (C) 2008 by Mogue Carpenter

    This file is part of mystarbox plugin for crystalspace.

    mystarbox is free software; you can redistribute it and/or modify
    it under the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    mystarbox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with mystarbox; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//File: `mystarbox.cpp'

#include "mystarbox.h"

#define SOL_DATA_DIR ""

SCF_IMPLEMENT_FACTORY(MyStarboxFactory);

MyStarbox::MyStarbox (iObjectRegistry* r)
  : scfImplementationType (this)

{
  object_reg = r ;
  galaxy_name = "";
  initialized = false;
  catcher.AttachNew (new csCameraCatcher (this));
  Initialize ( object_reg );

}

MyStarbox::~MyStarbox()
{
   // Delete all systems from this galaxy first
  for (std::vector<System*>::iterator itr = systems.begin();
    itr != systems.end();
      ++itr) {
      delete(*itr);
  } // end for iterate systems

}


bool MyStarbox::Initialize (iObjectRegistry *object_reg)
{
  printf("MyStarbox::Initialize\n");
  SetupPlugin ();

  return true;
}

// My methods

void MyStarbox::SetupPlugin ()
{

  if (initialized) return;

  printf("MyStarbox::SetupPlugin:initalising\n");
  if (!engine)
  {
    engine = csQueryRegistry<iEngine> (object_reg);
    if (engine)
      engine->AddEngineFrameCallback (catcher);
  }

  if (!g3d) g3d = csQueryRegistry<iGraphics3D> (object_reg);

  if (!g3d)
  {
    initialized = false;
    Report (CS_REPORTER_SEVERITY_ERROR, "No G3D!");
    return;
  }

  if (!g2d)
    g2d = g3d->GetDriver2D ();
  if (!g2d)
  {
    Report (CS_REPORTER_SEVERITY_ERROR, "No G2D!");
    return;
  }

  printf("MyStarbox::SetupPlugin:succesfull\n");
  initialized = true;

}


bool MyStarbox::LoadStarCatalogue(const std::string& file_name )
{
  // Data Format
  // ID RA         DEC       Distance  AbsMag  Star_Type/s
  // 1  0.00091185 1.0890133 921.37572 9.09766 F5

  std::ifstream inFile;
//std::ofstream outFile;
  std::string outLine;
  int x=1;

  std::string data;
  System* system;

  // Temp var for System data
  int id;
  int current_ra= 0;
  int syscount = 0;
  std::string name;
  float ra;
  float dec;
  float distance;
  std::ostringstream myStream;

  // Temp var for Star data
  std::string SpectralType;
  float AbsMag;

  printf("Loading Star Calalogue:%s\n", file_name.c_str() );

  try {
    inFile.open(  (SOL_DATA_DIR + file_name).c_str() );

    if (!inFile) {
      throw ( "Unable to open file ");
    }

    inFile >> data;  // Skip first entry, Total # stars

    while ( inFile >> data) {

      // Get data
      id = std::atoi(data.c_str());
      name = "sys:" ;
      name += data.c_str() ;
      inFile >> data ; // RA
      outLine += data + " " ;

      ra  = std::atof (data.c_str() );
      inFile >> data; // DEC
      outLine += data + " " ;
      dec = std::atof (data.c_str() );

      inFile >> data; // Distance
      outLine += data + " " ;
      distance = std::atol (data.c_str() );

      inFile >> data;   // AbsMag // Think it's actual the apparent mag
      outLine += data + " " ;
      AbsMag = std::atof (data.c_str());

      inFile >> data; // StarType
      outLine += data + " " ;
      SpectralType = data;
      // No System name yet, will do later
      system = new System;
      system->Init ( id , name , ra , dec , distance );

      if (id==0) {  current_sys = system; } // Keep a pointer to the start system

      // Loop through the systems Stars and create the star's
      // Note, only adding one star per system to start
      // Use the sys name + star type as temp name
      std::string tempName = name + ":" + SpectralType;
  //    printf ( "StarType:%s\n" , tempName.c_str() );
      syscount ++;
      system->Add_Star ( tempName,
                         SpectralType,
                         AbsMag,
                         StarColor(SpectralType, distance ) );

      systems.push_back(system);
      //  printf ("Loading System ID:%i\n" , id );
      // Set up the ra_system array
      if ( current_ra < (int)ra ) {
        ra_systems[(int)ra] =  syscount;
        current_ra = (int)ra;
    //    printf ( "ra-%i\n" , (int)ra );
      } // end if

      x++;
    } // end while data

  inFile.close();
  return true;

  }
  catch( const char * str ) {
    printf ("Exception raised:MyStarbox::LoadStarCatalogue: %s \n" , str );
    return false;
  } // end catch
}

int MyStarbox::StarColor ( std::string type, float dist ) {
  int starcolor;
  int colorpart;

  SetupPlugin ();

  colorpart = (int) (255 / ( dist/150));

  std::string tmp_str = type.substr(0,1);

  switch ( tmp_str[0] ) {
    case 'O':
      starcolor = g2d->FindRGB( colorpart,colorpart,colorpart ) ;
      break;
    case 'B':
      starcolor = g2d->FindRGB( colorpart,colorpart,colorpart ) ;
      break;
    case 'A':
      starcolor = g2d->FindRGB( colorpart,colorpart,colorpart ) ;
      break;
    case 'F':
      starcolor = g2d->FindRGB( colorpart,colorpart,colorpart ) ;
      break;
    case 'G':
      starcolor = g2d->FindRGB( colorpart,colorpart,colorpart ) ;
      break;
    case 'K':
      starcolor =  g2d->FindRGB( colorpart,colorpart,colorpart - (255 - 221) ) ;
      break;
    case 'M':
      starcolor =  g2d->FindRGB( colorpart,colorpart,colorpart - (255 -170) ) ;
      break;
    default:
      starcolor = g2d->FindRGB( colorpart,colorpart - (255 - 200),colorpart - (255-135) );

  }

  return starcolor;
}

bool MyStarbox::SetCurrentSystem(const int& new_id )
{
   // Select a new current_sys using ID
  for (std::vector<System*>::iterator itr = systems.begin();
    itr != systems.end();
      ++itr) {
      if ( (*itr)->Get_Id() == new_id ) {
        current_sys = *itr ;
        return true;
      };
  } // end for iterate systems

  return false;
}


bool MyStarbox::Background(const iCamera* c) {

  if ( !BackgroundImage(c) ) return false;
  return true;
};

bool MyStarbox::BackgroundImage(const iCamera* c)
{
  float fov;

//printf("MyStarbox::BackgroundImage:START\n");
  if (!g2d)
  {
    printf (" MyStarbox::BackgroundImage: No G2d\n");
    return false;
  }
  if (!c)
  {
    printf (" MyStarbox::BackgroundImage: No camera\n");
    return false;
  }
//printf("MyStarbox::BackgroundImage:test begindraw\n");

  g3d->BeginDraw( CSDRAW_CLEARSCREEN );
  fov = c->GetFOVAngle () ;


  float viewangle = getM3xAngle( c->GetTransform ().GetO2T ()) * (180/ PI ) ;
  if ( viewangle < 0 ) viewangle = 360 + viewangle ;
  viewangle = 360 - viewangle ;

  int w = g2d->GetWidth();
  int h = g2d->GetHeight();

  // Iterate STARS
  int startview = (int)(viewangle-(fov/2));
  int endview = (int)(viewangle+(fov/2));

  if ( startview < 0 ) startview = startview + 359 ;
  if ( endview > 359 ) endview = endview - 359 ;

//printf( "fov(%f) ViewAngle(%f) view(%i,%i)\n ", fov , viewangle , ra_systems[startview] , ra_systems[endview]);

//std::cout << "DEBUG: " << startview << "-" << endview << ":" << viewangle << std::endl;

  for (std::vector<System*>::iterator itr = systems.begin();
    itr != systems.end();
      ++itr) {
      DrawStar2D ( g2d, w , h, fov , c, (*itr) );
  } // end for iterate systems

  return true ;
}

void MyStarbox::DrawStar2D ( iGraphics2D* g2d,
                             int w,
                             int h,
                             float fov,
                             const iCamera* c,
                             System* itr  ) {

  csVector3 starpos;
  csVector3 cp1;

  if ( itr->GetDistance() < 1000 ) {

    Cords starcords = itr->Get_Pos ();

    starpos.x = starcords.x * 100000;
    starpos.y = starcords.y * 100000;
    starpos.z = starcords.z * 100000;
    // convert point to camera space

    csVector3 cp1 = c->GetTransform ().Other2This (starpos);
    csVector2 screen_spot = c->Perspective (cp1);
/*
    printf( "Starname(X,Y):%s (%i,%i) ra :%f\n ",
            itr->Get_Name().c_str(),
            screen_spot.x ,
            screen_spot.y,
            itr->Get_Ra() );
  */
//  std::cout << "DEBUG: ra " << itr->Get_Ra() << std::endl;

    if ( ( screen_spot.x < w &&
         screen_spot.y < h &&
         screen_spot.x > 0  &&
         screen_spot.y > 0 &&
         cp1.z > 0 ) )
    {
      // draw point on screen using 2d cords
      g2d->DrawPixel ( (int)screen_spot.x ,
          (int) (h - screen_spot.y) ,
          (int) itr->Get_Star()->Get_Color () );
    } // end if

  }; // end < 1000 LY
}

float MyStarbox::getM3xAngle ( csMatrix3 m3 )
{
  csQuaternion q;
  q.SetMatrix (m3);
  csVector3 euler_angles = q.GetEulerAngles ();

  return euler_angles.x;
}

float MyStarbox::getM3yAngle ( csMatrix3 m3 )
{
  csQuaternion q;
  q.SetMatrix (m3);
  csVector3 euler_angles = q.GetEulerAngles ();

  return euler_angles.y;
}

float MyStarbox::getM3zAngle ( csMatrix3 m3 )
{
  csQuaternion q;
  q.SetMatrix (m3);
  csVector3 euler_angles = q.GetEulerAngles ();

  return euler_angles.z;
}

void MyStarbox::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csRef<iReporter> rep = csQueryRegistry<iReporter> (object_reg);
  if (rep)
  {
    bool old_stdout = false;
    if (stdrep && severity == CS_REPORTER_SEVERITY_DEBUG)
    {
      old_stdout = stdrep->IsStandardOutput (severity);
      stdrep->SetStandardOutput (severity, true);
    }
    rep->ReportV (severity, "crystalspace.bugplug", msg, arg);
    if (stdrep && severity == CS_REPORTER_SEVERITY_DEBUG)
      stdrep->SetStandardOutput (severity, old_stdout);
  }
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}


//---------- Factory Implementation ---------

MyStarboxFactory::MyStarboxFactory (iBase* parent)
  : scfImplementationType (this, parent)
{}

csPtr<iMyStarbox> MyStarboxFactory::CreateObject ()
{
  return new MyStarbox( object_reg );
}

bool MyStarboxFactory::Initialize (iObjectRegistry* r)
{
  object_reg = r;
  return true;
}






