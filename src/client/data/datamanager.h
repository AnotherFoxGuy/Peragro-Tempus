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

class PointerLibrary;

#define JOIN2(a,b) a##b
#define JOIN(a,b) JOIN2(a,b)

#define IFNODE(parentNode, node)                          \
  csRef<iDocumentNode> node = parentNode->GetNode(#node); \
  if (node)                                               \

#define FOREACHNODE(parentNode, node, nodes)                                               \
  csRef<iDocumentNode> JOIN(nodes,__LINE__) = parentNode->GetNode(#nodes);                 \
  csRef<iDocumentNodeIterator> JOIN(iter,__LINE__);                                        \
  if (JOIN(nodes,__LINE__))                                                                \
    JOIN(iter,__LINE__) = (JOIN(nodes,__LINE__)->GetNodes(#node));                         \
  csRef<iDocumentNode> node;                                                               \
  while (JOIN(iter,__LINE__) && JOIN(iter,__LINE__)->HasNext() && (node = JOIN(iter,__LINE__)->Next()) )          \

namespace PT
{
  namespace Data
  {
    class DataManager
    {
    protected:
      iObjectRegistry* obj_reg;
      csRef<iVFS> vfs;

    protected:
      std::string file;

      DataManager(PointerLibrary* ptrlib);
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
