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

//File: `mystarbox.h'
    
#ifndef _H_MYSTARBOX_
#define _H_MYSTARBOX_

#include <crystalspace.h>
#include <imystarbox.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <csgfx/rgbpixel.h>

#include "system.h"

enum SpectralType 
{ 
  O, B, A, F, G, K, M
};

CS_IMPLEMENT_PLUGIN

class csCameraCatcher;


//--------------------------------------------------------------------------


// Implementation header
class MyStarbox : public scfImplementation1<MyStarbox, iMyStarbox  >
{
  private:
    iObjectRegistry* object_reg; 
    csRef<iEngine> engine;
    csRef<iGraphics3D> g3d;
    csRef<iGraphics2D> g2d;
    bool initialized ;
    csStringHash xmltokens;
    csRef<iStandardReporterListener> stdrep;
    // The camera catcher.
    csRef<csCameraCatcher> catcher;
  
    
    std::string galaxy_name;
    std::vector < System* > systems ;
    System* current_sys; // A pointer to current solar system 
  

    bool flg_useTexStars ;
    bool flg_useNebula ;
    /// the halo image to use as a star , one for each type of star 
    csRefArray<iTextureWrapper> star_tex;
    //iImage* star_img[7];
    csRef<iImage>  star_img;
    csRefArray<iImage> star_img_ar;

    csRGBpixel* apal;
    int ra_systems[360]; // used to speed up drawing stars 

  public:
    MyStarbox(iObjectRegistry* r);
    virtual ~MyStarbox();
  
    // From iStarbox.
    virtual void SetName(char const* name) { galaxy_name = name ; };  
    virtual char const*  GetName() const { return galaxy_name.c_str(); };
  
      virtual bool LoadStarCatalogue(const std::string& file_name );
    virtual bool LoadNebulaCatalogue(const std::string& file_name );
    virtual bool SetCurrentSystem(const int& new_id );
    virtual bool Background(const iCamera* c);
    virtual bool BackgroundImageNebula(const iCamera* c);  //api change 
    void DrawStarTexture(iCamera* c);
  
    bool Initialize (iObjectRegistry*);


  private:
    void SetupPlugin ();
  
    float getM3xAngle ( csMatrix3 m3 );
    float getM3yAngle ( csMatrix3 m3 );
    float getM3zAngle ( csMatrix3 m3 );

    int StarColor ( SpectralType type, float dist );
    int StarType( std::string type ) ;
  
    virtual bool BackgroundImage(const iCamera* c);



    void ProcessFrame();
    void Report (int severity, const char* msg, ...);
  
    csImageMemory* NovaImage
    ( 
      int seed,    /// Random seed for generating halo
      int star_size,    /// The size of the halo image in pixels
      int num_spokes,    /// Number of halo spokes
      float roundness,   /// The "roundness" factor
      csRGBcolor color   /// The color of the halo 
    );
  
    csImageMemory* NovaImageRGB
    ( 
      int seed,    /// Random seed for generating halo
      int star_size,    /// The size of the halo image in pixels
      int num_spokes,    /// Number of halo spokes
      float roundness,   /// The "roundness" factor
      csRGBcolor color   /// The color of the halo 
    );
  
  /*
    csImageMemory* HaloImage();
  */
    csImageMemory* HaloImageRGB
    ( 
      int star_size,    /// The size of the halo image in pixels
      float intensity,  /// The "intensity" factor 0-1
      float cross ,    /// The "cross" factor 0-1
      csRGBcolor color  /// The color of the halo 
    );


    bool SaveImage(std::string filename ) ; //, csImageMemory* img) ;
    bool SaveImage(std::string filename, csImageMemory* csImgMem ) ;
    iTextureWrapper* Create2dHaloTexture ( int startype );
  
};
  


// Factory Implementation header
class MyStarboxFactory : public scfImplementation2<MyStarboxFactory, iMyStarboxFactory, iComponent>
{
  private:
    iObjectRegistry* object_reg;

  public:
    MyStarboxFactory (iBase* p);
   
    virtual csPtr<iMyStarbox> CreateObject ();
   
    bool Initialize (iObjectRegistry* r);
};


//--------------------------------------------------------------------------

class csCameraCatcher : public scfImplementation1<csCameraCatcher, iEngineFrameCallback>
{
  public:
    iCamera* camera ;
    MyStarbox* starbox ;

  public:
    csCameraCatcher ( MyStarbox* astarbox) : scfImplementationType (this) {
        camera = 0; 
        starbox = astarbox; }

    virtual ~csCameraCatcher () { }
    virtual void StartFrame (iEngine* engine, iRenderView* rview)
    {


      if ( rview ) {
        camera = rview->GetCamera ();
        //if ( rview->GetGraphics3D().BeginDraw( engine->GetBeginDrawFlags() | CSDRAW_2DGRAPHICS ))
        
          starbox->Background ( camera );
          starbox->BackgroundImageNebula ( camera );

      } else { 
        printf("mystarbox::csCameraCatcher:No rview\n"); 
      } // end if 
    }
};

#endif 
// _H_MYSTARBOX_
