/*
    Copyright (C) 2008 by Mogue Carpenter

    This file is part of mystarbox plugin for crystalspace.

    mystarbox is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    mystarbox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mystarbox; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//File: `mystarbox.cpp'
    
#include "mystarbox.h"
#include "halogen.h"

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

  flg_useTexStars = true;

}
 
MyStarbox::~MyStarbox() 
{
   // Delete all systems from this galaxy first
  for( std::vector<System*>::iterator itr = systems.begin(); itr != systems.end();++itr) 
  {
    delete(*itr);
  } // end for iterate systems

  // Release the star textures.
  star_tex.DeleteAll();
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

  printf("  MyStarbox::SetupPlugin:initalising\n");
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

  // create textures for stars   
  default_star_size_tex_px = 16 ;
  for (int x = 0 ; x<=7 ; x++ )
  {
    star_tex.Insert(x, Create2dHaloTexture ( x ));
  }

  //SaveImage ( "testhalo.png" );


  printf("  MyStarbox::SetupPlugin:succesfull\n");
  initialized = true;

}


bool MyStarbox::LoadStarCatalogue(const std::string& file_name )
{
  // Data Format
  // ID RA         DEC       Distance  AbsMag  Star_Type/s
  // 1  0.00091185 1.0890133 921.37572 9.09766 F5

  std::ifstream inFile;  
//  std::ofstream outFile;  
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
  std::string star_classification; 

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
      star_classification = data;
      // No System name yet, will do later 
      system = new System;
      system->Init ( id , name , ra , dec , distance );
      
      if (id==0) {  current_sys = system; } // Keep a pointer to the start system

      // Loop through the systems Stars and create the star's
      // Note, only adding one star per system to start
      // Use the sys name + star type as temp name
      std::string tempName = name + ":" + star_classification;

  //    printf ( "StarType:%s\n" , tempName.c_str() );
      syscount ++;
      system->Add_Star 
      ( 
        tempName,
        star_classification,
        AbsMag,
        StarColor(star_classification, distance ),
        star_tex[StarType( star_classification )] 
      );

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

int MyStarbox::StarType( std::string type ) 
{

  SetupPlugin ();
  std::string tmp_str = type.substr(0,1);

  switch ( tmp_str[0] ) {
    case 'O':
      return 0;
      break; 
    case 'B':
      return 1;
      break; 
    case 'A':
      return 2;
      break; 
    case 'F':
      return 3;
      break; 
    case 'G':
      return 4;
      break; 
    case 'K':
      return 5;
      break; 
    case 'M':
      return 6;      
      break; 
    default: 
      return 7;      
  }
  return false;
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

//  printf("MyStarbox::BackgroundImage:START\n");
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
//  printf("MyStarbox::BackgroundImage:test begindraw\n");

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


//  printf( "fov(%f) ViewAngle(%f) view(%i,%i)\n ", fov , viewangle , ra_systems[startview] , ra_systems[endview]);

//  std::cout << "DEBUG: " << startview << "-" << endview << ":" << viewangle << std::endl;  

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

  if ( itr->GetDistance() < 1000  && itr->GetDistance() > 100)
  {

    Cords starcords = itr->Get_Pos ();

    starpos.x = starcords.x * 100000;
    starpos.y = starcords.y * 100000;
    starpos.z = starcords.z * 100000;
    // convert point to camera space 

    csVector3 cp1 = c->GetTransform ().Other2This (starpos);
    csVector2 screen_spot = c->Perspective (cp1);

    if ( ( screen_spot.x < w &&  screen_spot.y < h && 
       screen_spot.x > 0  && screen_spot.y > 0 &&  cp1.z > 0 ) ) 
    {

      if ( flg_useTexStars ) {
        DrawStar3D
        (
          (int)screen_spot.x ,
          (int)(h - screen_spot.y),
          35/itr->GetDistance(),
          itr->Get_StarType()
        );
      } else {
  

          // draw point on screen using 2d cords 
           g2d->DrawPixel (  (int)screen_spot.x ,
              (int) (h - screen_spot.y) ,
              (int) itr->Get_Star()->Get_Color () );
      } // end if flg_useTexStars 
    } // end if star on screen 
  }; // end < 1000 LY 
}

void MyStarbox::DrawStar3D ( int x , int y, float scale,int startype )
{
  int scale_size = int(default_star_size_tex_px * scale) ;
  int offset = int(scale_size/2);
  if ( scale_size == 0 ) return ;
  // virtual void iGraphics3D::DrawPixmap ( iTextureHandle *  hTex,sx,sy,sw,sh,tx,ty,tw,th,Alpha ) 
  g3d->DrawPixmap 
  (
    star_tex[startype]->GetTextureHandle (),
    x-offset,y-offset,
    scale_size ,scale_size ,
    0,0,
    default_star_size_tex_px ,default_star_size_tex_px,
    0 
  );

}

iTextureWrapper* MyStarbox::Create2dHaloTexture (int startype)
{


  csImageMemory* csImgMem;
  iTextureWrapper* my_tex;
  std::string tex_name ;
  csRGBcolor  color( 255,25,0 );

  tex_name = "starboxhalo" + startype ;

//  csImgMem = HaloImageRGB ( default_star_size_tex_px , 0.8 , 0.7 , csRGBcolor ( 255,203,131 ) );

  switch ( startype ) {
    case 0:
      csImgMem = NovaImageRGB ( 10 , default_star_size_tex_px , 10 , .9 , csRGBcolor ( 157,162,255 ) );
      break; 
    case 1:
      csImgMem = NovaImageRGB ( 10 , default_star_size_tex_px , 10 , .9 , csRGBcolor ( 233,242,255 ) );
      break; 
    case 2:
      csImgMem = NovaImageRGB ( 10 , default_star_size_tex_px , 10 , .9 , csRGBcolor ( 255,255,255 ) );
      break; 
    case 3:
      csImgMem = NovaImageRGB ( 10 , default_star_size_tex_px , 10 , .9 , csRGBcolor ( 238,238,201  ) );
      break; 
    case 4:
      csImgMem = NovaImageRGB ( 10 , default_star_size_tex_px , 10 , .9 , csRGBcolor ( 243 ,243,147 )  );
      break; 
    case 5:
      csImgMem = NovaImageRGB ( 10 , default_star_size_tex_px , 10 , .9 , csRGBcolor ( 255,203,131 ) );
      break; 
    case 6:
      csImgMem = NovaImageRGB ( 10 , default_star_size_tex_px , 10 , .9 , csRGBcolor ( 231,0,0 ) );
      break; 
    default: 
      csImgMem = NovaImageRGB ( 23410 , default_star_size_tex_px , 10 , .9 ,csRGBcolor ( 255,255,255 ) );

  }
  star_img_ar.Push(star_img);

  printf("Create2dHaloTexture1\n");

  csRef<iTextureManager>   tex_mng = g3d->GetTextureManager();
  if (!tex_mng ) printf("myHalo2::Create2dHaloTexture: Failed to load Texture Manager!\n");
  printf("Create2dHaloTexture4\n");

  csRef<iTextureList> tex_list = engine->GetTextureList();
  my_tex = tex_list->FindByName( tex_name.c_str() );

  if (!my_tex) {
    printf("Create2dHaloTexture:create Tex\n");
    csRef<iTextureHandle> tex_hand = tex_mng->RegisterTexture(
                   star_img, //iImage *image,
                   CS_TEXTURE_2D );  // int flags,
    my_tex = tex_list->NewTexture ( star_img ); 
    my_tex->QueryObject ()->SetName( tex_name.c_str() );
  }

  return my_tex;
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

csImageMemory* MyStarbox::NovaImage
  ( 
    int seed,    /// Random seed for generating halo
    int star_size,    /// The size of the halo image in pixels
    int num_spokes,    /// Number of halo spokes
    float roundness,   /// The "roundness" factor
    csRGBcolor color  /// The color of the halo 
  ) 
{
//  iImage* img_star;
//  csRGBpixel* apal[255];
  csImageMemory* csImgMem;
  csRGBpixel* apal = new csRGBpixel[256];

  void * imgdata = csGenerateNova ( star_size , seed ,  num_spokes , roundness );
  int r,g,b;

  for (int i = 0 ; i<256 ; i++) 
  {
    r = color.red - i;
    g = color.green - i;
    b = color.blue - i;
    if (r < 0  ) r = 0;
    if (g < 0  ) g = 0;
    if (b < 0  ) b = 0;
    apal[255-i].Set ( r , g, b  );

  }

  csImgMem = new csImageMemory 
  (
    star_size,    // w
    star_size,    // h
    imgdata,    // buffer 
    false ,      // destroy
    CS_IMGFMT_PALETTED8,  // format
    apal      // csRGBpixel* palette = 0
  );  

  star_img.AttachNew ( csImgMem );

return csImgMem;
}



csImageMemory* MyStarbox::NovaImageRGB
  ( 
    int seed,    /// Random seed for generating halo
    int star_size,    /// The size of the halo image in pixels
    int num_spokes,    /// Number of halo spokes
    float roundness,   /// The "roundness" factor
    csRGBcolor color  /// The color of the halo 
  )  
{

  void * imgdata = csGenerateNovaRGB ( star_size , seed ,  num_spokes , roundness, color );

  csImageMemory*  csImgMem = new csImageMemory (
              star_size,     //w
              star_size,    //h
              imgdata ,//buffer 
              true ,    // destroy
              CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA // format
              );  

  star_img.AttachNew ( csImgMem );

return csImgMem;

}




csImageMemory* MyStarbox::HaloImageRGB
  ( 
    int star_size,    /// The size of the halo image in pixels
    float intensity,  /// The "intensity" factor 0-1
    float cross ,    /// The "cross" factor 0-1
    csRGBcolor color  /// The color of the halo 
  )   
{
  csImageMemory* csImgMem;
  csRGBpixel* apal = new csRGBpixel[256];
  //csRef<iImage> img_star_alpha;
  int r,g,b;

  for (int i = 0 ; i<256 ; i++) 
  {
    r = color.red - i;
    g = color.green - i;
    b = color.blue - i;
    if (r < 0  ) r = 0;
    if (g < 0  ) g = 0;
    if (b < 0  ) b = 0;

//    apal[255-i].Set ( r , g, b  );
    apal[255-i].Set ( color.red , color.green, color.blue  );
  }


  uint8 * imgdata = csGenerateHalo ( star_size , intensity , cross );
  uint8 * img_star_alpha = csGenerateHalo ( star_size , intensity, cross );  

  csImgMem = new csImageMemory ( star_size, star_size, CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA );
  csImgMem->ConvertFromPal8 ( imgdata  , img_star_alpha , apal , 256 );

  star_img.AttachNew ( csImgMem );

return csImgMem;

}



bool MyStarbox::SaveImage(char const* filename) 
{
  csRef<iVFS> vfs;
  vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs) 
    return false;

  csRef<iImageIO> iio = csQueryRegistry<iImageIO> (object_reg);
  csString fn;

  // Create2dHaloTexture (1);
  fn.Format ("/this/TestHalo.png");
  // shot = csImageManipulate::Crop (shot, 0, g2dh - dim, dim, dim);
  csRef<iDataBuffer> data = iio->Save ( star_img, "image/png");
  if (!vfs->WriteFile (fn, data->GetData(), data->GetSize())) {
    printf ("Error Saving Screenshot \n");
  } else {
    printf ("Screenshot Saved \n");
  }  

return true;
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











