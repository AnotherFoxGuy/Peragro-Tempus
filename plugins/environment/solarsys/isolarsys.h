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
// File: `isolarsys.h'
 
	  
#ifndef _H_ISOLARSYS
#define _H_ISOLARSYS

#include "csutil/scf.h"

#include "imybody.h"
#include "imystarbox.h"

// Interface header
struct iSolarsys : public virtual iBase
{
  SCF_INTERFACE(iSolarsys,1,0,0);

  virtual void SetName(char const* name) = 0; 
  virtual void SetSector(csRef<iSector>& cursector) = 0; 
  virtual void SetStartCamera (csRef<iCamera>& camera) = 0;
//  virtual void SetStartTrans (csTransform* trans ) = 0;
  virtual void SetRootbody (csRef<iMyBody>& body) = 0;
  virtual void SetLongitude(long longitude ) = 0;
  virtual void SetLatitude(long latitude ) = 0;
  virtual void SetSurfaceBody (csRef<iMyBody>& body) = 0;
  virtual void SetStarbox (csRef<iMyStarbox>& sb ) = 0;

  virtual char const* GetName() const = 0;
  virtual iSector const* GetSector() const = 0;
//  virtual iBody const* GetRootbody() const = 0;
//  virtual bool AddBody( std::string& parent, Body* newbody) = 0;
  virtual void Draw( iCamera* c ) = 0;
  virtual void Draw( iCamera* c , long ts ) = 0;
//  virtual bool CreateBody()=0;
};


// Factory Header
struct iSolarsysFactory : public virtual iBase
{
  SCF_INTERFACE(iSolarsysFactory,1,0,0);
 
  virtual csPtr<iSolarsys> CreateObject () = 0;
};

#endif 
// _H_ISOLARSYS
