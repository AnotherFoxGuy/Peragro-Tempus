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
// File: `istarbox.h'


#ifndef _H_IMYSTARBOX_
#define _H_IMYSTARBOX_

#include "csutil/scf.h"

#include <string>

// Interface header
struct iMyStarbox : public virtual iBase
{
  SCF_INTERFACE(iMyStarbox,1,0,4);

  virtual void SetName(char const* name) = 0;
  virtual void SetSector(char const* name) = 0;  //api change 1.0.3
  virtual void SetAutoDraw(const bool val ) = 0;  //api change 1.0.2
  virtual void SetUseTextures (bool val ) = 0;  //api change 1.0.2
  virtual void SetUseNebula (bool val ) = 0;  //api change 1.0.2
  virtual void SetBaseStarSize (float val ) = 0;  //api change 1.0.3
  virtual void SetBaseStarRoundness (float val ) = 0;  //api change 1.0.4
  virtual void SetBaseStarExp (float val ) = 0;  //api change 1.0.4

  virtual char const* GetName() const = 0;
  virtual iSector const* GetSector() const = 0; //api change 1.0.3
  virtual bool GetAutoDraw() = 0;  //api change 1.0.2

  virtual bool LoadStarCatalogue(const std::string& file_name ) = 0;
  virtual bool LoadYaleStarCatalogue(const std::string& file_name ) = 0;  //api change 1.0.3
  virtual bool LoadNebulaCatalogue(const std::string& file_name ) = 0;
  virtual bool SetCurrentSystem(const int& new_id ) = 0;
  virtual bool Background(const iCamera* c) = 0;
  virtual bool BackgroundImageNebula(const iCamera* c) =0;  //api change 1.0.1


};


// Factory Header
struct iMyStarboxFactory : public virtual iBase
{
  SCF_INTERFACE(iMyStarboxFactory,1,0,0);

  virtual csPtr<iMyStarbox> CreateObject () = 0;
};

#endif
// _H_IMYSTARBOX_
