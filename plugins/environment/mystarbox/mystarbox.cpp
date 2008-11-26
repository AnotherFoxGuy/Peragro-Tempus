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

using namespace PT;

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
  }
  
  // Set up camera catcher so starbox can autodraw on frame events
  if (engine) engine->AddEngineFrameCallback(catcher);

  if (!g3d) g3d = csQueryRegistry<iGraphics3D>(object_reg);

  if (!g3d)
  {
    initialized = false;
    Report(CS_REPORTER_SEVERITY_ERROR, "MyStarbox::SetupPlugin:No G3D!\n");
    return false;
  }

  if (!g2d)
    g2d = g3d->GetDriver2D();
  if (!g2d)
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "MyStarbox::SetupPlugin:No G2D!\n");
    return false;
  }

  // Default star setting
  base_star_roundness = 0.2;
  base_star_size = 0.5;
  star_apr_mag_exp = 1.412;

  for (int x = 0; x<=7; x++)
  {
    Create2dHaloTexture(x);
  }

  printf("MyStarbox::SetupPlugin:succesfull\n");
  initialized = true;
  return true;
}

void MyStarbox::SetBaseStarSize (float val)
{
  base_star_size = val;
}

void MyStarbox::SetBaseStarRoundness (float val)
{

  base_star_roundness = val;
  // remove default textures and create new ones
  star_tex.DeleteAll();
  for (int x = 0; x<=7; x++)
  {
    Create2dHaloTexture(x);
  }

}
void MyStarbox::SetBaseStarExp (float val )
{
  star_apr_mag_exp = val;
}
void MyStarbox::SetSector (char const* name)
{
	sector = engine->GetSectors()->FindByName(name);
	if (!sector) printf("Warning:MyStarbox::SetSector: sector not found!\n");
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
  engine->SetClearScreen(false);  // to make sure the engine is not clearing the screen, ref peragro client.cpp , setclearscreen
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
      (*itr)->DrawStar3D(g3d, c, base_star_size, star_apr_mag_exp );
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

  switch (startype )
  {
    case 0:
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

  csRef<iTextureManager> tex_mng = g3d->GetTextureManager();
  if (!tex_mng) printf("myHalo2::Create2dHaloTexture: Failed to load Texture Manager!\n");
  csRef<iTextureList> tex_list = engine->GetTextureList();
  std::stringstream ss;
  ss << "starboxhalo" << startype;
  tex_name = ss.str();


  int seed = rand();
  int spokes = rand()%100;
  float roundness = base_star_roundness;
//  printf("base_star_roundness %2.2f\n",roundness );
  int num_frames = 9;
  for (int n=0; n< STAR_ANAM_FRAMES ; n++)
  {
    ss.str("");
    ss << "starboxhalo" << startype << n;
    tex_name = ss.str();


    seed = rand();
    spokes = rand()%100;
    roundness = base_star_roundness + (sin((n/10.0)*PI)/10.0);
//    printf (" seed:%i spokes:%i roundness:%2.2f modifer:%3.4f\n ", seed, spokes,roundness, (sin((n/10.0)*PI)/10.0) );

    csImgMem = NovaImageRGB(seed, SB_STAR_TEX_SIZE , spokes, roundness , color);

//    std::string filename = "starimages/";
//    filename+= tex_name + ".png";
//   SaveImage ( filename.c_str() , csImgMem );

    star_img_ar.Push(star_img);


    my_tex = tex_list->FindByName(tex_name.c_str());

    if (!my_tex) {
      csRef<iTextureHandle> tex_hand = tex_mng->RegisterTexture(star_img, CS_TEXTURE_2D);
      my_tex = tex_list->NewTexture(star_img);
      my_tex->QueryObject()->SetName(tex_name.c_str());
    } else
    {
      my_tex->SetImageFile(star_img);
      my_tex->Register(tex_mng);
    }
    // Add texxtures to star_tex array for later use
    star_tex.Insert( (startype*num_frames)+n, my_tex);

  }// end for animinate stars

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
  if (!vfs->WriteFile(fn, data->GetData(), data->GetSize())) {
    printf ("Error Saving Image:%s \n",filename.c_str());
  } else {
    printf ("Image Saved:%s \n",filename.c_str());
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
    printf ("Error Saving Image:%s \n",filename.c_str());
  } else {
    printf ("Image Saved:%s \n",filename.c_str());
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

  int x = 1;
  std::string line;

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

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs)
  {
    printf ("MyStarbox::LoadStarCatalogue: VFS failed!.\n");
    return false;
  }

  vfs->PushDir("/");
  csRef<iDataBuffer> buf = vfs->ReadFile (file_name.c_str());
  if (!buf)
  {
    printf ("MyStarbox::LoadStarCatalogue: %s failed!.\n" , file_name.c_str() );
    vfs->PopDir();
    return false;
  }

  csStringReader file(buf->GetData());
  csString data;
 
  // Skip first line , #stars
  if (file.HasMoreLines())
  {
     file.GetLine(data);
     line = data.GetData();
  }

  using namespace std;

  vector<std::string> vdata;
  vector<std::string>::iterator itr;

  while (file.HasMoreLines())
  {
    file.GetLine(data);
    line = data.GetData();
    // Split data into vector 
    string dlm (" ");
    vdata = tokenize_str( line, dlm );
    itr = vdata.begin();

    // Get data
    id = atoi( itr->c_str());

    name = "sys:";
    name += itr->c_str();


    itr++;
    ra  = atof( itr->c_str() );// RA.

    itr++;   
    dec = atof( itr->c_str()); // DEC.

    itr++;
    distance = atof( itr->c_str()); // Distance.

    itr++;
    AbsMag = std::atof( itr->c_str()); // AbsMag // magnatude of star as viewed from 32ly

    itr++;
    star_classification = itr->c_str() ;  // StarType.

    system = new System(id, name, ra, dec, distance);

    system->Calculate_Position();
    if (id==0) { current_sys = system; } // Keep a pointer to the start system

    // Loop through the systems Stars and create the star's
    // Note, only adding one star per system to start
    // Use the sys name + star type as temp name
    std::string tempName = name + star_classification;

    printf ("StarName:%s\n" , tempName.c_str() );
    printf ("Star classification:%s\n" , star_classification.c_str() );
    printf ("Star AbsMag:%4.10f\n" , AbsMag );


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

      // Add star images to anaminate star
      for (int n=1; n< STAR_ANAM_FRAMES ; n++)
      {
        system->Get_Star()->Set_Texture( star_tex.Get( (StarType(star_classification) * STAR_ANAM_FRAMES ) +n));
      }

    vdata.clear();
    x++;
    } // end while data

  return true;
}

bool MyStarbox::LoadYaleStarCatalogue(const std::string& file_name)
{
/*

Name    Col     Len    Type     Desc
-------------------------------------------------
ra	1	6	i6	ra  2000 hhmmss
dec	7	5	+-i4	dec 2000 -mmss
mag	12	3	{-}i	magnitude*100 -dd or -ddd
type	15	2	c2	object type
type1	15	1	c1	object type 1	/ star,ga,cl,neb,etc
type2	16	1	c1	object type 2	/ dbl,open,cl,etc
spec2	17	2	c1d1	spectral 2 'G2'
letter	19	2	c2	greek Bayer letter(s), or Flamsteed number
const	21	3	c3	constellation (I.A.U. designation)
name	24	30	c*	name,descrip.
newline	54	1	c1	newline (max final loc)
*/

  std::string line;
  System* system;

  // Temp var for System data.
  int id=1;
  int current_ra = 0;
  int syscount = 0;
  std::string name;
  std::string tempName;
  float ra;
  float dec;


  // Temp var for Star data
  std::string star_classification;

  float AbsMag=0;

  printf("Loading Yale Star Calalogue:%s\n", file_name.c_str());

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs)
  {
    printf ("MyStarbox::LoadYaleStarCatalogue: VFS failed!.\n");
    return false;
  }

  vfs->PushDir("/");
  csRef<iDataBuffer> buf = vfs->ReadFile (file_name.c_str());
  if (!buf)
  {
    printf ("MyStarbox::LoadYaleStarCatalogue: %s failed!.\n" , file_name.c_str() );
    vfs->PopDir();
    return false;
  }

  csStringReader file(buf->GetData());
  csString data;

  while (file.HasMoreLines())
  {
    file.GetLine(data);
    line = data.GetData();

    int rahh, ramm, rass;
    int decmm, decss;
    float apr_mag; 
    float abs_mag, distance;

    //   printf("Object       mV     MV     mV-MV     d(pc)   d(ly)\n");

    if ( line.length() > 14 )
    {
      rahh = atoi(line.substr(0,2).c_str());
      ramm = atoi(line.substr(2,2).c_str());
      rass = atoi(line.substr(4,2).c_str());

      decmm = atoi(line.substr(6,3).c_str());
      decss = atoi(line.substr(9,2).c_str());

      ra = static_cast<float>((rahh+((ramm+(rass/60.0f))/60.0f))*15);
      dec =static_cast<float>(decmm + (decss/60.0f));  // really not sure about this conversion

      apr_mag = (atof(line.substr(11,3).c_str()))/100;

      //       printf ("RA:%i:%i:%i Dec:%i:%i mag:%2.3f\n", rahh, ramm, rass, decmm, decss, apr_mag );
      //        printf ("RA:%3.4f Dec:%2.2f mag:%2.3f\n", ra, dec, apr_mag );

      // distance = std::atof(data.c_str()); need to work this out from classification and apr_mag

      star_classification = line.substr(16,2).c_str();
      distance = Estmate_Distance_Sequence_Fitting( star_classification, apr_mag );
      abs_mag = Estmate_Abs_Magnitude(star_classification);
      name = line.substr(23,29);
      system = new System(id, name, ra, dec, distance);
      system->Calculate_Position();
      tempName = star_classification + ":" + line.substr(20,3) + name;

      system->Add_Star
        (
        tempName,
        star_classification,
        AbsMag,
        StarColor(static_cast<SpectralType>(StarType(star_classification)), distance),
        star_tex[StarType(star_classification)*6]
      );

      // Add star images to anaminate star
      for (int n=1; n< STAR_ANAM_FRAMES ; n++)
      {
        system->Get_Star()->Set_Texture( star_tex.Get( (StarType(star_classification) * STAR_ANAM_FRAMES ) +n));
      }
      systems.push_back(system);

      // Set up the ra_system array.
      syscount++;
      if (current_ra < static_cast<int>(ra))
      {
        ra_systems[static_cast<int>(ra)] = syscount;
        current_ra = static_cast<int>(ra);
      } // end if
      id++;
    } // end if length check
  } // end while

  vfs->PopDir();
  return true;
}

