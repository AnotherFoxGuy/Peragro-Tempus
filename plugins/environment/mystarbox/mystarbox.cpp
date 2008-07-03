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
#include "const.h"


SCF_IMPLEMENT_FACTORY(MyStarboxFactory);


MyStarbox::MyStarbox(iObjectRegistry* r)
  : scfImplementationType(this)

{
  object_reg = r;
  galaxy_name = "";
  flg_useTexStars = true;
  flg_useNebula = false;
  initialized = false;
  flg_autodraw = true; 
  flg_useTexStars = false;
  flg_useNebula = false;
  catcher.AttachNew(new csCameraCatcher(this));
  Initialize(object_reg);
}
 
MyStarbox::~MyStarbox()
{
   // Delete all systems from this galaxy first
  for(std::vector<System*>::iterator itr = systems.begin(); itr != systems.end();++itr) 
  {
    delete(*itr);
  } // end for iterate systems

  // Release the star textures.
  star_tex.DeleteAll();
}


bool MyStarbox::Initialize(iObjectRegistry *object_reg)
{
  printf("MyStarbox::Initialize\n");
  return SetupPlugin();
}

// My methods

bool MyStarbox::SetupPlugin()
{

  if (initialized) return true;

  if (!engine)
  {
    engine = csQueryRegistry<iEngine>(object_reg);
    if (engine)
      engine->AddEngineFrameCallback(catcher);
  }

  if (!g3d) g3d = csQueryRegistry<iGraphics3D>(object_reg);

  if (!g3d)
  {
    initialized = false;
    Report(CS_REPORTER_SEVERITY_ERROR, "MyStarbox::SetupPlugin:No G3D!");
    return false;
  }

  if (!g2d)
    g2d = g3d->GetDriver2D();
  if (!g2d)
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "MyStarbox::SetupPlugin:No G2D!");
    return false;
  }


  for (int x = 0; x<=7; x++)
  {
    star_tex.Insert(x, Create2dHaloTexture(x));
  }

  printf("MyStarbox::SetupPlugin:succesfull\n");
  initialized = true;
  return true;
}

bool MyStarbox::LoadNebulaCatalogue(const std::string& file_name )
{
  // Data Format
  // ID  texture_name width(light years) height(light years)    
  // 1  txt_name 15 15

  std::ifstream inFile;
  std::string data;
  std::vector<System*>::iterator itr;  // star system iterator for adding nebula to star systems
  // Temp var for nubula data
  int id;
  std::string tex_name;
  float width;
  float height;

  
  printf("Loading Nebula Calalogue:%s\n", file_name.c_str() );

  try
  { 
    inFile.open((file_name).c_str());

    if (!inFile) 
    {
      throw("Unable to open file ");
    }

    while (inFile >> data) 
    {
      // Get data 
      id = std::atoi(data.c_str());

      inFile >> data ; // materal 
      tex_name = data.c_str();

      inFile >> data; // width light years
      width = std::atof (data.c_str());

      inFile >> data; // height light years
      height = std::atof (data.c_str());
      
      itr = systems.begin();
      do
      {
        itr++ ;
      } while ((itr != systems.end()) && ((*itr)->Get_Id() != id));

      if (itr != systems.end())
      {
        if ((*itr)->Get_Id() == id) 
        {
          if (!(*itr)->Add_Nebula(tex_name, width, height, engine))
          {
            printf("mystarbox:Faild to find Nebula Texture %s.\n",tex_name.c_str());
          }
        }
      }
    } // end while data 

  inFile.close();
  flg_useNebula = true ;
  return true;

  }
  catch(const char * str ) {
    printf ("Exception raised:MyStarbox::LoadNebulaCatalogue: %s.\n" , str );
    return false;
  } // end catch
} 


