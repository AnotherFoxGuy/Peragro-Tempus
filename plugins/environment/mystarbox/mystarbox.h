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

//File: `mystarbox.h'

#ifndef __MYSTARBOX_H__
#define __MYSTARBOX_H__

#include "crystalspace.h"
#include "imystarbox.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "system.h"

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

  int ra_systems[360]; // used to speed up drawing stars

public:
  MyStarbox(iObjectRegistry* r);
  virtual ~MyStarbox();

  // From iStarbox.
  virtual void SetName(char const* name) { galaxy_name = name ; };
  virtual char const*  GetName() const { return galaxy_name.c_str(); };

  virtual bool LoadStarCatalogue(const std::string& file_name );
  virtual bool SetCurrentSystem(const int& new_id );
  virtual bool Background(const iCamera* c);

  bool Initialize (iObjectRegistry*);


private:
  void SetupPlugin ();

  float getM3xAngle ( csMatrix3 m3 );
  float getM3yAngle ( csMatrix3 m3 );
  float getM3zAngle ( csMatrix3 m3 );

  int StarColor ( std::string type, float dist );

  virtual bool BackgroundImage(const iCamera* c);

  void DrawStar2D ( iGraphics2D* g2d,
                    int w,
                    int h,
                    float foc,
                    const iCamera* c,
                    System* itr);

  void ProcessFrame();
  void Report (int severity, const char* msg, ...);

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

class csCameraCatcher : public scfImplementation1<csCameraCatcher,
  iEngineFrameCallback>
{
public:
  iCamera* camera;
  MyStarbox* starbox;
public:
  csCameraCatcher (MyStarbox* astarbox) : scfImplementationType (this) {
    camera = 0;
    starbox = astarbox; }
  virtual ~csCameraCatcher () { }
  virtual void StartFrame (iEngine* engine, iRenderView* rview)
  {
    if ( rview ) {
      camera = rview->GetCamera ();
      if ( camera ) starbox->Background ( camera );
     }
      else { printf("No rview\n"); } //end if
  }

};

#endif
// __MYSTARBOX_H__
