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
// File: `istarbox.h'


#ifndef __IMYSTARBOX_H__
#define __IMYSTARBOX_H__

#include "csutil/scf.h"

#include <string>

// Interface header
struct iMyStarbox : public virtual iBase
{
  SCF_INTERFACE(iMyStarbox,1,0,0);

  virtual void SetName(char const* name) = 0;
  virtual char const*  GetName() const = 0;

  virtual bool LoadStarCatalogue(const std::string& file_name ) = 0;
  virtual bool SetCurrentSystem(const int& new_id ) = 0;
  virtual bool Background(const iCamera* c) = 0;

};


// Factory Header
struct iMyStarboxFactory : public virtual iBase
{
  SCF_INTERFACE(iMyStarboxFactory,1,0,0);

  virtual csPtr<iMyStarbox> CreateObject () = 0;
};

#endif
// __IMYSTARBOX_H__