float MyStarbox::Estmate_Distance_Sequence_Fitting(const std::string&  star_classification,float apr_mag )
{
  float M = Estmate_Abs_Magnitude(star_classification);
  float d = pow( 10, ((M - apr_mag + 5)/5) );

//  printf("Star Class %s est :%1.1f(0-7) M=%4.6f d=%4.4fly\n", star_classification.c_str(), star_type, M, d*3.26);
//   printf("Object       mV     MV     mV-MV     d(pc)   d(ly)\n");
//   printf("%s      %3.4f    %3.4f    %3.4f     %3.4f   %3.4f\n", star_classification.c_str(), apr_mag, M, (apr_mag-M), d, (d*3.26) );
// m-M = 5 log(d) - 5 -> m - M + 5 = 5 log(d) -> log(d) = (m - M + 5)/5 -> d = 10^[(m - M + 5)/5].

  return (d*3.26);
}

float MyStarbox::Estmate_Abs_Magnitude (const std::string&  star_classification)
{
   // guess a abs_mag using the star_clissification // high margin of error
  float star_type = StarType(star_classification);
  star_type += (atof(star_classification.substr(1,1).c_str())/10.0f) ;
  float M = (star_type*2.5)-5;
  return M;
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

std::vector<std::string> MyStarbox::tokenize_str(const std::string & str,
                                                 const std::string & delims )
{

  using namespace std;
  // Skip delims at beginning, find start of first token
  string::size_type lastPos = str.find_first_not_of(delims, 0);
  // Find next delimiter @ end of token
  string::size_type pos     = str.find_first_of(delims, lastPos);

  // output vector
  vector<string> tokens;

  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delims.  Note the "not_of". this is beginning of token
      lastPos = str.find_first_not_of(delims, pos);
      // Find next delimiter at end of token.
      pos = str.find_first_of(delims, lastPos);
    }

  return tokens;
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






