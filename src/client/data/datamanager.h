/*
    Copyright (C) 2007 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PT_DATAMANAGER_H
#define PT_DATAMANAGER_H

#include <string>

#include <csutil/ref.h>
#include <iutil/document.h>
#include <iutil/objreg.h>
#include <iutil/vfs.h>

namespace PT
{
  namespace Data
  {
    class DataManager
    {
    private:
      iObjectRegistry* oreg;
      csRef<iVFS> vfs;

    protected:
      std::string file;

      DataManager();
      virtual ~DataManager();

      virtual bool parseElement(iDocumentNode* node) = 0;

      virtual const char* getRootName() = 0;
      virtual const char* getElementName() = 0;

    public:
      bool parse();
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATAMANAGER_H