bool MyStarbox::LoadStarCatalogue(const std::string& file_name)
{
  // Data Format.
  // ID RA         DEC       Distance  AbsMag  Star_Type/s
  // 1  0.00091185 1.0890133 921.37572 9.09766 F5

  std::ifstream inFile;
  int x = 1;

  std::string data;
  System* system; 

  // Temp var for System data.
  int id;
  int current_ra = 0;
  int syscount = 0;
  std::string name;
  float ra;
  float dec;
  float distance;


  // Temp var for Star data
  std::string star_classification;

  float AbsMag;

  printf("Loading Star Calalogue:%s\n", file_name.c_str());

  try {
    inFile.open((file_name).c_str());

    if (!inFile)
    {
      throw ("Unable to open file ");
    }

    inFile >> data;  // Skip first entry, Total # stars
  
    while (inFile >> data)
    {
      // Get data 
      id = std::atoi(data.c_str()); 
      name = "sys:";
      name += data.c_str();
      inFile >> data; // RA.
      ra  = std::atof(data.c_str());

      inFile >> data; // DEC.
      dec = std::atof(data.c_str());

      inFile >> data; // Distance.
      distance = std::atof(data.c_str());

      inFile >> data;   // AbsMag // Think it's actual the apparent mag 
      AbsMag = std::atof(data.c_str());

      inFile >> data; // StarType.
      star_classification = data;

      system = new System(id, name, ra, dec, distance);

      system->Calculate_Position();
      if (id==0) { current_sys = system; } // Keep a pointer to the start system

      // Loop through the systems Stars and create the star's
      // Note, only adding one star per system to start
      // Use the sys name + star type as temp name
      std::string tempName = name + star_classification;

/*
      printf ("StarName:%s\n" , tempName.c_str() );
      printf ("Star classification:%s\n" , star_classification.c_str() );
      printf ("Star AbsMag:%4.10f\n" , AbsMag );
*/

      syscount ++;
      system->Add_Star
      (
        tempName,
        star_classification,
        AbsMag,
        StarColor(static_cast<SpectralType>(StarType(star_classification)), distance),
        star_tex[StarType(star_classification)] 
      );

      systems.push_back(system);
      // printf ("Loading System ID:%i.\n" , id );
      // Set up the ra_system array.
      if (current_ra < static_cast<int>(ra))
      {
        ra_systems[static_cast<int>(ra)] = syscount;
        current_ra = static_cast<int>(ra);
      } // end if

      x++;
    } // end while data

  inFile.close();

  return true;

  }
  catch(const char * str ) {
    printf("Exception raised:MyStarbox::LoadStarCatalogue: %s.\n", str);
    return false;
  } // end catch
} 

int MyStarbox::StarColor(SpectralType type, float dist)
{
  int starcolor;
  int colorpart;

  SetupPlugin();

  colorpart = static_cast<int>(255 / (dist/150));

  //std::string tmp_str = type.substr(0,1);

  switch (type)
  {
    case 0:
      starcolor = g2d->FindRGB(colorpart, colorpart, colorpart);
      break;
    case 1:
      starcolor = g2d->FindRGB(colorpart, colorpart, colorpart);
      break;
    case 2:
      starcolor = g2d->FindRGB(colorpart, colorpart, colorpart);
      break;
    case 3:
      starcolor = g2d->FindRGB(colorpart, colorpart, colorpart);
      break;
    case 4: 
      starcolor = g2d->FindRGB(colorpart, colorpart, colorpart);
      break;
    case 5:
      starcolor = g2d->FindRGB(colorpart,colorpart,colorpart - (255 - 221));
      break;
    case 6:
      starcolor = g2d->FindRGB(colorpart, colorpart, colorpart - (255 -170));
      break;
    default: 
      starcolor = g2d->FindRGB(colorpart, colorpart - (255 - 200), colorpart - (255-135));

  }

  return starcolor;
}

int MyStarbox::StarType(std::string type )
{

  SetupPlugin();
  std::string tmp_str = type.substr(0,1);

  switch (tmp_str[0])
  {
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


bool MyStarbox::SetCurrentSystem(const int& new_id)
{
   // Select a new current_sys using ID
  for (std::vector<System*>::iterator itr = systems.begin(); itr != systems.end(); ++itr) 
  {
    if ((*itr)->Get_Id() == new_id) 
    {
      current_sys = *itr;
      return true;
    };
  } // end for iterate systems

  return false;
}


bool MyStarbox::Background(const iCamera* c)
{
  if (!BackgroundImage(c)) return false;
  return true;
}; 

bool MyStarbox::BackgroundImage(const iCamera* c)
{ 
  float fov;

  if (!g2d)
  { 
    printf ("MyStarbox::BackgroundImage: No G2d!\n");
    return false;
  }
  if (!c)
  {
    printf ("MyStarbox::BackgroundImage: No camera!\n");
    return false;
  }
  //printf("MyStarbox::BackgroundImage:test begindraw.\n");

  g3d->BeginDraw(CSDRAW_CLEARSCREEN);
  fov = c->GetFOVAngle();
  

  float viewangle = getM3xAngle(c->GetTransform().GetO2T()) * (180/ PI) ;
  if (viewangle < 0) viewangle = 360 + viewangle;
  viewangle = 360 - viewangle;



  // Iterate STARS 
  int startview = static_cast<int>(viewangle - (fov/2));
  int endview = static_cast<int>(viewangle + (fov/2));

  if (startview < 0) startview = startview + 359;
  if (endview > 359) endview = endview - 359;

//  printf("fov(%f) ViewAngle(%f) view(%i,%i)\n ", fov , viewangle , ra_systems[startview] , ra_systems[endview]);
//  std::cout << "DEBUG: " << startview << "-" << endview << ":" << viewangle << std::endl;  

  for (std::vector<System*>::iterator itr = systems.begin();
    itr != systems.end();
      ++itr) 
  {
    if (flg_useTexStars == false)
    {
      (*itr)->DrawStar2D(g2d, c);
    } else 
    {
 //     (*itr)->DrawStar2D(g2d, c);
      (*itr)->DrawStar3D(g3d, c);
    }
  } // end for iterate systems  
  
  return true ;
}

bool MyStarbox::BackgroundImageNebula(const iCamera* c)
{ 
  float fov;

//  printf("MyStarbox::BackgroundImage:START\n");
  if (!g2d) 
  { 
    printf ("MyStarbox::BackgroundImage: No G2d\n");
    return false;
  }
  if (!c)
  {
    printf ("MyStarbox::BackgroundImage: No camera\n");
    return false;
  }

  fov = c->GetFOVAngle () ;
  

  float viewangle = getM3xAngle(c->GetTransform().GetO2T()) * (180/ PI);
  if (viewangle < 0) viewangle = 360 + viewangle;
  viewangle = 360 - viewangle;



  // Iterate STARS 
  int startview = static_cast<int>(viewangle-(fov/2));
  int endview = static_cast<int>(viewangle+(fov/2));

  if (startview < 0) startview = startview + 359;
  if (endview > 359) endview = endview - 359;


  //printf("fov(%f) ViewAngle(%f) view(%i,%i)\n ", fov , viewangle , ra_systems[startview] , ra_systems[endview]);
  //std::cout << "DEBUG: " << startview << "-" << endview << ":" << viewangle << std::endl;  

  for (std::vector<System*>::iterator itr = systems.begin();
    itr != systems.end(); ++itr) 
  {
    if (flg_useNebula && (*itr)->Has_Nebula())
    {
      (*itr)->DrawNebulaTexture(c, g3d);
    } // end if draw nebula 
  } // end for iterate systems



  return true;
}


iTextureWrapper* MyStarbox::Create2dHaloTexture(int startype)
{

// really nead the stars to do this , not sure how to keep resource usage low though 
  csImageMemory* csImgMem;
  iTextureWrapper* my_tex;
  std::string tex_name;
  csRGBcolor color(255,25,0);

  tex_name = "starboxhalo" + startype;

//  csImgMem = HaloImageRGB (default_star_size_tex_px , 0.8 , 0.7 , csRGBcolor (255,203,131 ) );

  switch (startype )
 {
    case 0:
      //csImgMem = NovaImageRGB (10 , SB_STAR_TEX_SIZE , 10 , .9 , csRGBcolor (157,162,255 ) );
      color = csRGBcolor(157, 162, 255);
      break;
    case 1:
      color = csRGBcolor(233, 242, 255);
      break;
    case 2:
      color = csRGBcolor(255, 255, 255);
      break;
    case 3:
      color = csRGBcolor(238, 238, 201);
      break; 
    case 4:
      color = csRGBcolor(243, 243, 147);
      break; 
    case 5:
      color = csRGBcolor(255, 203, 131);
      break; 
    case 6:
      color = csRGBcolor(231, 0, 0);
      break; 
    default: 
      color = csRGBcolor(255, 255, 255);

  }

   if (startype > 1 ) 
   {
     csImgMem = NovaImageRGB(10, static_cast<int>(SB_STAR_TEX_SIZE * NOVA_SCALE_FACTOR), 10, 0.9, color);
   } else {
     csImgMem = HaloImageRGB(SB_STAR_TEX_SIZE * HALO_SCALE_FACTOR, 0.8, 0.7, color);
   }

  star_img_ar.Push(star_img);

  csRef<iTextureManager> tex_mng = g3d->GetTextureManager();
  if (!tex_mng) printf("myHalo2::Create2dHaloTexture: Failed to load Texture Manager!\n");

  csRef<iTextureList> tex_list = engine->GetTextureList();
  my_tex = tex_list->FindByName(tex_name.c_str());

  if (!my_tex) {
    csRef<iTextureHandle> tex_hand = tex_mng->RegisterTexture(star_img, CS_TEXTURE_2D);
    my_tex = tex_list->NewTexture(star_img); 
    my_tex->QueryObject()->SetName(tex_name.c_str());
  }

  return my_tex;
}



float MyStarbox::getM3xAngle(csMatrix3 m3)
{
  csQuaternion q;
  q.SetMatrix(m3);
  csVector3 euler_angles = q.GetEulerAngles();

  return euler_angles.x;    
}


float MyStarbox::getM3yAngle(csMatrix3 m3)
{
  csQuaternion q;
  q.SetMatrix (m3);
  csVector3 euler_angles = q.GetEulerAngles();

  return euler_angles.y;    
}

float MyStarbox::getM3zAngle(csMatrix3 m3)
{
  csQuaternion q;
  q.SetMatrix (m3);
  csVector3 euler_angles = q.GetEulerAngles();

  return euler_angles.z;    
}

void MyStarbox::Report(int severity, const char* msg, ...)
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

  void * imgdata = csGenerateNova(star_size , seed ,  num_spokes , roundness);
  int r,g,b;

  for (int i = 0; i<256; i++) 
  {
    r = color.red - i;
    g = color.green - i;
    b = color.blue - i;
    if (r < 0  ) r = 0;
    if (g < 0  ) g = 0;
    if (b < 0  ) b = 0;
    apal[255-i].Set (r, g, b);

  }

  csImgMem = new csImageMemory 
  (
    star_size,    // w
    star_size,    // h
    imgdata,    // buffer
    false,      // destroy
    CS_IMGFMT_PALETTED8,  // format
    apal      // csRGBpixel* palette = 0
  );

  star_img.AttachNew(csImgMem);

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

  void * imgdata = csGenerateNovaRGB(star_size, seed,  num_spokes, roundness, color);

  csImageMemory*  csImgMem = new csImageMemory (
              star_size,     //w
              star_size,    //h
              imgdata ,//buffer 
              true,    // destroy
              CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA // format
              );

  star_img.AttachNew(csImgMem);

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

  for (int i = 0; i<256 ; i++) 
  {
    r = color.red - i;
    g = color.green - i;
    b = color.blue - i;
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;

//    apal[255-i].Set (r , g, b  );
    apal[255-i].Set(color.red, color.green, color.blue);
  }


  uint8 * imgdata = csGenerateHalo(star_size, intensity, cross );
  uint8 * img_star_alpha = csGenerateHalo(star_size , intensity, cross);

  csImgMem = new csImageMemory(star_size, star_size, CS_IMGFMT_TRUECOLOR | CS_IMGFMT_ALPHA);
  csImgMem->ConvertFromPal8(imgdata, img_star_alpha, apal, 256 );

  star_img.AttachNew(csImgMem);

return csImgMem;
}



bool MyStarbox::SaveImage(std::string filename) 
{
  csRef<iVFS> vfs;
  vfs = csQueryRegistry<iVFS>(object_reg);
  if (!vfs) return false;

  csRef<iImageIO> iio = csQueryRegistry<iImageIO>(object_reg);
  csString fn;

  fn.Format(filename.c_str());
  // shot = csImageManipulate::Crop (shot, 0, g2dh - dim, dim, dim);
  csRef<iDataBuffer> data = iio->Save(star_img, "image/png");
  if (!vfs->WriteFile (fn, data->GetData(), data->GetSize())) {
    printf ("Error Saving Screenshot \n");
  } else {
    printf ("Screenshot Saved \n");
  }

return true;
}
bool MyStarbox::SaveImage(std::string filename, csImageMemory* csImgMem)
{
  csRef<iVFS> vfs;
  vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs)
    return false;

  csRef<iImageIO> iio = csQueryRegistry<iImageIO> (object_reg);
  csString fn;

  // Create2dHaloTexture (1);
  fn.Format(filename.c_str());
  // shot = csImageManipulate::Crop (shot, 0, g2dh - dim, dim, dim);
  csRef<iDataBuffer> data = iio->Save(csImgMem, "image/png");
  if (!vfs->WriteFile(fn, data->GetData(), data->GetSize())) {
    printf ("Error Saving Screenshot \n");
  } else {
    printf ("Screenshot Saved \n");
  }  

return true;
}

bool MyStarbox::SaveVisableStarCatalog(std::string filename)
{
  // Data Format.
  // ID RA         DEC       Distance  AbsMag  Star_Type/s
  // 1  0.00091185 1.0890133 921.37572 9.09766 F5

  std::ofstream outFile;
  std::string data;

  // Temp var for Star data
  std::string star_classification;
  int id = 0;
  printf("Saving Visable Star Calalogue:%s\n", filename.c_str());

  outFile.open(filename.c_str());

  if (!outFile)
  {
    printf("Unable to create file\n");
    return false;
  }

  // check all systems for abs_mag < 6.5 
  for(std::vector<System*>::iterator itr = systems.begin(); itr != systems.end();++itr) 
  {
	char szTempString[256];
    if ( (*itr)->Get_Apr_Magnitude()<=6.5 ) 
    {
      // output line in catalog
//      ss << (*itr)->Get_Catalogue_Line();
      sprintf(szTempString , "%i  %3.8f %3.8f %4.8f %3.8f %s \n",
              id,
              (*itr)->Get_Ra(), 
              (*itr)->Get_Dec(),
              (*itr)->Get_Distance(),
              (*itr)->Get_Luminosity(),
              (*itr)->Get_Classification().c_str() );

      outFile << szTempString ;
      id ++;

    }
  } // end for iterate systems

  outFile.close();

  return true;
}

//---------- Factory Implementation ---------

MyStarboxFactory::MyStarboxFactory(iBase* parent)
  : scfImplementationType (this, parent)
{}
 
csPtr<iMyStarbox> MyStarboxFactory::CreateObject ()
{
  return new MyStarbox(object_reg );
}
 
bool MyStarboxFactory::Initialize(iObjectRegistry* r)
{
  object_reg = r;
  return true;
}






